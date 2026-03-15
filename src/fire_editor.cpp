/* ひらがなごとの弾の発射位置・方向をGUIで設定し、保存・プレビューができる開発用ツール

・ひらがなの弾の発射位置エディターについて
各ひらがなの弾の発射位置と飛ぶ方向を設定するためのツールです。
設定したデータはassets/char_fire.txtに保存され、ゲーム本編で読み込まれます。

操作方法：
左クリック (拡大画面)     : カーソルの位置に発射ポイントを追加
右クリック (拡大画面)     : マウスから一番近い発射ポイントを削除
マウスホイール / 左右矢印 : 弾を撃つ方向を変更
左クリック (文字リスト)   : 設定するひらがなを選択
Pキー                    : 現在の設定で弾を試し撃ち (プレビュー)
Qキー                    : 全ての変更した設定を保存
ESCキー                  : 終了

起動コマンド: make run_fire
*/

#include "raylib.h"
#include "char_fire.h"
#include "sprites.h"
#include "constants.h"
#include <string>
#include <vector>
#include <filesystem>
#include <algorithm>
#include <cmath>

namespace fs = std::filesystem;

// 画面レイアウト構成
const int CHAR_LIST_W = 220;   // 画面左側にある「ひらがな一覧リスト」のパネルの幅
const int ZOOM_SIZE   = 512;   // 画面中央にある「拡大画面」のサイズ
const int INFO_W      = 280;   // 画面右側にある「操作説明などの情報」パネルの幅
const int WIN_W       = CHAR_LIST_W + ZOOM_SIZE + INFO_W;   // 上記3つのパネルの幅をすべて足し算して、ウィンドウ全体の横幅を自動計算している
const int WIN_H       = 700;   // ウィンドウ全体の高さ
const int ZOOM_FACTOR = ZOOM_SIZE / CELL_SIZE;  // 拡大画面のズーム倍率(8倍)

// プレビュー用の弾のデータ構造
struct PreviewBullet {
    float x, y, dx, dy;
    bool  active;
};

// 画面左側のリストに表示される、ひらがな1文字分のデータをまとめたデータ構造
struct CharEntry {
    std::string name;
    Texture2D   tex; 
};

// 方向ベクトルを計算する関数
static void DirVec(int dir, float& dx, float& dy) {
    float sp = 1.0f;
    float d  = sp / sqrtf(2.0f);
    switch (dir) {
        case 0: dx=-d;  dy=-d;  break;  // 左上
        case 1: dx=-sp; dy=0;   break;  // 左
        case 2: dx=-d;  dy=d;   break;  // 左下
        case 3: dx=0;   dy=sp;  break;  // 下
        case 4: dx=d;   dy=d;   break;  // 右下
        case 5: dx=sp;  dy=0;   break;  // 右
        case 6: dx=d;   dy=-d;  break;  // 右上
        default:dx=sp;  dy=0;   break;
    }
}

// 矢印を描画する関数
static void DrawDirArrow(float cx, float cy, int dir, float r, Color col) {
    float dx, dy;   // 向いている方向の移動量
    DirVec(dir, dx, dy);
    Vector2 from = {cx, cy};
    Vector2 to   = {cx + dx * r, cy + dy * r};
    DrawLineEx(from, to, 2.0f, col);    // 太さ2.0の直線を引く
    // 矢印の頭を描画する
    float hx = -dy * 0.35f * r;
    float hy =  dx * 0.35f * r;
    DrawTriangle(
        {to.x, to.y},
        {to.x - dx*0.4f*r + hx, to.y - dy*0.4f*r + hy},
        {to.x - dx*0.4f*r - hx, to.y - dy*0.4f*r - hy},
        col
    );
}

