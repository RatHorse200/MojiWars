// ゲームフィールドの状態を管理するデータ構造と、操作関数の目次ファイル
#pragma once
#include "constants.h"
#include "raylib.h"
#include <string>

// ゲームフィールドの状態管理
struct GameBoard {
    std::string cells[GRID_SIZE][GRID_SIZE];
    int cursorX;
    int cursorY;
    std::string inputBuffer;

    // プレイヤーのHP
    int fieldHP = 100;

    // 難易度によって数値は変わる
    int maxChars     = 5;  // 文字数の上限
    int maxDuplicates = 1; // 同じ文字の重複上限

    // 置かれた文字が弾を撃つまでの待ち時間を計る
    float cellFireTimers[GRID_SIZE][GRID_SIZE];

    // 現在のHPに応じてフィールドの枠の色を変える関数
    Color GetFieldBorderColor() const {
        if (fieldHP >= 50) return GREEN;
        if (fieldHP >= 20) return YELLOW;
        return RED;
    }

    // フィールドに何文字置かれているかを数える関数
    int CharCount() const {
        int count = 0;
        for (int y = 0; y < GRID_SIZE; y++)
            for (int x = 0; x < GRID_SIZE; x++)
                if (!cells[y][x].empty()) count++;
        return count;
    }

    // 指定した特定の文字が盤面にいくつあるかを数える関数
    int CharCountOf(const std::string& ch, int exX = -1, int exY = -1) const {
        int count = 0;
        for (int y = 0; y < GRID_SIZE; y++)
            for (int x = 0; x < GRID_SIZE; x++)
                if (!(x == exX && y == exY) && cells[y][x] == ch) count++;
        return count;
    }

    // ゲーム開始時にフィールドを初期化する関数
    void Init() {
        for (int y = 0; y < GRID_SIZE; y++)
            for (int x = 0; x < GRID_SIZE; x++) {
                cells[y][x] = "";
                cellFireTimers[y][x] = 0.0f;
            }
        cursorX = -1;
        cursorY = -1;
        inputBuffer = "";
        fieldHP = 100;
        maxChars      = 5;
        maxDuplicates = 1;
    }

    // カーソルを移動する関数
    void SetCursor(int x, int y) {
        if (x >= 0 && x < GRID_SIZE && y >= 0 && y < GRID_SIZE) {
            cursorX = x;
            cursorY = y;
            inputBuffer = "";
        }
    }

    // カーソルが合っているマスに文字を置く関数
    void SetCell(const std::string& romaji) {
        if (cursorX >= 0 && cursorY >= 0)
            cells[cursorY][cursorX] = romaji;
    }

    // カーソルが合っているマスを空にする関数
    void ClearCell() {
        if (cursorX >= 0 && cursorY >= 0)
            cells[cursorY][cursorX] = "";
    }

    // 入力されたローマ字を処理して、文字を配置する関数（関数の中身はboard.cppに実装）
    bool ProcessInput(char c);
};

// ゲーム本体とエディタで共有する設定
extern GameBoard gameBoard;
