// タイトル、難易度選択、ゲーム本編、ゲームオーバーなど、ゲームの各シーンの画面描画とメニューの当たり判定を統括するファイル
#include "title.h"
#include "sprites.h"
#include "board.h"
#include "enemy.h"
#include "constants.h"
#include "raylib.h"
#include <cmath>
#include <string>
#include <algorithm>

TitleSprite titleSprites[maxTitleSprites];
int titleSpriteCount = 0;

// 白い背景と等間隔の縦線と横線を引いて碁盤を描画する関数
// 碁盤の色は白色、線の色は黒色
void DrawGobanBackground(int offsetX, int offsetY, int cols, int rows, int cellSize) {
    Color boardColor = RAYWHITE;
    Color lineColor = BLACK;

    int boardWidth = cols * cellSize;
    int boardHeight = rows * cellSize;

    DrawRectangle(offsetX, offsetY, boardWidth, boardHeight, boardColor);

    for (int x = 0; x <= cols; x++) {
        DrawLineEx(
            { (float)(offsetX + x * cellSize), (float)offsetY },
            { (float)(offsetX + x * cellSize), (float)(offsetY + boardHeight) },
            BOARD_LINE_WIDTH, lineColor
        );
    }

    for (int y = 0; y <= rows; y++) {
        DrawLineEx(
            { (float)offsetX, (float)(offsetY + y * cellSize) },
            { (float)(offsetX + boardWidth), (float)(offsetY + y * cellSize) },
            BOARD_LINE_WIDTH, lineColor
        );
    }
}

// 画面全体で必要なマス数を計算する関数
TitleLayout CalcTitleLayout() {
    TitleLayout L;
    L.cols = SCREEN_WIDTH  / TITLE_CELL_SIZE + 1;
    L.rows = SCREEN_HEIGHT / TITLE_CELL_SIZE + 1;

    // もじうぉーず（タイトル）の座標
    L.titleX = ((SCREEN_WIDTH  - 6 * TITLE_CELL_SIZE) / 2 / TITLE_CELL_SIZE) * TITLE_CELL_SIZE;
    L.titleY = (SCREEN_HEIGHT / 3 / TITLE_CELL_SIZE) * TITLE_CELL_SIZE;

    // はじめるボタンの座標
    L.startX = ((SCREEN_WIDTH  - 4 * TITLE_CELL_SIZE) / 2 / TITLE_CELL_SIZE) * TITLE_CELL_SIZE;
    L.startY = (SCREEN_HEIGHT / 2 / TITLE_CELL_SIZE) * TITLE_CELL_SIZE - TITLE_CELL_SIZE;

    // おわるボタンの座標
    L.endX   = ((SCREEN_WIDTH  - 3 * TITLE_CELL_SIZE) / 2 / TITLE_CELL_SIZE) * TITLE_CELL_SIZE - TITLE_CELL_SIZE;
    L.endY   = (SCREEN_HEIGHT / 2 / TITLE_CELL_SIZE) * TITLE_CELL_SIZE;

    return L;
}

// タイトル画面の画像を描画する関数
void DrawTitleScreen() {
    ClearBackground(RAYWHITE);

    TitleLayout L = CalcTitleLayout();
    DrawGobanBackground(0, 0, L.cols, L.rows, TITLE_CELL_SIZE);

    // もじうぉーず
    const std::string title[] = {"mo","ji","u","xo","chouon","zu"};
    for (int i = 0; i < 6; i++)
        DrawBlackCellContent(title[i], L.titleX + i * TITLE_CELL_SIZE, L.titleY, TITLE_CELL_SIZE);

    // はじめる
    const std::string startText[] = {"ha","ji","me","ru"};
    bool highlightStart = (hoveredMenu == MENU_START);
    for (int i = 0; i < 4; i++)
        DrawBlackCellContent(startText[i], L.startX + i * TITLE_CELL_SIZE, L.startY, TITLE_CELL_SIZE, highlightStart);

    // おわる
    const std::string endText[] = {"o","wa","ru"};
    bool highlightEnd = (hoveredMenu == MENU_END);
    for (int i = 0; i < 3; i++)
        DrawBlackCellContent(endText[i], L.endX + i * TITLE_CELL_SIZE, L.endY, TITLE_CELL_SIZE, highlightEnd);

    // ナビゲーション
    int hintY1 = L.endY + 2 * TITLE_CELL_SIZE;
    int hintY2 = L.endY + 3 * TITLE_CELL_SIZE;
    int hintX = ((SCREEN_WIDTH - 13 * TITLE_CELL_SIZE) / 2 / TITLE_CELL_SIZE) * TITLE_CELL_SIZE;

    const std::string hint1[] = {"hidari","ku","ri","xtu","ku","de","ga","me","n","ga","su","su","mu"};
    const std::string hint2[] = {"migi","ku","ri","xtu","ku","de","ga","me","n","ga","mo","do","ru"};
    for (int i = 0; i < 13; i++) {
        DrawBlackCellContent(hint1[i], hintX + i * TITLE_CELL_SIZE, hintY1, TITLE_CELL_SIZE);
        DrawBlackCellContent(hint2[i], hintX + i * TITLE_CELL_SIZE, hintY2, TITLE_CELL_SIZE);
    }
}

