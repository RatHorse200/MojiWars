/* ゲーム内で使用する文字画像の表示倍率を視覚的に調整し、設定ファイルに保存するための開発用ツール

・文字スケール調整ツール
文字のテクスチャを確認し、スケールを調整するためのツールです。
調節された文字スケールは assets/char_scale.txt に保存され、main.cpp で読み込まれます。

操作説明
Tabキー           : black_ch と red_ch 間のフォルダの切り替え
← →       　      : ページを移動
左クリック         : 文字を選択
スクロール/+/-     : 均一にスケールを調整
Wキー / Sキー      : 縦のスケールを調整
Aキー / Dキー      : 横のスケールを調整
Rキー              : スケールを1.0にリセット
Qキー              : 全ての変更した設定を保存
ESCキー            : 終了

起動コマンド：make run_viewer
*/

#include "raylib.h"     
#include <string>      
#include <vector>      
#include <map>          
#include <fstream>      
#include <sstream>      
#include <filesystem>   
#include <algorithm>    
#include <cmath>        

// filesystem を fs に省略する
namespace fs = std::filesystem;

// 定数
const char* CONFIG_FILE = "assets/char_scale.txt"; // 調整したスケールを格納したファイル
const int   CELL_SIZE = 128;   // 1マスの縦横サイズ
const int   COLS = 8;     // 横の列数
const int   ROWS = 4;     // 縦の列数
const int   CHARS_PER_PAGE = COLS * ROWS; // 1ページに表示できる最大文字数
const int   PANEL_W = 300;   // サイドパネルの横幅
const int   HEADER_H = 44;    // 画面の上部の余白の高さ
const int   FOOTER_H = 36;    // 画面の下部の余白の高さ

// スケール設定
struct ScaleEntry {float sx = 1.0f, sy = 1.0f;}; // 横方向と縦方向の数値を入れる箱
static std::map<std::string, ScaleEntry> scaleMap; // 読み込んだ設定をすべて保存しておく辞書

