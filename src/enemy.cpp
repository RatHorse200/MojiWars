// 敵キャラクターと弾の生成・移動・当たり判定・描画など、戦闘システム全般を管理するファイル
#include "enemy.h"
#include "ai.h"
#include "sprites.h"
#include "constants.h"
#include "char_fire.h"
#include "audio.h"
#include "raylib.h"
#include <cstdlib>
#include <cmath>
#include <algorithm>

std::vector<Enemy> enemies;     // 全ての敵のデータ
std::vector<Bullet> bullets;    // 全ての弾のデータ

static int nextEnemyId = 1;     // 次に新しく生み出す敵のID

extern int gameDifficulty;      // 難易度（main.cppで定義）

int AllocEnemyId() { return nextEnemyId++; } // 敵のIDを新しく割り当てる関数

// ゆみのスポーン位置を指定する配列
// 補足：フィールドからひらがなが斜め方向で弾を撃ってヒットする位置をスポーン位置として指定している
// フィールドの右側
static const int YUMI_RIGHT[][2] = {
    {13,2},{14,2},{15,2},
    {13,3},{14,3},
    {13,4},
    {13,5},{14,5},
    {13,6},{14,6},{15,6}
};
static const int YUMI_RIGHT_N = 11;

// フィールドの左側
static const int YUMI_LEFT[][2] = {
    {3,2},{4,2},{5,2},
    {4,3},{5,3},
    {5,4},
    {4,5},{5,5},
    {3,6},{4,6},{5,6}
};
static const int YUMI_LEFT_N = 11;

// フィールドの下側
static const int YUMI_BOTTOM[][2] = {
    {7,8},{7,9},{7,10},
    {8,8},{8,9},
    {9,8},
    {10,8},{10,9},
    {11,8},{11,9},{11,10}
};
static const int YUMI_BOTTOM_N = 11;

// ゆみの敵情報とスポーン位置を決める関数（forcedSideでAIが指定した方向からスポーンする）
static void SpawnYumi(Enemy& e, int cellSize, SpawnSide forcedSide) {
    e.type = ENEMY_YUMI;
    e.hp   = 6;     // 現在の体力
    e.maxHp = 6;    // 最大体力
    e.dx   = 0.0f;  // 移動しないので速度0
    e.dy   = 0.0f;  // 移動しないので速度0
    e.shootTimer = 0.0f;    // 弾を撃つまでの時間を計るタイマー
    e.fieldHitCount = 0;    // フィールドにダメージを与えた回数

    // AIが指定した方向からスポーンする
    if (forcedSide == SIDE_RIGHT) {
        int idx  = GetRandomValue(0, YUMI_RIGHT_N - 1);
        e.x = (float)(YUMI_RIGHT[idx][0] * cellSize);
        e.y = (float)(YUMI_RIGHT[idx][1] * cellSize);
        e.spawnSide = SIDE_RIGHT;
    } else if (forcedSide == SIDE_LEFT) {
        int idx  = GetRandomValue(0, YUMI_LEFT_N - 1);
        e.x = (float)(YUMI_LEFT[idx][0] * cellSize);
        e.y = (float)(YUMI_LEFT[idx][1] * cellSize);
        e.spawnSide = SIDE_LEFT;
    } else {
        int idx  = GetRandomValue(0, YUMI_BOTTOM_N - 1);
        e.x = (float)(YUMI_BOTTOM[idx][0] * cellSize);
        e.y = (float)(YUMI_BOTTOM[idx][1] * cellSize);
        e.spawnSide = SIDE_BOTTOM;
    }
}

// けんとたての敵情報とスポーン位置を決める関数（forcedSideでAIが指定した方向からスポーンする）
static void SpawnKenTate(Enemy& e, EnemyType type, int cellSize, SpawnSide forcedSide) {
    e.type  = type;
    e.hp    = 16;   // 現在の体力
    e.maxHp = 16;   // 最大体力
    e.fieldHitCount = 0;    // フィールドにダメージを与えた回数
    float speed = (type == ENEMY_KEN) ? 0.25f * cellSize : 0.125f * cellSize; // けんは0.25倍、たては0.125倍の速度で移動する

    // AIが指定した方向からスポーンする
    if (forcedSide == SIDE_RIGHT) {
        e.spawnSide = SIDE_RIGHT;
        e.x  = (float)(17 * cellSize);
        e.y  = (float)(GetRandomValue(2, 6) * cellSize);
        e.dx = -speed;
        e.dy = 0.0f;
    } else if (forcedSide == SIDE_LEFT) {
        e.spawnSide = SIDE_LEFT;
        e.x  = (float)(1 * cellSize);
        e.y  = (float)(GetRandomValue(2, 6) * cellSize);
        e.dx =  speed;
        e.dy = 0.0f;
    } else {
        e.spawnSide = SIDE_BOTTOM;
        e.x  = (float)(GetRandomValue(7, 11) * cellSize);
        e.y  = (float)(12 * cellSize);
        e.dx = 0.0f;
        e.dy = -speed;
    }
}