// どのボタンの上にマウスがあるかを判定する関数
MenuSelection GetMenuAtPosition(Vector2 mousePos) {
    TitleLayout L = CalcTitleLayout();

    Rectangle startRect = {
        (float)L.startX, (float)L.startY,
        (float)(4 * TITLE_CELL_SIZE), (float)TITLE_CELL_SIZE
    };
    if (CheckCollisionPointRec(mousePos, startRect)) return MENU_START;

    Rectangle endRect = {
        (float)L.endX, (float)L.endY,
        (float)(3 * TITLE_CELL_SIZE), (float)TITLE_CELL_SIZE
    };
    if (CheckCollisionPointRec(mousePos, endRect)) return MENU_END;

    return MENU_NONE;
}

// フィールドの枠線をHPゲージとして描画する関数
void DrawFieldBorderHP(int offsetX, int offsetY, int boardWidth, int fieldHP, float lineW) {
    Color darkColor = Color{50, 50, 50, 255};
    Color litColor;
    if (fieldHP >= 50) litColor = GREEN;
    else if (fieldHP >= 20) litColor = YELLOW;
    else litColor = RED;

    float perim  = 4.0f * (float)boardWidth; // 盤面の外周の長さ
    float litLen = (fieldHP / 100.0f) * perim; // 色を塗るべき枠線の合計の長さ

    float x0 = (float)offsetX,              y0 = (float)offsetY;
    float x1 = (float)(offsetX + boardWidth), y1 = (float)(offsetY + boardWidth);

    // 時計回りに線を引いていくための、フィールドの4つの辺のスタート座標とゴール座標
    struct Seg { float sx, sy, ex, ey; };
    Seg segs[4] = {
        {x0, y0, x1, y0},  // 上
        {x1, y0, x1, y1},  // 右
        {x1, y1, x0, y1},  // 下
        {x0, y1, x0, y0},  // 左
    };

    // どの辺にどれだけ色付きの線を引き、どれだけ黒線を引くか
    float drawn = 0.0f;
    for (int i = 0; i < 4; i++) {
        float segLen    = (float)boardWidth;
        float litInSeg  = std::min(std::max(litLen - drawn, 0.0f), segLen);
        float darkInSeg = segLen - litInSeg;

        float dirX = (segs[i].ex - segs[i].sx) / segLen;
        float dirY = (segs[i].ey - segs[i].sy) / segLen;

        if (litInSeg > 0.0f) {
            Vector2 from = {segs[i].sx, segs[i].sy};
            Vector2 to   = {segs[i].sx + dirX * litInSeg, segs[i].sy + dirY * litInSeg};
            DrawLineEx(from, to, lineW, litColor);
        }
        if (darkInSeg > 0.0f) {
            Vector2 from = {segs[i].sx + dirX * litInSeg, segs[i].sy + dirY * litInSeg};
            Vector2 to   = {segs[i].ex, segs[i].ey};
            DrawLineEx(from, to, lineW, darkColor);
        }
        drawn += segLen;
    }
}

