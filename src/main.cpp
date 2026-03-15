// ゲームの初期化、メインループ、画面遷移、入力処理を統括するメインファイル
#include "raylib.h"
#include "constants.h"
#include "romaji.h"
#include "sprites.h"
#include "board.h"
#include "enemy.h"
#include "title.h"
#include "tutorial.h"
#include "char_fire.h"
#include "audio.h"
#include "ai.h"

// 画面遷移とメニューのホバー状態
GameBoard gameBoard;
GameState currentState = STATE_TITLE;
MenuSelection hoveredMenu = MENU_NONE;
GameOverMenu hoveredGameOverMenu = GOMENU_NONE;
DifficultyMenu hoveredDifficultyMenu = DIFFMENU_NONE;

// チュートリアル用の状態管理
int tutorialPage = 0;
TutBox tutBoxes[3];
int tutBoxCount = 0;
int tutSelectedBox = -1;

// ステージ進行と演出の管理
int currentStage = 1;
int enemiesSpawnedThisStage = 0;
int enemiesRemaining = 0;
bool stageClearShowing = false;
float stageClearTimer = 0.0f;
bool gameAllCleared = false;
bool stageStartPending = false;

// 0=むずかしい(最大5文字/重複1), 1=かんたん(最大8文字/重複2)
int gameDifficulty = 0;

const int stageEnemyCounts[5] = {20, 25, 30, 35, 40}; // ステージごとの敵の総数
const float stageSpawnIntervals[5] = {5.0f, 5.0f, 4.5f, 4.5f, 4.0f}; // ステージごとの敵のスポーン間隔