// 文字ごとの横の倍率と縦の倍率を辞書に記憶させる関数
static void LoadConfig() {
    std::ifstream f(CONFIG_FILE);   
    if (!f.is_open()) return;       
    std::string line;              
    while (std::getline(f, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream ss(line);
        std::string key; float sx, sy;
        if (!(ss >> key >> sx)) continue;
        if (!(ss >> sy)) sy = sx;  
        scaleMap[key] = {sx, sy};
    }
}

// 文字スケール設定をテキストファイルchar_scale.txtに書き込む関数
static void SaveConfig() {
    std::ofstream f(CONFIG_FILE);
    f << "# 文字スケール設定ファイル\n";
    f << "# 書式: フォルダ名/文字  横の倍率  縦の倍率\n";
    f << "# 黒文字 タイピングなし\n";
    f << "# 赤文字 タイピングあり\n";
    f << "# デフォルトは1.0, 1.0\n";
    for (auto& [k, v] : scaleMap) {
        f << k << " " << v.sx << " " << v.sy << "\n";
    }
}

// スケールを辞書から取り出す関数
static ScaleEntry GetScale(const std::string& key) {
    auto it = scaleMap.find(key);
    return (it != scaleMap.end()) ? it->second : ScaleEntry{1.0f, 1.0f};
}

// 読み込んだ画像から白い背景を透明にする関数
static void ApplyAlphaThreshold(Image& img, int threshold) {
    ImageFormat(&img, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    Color* pixels = (Color*)img.data;
    int count = img.width * img.height;
    for (int i = 0; i < count; i++) {
        int brightness = (pixels[i].r + pixels[i].g + pixels[i].b) / 3;
        if (brightness >= threshold) pixels[i] = BLANK;
        else pixels[i].a = 255;
    }
}

// 画像の透過処理と余白を切り取る関数
static Texture2D LoadCharTex(const std::string& basePath) {
    std::string png = basePath + ".png";
    Image img = {};
    if (FileExists(png.c_str()))      img = LoadImage(png.c_str());
    if (!img.data) { Texture2D e = {}; return e; }
    ApplyAlphaThreshold(img, 220);
    ImageAlphaCrop(&img, 0.05f);
    Texture2D tex = LoadTextureFromImage(img);
    UnloadImage(img);
    return tex;
}

// 文字エントリー
struct CharEntry {
    std::string key;   // "black/あ" or "red/あ"
    std::string name;  // "あ"
    Texture2D tex = {};
};

// 画像ファイルをリストにして名前順に並び替える関数
static void LoadFolder(const std::string& path, const std::string& folderKey, std::vector<CharEntry>& out) {
    if (!fs::exists(path)) return;
    for (auto& entry : fs::directory_iterator(path)) {
        std::string stem = entry.path().stem().string();
        CharEntry ce;
        ce.key = folderKey + "/" + stem;
        ce.name = stem;
        ce.tex = LoadCharTex(path + "/" + stem);
        out.push_back(ce);
    }
    std::sort(out.begin(), out.end(), [](const CharEntry& a, const CharEntry& b) {return a.name < b.name;});
}

// スケールの値を制限するために小数点以下2桁に丸める関数
static float ClampScale(float v) {
    return std::round(std::clamp(v, 0.1f, 4.0f) * 100.0f) / 100.0f;
}

// メイン関数
int main() {
    const int SW = COLS * CELL_SIZE + PANEL_W;
    const int SH = ROWS * CELL_SIZE + HEADER_H + FOOTER_H;

    InitWindow(SW, SH, "Character Texture Debug Viewer");
    SetTargetFPS(60);

    LoadConfig();

    std::vector<CharEntry> blackChars, redChars;
    LoadFolder("assets/black_ch", "black", blackChars);
    LoadFolder("assets/red_ch", "red", redChars);

    int  folder = 0;  
    int  page = 0;
    int  selectedIdx = -1;
    bool modified = false;

    auto GetChars = [&]() -> std::vector<CharEntry>& {
        return folder == 0 ? blackChars : redChars;
    };

    // 今何行目の何列目のマスをクリックしたかを逆算し、選択された文字を決定する
    while (!WindowShouldClose()) {
        auto& chars = GetChars();
        int totalPages = ((int)chars.size() + CHARS_PER_PAGE - 1) / CHARS_PER_PAGE;
        if (totalPages == 0) totalPages = 1;

        // 入力
        if (IsKeyPressed(KEY_TAB)) {
            folder = 1 - folder;
            page = 0; selectedIdx = -1;
        }
        if (IsKeyPressed(KEY_RIGHT) && page < totalPages - 1) { page++; selectedIdx = -1; }
        if (IsKeyPressed(KEY_LEFT)  && page > 0)              { page--; selectedIdx = -1; }

        // クリックで選択
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mp = GetMousePosition();
            if (mp.x < COLS * CELL_SIZE &&
                mp.y >= HEADER_H && mp.y < HEADER_H + ROWS * CELL_SIZE) {
                int col = (int)mp.x / CELL_SIZE;
                int row = ((int)mp.y - HEADER_H) / CELL_SIZE;
                int idx = page * CHARS_PER_PAGE + row * COLS + col;
                if (idx < (int)chars.size()) selectedIdx = idx;
            }
        }

        // スケール調整
        if (selectedIdx >= 0 && selectedIdx < (int)chars.size()) {
            const std::string& key = chars[selectedIdx].key;
            ScaleEntry cur = GetScale(key);

            // 均一スケール
            float uniDelta = 0.0f;
            float wheel = GetMouseWheelMove();
            if (wheel != 0.0f)                                            uniDelta =  wheel * 0.05f;
            if (IsKeyPressed(KEY_EQUAL) || IsKeyPressed(KEY_KP_ADD))      uniDelta =  0.05f;
            if (IsKeyPressed(KEY_MINUS) || IsKeyPressed(KEY_KP_SUBTRACT)) uniDelta = -0.05f;

            // 縦スケール
            float yDelta = 0.0f;
            if (IsKeyPressed(KEY_W)) yDelta =  0.05f;
            if (IsKeyPressed(KEY_S)) yDelta = -0.05f;

            // 横スケール
            float xDelta = 0.0f;
            if (IsKeyPressed(KEY_D)) xDelta =  0.05f;
            if (IsKeyPressed(KEY_A)) xDelta = -0.05f;

            if (IsKeyPressed(KEY_R)) {
                scaleMap.erase(key);
                modified = true;
            } else if (uniDelta != 0.0f || xDelta != 0.0f || yDelta != 0.0f) {
                cur.sx = ClampScale(cur.sx + uniDelta + xDelta);
                cur.sy = ClampScale(cur.sy + uniDelta + yDelta);
                scaleMap[key] = cur;
                modified = true;
            }
        }

        // 保存
        if (IsKeyPressed(KEY_Q)) {
            SaveConfig();
            modified = false;
        }

        // 描画
        BeginDrawing();
        ClearBackground(Color{30, 30, 30, 255});

        // ヘッダー
        const char* folderLabel = folder == 0
            ? "black_ch  [no-type chars]"   // タイピングなし
            : "red_ch    [typed chars]";     // タイピングあり
        DrawText(folderLabel, 10, 12, 20, WHITE);
        DrawText(TextFormat("Page %d / %d", page + 1, totalPages), COLS * CELL_SIZE / 2 - 50, 12, 18, LIGHTGRAY);
        if (modified)
            DrawText("unsaved  (press Q to save)", COLS * CELL_SIZE - 220, 12, 18, YELLOW);

        // 文字グリッド
        int startIdx = page * CHARS_PER_PAGE;
        for (int i = 0; i < CHARS_PER_PAGE; i++) {
            int idx = startIdx + i;
            if (idx >= (int)chars.size()) break;

            int col = i % COLS;
            int row = i / COLS;
            int cx = col * CELL_SIZE;
            int cy = HEADER_H + row * CELL_SIZE;

            Color bg = (idx == selectedIdx)
                ? Color{60, 80, 180, 255}
                : Color{50, 50, 50, 255};
            DrawRectangle(cx, cy, CELL_SIZE, CELL_SIZE, bg);
            DrawRectangleLines(cx, cy, CELL_SIZE, CELL_SIZE, GRAY);

            // XYスケールでテクスチャ描画
            if (chars[idx].tex.id != 0) {
                ScaleEntry sc = GetScale(chars[idx].key);
                float dw = CELL_SIZE * sc.sx;
                float dh = CELL_SIZE * sc.sy;
                float dx = cx + (CELL_SIZE - dw) * 0.5f;
                float dy = cy + (CELL_SIZE - dh) * 0.5f;
                Rectangle src = {0, 0, (float)chars[idx].tex.width, (float)chars[idx].tex.height};
                Rectangle dst = {dx, dy, dw, dh};
                DrawTexturePro(chars[idx].tex, src, dst, {0, 0}, 0.0f, WHITE);
            }

            // デフォルト以外のスケールを表示
            ScaleEntry sc = GetScale(chars[idx].key);
            bool nonDefault = (fabsf(sc.sx - 1.0f) > 0.001f || fabsf(sc.sy - 1.0f) > 0.001f);
            if (nonDefault) {
                if (fabsf(sc.sx - sc.sy) < 0.001f) {
                    DrawText(TextFormat("%.2f", sc.sx), cx + 4, cy + CELL_SIZE - 18, 14, YELLOW);
                } else {
                    DrawText(TextFormat("X%.2f", sc.sx), cx + 4, cy + CELL_SIZE - 30, 12, Color{255,200,80,255});
                    DrawText(TextFormat("Y%.2f", sc.sy), cx + 4, cy + CELL_SIZE - 16, 12, Color{80,220,255,255});
                }
            }
        }

        // サイドパネル
        int px = COLS * CELL_SIZE;
        DrawRectangle(px, 0, PANEL_W, SH, Color{20, 20, 20, 255});
        px += 10;

        if (selectedIdx >= 0 && selectedIdx < (int)chars.size()) {
            auto& ch = chars[selectedIdx];
            ScaleEntry sc = GetScale(ch.key);

            DrawText(TextFormat("Scale X: %.2f", sc.sx), px, 12, 18, Color{255,200,80,255});
            DrawText(TextFormat("Scale Y: %.2f", sc.sy), px, 34, 18, Color{80,220,255,255});

            DrawText("Scroll / +- : uniform scale", px, 62, 13, LIGHTGRAY);
            DrawText("A / D : X scale (width)", px, 79, 13, Color{255,200,80,255});
            DrawText("W / S : Y scale (height)", px, 96, 13, Color{80,220,255,255});
            DrawText("R : reset to 1.0 1.0", px, 113, 13, LIGHTGRAY);

            // 大きいプレビュー
            int psz = PANEL_W - 20;
            int ppx = COLS * CELL_SIZE + 10;
            int ppy = 228;
            int availH = SH - ppy - FOOTER_H - 20;
            if (psz > availH) psz = availH;
            DrawRectangle(ppx, ppy, psz, psz, Color{50, 50, 50, 255});
            DrawRectangleLines(ppx, ppy, psz, psz, GRAY);

            if (ch.tex.id != 0) {
                float dw = psz * sc.sx;
                float dh = psz * sc.sy;
                float dx = ppx + (psz - dw) * 0.5f;
                float dy = ppy + (psz - dh) * 0.5f;
                Rectangle src = {0, 0, (float)ch.tex.width, (float)ch.tex.height};
                Rectangle dst = {dx, dy, dw, dh};
                DrawTexturePro(ch.tex, src, dst, {0, 0}, 0.0f, WHITE);
            }
            DrawText("Preview", ppx, ppy + psz + 4, 14, GRAY);
        } else {
            DrawText("Click a char to select",
                     COLS * CELL_SIZE + 10, SH / 2 - 10, 16, GRAY);
        }

        // フッター（操作説明）
        int fy = HEADER_H + ROWS * CELL_SIZE;
        DrawRectangle(0, fy, COLS * CELL_SIZE, FOOTER_H, Color{20, 20, 20, 255});
        DrawText("Tab : switch   </> : page   Click : select   Scroll / +- : uniform   A/D : X-width   W/S : Y-height   R : reset   S : save   ESC : quit", 6, fy + 6, 11, GRAY);

        EndDrawing();
    }

    for (auto& c : blackChars) if (c.tex.id) UnloadTexture(c.tex);
    for (auto& c : redChars)   if (c.tex.id) UnloadTexture(c.tex);
    CloseWindow();
    return 0;
}