// 背景の碁盤の目を描画する関数
void DrawGameScreen() {
    ClearBackground(RAYWHITE);

    int bgCols = SCREEN_WIDTH  / CELL_SIZE + 1;
    int bgRows = SCREEN_HEIGHT / CELL_SIZE + 1;
    DrawGobanBackground(0, 0, bgCols, bgRows, CELL_SIZE);

    int boardWidth  = GRID_SIZE * CELL_SIZE;
    int offsetX = ((SCREEN_WIDTH  - boardWidth) / 2 / CELL_SIZE) * CELL_SIZE;
    int offsetY = 2 * CELL_SIZE;

    // フィールドを描画する
    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            int cellX = offsetX + x * CELL_SIZE;
            int cellY = offsetY + y * CELL_SIZE;

            if (x == gameBoard.cursorX && y == gameBoard.cursorY)
                DrawRectangle(cellX + 2, cellY + 2, CELL_SIZE - 4, CELL_SIZE - 4, Fade(YELLOW, 0.5f));

            DrawRedCellContent(gameBoard.cells[y][x], cellX, cellY, CELL_SIZE);
        }
    }

    DrawFieldBorderHP(offsetX, offsetY, boardWidth, gameBoard.fieldHP, 8.0f);

    // 現在のカーソル位置を赤い枠で描画することで強調している
    if (gameBoard.cursorX >= 0 && gameBoard.cursorY >= 0) {
        int cursorScreenX = offsetX + gameBoard.cursorX * CELL_SIZE;
        int cursorScreenY = offsetY + gameBoard.cursorY * CELL_SIZE;
        DrawRectangleLinesEx({ (float)cursorScreenX, (float)cursorScreenY, (float)CELL_SIZE, (float)CELL_SIZE }, 3, RED);
    }

    // 敵と弾を描画する
    DrawEnemies(CELL_SIZE);

    // 画面の左上に現在のステージ数を "すてーじ（数字）" と描画する
    {
        const std::string stageLabel[] = {"su","te","chouon","ji"};
        for (int i = 0; i < 4; i++)
            DrawBlackCellContent(stageLabel[i], i * CELL_SIZE, 0, CELL_SIZE);
        DrawBlackCellContent(std::to_string(currentStage), 4 * CELL_SIZE, 0, CELL_SIZE);
    }

    // 画面の左下に "てきのこり" と描画する
    {
        const std::string tekiLabel[] = {"te","ki","no","ko","ri"};
        for (int i = 0; i < 5; i++)
            DrawBlackCellContent(tekiLabel[i], (1 + i) * CELL_SIZE, 11 * CELL_SIZE, CELL_SIZE);
    }

    // 残り数を "てきのこり" の下に "（数字）たい" と描画する
    {
        int count = enemiesRemaining;
        if (count < 0) count = 0;
        int tens  = (count / 10) % 10;
        int units = count % 10;
        DrawBlackCellContent(std::to_string(tens),  2 * CELL_SIZE, 12 * CELL_SIZE, CELL_SIZE);
        DrawBlackCellContent(std::to_string(units), 3 * CELL_SIZE, 12 * CELL_SIZE, CELL_SIZE);
        DrawBlackCellContent("ta", 4 * CELL_SIZE, 12 * CELL_SIZE, CELL_SIZE);
        DrawBlackCellContent("i", 5 * CELL_SIZE, 12 * CELL_SIZE, CELL_SIZE);
    }

    // ステージクリアとゲームクリアを描画する
    if (stageClearShowing) {
        if (gameAllCleared) {
            // 全ステージクリア
            // "くりあせいこう！"
            const std::string allClear[] = {"ku","ri","a","se","i","ko","u","exclamation"};
            for (int i = 0; i < 8; i++)
                DrawBlackCellContent(allClear[i], (6 + i) * CELL_SIZE, 7 * CELL_SIZE, CELL_SIZE);
            // "おめでとう！"
            const std::string congrats[] = {"o","me","de","to","u","exclamation"};
            for (int i = 0; i < 6; i++)
                DrawBlackCellContent(congrats[i], (7 + i) * CELL_SIZE, 8 * CELL_SIZE, CELL_SIZE);
        } else {
            // ステージクリア
            // "すてーじくりあ！"
            const std::string clearText[] = {"su","te","chouon","ji","ku","ri","a","exclamation"};
            for (int i = 0; i < 8; i++)
                DrawBlackCellContent(clearText[i], (6 + i) * CELL_SIZE, 7 * CELL_SIZE, CELL_SIZE);
            // "さきへすすむ"
            Vector2 mp = GetMousePosition();
            Rectangle continueRect = {6.0f * CELL_SIZE, 9.0f * CELL_SIZE, 6.0f * CELL_SIZE, (float)CELL_SIZE};
            bool hlContinue = CheckCollisionPointRec(mp, continueRect);
            const std::string cont[] = {"sa","ki","he","su","su","mu"};
            for (int i = 0; i < 6; i++)
                DrawBlackCellContent(cont[i], (6 + i) * CELL_SIZE, 9 * CELL_SIZE, CELL_SIZE, hlContinue);
        }
    }

    // ステージ開始前に"くりっくしてすたーと"を描画する
    if (stageStartPending) {
        int cx = 5 * CELL_SIZE;
        int cy = 7 * CELL_SIZE;
        // semi-transparent background
        DrawRectangle(cx - 4, cy - 4, 10 * CELL_SIZE + 8, CELL_SIZE + 8, Fade(RAYWHITE, 0.85f));
        const std::string startMsg[] = {
            "ku","ri","xtu","ku","shi",
            "te","su","ta","chouon","to"
        };
        for (int i = 0; i < 10; i++)
            DrawBlackCellContent(startMsg[i], cx + i * CELL_SIZE, cy, CELL_SIZE);
    }

    // 現在タイピングしている途中のローマ字を、マスに小さく表示する
    if (gameBoard.cursorX >= 0 && gameBoard.cursorY >= 0 && !gameBoard.inputBuffer.empty()) {
        int cx = offsetX + gameBoard.cursorX * CELL_SIZE;
        int cy = offsetY + gameBoard.cursorY * CELL_SIZE;
        DrawText(gameBoard.inputBuffer.c_str(), cx + 4, cy + CELL_SIZE - 22, 16, DARKBLUE);
    }
}

