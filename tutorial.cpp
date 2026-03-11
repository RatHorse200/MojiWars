// チュートリアルの全5ページの画面描画と、練習用のダミー敵・入力ボックスの挙動を管理するファイル
#include "tutorial.h"
#include "title.h"
#include "sprites.h"
#include "enemy.h"
#include "constants.h"
#include "raylib.h"
#include <vector>
#include <algorithm>
#include <cmath>

// グリッド座標で文字を描画する
static void DrawTutChars(int col, int row, const std::vector<std::string>& chars, int cs = TUTO_CELL_SIZE) {
    for (int i = 0; i < (int)chars.size(); i++) {
        if (!chars[i].empty())
            DrawBlackCellContent(chars[i], (col + i) * cs, row * cs, cs);
    }
}

// 全ページ共通ヘッダー "ちゅーとりある　（数字）まいめ" という見出しを描画する
static void DrawTutHeader(int page) {
    const std::string nums[] = {"1","2","3","4","5"};
    DrawTutChars(4, 0, {"chi","xyu","chouon","to","ri","a","ru","", nums[page],"ma","i","me"});
}

// 画面上にゆみ、けん、たてのデモ敵をスポーンさせる関数
void TutorialRespawnEnemies() {
    int CS = CELL_SIZE;
    bool hasYumi = false, hasKen = false, hasTate = false;
    for (const auto& e : enemies) {
        if (!e.active) continue;
        if (e.type == ENEMY_YUMI) hasYumi = true;
        if (e.type == ENEMY_KEN)  hasKen  = true;
        if (e.type == ENEMY_TATE) hasTate = true;
    }
    // ゆみ
    if (!hasYumi) {
        Enemy e;
        e.type = ENEMY_YUMI; e.hp = 6; e.maxHp = 6; e.shootTimer = 0.0f;
        e.id = AllocEnemyId(); e.fieldHitCount = 0;
        e.x = (float)(13 * CS); e.y = (float)(5 * CS);
        e.dx = 0.0f; e.dy = 0.0f;
        e.active = true; e.spawnSide = SIDE_BOTTOM; e.hitFieldOnce = false;
        enemies.push_back(e);
    }
    // けん
    if (!hasKen) {
        Enemy e;
        e.type = ENEMY_KEN; e.hp = 16; e.maxHp = 16; e.shootTimer = 0.0f;
        e.id = AllocEnemyId(); e.fieldHitCount = 0;
        e.x = (float)(15 * CS); e.y = (float)(5 * CS);
        e.dx = 0.0f; e.dy = -0.25f * CS;
        e.active = true; e.spawnSide = SIDE_BOTTOM; e.hitFieldOnce = false;
        enemies.push_back(e);
    }
    // たて
    if (!hasTate) {
        Enemy e;
        e.type = ENEMY_TATE; e.hp = 16; e.maxHp = 16; e.shootTimer = 0.0f;
        e.id = AllocEnemyId(); e.fieldHitCount = 0;
        e.x = (float)(17 * CS); e.y = (float)(5 * CS);
        e.dx = 0.0f; e.dy = -0.125f * CS;
        e.active = true; e.spawnSide = SIDE_BOTTOM; e.hitFieldOnce = false;
        enemies.push_back(e);
    }
}

// チュートリアルのページ3の画面上にゆみ、けん、たてのデモ敵をスポーンさせる関数
void TutorialPage3RespawnEnemies() {
    int CS = CELL_SIZE;
    bool hasYumi = false, hasKen = false, hasTate = false;
    for (const auto& e : enemies) {
        if (!e.active) continue;
        if (e.type == ENEMY_YUMI) hasYumi = true;
        if (e.type == ENEMY_KEN)  hasKen  = true;
        if (e.type == ENEMY_TATE) hasTate = true;
    }
    // ゆみ
    if (!hasYumi) {
        Enemy e;
        e.type = ENEMY_YUMI; e.hp = 6; e.maxHp = 6; e.shootTimer = 0.0f;
        e.id = AllocEnemyId(); e.fieldHitCount = 0;
        e.x = (float)(14 * CS); e.y = (float)(6 * CS);
        e.dx = 0.0f; e.dy = 0.0f;
        e.active = true; e.spawnSide = SIDE_RIGHT; e.hitFieldOnce = false;
        enemies.push_back(e);
    }
    // けん
    if (!hasKen) {
        Enemy e;
        e.type = ENEMY_KEN; e.hp = 16; e.maxHp = 16; e.shootTimer = 0.0f;
        e.id = AllocEnemyId(); e.fieldHitCount = 0;
        e.x = (float)(14 * CS); e.y = (float)(3 * CS);
        e.dx = -0.25f * CS; e.dy = 0.0f;
        e.active = true; e.spawnSide = SIDE_RIGHT; e.hitFieldOnce = false;
        enemies.push_back(e);
    }
    // たて
    if (!hasTate) {
        Enemy e;
        e.type = ENEMY_TATE; e.hp = 16; e.maxHp = 16; e.shootTimer = 0.0f;
        e.id = AllocEnemyId(); e.fieldHitCount = 0;
        e.x = (float)(14 * CS); e.y = (float)(5 * CS);
        e.dx = -0.125f * CS; e.dy = 0.0f;
        e.active = true; e.spawnSide = SIDE_RIGHT; e.hitFieldOnce = false;
        enemies.push_back(e);
    }
}