// 難易度によって変化する、同時に置ける文字数と同じ文字をいくつ置けるかのルールをセットする関数
static void ApplyDifficulty() {
    gameBoard.maxChars     = (gameDifficulty == 1) ? 8 : 5;
    gameBoard.maxDuplicates = (gameDifficulty == 1) ? 2 : 1;
}

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Moji Wars");
    SetTargetFPS(60);

    // 文字サイズ設定、弾の飛び方設定、ローマ字変換表、盤面、タイトル装飾、すべての画像、オーディオを、一気に読み込んで準備（初期化）
    // 最後にタイトル画面用のbgmを再生する
    LoadCharScaleConfig();
    LoadCharFireConfig();
    InitRomajiTable();
    gameBoard.Init();
    SetupTitleSprites();
    PreloadAllSprites();
    InitGameAudio();
    SetBGM(BGM_TITLE);

    // 画面中央にフィールドを配置するための座標計算
    int boardWidth   = GRID_SIZE * CELL_SIZE;
    int boardOffsetX = ((SCREEN_WIDTH  - boardWidth) / 2 / CELL_SIZE) * CELL_SIZE;
    int boardOffsetY = 2 * CELL_SIZE;

    static float spawnTimer = 0.0f;

    while (!WindowShouldClose()) {
        Vector2 mousePos = GetMousePosition(); // マウスカーソルの座標
        float dt = GetFrameTime(); // 進んだ時間
        UpdateGameAudio(); // オーディオの更新処理
        switch (currentState) {
            case STATE_TITLE:
                hoveredMenu = GetMenuAtPosition(mousePos); // カーソルがボタンの上にあればハイライトさせる
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    MenuSelection clicked = GetMenuAtPosition(mousePos);
                    if (clicked == MENU_START) {
                        PlaySE_Click(); // クリック音を鳴らす
                        tutorialPage = 0;
                        SetupTutorialPage(0);
                        currentState = STATE_TUTORIAL;
                    } else if (clicked == MENU_END) {
                        PlaySE_Click();
                        CloseWindow();
                        return 0;
                    }
                }
                break;

            case STATE_TUTORIAL:
                // 敵の更新処理
                if (tutorialPage == 0 || tutorialPage == 1 || tutorialPage == 2) {
                    if (tutorialPage == 2) {
                        TutorialEnemyUpdate(dt, boardOffsetX, boardOffsetY);
                        FireCellBullets(dt, gameBoard, boardOffsetX, boardOffsetY, CELL_SIZE);
                    } else {
                        TutorialEnemyUpdate(dt, boardOffsetX, boardOffsetY);
                    }
                }

                // 入力ボックスの中に文字が入っていれば、そこから弾を発射する処理
                if (tutorialPage == 0 || tutorialPage == 1) {
                    float bulletSpeed = 3.0f * CELL_SIZE;
                    for (int i = 0; i < tutBoxCount; i++) {
                        TutBox& box = tutBoxes[i];
                        if (box.content.empty()) continue;
                        box.fireTimer += dt;
                        if (box.fireTimer < 2.0f) continue;
                        box.fireTimer = 0.0f;
                        auto it = charFireConfig.find(box.content);
                        if (it == charFireConfig.end()) continue;
                        for (const auto& fp : it->second) {
                            Bullet b;
                            b.active      = true;
                            b.fromYumi    = false;
                            b.fromCell    = true;
                            b.hasHitField = false;
                            b.ownerId     = -1;
                            b.x = (float)(box.x + fp.relX);
                            b.y = (float)(box.y + fp.relY);
                            GetFireVelocity(fp.direction, bulletSpeed, b.dx, b.dy);
                            bullets.push_back(b);
                        }
                    }
                }

                // 左クリックの処理
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    if (tutorialPage == 2) {
                        // フィールド内をクリックすればマスを選択し、フィールド外をクリックすれば次のページへ進む
                        int gx = (int)(mousePos.x - boardOffsetX) / CELL_SIZE;
                        int gy = (int)(mousePos.y - boardOffsetY) / CELL_SIZE;
                        if (gx >= 0 && gx < GRID_SIZE && gy >= 0 && gy < GRID_SIZE) {
                            gameBoard.SetCursor(gx, gy);
                        } else {
                            tutorialPage++;
                            SetupTutorialPage(tutorialPage);
                        }
                    } else if (IsInAnyTutBox(mousePos)) {
                        // 入力ボックスをクリックすればそのボックスが選択される
                        for (int i = 0; i < tutBoxCount; i++) {
                            Rectangle r = {(float)tutBoxes[i].x, (float)tutBoxes[i].y, (float)tutBoxes[i].size, (float)tutBoxes[i].size};
                            if (CheckCollisionPointRec(mousePos, r)) {
                                tutSelectedBox = i;
                                break;
                            }
                        }
                    } else {
                        PlaySE_Click();
                        tutSelectedBox = -1;
                        if (tutorialPage < 4) {
                            tutorialPage++;
                            SetupTutorialPage(tutorialPage);
                        } else {
                            // ページ5が終わったら難易度選択画面へ
                            currentState = STATE_DIFFICULTY;
                            hoveredDifficultyMenu = DIFFMENU_NONE;
                        }
                    }
                }

                // 右クリックの処理
                if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
                    if (tutorialPage == 0 || tutorialPage == 1) {
                        // クリックした箱の中身を空っぽにする処理
                        // 箱以外なら前のページへ戻る
                        bool clearedBox = false;
                        for (int i = 0; i < tutBoxCount; i++) {
                            Rectangle r = {(float)tutBoxes[i].x, (float)tutBoxes[i].y, (float)tutBoxes[i].size, (float)tutBoxes[i].size};
                            if (CheckCollisionPointRec(mousePos, r)) {
                                tutBoxes[i].content = "";
                                tutBoxes[i].buffer  = "";
                                clearedBox = true;
                                break;
                            }
                        }
                        if (!clearedBox) {
                            if (tutorialPage == 0) currentState = STATE_TITLE;
                            else { tutorialPage--; SetupTutorialPage(tutorialPage); }
                        }
                    } else if (tutorialPage == 2) {
                        // クリックしたフィールドの文字を消去する処理。フィールド外なら前のページへ戻る
                        int gx = (int)(mousePos.x - boardOffsetX) / CELL_SIZE;
                        int gy = (int)(mousePos.y - boardOffsetY) / CELL_SIZE;
                        if (gx >= 0 && gx < GRID_SIZE && gy >= 0 && gy < GRID_SIZE) {
                            gameBoard.SetCursor(gx, gy);
                            gameBoard.ClearCell();
                        } else {
                            tutorialPage--;
                            SetupTutorialPage(tutorialPage);
                        }
                    } else if (!IsInAnyTutBox(mousePos)) {
                        // 前のページへ戻る、または0ページ目ならタイトル画面に戻る
                        tutSelectedBox = -1;
                        if (tutorialPage > 0) {
                            tutorialPage--;
                            SetupTutorialPage(tutorialPage);
                        } else {
                            currentState = STATE_TITLE;
                        }
                    }
                }

                // キーボード入力処理
                if (tutorialPage == 2) {
                    // フィールドにローマ字を入力する
                    for (int key = KEY_A; key <= KEY_Z; key++) {
                        if (IsKeyPressed(key)) {
                            char c = 'a' + (key - KEY_A);
                            if (gameBoard.ProcessInput(c)) PlaySE_Place();
                        }
                    }
                    if (IsKeyPressed(KEY_BACKSPACE)) {
                        if (!gameBoard.inputBuffer.empty()) gameBoard.inputBuffer.pop_back();
                        else                                gameBoard.ClearCell();
                    }
                } else if (tutSelectedBox >= 0 && tutSelectedBox < tutBoxCount) {
                    // 入力ボックスにローマ字を入力する
                    TutBox& box = tutBoxes[tutSelectedBox];
                    for (int key = KEY_A; key <= KEY_Z; key++) {
                        if (IsKeyPressed(key)) {
                            char c = 'a' + (key - KEY_A);
                            box.buffer += c;
                            auto it = romaji_to_canonical.find(box.buffer);
                            if (it != romaji_to_canonical.end()) {
                                box.content = it->second;
                                box.buffer  = "";
                                PlaySE_Place();
                            } else if (!IsPrefixOfRomaji(box.buffer)) {
                                box.buffer = "";
                            }
                        }
                    }
                    // BACKSPACEキーを押してもひらがなやローマ字を消去できる
                    if (IsKeyPressed(KEY_BACKSPACE)) {
                        if (!box.buffer.empty()) box.buffer.pop_back();
                        else                     box.content = "";
                    }
                }

                if (IsKeyPressed(KEY_ESCAPE)) {
                    currentState = STATE_TITLE;
                }
                break;

            // 難易度選択画面の処理
            case STATE_DIFFICULTY:
                hoveredDifficultyMenu = GetDifficultyMenuAtPosition(mousePos);
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    DifficultyMenu sel = GetDifficultyMenuAtPosition(mousePos);
                    if (sel == DIFFMENU_EASY || sel == DIFFMENU_HARD) {
                        PlaySE_Click();
                        gameDifficulty          = (sel == DIFFMENU_EASY) ? 1 : 0;
                        currentStage            = 1;
                        enemiesSpawnedThisStage = 0; // 出現済み数をリセット
                        enemiesRemaining        = stageEnemyCounts[0]; // 残り敵数をリセット
                        spawnTimer              = 0.0f;
                        stageClearShowing       = false; // クリア表示を消す
                        gameAllCleared          = false; // 全クリアフラグを消す
                        stageStartPending       = true; // ステージ開始待ちフラグを立てる
                        gameBoard.Init();
                        ApplyDifficulty();
                        ResetAI();
                        enemies.clear();
                        bullets.clear();
                        currentState = STATE_PLAYING;
                    }
                }
                break;

            // プレイ中の処理
            case STATE_PLAYING:
                // ステージ開始待ち
                if (stageStartPending) {
                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        PlaySE_Click();
                        stageStartPending = false;
                    }
                    break;
                }

                // ステージクリア表示中の処理
                if (stageClearShowing) {
                    // 全ステージクリア時：クリックでタイトルへ戻る
                    if (gameAllCleared) {
                        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) ||
                            IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
                            gameAllCleared    = false;
                            stageClearShowing = false;
                            currentState      = STATE_TITLE;
                        }
                    } else {
                        // 1ステージクリア時：「さきへすすむ」ボタンのクリック判定
                        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                            Rectangle continueRect = {6.0f * CELL_SIZE, 9.0f * CELL_SIZE, 6.0f * CELL_SIZE, (float)CELL_SIZE};
                            if (CheckCollisionPointRec(mousePos, continueRect)) {
                                PlaySE_Click();
                                stageClearShowing = false;
                                currentStage++;
                                enemiesSpawnedThisStage = 0;
                                enemiesRemaining = stageEnemyCounts[currentStage - 1];
                                spawnTimer = 0.0f;
                                stageStartPending = true;
                                gameBoard.Init();
                                ApplyDifficulty();
                                ResetAI();
                                enemies.clear();
                                bullets.clear();
                            }
                        }
                    }
                    break;
                }

                // フィールドHPが0になったらゲームオーバーへ
                if (gameBoard.fieldHP <= 0) {
                    currentState = STATE_GAMEOVER;
                    hoveredGameOverMenu = GOMENU_NONE;
                    PlaySE_GameOver();
                    break;
                }

                // 左クリック：クリックした位置のマスを選択（カーソル移動）
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    int gridX = (int)(mousePos.x - boardOffsetX) / CELL_SIZE;
                    int gridY = (int)(mousePos.y - boardOffsetY) / CELL_SIZE;
                    if (gridX >= 0 && gridX < GRID_SIZE && gridY >= 0 && gridY < GRID_SIZE)
                        gameBoard.SetCursor(gridX, gridY);
                }

                // 右クリック：クリックした位置の文字を消去
                if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
                    int gridX = (int)(mousePos.x - boardOffsetX) / CELL_SIZE;
                    int gridY = (int)(mousePos.y - boardOffsetY) / CELL_SIZE;
                    if (gridX >= 0 && gridX < GRID_SIZE && gridY >= 0 && gridY < GRID_SIZE) {
                        gameBoard.SetCursor(gridX, gridY);
                        gameBoard.ClearCell();
                    }
                }

                // タイピング処理
                for (int key = KEY_A; key <= KEY_Z; key++) {
                    if (IsKeyPressed(key)) {
                        char c = 'a' + (key - KEY_A);
                        if (gameBoard.ProcessInput(c)) PlaySE_Place();
                    }
                }

                // Backspaceキー：文字を消す、または入力中のローマ字を1文字消す
                if (IsKeyPressed(KEY_BACKSPACE)) {
                    if (!gameBoard.inputBuffer.empty()) gameBoard.inputBuffer.pop_back();
                    else                                gameBoard.ClearCell();
                }

                // Tabキー
                if (IsKeyPressed(KEY_TAB)) {
                    currentState = STATE_TITLE;
                }

                // 敵スポーンロジック
                {
                    int stageIdx = currentStage - 1;
                    int totalNeeded = stageEnemyCounts[stageIdx]; // このステージのノルマ数
                    // かんたんはAIの適応型難易度でスポーン間隔を変化させる
                    float interval = stageSpawnIntervals[stageIdx] * GetAISpawnIntervalMult(gameDifficulty);

                    if (enemiesSpawnedThisStage < totalNeeded) {
                        spawnTimer += dt;
                        if (spawnTimer >= interval) {
                            spawnTimer = 0.0f;
                            SpawnEnemy(boardOffsetX, boardOffsetY, GRID_SIZE, CELL_SIZE, currentStage, gameDifficulty, GetAISpawnSide());
                            enemiesSpawnedThisStage++;
                        }
                    } else if (enemies.empty()) {
                        // ノルマ分出し切り、かつ画面上の敵が全滅したらステージクリア
                        stageClearShowing = true;
                        if (currentStage >= 5) {
                            gameAllCleared = true;
                            PlaySE_GameClear();
                        } else {
                            PlaySE_StageClear();
                        }
                    }
                }

                // AIの更新：ユーティリティスコアと適応型難易度を更新する
                UpdateAI(dt, gameBoard, gameDifficulty);

                // 全体の更新：敵の移動判定、味方文字からの弾発射
                UpdateEnemies(dt, boardOffsetX, boardOffsetY, GRID_SIZE, CELL_SIZE, SCREEN_WIDTH, SCREEN_HEIGHT, gameBoard);
                FireCellBullets(dt, gameBoard, boardOffsetX, boardOffsetY, CELL_SIZE);

                // UI用の「てきのこり」数を計算（画面上の敵 + まだ出現していない敵）
                {
                    int stageIdx = currentStage - 1;
                    int notSpawned = stageEnemyCounts[stageIdx] - enemiesSpawnedThisStage;
                    enemiesRemaining = (int)enemies.size() + notSpawned;
                    if (enemiesRemaining < 0) enemiesRemaining = 0;
                }
                break;

            // ゲームオーバー画面の処理
            case STATE_GAMEOVER:
                // マウスがどのメニュー項目（リトライ、最初から、タイトルへ）の上にあるか判定
                hoveredGameOverMenu = GetGameOverMenuAtPosition(mousePos);
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    switch (hoveredGameOverMenu) {
                        case GOMENU_RETRY: // 今のステージをやり直す
                            PlaySE_Click();
                            enemiesSpawnedThisStage = 0;
                            enemiesRemaining = stageEnemyCounts[currentStage - 1];
                            spawnTimer = 0.0f;
                            stageClearShowing = false;
                            gameAllCleared    = false;
                            stageStartPending = true;
                            gameBoard.Init();
                            ApplyDifficulty();
                            ResetAI();
                            enemies.clear();
                            bullets.clear();
                            currentState = STATE_PLAYING;
                            break;
                        case GOMENU_RESTART: // ステージ1からやり直す
                            PlaySE_Click();
                            currentStage = 1;
                            enemiesSpawnedThisStage = 0;
                            enemiesRemaining = stageEnemyCounts[0];
                            spawnTimer = 0.0f;
                            stageClearShowing = false;
                            gameAllCleared    = false;
                            stageStartPending = true;
                            gameBoard.Init();
                            ApplyDifficulty();
                            ResetAI();
                            enemies.clear();
                            bullets.clear();
                            currentState = STATE_PLAYING;
                            break;
                        case GOMENU_END: // タイトル画面に戻る
                            PlaySE_Click();
                            currentState = STATE_TITLE;
                            break;
                        default: break;
                    }
                }
                break;
        }

        // bgm切り替え
        {
            BGMTrack desired = BGM_NONE;
            // タイトル、チュートリアル、難易度選択画面ではタイトルbgm
            if (currentState == STATE_TITLE || currentState == STATE_TUTORIAL ||
                currentState == STATE_DIFFICULTY) {
                desired = BGM_TITLE;
            // プレイ中であり、かつ開始待ちやクリア演出中でなければゲームbgm
            } else if (currentState == STATE_PLAYING && !stageStartPending && !stageClearShowing) {
                desired = BGM_GAME;
            }
            SetBGM(desired);
        }

        BeginDrawing(); // 描画開始

        switch (currentState) {
            // 現在の状態に対応する描画関数（title.cppやtutorial.cppで定義）を呼び出す
            case STATE_TITLE:      DrawTitleScreen();      break;
            case STATE_TUTORIAL:   DrawTutorialScreen();   break;
            case STATE_DIFFICULTY: DrawDifficultyScreen(); break;
            case STATE_PLAYING:    DrawGameScreen();        break;
            case STATE_GAMEOVER:   DrawGameOverScreen();   break;
        }

        EndDrawing(); // 描画終了、画面が更新する
    }

    // 読み込んだ画像データをビデオメモリから解放する
    for (auto& pair : redSpriteTextures)
        if (pair.second.id != 0) UnloadTexture(pair.second);
    for (auto& pair : blackSpriteTextures)
        if (pair.second.id != 0) UnloadTexture(pair.second);

    CloseGameAudio(); // オーディオデバイスの終了処理
    CloseWindow(); // ウィンドウを閉じる
    return 0; // プログラム終了
}
