// 画面サイズ、マスの大きさ、ゲームの状態をゲーム全体で共有する定数をまとめたファイル
#pragma once

const int SCREEN_COLS = 19;     // 画面の横方向のセル数
const int SCREEN_ROWS = 14;     // 画面の縦方向のセル数
const int CELL_SIZE = 64;       // セルの大きさ
const int SCREEN_WIDTH = SCREEN_COLS * CELL_SIZE;  // ゲームウィンドウの幅 1216
const int SCREEN_HEIGHT = SCREEN_ROWS * CELL_SIZE;  // ゲームウィンドウの高さ 896

const int GRID_SIZE = 5;    // フィールドのサイズ
const int BOARD_LINE_WIDTH = 2;     // フィールドの線の太さ     
const float SPRITE_SCALE = 1.0f;    // 文字画像の拡大率

// タイトル画面とチュートリアル画面もゲームプレイ画面と同じセルサイズで統一する
const int TITLE_CELL_SIZE = CELL_SIZE;
const int TITLE_GRID_COLS = SCREEN_COLS;
const int TITLE_GRID_ROWS = SCREEN_ROWS;
const int TUTO_CELL_SIZE = CELL_SIZE;

// ゲームがどの画面にいるかを管理する
enum GameState {
    STATE_TITLE,
    STATE_TUTORIAL,
    STATE_DIFFICULTY,
    STATE_PLAYING,
    STATE_GAMEOVER
};

// タイトル画面でプレイヤーがどのボタンを触っているか
enum MenuSelection { MENU_NONE, MENU_START, MENU_END };

// ゲームオーバー画面でプレイヤーがどのボタンを触っているか
enum GameOverMenu {
    GOMENU_NONE,
    GOMENU_RETRY,    // このステージをやり直す
    GOMENU_RESTART,  // ステージ1から再挑戦する
    GOMENU_END
};