// チュートリアル2ページ目にて、入力ボックスと弾の当たり判定を行う関数
void CheckBulletsVsTutBoxes() {
    for (auto& b : bullets) {
        if (!b.active || !b.fromYumi) continue;
        for (int i = 0; i < tutBoxCount; i++) {
            if (tutBoxes[i].content.empty()) continue;
            Rectangle r = {(float)tutBoxes[i].x, (float)tutBoxes[i].y, (float)tutBoxes[i].size, (float)tutBoxes[i].size};
            if (CheckCollisionPointRec({b.x, b.y}, r)) {
                tutBoxes[i].content = "";
                tutBoxes[i].buffer  = "";
                b.active = false;
                break;
            }
        }
    }
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
        [](const Bullet& blt) { return !blt.active; }), bullets.end());
}

// チュートリアル用の敵更新と、状況のセットアップを行う関数
void TutorialEnemyUpdate(float dt, int boardOffsetX, int boardOffsetY) {
    bool damage = (tutorialPage == 2);
    UpdateEnemies(dt, boardOffsetX, boardOffsetY, GRID_SIZE, CELL_SIZE, SCREEN_WIDTH, SCREEN_HEIGHT, gameBoard, damage);
    if (tutorialPage == 1) {
        CheckBulletsVsTutBoxes();
    } else if (tutorialPage == 2) TutorialPage3RespawnEnemies();
}

// チュートリアルのページをセットアップする関数
void SetupTutorialPage(int page) {
    tutBoxCount    = 0;
    tutSelectedBox = -1;
    for (int i = 0; i < 3; i++) { tutBoxes[i].buffer = ""; tutBoxes[i].content = ""; }

    enemies.clear();
    bullets.clear();

    // 1ページ目
    if (page == 0) {
        int bx = (SCREEN_COLS - 5) * CELL_SIZE;  
        int by = (SCREEN_ROWS - 5) * CELL_SIZE; 
        tutBoxes[0] = {bx, by, CELL_SIZE, "", ""};
        tutBoxCount = 1;
    // 2ページ目
    } else if (page == 1) {
        int by = 2 * CELL_SIZE;
        for (int i = 0; i < 3; i++)
            tutBoxes[i] = {(13 + i * 2) * CELL_SIZE, by, CELL_SIZE, "", ""};
        tutBoxCount = 3;
        TutorialRespawnEnemies();
    // 3ページ目
    } else if (page == 2) {
        gameBoard.Init();
        gameBoard.maxChars     = 25;
        gameBoard.maxDuplicates = 25;
        TutorialPage3RespawnEnemies();
    }
}

// カーソルの座標が入力ボックスのどれかの上に乗っているかを判定する関数
bool IsInAnyTutBox(Vector2 pos) {
    for (int i = 0; i < tutBoxCount; i++) {
        Rectangle r = {(float)tutBoxes[i].x, (float)tutBoxes[i].y, (float)tutBoxes[i].size, (float)tutBoxes[i].size};
        if (CheckCollisionPointRec(pos, r)) return true;
    }
    return false;
}

// 入力ボックスを描画する関数
void DrawTutBox(int idx) {
    TutBox& b   = tutBoxes[idx];
    bool    sel = (tutSelectedBox == idx);
    DrawRectangle(b.x, b.y, b.size, b.size, WHITE);
    DrawRectangleLinesEx({(float)b.x, (float)b.y, (float)b.size, (float)b.size}, 3, sel ? YELLOW : GREEN);
    if (!b.content.empty())
        DrawRedCellContent(b.content, b.x, b.y, b.size);
    if (sel && !b.buffer.empty())
        DrawText(b.buffer.c_str(), b.x + 4, b.y + b.size - 22, 16, DARKGRAY);
}

// 下向き矢印を描画する関数
static void DrawDownArrow(int col, int row, int cs) {
    float cx = (col + 0.5f) * cs;
    float cy = (row + 0.5f) * cs;
    float r  = cs * 0.32f;
    DrawLineEx({cx, cy - r * 0.4f}, {cx, cy + r * 0.2f}, 3.0f, BLACK);
    DrawTriangle({cx, cy + r}, {cx + r*0.5f, cy + r*0.1f}, {cx - r*0.5f, cy + r*0.1f}, BLACK);
}

