// 文字画像の読み込み・透過加工・画面描画など、グラフィック処理全般の関数の目次ファイル
#pragma once
#include "raylib.h"
#include <string>
#include <map>

struct CharScaleEntry { float sx = 1.0f, sy = 1.0f; };

// 一度読み込んだ文字画像を文字の名前とセットで記憶する辞書
extern std::map<std::string, Texture2D> redSpriteTextures;
extern std::map<std::string, Texture2D> blackSpriteTextures;
extern std::map<std::string, CharScaleEntry> charScaleConfig;

extern const int ALPHA_THRESHOLD;

// フォルダパスとローマ字の名前に.pngを付け足して画像を読み込む関数
Image          LoadImageAuto(const std::string& basePath);
// 読み込んだ画像データの白い背景を透明にする関数
void           ApplyAlphaThreshold(Image& img, int threshold);
// assets/char_scale.txtを開く関数
void           LoadCharScaleConfig();
// すべての画像のファイル名をリストアップして配列にまとめる
void           PreloadAllSprites();
// ローマ字のファイル名からassets/red_ch/の画像を読み込む関数
CharScaleEntry GetCharScale(const std::string& folder, const std::string& ch);

// 赤文字の画像データを取得する関数
Texture2D GetRedSpriteTexture(const std::string& hiragana);
// 黒文字の画像データを取得する関数
Texture2D GetBlackSpriteTexture(const std::string& hiragana);

// ローマ字が拗音や促音かどうか判定する関数
bool IsSmallKana(const std::string& ch);

// 画像をマスの中に描画する関数
void DrawTextureInCell(Texture2D tex, int cellX, int cellY, int cellSize, Color tint, bool small = false, float scaleX = 1.0f, float scaleY = 1.0f);

// 赤文字の画像をマスの中に描画する関数
void DrawRedCellContent(const std::string& hiragana, int x, int y, int cellSize);

// 黒文字の画像をマスの中に描画する関数
void DrawBlackCellContent(const std::string& hiragana, int x, int y, int cellSize, bool highlight = false);

// 黒文字の画像を好きな色でマスの中に描画する関数
void DrawBlackCellContentColored(const std::string& hiragana, int x, int y, int cellSize, Color tint);