// ゲームオーバー画面を描画する関数
void DrawGameOverScreen() {
    ClearBackground(RAYWHITE);
    int bgCols = SCREEN_WIDTH  / CELL_SIZE + 1;
    int bgRows = SCREEN_HEIGHT / CELL_SIZE + 1;
    DrawGobanBackground(0, 0, bgCols, bgRows, CELL_SIZE);

    // "ざんねん…"
    const std::string zannen[] = {"za","n","ne","n","ellipsis"};
    for (int i = 0; i < 5; i++)
        DrawBlackCellContent(zannen[i], (7 + i) * CELL_SIZE, 7 * CELL_SIZE, CELL_SIZE);

    // "いまのすてーじをもういちど"
    const std::string retry[] = {"i","ma","no","su","te","chouon","ji","wo","mo","u","i","chi","do"};
    bool hlRetry = (hoveredGameOverMenu == GOMENU_RETRY);
    for (int i = 0; i < 13; i++)
        DrawBlackCellContent(retry[i], (3 + i) * CELL_SIZE, 9 * CELL_SIZE, CELL_SIZE, hlRetry);

    // "さいしょからやりなおす"
    const std::string restart[] = {"sa","i","shi","xyo","ka","ra","ya","ri","na","o","su"};
    bool hlRestart = (hoveredGameOverMenu == GOMENU_RESTART);
    for (int i = 0; i < 11; i++)
        DrawBlackCellContent(restart[i], (4 + i) * CELL_SIZE, 10 * CELL_SIZE, CELL_SIZE, hlRestart);

    // "たいとるがめんにもどる"
    const std::string end[] = {"ta","i","to","ru","ga","me","n","ni","mo","do","ru"};
    bool hlEnd = (hoveredGameOverMenu == GOMENU_END);
    for (int i = 0; i < 11; i++)
        DrawBlackCellContent(end[i], (4 + i) * CELL_SIZE, 11 * CELL_SIZE, CELL_SIZE, hlEnd);
}

// ゲームオーバー画面のボタンの当たり判定を定義する関数
GameOverMenu GetGameOverMenuAtPosition(Vector2 mousePos) {
    Rectangle retryRect    = {3.0f*CELL_SIZE,  9.0f*CELL_SIZE,  13.0f*CELL_SIZE, (float)CELL_SIZE};
    Rectangle restartRect  = {4.0f*CELL_SIZE,  10.0f*CELL_SIZE, 11.0f*CELL_SIZE, (float)CELL_SIZE};
    Rectangle endRect      = {4.0f*CELL_SIZE,  11.0f*CELL_SIZE, 11.0f*CELL_SIZE, (float)CELL_SIZE};

    if (CheckCollisionPointRec(mousePos, retryRect)) return GOMENU_RETRY;
    if (CheckCollisionPointRec(mousePos, restartRect)) return GOMENU_RESTART;
    if (CheckCollisionPointRec(mousePos, endRect)) return GOMENU_END;
    return GOMENU_NONE;
}

