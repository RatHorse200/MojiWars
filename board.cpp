// ゲーム盤面の状態管理、ローマ字入力の判定、文字の配置ルールの制御を行うファイル
#include "board.h"
#include "romaji.h"

// 入力されたローマ字を処理して、文字を配置する関数
bool GameBoard::ProcessInput(char c) {
    if (cursorX < 0 || cursorY < 0) return false;

    inputBuffer += c;

    // 文字がローマ字変換表に載っているかを確認する
    if (romaji_to_canonical.find(inputBuffer) != romaji_to_canonical.end()) {
        const std::string& romaji  = romaji_to_canonical[inputBuffer];
        const std::string& oldChar = cells[cursorY][cursorX];

        // ルール１：同じ文字の再入力について
        if (oldChar == romaji) {
            SetCell(romaji);
            inputBuffer = "";
            return false;
        }

        // ルール２：文字数の上限について
        if (oldChar.empty() && CharCount() >= maxChars) {
            inputBuffer = "";
            return false;
        }

        // ルール３：重複文字の上限について
        if (CharCountOf(romaji, cursorX, cursorY) >= maxDuplicates) {
            inputBuffer = "";
            return false;
        }

        // すべてのルールをクリアしたので、文字を配置する
        SetCell(romaji);
        inputBuffer = "";
        return true;
    }

    // ローマ字が完成していないときの処理
    if (!IsPrefixOfRomaji(inputBuffer)) {
        inputBuffer = "";
    }
    return false;
}