// 以下、チュートリアルの各ページの描画関数と、ゲームオーバー画面の描画関数
// チュートリアルの全文に関しては、assets/tutorial.txtを参照

// チュートリアル1ページ目
static void DrawTutPage1() {
    DrawGobanBackground(0, 0, SCREEN_COLS, SCREEN_ROWS, CELL_SIZE);
    DrawTutHeader(0);
    DrawTutChars(1, 2, {"ma","u","su","no","hidari","ku","ri","xtu","ku","de"});
    DrawTutChars(1, 3, {"wa","ku","no","na","ka","ni","mo","ji","ga","u","te","ru","yo"});
    DrawTutChars(1, 5, {"ma","u","su","no","migi","ku","ri","xtu","ku","de"});
    DrawTutChars(1, 6, {"mo","ji","wo","ke","se","ru","yo"});
    DrawTutChars(1, 8, {"ma","su","me","ni","ro","chouon","ma","ji","de"});
    DrawTutChars(1, 9, {"mo","ji","wo","u","to","u"});
    DrawTutChars(1, 11, {"mo","ji","wo","ka","ku","ho","u","ko","u","ni"});
    DrawTutChars(1, 12, {"ta","ma","ga","de","ru","yo"});
    DrawTutChars(11, 7, {"ta","me","shi","te","mi","yo","u"});
    DrawDownArrow(14, 8, CELL_SIZE);
    if (tutBoxCount > 0) DrawTutBox(0);
    DrawEnemies(CELL_SIZE);
}

// チュートリアル2ページ目
static void DrawTutPage2() {
    DrawGobanBackground(0, 0, SCREEN_COLS, SCREEN_ROWS, CELL_SIZE);
    DrawTutHeader(1);
    DrawTutChars(1, 2, {"ta","ma","ga","te","ki","ni","a","ta","ru","to"});
    DrawTutChars(1, 3, {"da","me","chouon","ji","wo","a","ta","e","ru","yo"});
    DrawTutChars(1, 5, {"te","ki","no","shi","xyu","ru","i"});
    DrawTutChars(3, 6, {"1","ellipsis","yu","mi"});
    DrawTutChars(1, 7, {"to","o","ku","ka","ra","ko","u","ge","ki","shi","te"});
    DrawTutChars(1, 8, {"mo","ji","wo","ke","se","ru"});
    DrawTutChars(3, 9, {"2","ellipsis","ke","n"});
    DrawTutChars(1, 10, {"ma","xtu","su","gu","su","su","mu"});
    DrawTutChars(3, 11, {"3","ellipsis","ta","te"});
    DrawTutChars(1, 12, {"shi","xyo","u","me","n","ka","ra","u","ke","ru","da","me","chouon","ji","ga","hi","ku","i"});
    DrawTutChars(1, 13, {"ke","n","yo","ri","mo","su","su","mu","no","ga","o","so","i"});
    DrawTutChars(13, 8, {"ta","i","ri","xyo","ku","ga"});
    DrawTutChars(14, 9, {"hi","ku","i","to"});
    DrawTutChars(10, 10, {"te","ki","ga","a","ka","ku","na","ru","yo"});

    if (tutBoxCount >= 3)
        for (int i = 0; i < 3; i++) DrawTutBox(i);

    DrawEnemies(CELL_SIZE);
}

