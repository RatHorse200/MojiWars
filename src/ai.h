// ユーティリティベースAIと適応型難易度を管理する目次ファイル
// ユーティリティベースAI：防御が薄い方向から敵をスポーンさせる
// 適応型難易度：敵を撃破するまでの時間（TTK）に応じてスポーン間隔を変化させる
#pragma once
#include "enemy.h"
#include "board.h"

// 適応型難易度で参照する直近の撃破数
const int TTK_WINDOW = 5;

// AIの状態を管理する構造体
struct AIState {
    // ユーティリティベースAI用
    float utilLeft;    // 左方向へのスポーン重み（防御が薄いほど高い）
    float utilRight;   // 右方向へのスポーン重み
    float utilBottom;  // 下方向へのスポーン重み

    // 適応型難易度用
    float recentTTKs[TTK_WINDOW]; // 直近TTK_WINDOW体分の撃破時間（秒）
    int   ttkIdx;                 // 次に書き込むインデックス
    int   ttkCount;               // 記録済みの撃破数（TTK_WINDOW未満の場合に使用）
    float spawnIntervalMult;      // スポーン間隔の倍率（大きいほど間隔が長い＝易しい）
};

extern AIState aiState;

// AIの状態を初期化する
void ResetAI();

// AIの状態を毎フレーム更新する（ユーティリティスコアのみ）
void UpdateAI(const GameBoard& board);

// 敵が撃破されたことをAIに通知し、適応型難易度を更新する
void NotifyEnemyKilled(float ttk);

// ユーティリティスコアを確率として使った重み付きランダムで、スポーン方向を決定する
SpawnSide GetAISpawnSide();

// スポーン間隔の倍率を返す
float GetAISpawnIntervalMult();