// 敵を生成するための情報を決めてenemiesに追加する関数
void SpawnEnemy(int boardOffsetX, int boardOffsetY, int gridSize, int cellSize, int stage, int difficulty, SpawnSide forcedSide) {
    // 難易度 １：かんたん　０：むずかしい
    // フォーマット：　ステージ〇： けんのスポーン確率/たてのスポーン確率/ゆみのスポーン確率
    // かんたん：  ステージ１: 60/20/20  ステージ２－３: 50/25/25  ステージ４－５: 40/30/30
    // むずかしい：  ステージ２: 50/25/25  ステージ２－３: 40/30/30  ステージ４－５: 30/35/35
    int kenPct, tatePct;
    if (difficulty == 1) {
        if      (stage <= 1) { kenPct = 60; tatePct = 20; }
        else if (stage <= 3) { kenPct = 50; tatePct = 25; }
        else                 { kenPct = 40; tatePct = 30; }
    } else {
        if      (stage <= 1) { kenPct = 50; tatePct = 25; }
        else if (stage <= 3) { kenPct = 40; tatePct = 30; }
        else                 { kenPct = 30; tatePct = 35; }
    }
    int roll = GetRandomValue(1, 100);
    EnemyType type;
    if      (roll <= kenPct)            type = ENEMY_KEN;
    else if (roll <= kenPct + tatePct)  type = ENEMY_TATE;
    else                                type = ENEMY_YUMI;

    Enemy e;    // すべての敵に共通する情報
    e.active = true; // true：生きている
    e.hitFieldOnce = false;
    e.shootTimer = 0.0f;
    e.id = nextEnemyId++;
    e.spawnTime = (float)GetTime(); // スポーン時刻を記録（TTK計測用）

    if (type == ENEMY_YUMI)  SpawnYumi(e, cellSize, forcedSide);
    else                     SpawnKenTate(e, type, cellSize, forcedSide);

    enemies.push_back(e);
}

// 敵の現在HPに応じたダメージの赤みの色合いを返す関数
static Color EnemyDamageTint(int hp, int maxHp) {
    if (maxHp <= 0) return Color{255, 0, 0, 255};
    float frac = (float)hp / (float)maxHp;
    int step;
    if      (frac > 0.75f) step = 0;
    else if (frac > 0.50f) step = 1;
    else if (frac > 0.25f) step = 2;
    else                   step = 3;
    int r = step * 85;
    if (r > 255) r = 255;
    return Color{(unsigned char)r, 0, 0, 255};
}

