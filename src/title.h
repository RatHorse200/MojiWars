// タイトル、難易度選択、ゲーム本編、ゲームオーバーなど、ゲームの各シーンの画面描画とメニューの当たり判定を管理する目次ファイル
#pragma once
#include "raylib.h"
#include "constants.h"
#include <string>

struct TitleSprite {
    int gridX;
    int gridY;
    std::string romaji;
    bool active;
};

const int maxTitleSprites = 50;
extern TitleSprite titleSprites[maxTitleSprites];
extern int titleSpriteCount;

extern MenuSelection  hoveredMenu;
extern GameOverMenu   hoveredGameOverMenu;

// ステージ管理
extern int  currentStage;
extern int  enemiesRemaining;
extern bool stageClearShowing;
extern bool gameAllCleared;
extern bool stageStartPending;

struct TitleLayout {
    int cols, rows;
    int titleX, titleY;
    int startX, startY;
    int endX,   endY;
};

TitleLayout    CalcTitleLayout();

// フィールドの枠線をHPゲージとして描画する関数
void DrawFieldBorderHP(int offsetX, int offsetY, int boardWidth, int fieldHP, float lineW);
// 白い背景と等間隔の縦線と横線を引いて碁盤を描画する関数
void DrawGobanBackground(int offsetX, int offsetY, int cols, int rows, int cellSize);
// タイトル画面の画像を描画する関数
void DrawTitleScreen();
// 背景の碁盤の目を描画する関数
void DrawGameScreen();
// ゲームオーバー画面を描画する関数
void DrawGameOverScreen();

// どのボタンの上にマウスがあるかを判定する関数
MenuSelection  GetMenuAtPosition(Vector2 mousePos);
// ゲームオーバー画面のボタンの当たり判定を定義する関数
GameOverMenu   GetGameOverMenuAtPosition(Vector2 mousePos);

// タイトル画面の背景に文字を置くための関数
void AddTitleSprite(int gridX, int gridY, const std::string& romaji);
// タイトル画面に"げーむ"を描画する関数
void SetupTitleSprites();

enum DifficultyMenu { DIFFMENU_NONE, DIFFMENU_EASY, DIFFMENU_HARD };
extern DifficultyMenu hoveredDifficultyMenu;
// 難易度選択画面を描画する関数
void DrawDifficultyScreen();
// 難易度選択画面でのマウスクリック判定を行う関数
DifficultyMenu GetDifficultyMenuAtPosition(Vector2 mousePos);