// タイトル画面の背景に文字を置くための関数
void AddTitleSprite(int gridX, int gridY, const std::string& romaji) {
    if (titleSpriteCount < maxTitleSprites) {
        titleSprites[titleSpriteCount].gridX = gridX;
        titleSprites[titleSpriteCount].gridY = gridY;
        titleSprites[titleSpriteCount].romaji = romaji;
        titleSprites[titleSpriteCount].active = true;
        titleSpriteCount++;
    }
}

// 難易度選択画面を描画する関数
void DrawDifficultyScreen() {
    ClearBackground(RAYWHITE);
    int bgCols = SCREEN_WIDTH  / CELL_SIZE + 1;
    int bgRows = SCREEN_HEIGHT / CELL_SIZE + 1;
    DrawGobanBackground(0, 0, bgCols, bgRows, CELL_SIZE);
    int cs = CELL_SIZE;

    // "なんいどをえらんで"
    const std::string hdr[] = {"na","n","i","do","wo","e","ra","bo","u"};
    for (int i = 0; i < 9; i++)
        DrawBlackCellContent(hdr[i], (5 + i) * cs, 2 * cs, cs);

    // "かんたん"
    bool hlEasy = (hoveredDifficultyMenu == DIFFMENU_EASY);
    const std::string easy[] = {"ka","n","ta","n"};
    for (int i = 0; i < 4; i++)
        DrawBlackCellContent(easy[i], (7 + i) * cs, 4 * cs, cs, hlEasy);

    // "おけるもじがさいだい8こまで"
    const std::string easyD1[] = {"o","ke","ru","mo","ji","ga","sa","i","da","i","8","ko","ma","de"};
    for (int i = 0; i < 14; i++)
        DrawBlackCellContent(easyD1[i], (2 + i) * cs, 6 * cs, cs);

    // "おなじもじは2こまでおいてもいい"
    const std::string easyD2[] = {"o","na","ji","mo","ji","ha","2","ko","ma","de","o","i","te","mo","i","i"};
    for (int i = 0; i < 16; i++)
        DrawBlackCellContent(easyD2[i], (1 + i) * cs, 7 * cs, cs);

    // "むずかしい"
    bool hlHard = (hoveredDifficultyMenu == DIFFMENU_HARD);
    const std::string hard[] = {"mu","zu","ka","shi","i"};
    for (int i = 0; i < 5; i++)
        DrawBlackCellContent(hard[i], (7 + i) * cs, 9 * cs, cs, hlHard);

    // "おけるもじがさいだい5こまで"
    const std::string hardD1[] = {"o","ke","ru","mo","ji","ga","sa","i","da","i","5","ko","ma","de"};
    for (int i = 0; i < 14; i++)
        DrawBlackCellContent(hardD1[i], (2 + i) * cs, 11 * cs, cs);
    
    // "おなじもじは1こしかおけない"
    const std::string hardD2[] = {"o","na","ji","mo","ji","ha","1","ko","shi","ka","o","ke","na","i"};
    for (int i = 0; i < 14; i++)
        DrawBlackCellContent(hardD2[i], (2 + i) * cs, 12 * cs, cs);
}

// 難易度選択画面でのマウスクリック判定を行う関数
DifficultyMenu GetDifficultyMenuAtPosition(Vector2 mousePos) {
    int cs = CELL_SIZE;
    Rectangle easyRect = {7.0f * cs, 4.0f * cs, 4.0f * cs, (float)cs};
    Rectangle hardRect = {7.0f * cs, 9.0f * cs, 5.0f * cs, (float)cs};
    if (CheckCollisionPointRec(mousePos, easyRect)) return DIFFMENU_EASY;
    if (CheckCollisionPointRec(mousePos, hardRect)) return DIFFMENU_HARD;
    return DIFFMENU_NONE;
}

// タイトル画面に"げーむ"を描画する関数
void SetupTitleSprites() {
    titleSpriteCount = 0;
    AddTitleSprite(3, 3, "ge");
    AddTitleSprite(4, 3, "chouon");
    AddTitleSprite(5, 3, "mu");
}