// チュートリアル3ページ目
static void DrawTutPage3() {
    int cs = CELL_SIZE;
    DrawGobanBackground(0, 0, SCREEN_COLS, SCREEN_ROWS, cs);
    DrawTutHeader(2);

    int fieldW = GRID_SIZE * cs;
    int fieldX = ((SCREEN_WIDTH - fieldW) / 2 / cs) * cs;
    int fieldY = 2 * cs;

    // フィールドはゲームプレイと同じ枠・ダメージ仕様
    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            int cellX = fieldX + x * cs;
            int cellY = fieldY + y * cs;
            DrawRectangle(cellX, cellY, cs, cs, RAYWHITE);
            if (x == gameBoard.cursorX && y == gameBoard.cursorY)
                DrawRectangle(cellX + 2, cellY + 2, cs - 4, cs - 4, Fade(YELLOW, 0.5f));
            DrawRedCellContent(gameBoard.cells[y][x], cellX, cellY, cs);
            if (x == gameBoard.cursorX && y == gameBoard.cursorY && !gameBoard.inputBuffer.empty())
                DrawText(gameBoard.inputBuffer.c_str(), cellX + 4, cellY + cs - 22, 16, DARKBLUE);
        }
    }

    for (int x = 0; x <= GRID_SIZE; x++)
        DrawLineEx({(float)(fieldX + x*cs), (float)fieldY}, {(float)(fieldX + x*cs), (float)(fieldY + fieldW)}, 2, BLACK);
    for (int y = 0; y <= GRID_SIZE; y++)
        DrawLineEx({(float)fieldX, (float)(fieldY + y*cs)}, {(float)(fieldX + fieldW), (float)(fieldY + y*cs)}, 2, BLACK);

    if (gameBoard.cursorX >= 0 && gameBoard.cursorY >= 0) {
        int curX = fieldX + gameBoard.cursorX * cs;
        int curY = fieldY + gameBoard.cursorY * cs;
        DrawRectangleLinesEx({(float)curX, (float)curY, (float)cs, (float)cs}, 3, RED);
    }

    DrawFieldBorderHP(fieldX, fieldY, fieldW, gameBoard.fieldHP, 8.0f);

    DrawEnemies(cs);

    DrawTutChars(1, 2, {"mo","ji","wo","ka","ke","ru"});
    DrawTutChars(1, 3, {"fu","xi","chouon","ru","do","ha"});
    DrawTutChars(1, 4, {"ko","no","na","ka"});
    DrawTutChars(1, 7, {"mi","do","ri","i","ro","no","wa","ku","ga","ta","i","ri","xyo","ku","ge","chouon","ji"});
    DrawTutChars(1, 9, {"fu","xi","chouon","ru","do","no","wa","ku","ga"});
    DrawTutChars(1, 10, {"te","ki"," ","ma","ta","ha"," ","te","ki","no","ta","ma","ni","a","ta","ru","to"});
    DrawTutChars(1, 11, {"da","me","chouon","ji","wo","u","ke","te"});
    DrawTutChars(1, 12, {"ku","ro","ku","na","ru","yo"});
}

// チュートリアル4ページ目
static void DrawTutPage4() {
    int cs = CELL_SIZE;
    DrawGobanBackground(0, 0, SCREEN_COLS, SCREEN_ROWS, cs);
    DrawTutHeader(3);
    DrawTutChars(1,  2, {"fu","xi","chouon","ru","do","no","wa","ku","ga"});
    DrawTutChars(1,  3, {"ze","n","bu","ku","ro","ku","na","ru","to"});
    DrawTutChars(1,  4, {"ge","chouon","mu","o","chouon","ba","chouon","ni","na","ru"});
    DrawTutChars(1,  6, {"te","ki","wo","ze","n","me","tsu","sa","se","ru","to"});
    DrawTutChars(1,  7, {"su","te","chouon","ji","ku","ri","a","exclamation"});
    DrawTutChars(1,  9, {"su","te","chouon","ji","ha","ze","n","bu","de","5","tsu"});
    DrawTutChars(1,  11, {"ze","n","bu","no","su","te","chouon","ji","no"});
    DrawTutChars(1,  12, {"ku","ri","a","wo","me","za","so","u","exclamation"});
}

// チュートリアル5ページ目
static void DrawTutPage5() {
    int cs = CELL_SIZE;
    DrawGobanBackground(0, 0, SCREEN_COLS, SCREEN_ROWS, cs);
    DrawTutHeader(4);
    DrawTutChars(1,  2, {"ko","u","ri","xya","ku","no"});
    DrawTutChars(3,  3, {"hi","n","to","1"});
    DrawTutChars(8,  2, {"yu","mi","to","ta","te","ha","na","na","me","ka","ra"});
    DrawTutChars(8,  3, {"ko","u","ge","ki","su","ru","ka"});
    DrawTutChars(8,  4, {"ta","ku","sa","n","no","mo","ji","wo","o","i","te"});
    DrawTutChars(8,  5, {"i","xtu","ki","ni","ta","o","so","u"});
    DrawTutChars(1,  8, {"ko","u","ri","xya","ku","no"});
    DrawTutChars(3,  9, {"hi","n","to","2"});
    DrawTutChars(8,  8, {"tsu","yo","so","u","na","hi","ra","ga","na","ha"});
    DrawTutChars(8,  9, {"o","bo","e","te","o","ku","ka"});
    DrawTutChars(8,  10, {"ka","mi","ni","me","mo","wo","to","xtu","te"});
    DrawTutChars(8,  11, {"hi","tsu","yo","u","na","to","ki","ni"});
    DrawTutChars(8,  12, {"tsu","ka","e","ru","yo","u","ni","shi","yo","u"});
}

// チュートリアル画面の振り分けを行う関数
void DrawTutorialScreen() {
    switch (tutorialPage) {
        case 0: DrawTutPage1(); break;
        case 1: DrawTutPage2(); break;
        case 2: DrawTutPage3(); break;
        case 3: DrawTutPage4(); break;
        case 4: DrawTutPage5(); break;
    }
}
