// 文字画像の読み込み・透過加工・サイズ調整・画面への描画など、グラフィック処理全般を管理するファイル
#include "sprites.h"
#include "constants.h"
#include "raylib.h"
#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <cstring>

// 一度読み込んだ文字画像を文字の名前とセットで記憶する辞書
// キャッシュの役割を果たし、ゲームの動作を軽くする
std::map<std::string, Texture2D> redSpriteTextures;
std::map<std::string, Texture2D> blackSpriteTextures;
std::map<std::string, CharScaleEntry> charScaleConfig;

// 値が大きいほど線が太く残る
const int ALPHA_THRESHOLD = 240;

// フォルダパスとローマ字の名前に.pngを付け足して画像を読み込む関数
Image LoadImageAuto(const std::string& basePath) {
    std::string png = basePath + ".png";
    if (FileExists(png.c_str())) return LoadImage(png.c_str());
    Image empty = { 0 };
    return empty;
}

// 読み込んだ画像データの白い背景を透明にする関数
// まず画像をRGBA（赤・緑・青・透明度）という形式に統一し、すべての点を調べて点の平均の明るさが240以上なら背景の白と判定し、透明で塗る
void ApplyAlphaThreshold(Image& img, int threshold) {
    ImageFormat(&img, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    Color* pixels = (Color*)img.data;
    int count = img.width * img.height;
    for (int i = 0; i < count; i++) {
        int brightness = (pixels[i].r + pixels[i].g + pixels[i].b) / 3;
        if (brightness >= threshold) {
            pixels[i] = BLANK;
        } else {
            pixels[i].a = 255;
        }
    }
}

// assets/char_scale.txtを開く関数
void LoadCharScaleConfig() {
    std::ifstream f("assets/char_scale.txt");
    if (!f.is_open()) return;
    std::string line;
    while (std::getline(f, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream ss(line);
        std::string key; float sx, sy;
        if (!(ss >> key >> sx)) continue;
        if (!(ss >> sy)) sy = sx;
        charScaleConfig[key] = {sx, sy};
    }
}

// char_scale.txtから文字の倍率を取り出す関数
CharScaleEntry GetCharScale(const std::string& folder, const std::string& ch) {
    std::string key = folder + "/" + ch;
    auto it = charScaleConfig.find(key);
    return (it != charScaleConfig.end()) ? it->second : CharScaleEntry{1.0f, 1.0f};
}

// ローマ字のファイル名からassets/red_ch/の画像を読み込む関数
static Texture2D LoadRedSprite(const std::string& romaji) {
    Image img = LoadImageAuto("assets/red_ch/" + romaji);
    if (img.data == nullptr) {
        Texture2D empty = { 0 };
        return empty;
    }
    ImageResize(&img, 256, 256);
    ApplyAlphaThreshold(img, ALPHA_THRESHOLD);
    ImageAlphaCrop(&img, 0.05f);
    Texture2D tex = LoadTextureFromImage(img);
    UnloadImage(img);
    return tex;
}

// ローマ字のファイル名からassets/black_ch/の画像を読み込む関数
static Texture2D LoadBlackSprite(const std::string& romaji) {
    Image img = LoadImageAuto("assets/black_ch/" + romaji);
    if (img.data == nullptr) {
        Texture2D empty = { 0 };
        return empty;
    }
    ImageResize(&img, 256, 256);
    ApplyAlphaThreshold(img, ALPHA_THRESHOLD);
    // 不透明の点を白色にする
    ImageFormat(&img, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    Color* pixels = (Color*)img.data;
    int count = img.width * img.height;
    for (int i = 0; i < count; i++) {
        if (pixels[i].a > 0) {
            pixels[i] = Color{255, 255, 255, 255};
        }
    }
    ImageAlphaCrop(&img, 0.05f);
    Texture2D tex = LoadTextureFromImage(img);
    UnloadImage(img);
    return tex;
}

// すべての画像のファイル名をリストアップして配列にまとめる
void PreloadAllSprites() {
    static const char* allKana[] = {
        "a","i","u","e","o",
        "ka","ki","ku","ke","ko","ga","gi","gu","ge","go",
        "sa","shi","su","se","so","za","ji","zu","ze","zo",
        "ta","chi","tsu","te","to","da","di","du","de","do",
        "na","ni","nu","ne","no",
        "ha","hi","fu","he","ho","ba","bi","bu","be","bo","pa","pi","pu","pe","po",
        "ma","mi","mu","me","mo",
        "ya","yu","yo",
        "ra","ri","ru","re","ro",
        "wa","wo","n",
    };
    static const char* blackOnly[] = {
        "xtu","xa","xi","xu","xe","xo","xya","xyu","xyo",
        "chouon","ellipsis","arrow_down","exclamation","migi","hidari",
        "0","1","2","3","4","5","6","7","8","9",
    };
    for (auto k : allKana) {
        GetRedSpriteTexture(k);
        GetBlackSpriteTexture(k);
    }
    for (auto k : blackOnly) {
        GetBlackSpriteTexture(k);
    }
}

// 赤文字の画像データを取得する関数
Texture2D GetRedSpriteTexture(const std::string& romaji) {
    if (romaji.empty()) {
        Texture2D empty = { 0 };
        return empty;
    }
    if (redSpriteTextures.find(romaji) == redSpriteTextures.end()) {
        redSpriteTextures[romaji] = LoadRedSprite(romaji);
    }
    return redSpriteTextures[romaji];
}

// 黒文字の画像データを取得する関数
Texture2D GetBlackSpriteTexture(const std::string& romaji) {
    if (romaji.empty()) {
        Texture2D empty = { 0 };
        return empty;
    }
    if (blackSpriteTextures.find(romaji) == blackSpriteTextures.end()) {
        blackSpriteTextures[romaji] = LoadBlackSprite(romaji);
    }
    return blackSpriteTextures[romaji];
}

// ローマ字が拗音や促音かどうか判定する関数
bool IsSmallKana(const std::string& ch) {
    static const char* smallList[] = {
        "xtu", "xa", "xi", "xu", "xe", "xo",
        "xya", "xyu", "xyo", nullptr
    };
    for (int i = 0; smallList[i]; i++) {
        if (ch == smallList[i]) return true;
    }
    return false;
}

// 画像をマスの中に描画する関数
void DrawTextureInCell(Texture2D tex, int cellX, int cellY, int cellSize, Color tint, bool small, float scaleX, float scaleY) {
    Rectangle src = { 0, 0, (float)tex.width, (float)tex.height };
    Rectangle dst;
    float totalScaleX = SPRITE_SCALE * scaleX;
    float totalScaleY = SPRITE_SCALE * scaleY;

    // 拗音や促音をマスの左下寄りに配置する
    if (small) {
        float halfW = cellSize * 0.5f * totalScaleX;
        float halfH = cellSize * 0.5f * totalScaleY;
        float offsetX = cellSize * 0.1f;
        float offsetY = cellSize * 0.1f;
        dst = { (float)cellX + offsetX, (float)(cellY + cellSize) - halfH - offsetY, halfW, halfH };
    } else {
        float drawW = cellSize * totalScaleX;
        float drawH = cellSize * totalScaleY;
        float drawX = cellX + (cellSize - drawW) * 0.5f;
        float drawY = cellY + (cellSize - drawH) * 0.5f;
        dst = { drawX, drawY, drawW, drawH };
    }

    DrawTexturePro(tex, src, dst, { 0, 0 }, 0.0f, tint);
}

// 赤文字の画像をマスの中に描画する関数
void DrawRedCellContent(const std::string& romaji, int x, int y, int cellSize) {
    if (romaji.empty()) return;
    Texture2D tex = GetRedSpriteTexture(romaji);
    if (tex.id != 0) {
        auto cse = GetCharScale("red", romaji);
        DrawTextureInCell(tex, x, y, cellSize, WHITE, IsSmallKana(romaji), cse.sx, cse.sy);
    }
}

// 黒文字の画像をマスの中に描画する関数
void DrawBlackCellContent(const std::string& romaji, int x, int y, int cellSize, bool highlight) {
    if (romaji.empty()) return;
    Texture2D tex = GetBlackSpriteTexture(romaji);
    if (tex.id != 0) {
        Color tint = highlight ? RED : BLACK;
        auto cse = GetCharScale("black", romaji);
        DrawTextureInCell(tex, x, y, cellSize, tint, IsSmallKana(romaji), cse.sx, cse.sy);
    }
}

// 黒文字の画像を好きな色でマスの中に描画する関数
void DrawBlackCellContentColored(const std::string& romaji, int x, int y, int cellSize, Color tint) {
    if (romaji.empty()) return;
    Texture2D tex = GetBlackSpriteTexture(romaji);
    if (tex.id != 0) {
        auto cse = GetCharScale("black", romaji);
        DrawTextureInCell(tex, x, y, cellSize, tint, IsSmallKana(romaji), cse.sx, cse.sy);
    }
}