// すべての敵と弾の移動、射撃、当たり判定、そして不要になった文字の削除をする更新処理関数
void UpdateEnemies(float dt, int boardOffsetX, int boardOffsetY, int gridSize, int cellSize, int screenWidth, int screenHeight, GameBoard& board, bool dealFieldDamage) {

    int fieldRight  = boardOffsetX + gridSize * cellSize;   // フィールドの右端のx座標
    int fieldBottom = boardOffsetY + gridSize * cellSize;   // フィールドの下端のy座標

    // 画面にいるすべての敵について一体ずつ処理を行う
    for (auto& e : enemies) {
        if (!e.active) continue;

        // 敵がゆみなら、タイマーが3秒経ったら弓の中心座標から弾を撃つ
        // ゆみがいる方向に応じて弾の飛ぶ速度と歩行を設定し、飛んでいる弾のリストに新しく弾を追加する　
        if (e.type == ENEMY_YUMI) {
            e.shootTimer += dt;
            if (e.shootTimer >= 3.0f) {
                e.shootTimer = 0.0f;
                float bulletSpeed = 3.0f * cellSize;
                Bullet b;
                b.active      = true;
                b.fromYumi    = true;
                b.fromCell    = false;
                b.hasHitField = false;
                b.ownerId     = e.id;
                b.x = e.x + cellSize * 0.5f;
                b.y = (float)((int)(e.y / cellSize) * cellSize + cellSize / 2);
                if (e.spawnSide == SIDE_LEFT) {
                    b.dx =  bulletSpeed; b.dy = 0.0f;
                } else if (e.spawnSide == SIDE_RIGHT) {
                    b.dx = -bulletSpeed; b.dy = 0.0f;
                } else {
                    b.dx = 0.0f; b.dy = -bulletSpeed;
                }
                bullets.push_back(b);
            }
            continue;
        }

        e.x += e.dx * dt;
        e.y += e.dy * dt;

        // フィールド枠への接触判定（左右は先端が枠に触れた瞬間、下は上端が枠に触れた瞬間）
        // 枠に触れるとフィールドは5ダメージを受ける
        if (dealFieldDamage && !e.hitFieldOnce) {
            bool hitField = false;
            if (e.spawnSide == SIDE_LEFT   && e.x + 2 * cellSize >= (float)boardOffsetX) hitField = true;
            if (e.spawnSide == SIDE_RIGHT  && e.x                 <= (float)fieldRight)  hitField = true;
            if (e.spawnSide == SIDE_BOTTOM && e.y                 <= (float)fieldBottom) hitField = true;
            if (hitField) {
                board.fieldHP -= 5;
                if (board.fieldHP < 0) board.fieldHP = 0;
                e.hitFieldOnce = true;
                PlaySE_FieldDamage();
            }
        }

        // 敵が画面の反対側まで通り抜けた場合、その敵を倒した扱いにする
        bool exited = false;
        if (e.spawnSide == SIDE_LEFT   && e.x > (float)screenWidth)  exited = true;
        if (e.spawnSide == SIDE_RIGHT  && e.x < -(float)cellSize)    exited = true;
        if (e.spawnSide == SIDE_BOTTOM && e.y < -(float)cellSize)    exited = true;
        if (exited) e.active = false;
    }

    // 弾の処理
    for (auto& b : bullets) {
        if (!b.active) continue;

        b.x += b.dx * dt;
        b.y += b.dy * dt;

        // 弾が画面の反対側まで通り抜けた場合、その弾を消す
        if (b.x < -(float)cellSize || b.x > (float)(screenWidth  + cellSize) ||
            b.y < -(float)cellSize || b.y > (float)(screenHeight + cellSize)) {
            b.active = false;
            continue;
        }

        // ゆみの弾がフィールドの枠に触れた瞬間と、フィールド内にいる間の両方で当たり判定を行う
        if (b.fromYumi) {
            bool inField = (b.x >= (float)boardOffsetX && b.x <= (float)fieldRight &&
                            b.y >= (float)boardOffsetY && b.y <= (float)fieldBottom);
            if (inField) {
                // ひらがなとの当たり判定について
                // 弾の中心がひらがなのマスの中に入った瞬間に当たったとみなす
                int cellX = (int)((b.x - boardOffsetX) / cellSize);
                int cellY = (int)((b.y - boardOffsetY) / cellSize);
                if (cellX >= 0 && cellX < gridSize && cellY >= 0 && cellY < gridSize &&
                    !board.cells[cellY][cellX].empty()) {
                    board.cells[cellY][cellX] = "";
                    b.active = false;
                    continue;
                }
                // フィールドの枠に触れた瞬間、フィールドの体力を2減らす
                // ダメージを与えるのは10回までとする
                if (!b.hasHitField) {
                    if (dealFieldDamage) {
                        for (auto& owner : enemies) {
                            if (owner.id == b.ownerId) {
                                if (owner.fieldHitCount < 10) {
                                    board.fieldHP -= 2;
                                    if (board.fieldHP < 0) board.fieldHP = 0;
                                    owner.fieldHitCount++;
                                    PlaySE_FieldDamage();
                                }
                                break;
                            }
                        }
                    }
                    b.hasHitField = true;
                }
            }
        }

        // プレイヤーが置いた文字から発射された味方の弾の処理
        // 生きている全ての敵との当たり判定を行う
        if (b.fromCell && b.active) {
            for (auto& e : enemies) {
                if (!e.active) continue;
                Rectangle enemyRect;
                if (e.spawnSide == SIDE_LEFT || e.spawnSide == SIDE_RIGHT) {
                    enemyRect = {e.x, e.y, (float)(2 * cellSize), (float)cellSize};
                } else {
                    enemyRect = {e.x, e.y, (float)cellSize, (float)(2 * cellSize)};
                }
                if (CheckCollisionCircleRec({b.x, b.y}, 5.0f, enemyRect)) {
                    int damage = 2; // 味方の弾のダメージは2
                    if (e.type == ENEMY_TATE) {
                        // たてが正面から受けるダメージを2から1に減らす
                        bool frontHit = (e.dx > 0 && b.dx < 0) || (e.dx < 0 && b.dx > 0) || (e.dy > 0 && b.dy < 0) || (e.dy < 0 && b.dy > 0);
                        if (frontHit) damage = 1;
                    }
                    e.hp -= damage;
                    if (e.hp <= 0) {
                        e.active = false;
                        // 撃破までの時間をAIに通知（適応型難易度用）
                        NotifyEnemyKilled((float)GetTime() - e.spawnTime, gameDifficulty);
                    }
                    b.active = false;
                    break;
                }
            }
        }
    }

    // HPが0になったり、画面外に出たりすればリストから敵や弾を削除する
    enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
        [](const Enemy& e) { return !e.active; }), enemies.end());

    bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
        [](const Bullet& b) { return !b.active; }), bullets.end());
}