// メイン関数
int main() {
    InitWindow(WIN_W, WIN_H, "Hiragana Fire Point Editor");
    SetTargetFPS(60);   // 60FPSに固定

    LoadCharScaleConfig();
    LoadCharFireConfig();

    // red_chからひらがなリストを読み込む
    std::vector<CharEntry> chars;
    if (fs::exists("assets/red_ch")) {
        for (auto& entry : fs::directory_iterator("assets/red_ch")) {
            std::string stem = entry.path().stem().string();
            CharEntry ce;
            ce.name = stem;
            ce.tex = GetRedSpriteTexture(stem);     // 読み込んだひらがなの画像をセットする
            chars.push_back(ce);
        }
        // 並び替えを行う
        std::sort(chars.begin(), chars.end(), [](const CharEntry& a, const CharEntry& b) {
            return a.name < b.name;
        });
    }

    int   selectedIdx  = -1;    // どの文字も選ばれていない状態
    int   currentDir   = FIRE_RIGHT;    // 弾の方向は、最初は右方向とする
    bool  modified     = false;     // まだ何も設定を変更していないので、未保存の変更なし、とする

    std::vector<PreviewBullet> previewBullets;  // Pキーで撃つテスト用の弾を入れる空のリスト

    // ひらがなリスト
    int charListScrollY = 0;    // 現在のスクロール位置（最初は0）
    const int CHAR_ITEM_H   = 48;   // リストに表示する文字一個分の縦の高さ
    const int CHARS_PER_COL = 4;    // 横に四列
    const int CHAR_COL_W = CHAR_LIST_W / CHARS_PER_COL;     // 一列あたりの横幅

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();      // 前回の画面更新からの経過時間
        Vector2 mp = GetMousePosition();    // 現在のマウスカーソルの座標

        // 中央の拡大画面がどの座標から始まっているか
        int zoomX = CHAR_LIST_W;
        int zoomY = 0;

        // マウスホイールの回転量とキーボードの左右矢印キーが押されたかの判定
        float wheel = GetMouseWheelMove();
        if (wheel > 0)  currentDir = (currentDir + 1) % 7;
        if (wheel < 0)  currentDir = (currentDir + 6) % 7;
        if (IsKeyPressed(KEY_RIGHT)) currentDir = (currentDir + 1) % 7;
        if (IsKeyPressed(KEY_LEFT))  currentDir = (currentDir + 6) % 7;

        // ひらがなのクリック判定（何列目の何行目をクリックしたか）
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
            mp.x >= 0 && mp.x < CHAR_LIST_W) {
            int row = ((int)mp.y + charListScrollY) / CHAR_ITEM_H;
            int col = (int)mp.x / CHAR_COL_W;
            int idx = row * CHARS_PER_COL + col;
            if (idx >= 0 && idx < (int)chars.size()) {
                selectedIdx = idx;
                previewBullets.clear();
            }
        }

        // ひらがなリストのスクロール処理
        if (mp.x < CHAR_LIST_W) {   // マウスが左パネルの上にある状態か判定する
            int totalRows = ((int)chars.size() + CHARS_PER_COL - 1) / CHARS_PER_COL;
            int maxScroll = std::max(0, totalRows * CHAR_ITEM_H - WIN_H);
            charListScrollY -= (int)(GetMouseWheelMove() * CHAR_ITEM_H);
            charListScrollY = std::clamp(charListScrollY, 0, maxScroll);
        }

        // 拡大画面内の処理
        if (selectedIdx >= 0) {
            const std::string& ch = chars[selectedIdx].name;

            // 左クリック（元の画像の中の、どの座標をクリックしたか）
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
                mp.x >= zoomX && mp.x < zoomX + ZOOM_SIZE &&
                mp.y >= zoomY && mp.y < zoomY + ZOOM_SIZE) {
                int relX = (int)((mp.x - zoomX) / ZOOM_FACTOR);
                int relY = (int)((mp.y - zoomY) / ZOOM_FACTOR);
                relX = std::clamp(relX, 0, CELL_SIZE - 1);
                relY = std::clamp(relY, 0, CELL_SIZE - 1);
                charFireConfig[ch].push_back({relX, relY, currentDir});
                modified = true;
            }

            // 右クリック（左クリックの処理と同様にして、一番近いポイントをリストから削除する）
            if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) &&
                mp.x >= zoomX && mp.x < zoomX + ZOOM_SIZE &&
                mp.y >= zoomY && mp.y < zoomY + ZOOM_SIZE) {
                float clickRelX = (mp.x - zoomX) / ZOOM_FACTOR;
                float clickRelY = (mp.y - zoomY) / ZOOM_FACTOR;
                auto it = charFireConfig.find(ch);
                if (it != charFireConfig.end() && !it->second.empty()) {
                    int bestIdx = 0;
                    float bestDist = 1e9f;
                    for (int i = 0; i < (int)it->second.size(); i++) {
                        float dx = it->second[i].relX - clickRelX;
                        float dy = it->second[i].relY - clickRelY;
                        float dist = dx*dx + dy*dy;
                        if (dist < bestDist) { bestDist = dist; bestIdx = i; }
                    }
                    it->second.erase(it->second.begin() + bestIdx);
                    modified = true;
                }
            }

            //  Pボタンによる弾の発射テスト
            if (IsKeyPressed(KEY_P)) {
                auto it = charFireConfig.find(ch);
                if (it != charFireConfig.end()) {
                    for (const auto& fp : it->second) {
                        float pdx, pdy;
                        GetFireVelocity(fp.direction, 200.0f, pdx, pdy);
                        PreviewBullet pb;
                        pb.x  = zoomX + fp.relX * ZOOM_FACTOR + ZOOM_FACTOR / 2.0f;
                        pb.y  = zoomY + fp.relY * ZOOM_FACTOR + ZOOM_FACTOR / 2.0f;
                        pb.dx = pdx;
                        pb.dy = pdy;
                        pb.active = true;
                        previewBullets.push_back(pb);
                    }
                }
            }
        }

        // Qボタンによるセーブ
        if (IsKeyPressed(KEY_Q)) {
            SaveCharFireConfig();
            modified = false;
        }

        // 発射テストの弾の移動と画面外判定
        for (auto& pb : previewBullets) {
            if (!pb.active) continue;
            pb.x += pb.dx * dt;
            pb.y += pb.dy * dt;
            if (pb.x < 0 || pb.x > WIN_W || pb.y < 0 || pb.y > WIN_H)
                pb.active = false;
        }
        previewBullets.erase(
            std::remove_if(previewBullets.begin(), previewBullets.end(),
                [](const PreviewBullet& b){ return !b.active; }),
            previewBullets.end());

        // 描画処理を始める
        BeginDrawing();
        ClearBackground(Color{25, 25, 25, 255});

        // 左パネルの背景、枠線、文字
        DrawRectangle(0, 0, CHAR_LIST_W, WIN_H, Color{35, 35, 35, 255});
        DrawRectangleLines(0, 0, CHAR_LIST_W, WIN_H, DARKGRAY);
        DrawText("Select character", 6, 6, 14, GRAY);

        // 描画処理を行う
        for (int i = 0; i < (int)chars.size(); i++) {
            int row = i / CHARS_PER_COL;
            int col = i % CHARS_PER_COL;
            int bx = col * CHAR_COL_W;
            int by = row * CHAR_ITEM_H - charListScrollY;
            if (by + CHAR_ITEM_H < 0 || by > WIN_H) continue; // 画面に表示されていない所は描画しない

            // 自分が今選択している文字の背景色を青色にし、そうでない文字の背景色を暗いグレー色にする
            Color bg = (i == selectedIdx) ? Color{60, 100, 200, 255} : Color{45, 45, 45, 255};
            DrawRectangle(bx, by, CHAR_COL_W - 2, CHAR_ITEM_H - 2, bg);

            // 文字の画像データが存在すれば、ゲーム本編のスケール設定を読み込む
            if (chars[i].tex.id != 0) {
                float maxSz = (float)(CHAR_COL_W - 8);
                CharScaleEntry cse = GetCharScale("red", chars[i].name);
                float drawW = maxSz * cse.sx;
                float drawH = maxSz * cse.sy;
                float dx2 = bx + (CHAR_COL_W - drawW) * 0.5f;
                float dy2 = by + (CHAR_ITEM_H - drawH) * 0.5f;
                Rectangle src = {0, 0, (float)chars[i].tex.width, (float)chars[i].tex.height};
                Rectangle dst = {dx2, dy2, drawW, drawH};
                DrawTexturePro(chars[i].tex, src, dst, {0,0}, 0, WHITE);
            }

            // その文字に発射ポイントが一つ以上設定できていれば、リストの右上にオレンジ色の丸印を追加し、設定済みを示す
            auto it = charFireConfig.find(chars[i].name);
            if (it != charFireConfig.end() && !it->second.empty()) {
                DrawCircle(bx + CHAR_COL_W - 8, by + 8, 4, ORANGE);
            }
        }

        // 中央に暗いグレー色の背景を描く
        DrawRectangle(zoomX, zoomY, ZOOM_SIZE, ZOOM_SIZE, Color{40, 40, 40, 255});

        // ひらがなが選択されていれば、そのひらがな画像を八倍のサイズに拡大して描画する
        if (selectedIdx >= 0 && chars[selectedIdx].tex.id != 0) {
            CharScaleEntry cse = GetCharScale("red", chars[selectedIdx].name);
            float drawW = ZOOM_SIZE * cse.sx;
            float drawH = ZOOM_SIZE * cse.sy;
            float drawX = (float)zoomX + (ZOOM_SIZE - drawW) * 0.5f;
            float drawY = (float)zoomY + (ZOOM_SIZE - drawH) * 0.5f;
            Rectangle src = {0, 0,
                (float)chars[selectedIdx].tex.width,
                (float)chars[selectedIdx].tex.height};
            Rectangle dst = {drawX, drawY, drawW, drawH};
            DrawTexturePro(chars[selectedIdx].tex, src, dst, {0,0}, 0, WHITE);
        } else if (selectedIdx < 0) {
            DrawText("Select a character", zoomX + ZOOM_SIZE/2 - 80, WIN_H/2 - 10, 18, GRAY);
        }

        // 拡大画面上の8ピクセルごとに縦線と横線を引く
        for (int x = 0; x <= CELL_SIZE; x += 8) {
            Color gc = (x == 0 || x == CELL_SIZE) ? GRAY : Color{60, 60, 60, 255};
            DrawLineEx({(float)(zoomX + x * ZOOM_FACTOR), (float)zoomY},
                       {(float)(zoomX + x * ZOOM_FACTOR), (float)(zoomY + ZOOM_SIZE)}, 1, gc);
        }
        for (int y = 0; y <= CELL_SIZE; y += 8) {
            Color gc = (y == 0 || y == CELL_SIZE) ? GRAY : Color{60, 60, 60, 255};
            DrawLineEx({(float)zoomX, (float)(zoomY + y * ZOOM_FACTOR)},
                       {(float)(zoomX + ZOOM_SIZE), (float)(zoomY + y * ZOOM_FACTOR)}, 1, gc);
        }
        DrawRectangleLinesEx({(float)zoomX, (float)zoomY, (float)ZOOM_SIZE, (float)ZOOM_SIZE}, 2, GRAY);

        // 弾の発射座標を設定する
        // 8×8ピクセルのマスの中央に発射座標を置き、そこに黄色い丸と赤い矢印と番号を重ねて描画する
        if (selectedIdx >= 0) {
            const std::string& ch = chars[selectedIdx].name;
            auto it = charFireConfig.find(ch);
            if (it != charFireConfig.end()) {
                for (int i = 0; i < (int)it->second.size(); i++) {
                    const auto& fp = it->second[i];
                    float cx2 = zoomX + fp.relX * ZOOM_FACTOR + ZOOM_FACTOR / 2.0f;
                    float cy2 = zoomY + fp.relY * ZOOM_FACTOR + ZOOM_FACTOR / 2.0f;
                    DrawCircle((int)cx2, (int)cy2, 6, YELLOW);
                    DrawDirArrow(cx2, cy2, fp.direction, 18.0f, RED);
                    DrawText(std::to_string(i + 1).c_str(), (int)cx2 + 8, (int)cy2 - 8, 12, WHITE);
                }
            }
        }

        // マウスカーソルが拡大画面の中にあるか確認し、あればカーソルの座標を計算する
        // カーソルが乗っている8×8ピクセルに、半透明の白色で四角い枠線を描画する
        if (mp.x >= zoomX && mp.x < zoomX + ZOOM_SIZE &&
            mp.y >= zoomY && mp.y < zoomY + ZOOM_SIZE) {
            int relX = (int)((mp.x - zoomX) / ZOOM_FACTOR);
            int relY = (int)((mp.y - zoomY) / ZOOM_FACTOR);
            DrawRectangleLinesEx({
                (float)(zoomX + relX * ZOOM_FACTOR),
                (float)(zoomY + relY * ZOOM_FACTOR),
                (float)ZOOM_FACTOR, (float)ZOOM_FACTOR
            }, 2, Fade(WHITE, 0.5f));
            // マスの中心座標を計算し、半透明の黄色でプレビューの矢印を描画する
            float px2 = zoomX + relX * ZOOM_FACTOR + ZOOM_FACTOR / 2.0f;
            float py2 = zoomY + relY * ZOOM_FACTOR + ZOOM_FACTOR / 2.0f;
            DrawDirArrow(px2, py2, currentDir, 20.0f, Fade(YELLOW, 0.7f));
        }

        // テスト弾の描画
        for (const auto& pb : previewBullets) {
            DrawCircle((int)pb.x, (int)pb.y, 5, RED);
        }

        // 右パネルの背景として、暗いグレー色の四角形とタイトルを描画する
        int px = CHAR_LIST_W + ZOOM_SIZE + 10;
        DrawRectangle(CHAR_LIST_W + ZOOM_SIZE, 0, INFO_W, WIN_H, Color{30, 30, 30, 255});

        DrawText("Fire Point Editor", px, 10, 18, WHITE);

        // 選択された文字の名前を表示し、設定済みの発射座標とその番号と方向を箇条書きで表示する
        if (selectedIdx >= 0) {
            DrawText(TextFormat("Selected: %s", chars[selectedIdx].name.c_str()), px, 40, 16, YELLOW);

            const std::string& ch = chars[selectedIdx].name;
            auto it = charFireConfig.find(ch);
            int pointCount = (it != charFireConfig.end()) ? (int)it->second.size() : 0;
            DrawText(TextFormat("Fire points: %d", pointCount), px, 65, 15, LIGHTGRAY);

            if (it != charFireConfig.end()) {
                for (int i = 0; i < (int)it->second.size() && i < 12; i++) {
                    const auto& fp = it->second[i];
                    DrawText(TextFormat("%d: (%d,%d) %s", i+1, fp.relX, fp.relY, FIRE_DIR_NAMES[fp.direction]),
                             px, 90 + i * 18, 13, WHITE);
                }
            }
        }

        // 設定しようとしている弾の方向を、オレンジ色の文字で表す
        int dirY = WIN_H - 250;
        DrawText("Direction (wheel/left/right):", px, dirY, 14, GRAY);
        DrawText(FIRE_DIR_NAMES[currentDir], px, dirY + 20, 20, ORANGE);

        DrawText("All directions:", px, dirY + 50, 13, GRAY);
        for (int d = 0; d < 7; d++) {
            Color c = (d == currentDir) ? ORANGE : DARKGRAY;
            DrawText(TextFormat("%d:%s", d, FIRE_DIR_NAMES[d]),
                     px, dirY + 70 + d * 18, 13, c);
        }

        // 操作方法を表示する
        int helpY = WIN_H - 120;
        DrawText("Left click: add fire point",   px, helpY,      12, LIGHTGRAY);
        DrawText("Right click: remove nearest",  px, helpY + 16, 12, LIGHTGRAY);
        DrawText("P: preview bullets",           px, helpY + 32, 12, LIGHTGRAY);
        DrawText("Q: save",                      px, helpY + 48, 12, LIGHTGRAY);
        DrawText("ESC: quit",                    px, helpY + 64, 12, LIGHTGRAY);

        if (modified)
            DrawText("Unsaved changes (press Q to save)", px, WIN_H - 30, 14, YELLOW);
        else
            DrawText("Saved", px, WIN_H - 30, 14, GREEN);

        EndDrawing();
    }

    // ウィンドウを閉じる際にすべての画像をメモリから消去してから終了する
    for (auto& pair : redSpriteTextures)
        if (pair.second.id != 0) UnloadTexture(pair.second);
    CloseWindow();
    return 0;
}
