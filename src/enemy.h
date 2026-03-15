// 敵キャラクターと弾のデータ構造および戦闘管理関数の目次ファイル
#pragma once
#include "raylib.h"
#include "board.h"
#include <vector>

enum EnemyType  { ENEMY_YUMI, ENEMY_KEN, ENEMY_TATE };  // 敵の種類
enum SpawnSide  { SIDE_LEFT, SIDE_RIGHT, SIDE_BOTTOM }; // 敵が出現する方向

// 弾のデータ構造
struct Bullet {
    float x, y;         // 弾の現在位置
    float dx, dy;       // 1秒間にx方向、y方向にどれくらい進むか
    bool  fromYumi;     // 敵が撃った弾かどうか
    bool  fromCell;     // 味方のひらがなが撃った弾かどうか
    bool  active;       // この弾が現在ゲーム画面に存在しているか
    bool  hasHitField;  // 敵の弾がフィールドにダメージを与えているか
    int   ownerId;      // どのゆみがこの弾を撃ったか（味方の弾は-1）
};

// 敵のデータ構造
struct Enemy {
    EnemyType  type;    // この敵の種類
    float x, y;         // 敵の現在位置
    float dx, dy;       // 1秒間に進むスピードと向き
    int   hp;           // 現在の体力
    int   maxHp;        // 最大体力
    float shootTimer;   // 弾を撃つまでのタイマー（ゆみが使用する）
    bool  active;       // 敵が生きているかどうか
    SpawnSide spawnSide;// 敵がどの方向からスポーンしたか
    bool  hitFieldOnce; // 敵がフィールドの枠に接触してダメージを与え終わったかどうか
    int   id;           // この弾が誰の撃った弾かを識別するためのid
    int   fieldHitCount;// このゆみが撃った弾が既に何回フィールドにダメージを与えているか
    float spawnTime;    // この敵がスポーンした時刻（適応型難易度のTTK計測用）
};

extern std::vector<Enemy> enemies;      // ゲーム本体とエディタで共有する敵の設定
extern std::vector<Bullet> bullets;     // ゲーム本体とエディタで共有する弾の設定

// 新しい敵に番号を割り当てる関数
int AllocEnemyId();

// 敵を生成するための情報を決めてenemiesに追加する関数
void SpawnEnemy(int boardOffsetX, int boardOffsetY, int gridSize, int cellSize, int stage, int difficulty, SpawnSide forcedSide);

// すべての敵と弾の移動、射撃、当たり判定、そして不要になった文字の削除をする更新処理関数
void UpdateEnemies(float dt, int boardOffsetX, int boardOffsetY, int gridSize, int cellSize, int screenWidth, int screenHeight, GameBoard& board, bool dealFieldDamage = true);

// ひらがなから一定時間(2秒)ごとに自動で弾を撃つ迎撃処理
void FireCellBullets(float dt, GameBoard& board, int boardOffsetX, int boardOffsetY, int cellSize);

// 生きている全ての敵と全ての弾を画面に描画する処理を行う
void DrawEnemies(int cellSize);