// ひらがなから一定時間(2秒)ごとに自動で弾を撃つ迎撃処理
void FireCellBullets(float dt, GameBoard& board, int boardOffsetX, int boardOffsetY, int cellSize) {
    float bulletSpeed = 3.0f * cellSize; // 1秒に3マス分進む速さ

    // フィールド上のマスを一マスずつ順番に調べていく
    for (int cy = 0; cy < GRID_SIZE; cy++) {
        for (int cx = 0; cx < GRID_SIZE; cx++) {
            const std::string& ch = board.cells[cy][cx];
            // 発射タイマーの更新と発射タイミングの判定
            if (ch.empty()) {
                board.cellFireTimers[cy][cx] = 0.0f;
                continue;
            }
            board.cellFireTimers[cy][cx] += dt;
            if (board.cellFireTimers[cy][cx] < 2.0f) continue;
            board.cellFireTimers[cy][cx] = 0.0f;

            // 発射設定の読み込み
            auto it = charFireConfig.find(ch);
            if (it == charFireConfig.end()) continue;

            // 現在のマスが実際のゲーム画面のどの座標にあるかを計算する
            int cellScreenX = boardOffsetX + cx * cellSize;
            int cellScreenY = boardOffsetY + cy * cellSize;

            // ひらがなから撃たれた弾の情報をセットしてbulletsリストに追加し、発射の準備をする
            // ひらがなに設定された発射ポイントの数だけループする
            for (const auto& fp : it->second) {
                Bullet b;
                b.active      = true;
                b.fromYumi    = false;
                b.fromCell    = true;
                b.hasHitField = false;
                b.ownerId     = -1;
                b.x = (float)(cellScreenX + fp.relX);
                b.y = (float)(cellScreenY + fp.relY);
                GetFireVelocity(fp.direction, bulletSpeed, b.dx, b.dy);
                bullets.push_back(b);
            }
        }
    }
}

// 生きている全ての敵と全ての弾を画面に描画する処理を行う
void DrawEnemies(int cellSize) {
    for (const auto& e : enemies) {
        if (!e.active) continue;

        std::string char1, char2;
        if (e.type == ENEMY_YUMI)      { char1 = "yu"; char2 = "mi"; } // ゆみ
        else if (e.type == ENEMY_KEN)  { char1 = "ke"; char2 = "n";  } // けん
        else                           { char1 = "ta"; char2 = "te"; } // たて

        int px = (int)e.x;
        int py = (int)e.y;

        Color tint = EnemyDamageTint(e.hp, e.maxHp); // 現在のHPに応じたダメージの赤みの色合いを計算する

        // 敵が横方向から攻めてくる場合は二つの文字を横並びにし、縦方向から攻めてくる場合は二つの文字を縦並びにする
        if (e.spawnSide == SIDE_LEFT || e.spawnSide == SIDE_RIGHT) {
            DrawBlackCellContentColored(char1, px,            py, cellSize, tint);
            DrawBlackCellContentColored(char2, px + cellSize, py, cellSize, tint);
        } else {
            DrawBlackCellContentColored(char1, px, py,            cellSize, tint);
            DrawBlackCellContentColored(char2, px, py + cellSize, cellSize, tint);
        }
    }

    for (const auto& b : bullets) {
        if (!b.active) continue;
        if (b.fromCell) {
            DrawCircle((int)b.x, (int)b.y, 5, RED); // 味方の弾は半径5の赤色
        } else {
            DrawCircle((int)b.x, (int)b.y, 7, BLUE); // 敵の弾は半径7の青色
        }
    }
}
