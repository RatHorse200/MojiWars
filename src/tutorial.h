// チュートリアル専用の入力ボックス構造体と、画面描画・進行管理関数の目次ファイル
#pragma once
#include "raylib.h"
#include "constants.h"
#include <string>
#include <vector>

struct TutBox {
    int x, y, size;
    std::string buffer;        // 入力中のローマ字
    std::string content;       // 確定したひらがな
    float fireTimer = 0.0f;
};

// main.cpp で定義
extern int    tutorialPage;
extern TutBox tutBoxes[3];
extern int    tutBoxCount;
extern int    tutSelectedBox;

// チュートリアルのページをセットアップする関数
void SetupTutorialPage(int page);
// カーソルの座標が入力ボックスのどれかの上に乗っているかを判定する関数
bool IsInAnyTutBox(Vector2 pos);
// 入力ボックスを描画する関数
void DrawTutBox(int idx);
// チュートリアル画面の振り分けを行う関数
void DrawTutorialScreen();

// 画面上にゆみ、けん、たてのデモ敵をスポーンさせる関数
void TutorialRespawnEnemies();
// チュートリアルのページ3の画面上にゆみ、けん、たてのデモ敵をスポーンさせる関数
void TutorialPage3RespawnEnemies();
// チュートリアル用の敵更新と、状況のセットアップを行う関数
void TutorialEnemyUpdate(float dt, int boardOffsetX, int boardOffsetY);
// チュートリアル2ページ目にて、入力ボックスと弾の当たり判定を行う関数
void CheckBulletsVsTutBoxes();
