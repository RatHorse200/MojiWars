// ユーティリティベースAIと適応型難易度の実装をするファイル
// このプログラムは、『人工知能の作り方 ―「おもしろい」ゲームAIはいかにして動くのか』（三宅 陽一郎著）を参考にしています。
#include "ai.h"
#include "char_fire.h"
#include "constants.h"
#include "raylib.h"
#include <algorithm>

AIState aiState;

// AIの状態を初期化する
void ResetAI() {
    aiState.utilLeft          = 1.0f;
    aiState.utilRight         = 1.0f;
    aiState.utilBottom        = 1.0f;
    // TTK（敵の撃破時間）の初期値は14秒（中立値）で埋める
    for (int i = 0; i < TTK_WINDOW; i++) aiState.recentTTKs[i] = 14.0f;
    aiState.ttkIdx            = 0;
    aiState.ttkCount          = 0;
    aiState.spawnIntervalMult = 1.0f;
}

// 指定した方向に対する防御密度を計算する（0.0=無防備、1.0以上=十分な防御）
// 左側：左上 / 左 / 左下 の合計発射点数
// 右側：右上 / 右 / 右下 の合計発射点数
// 下側：左下 / 下 / 右下 の合計発射点数
static float CalcDefense(const GameBoard& board, SpawnSide side) {
    int firePoints = 0;
    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            const std::string& ch = board.cells[y][x];
            if (ch.empty()) continue;
            auto it = charFireConfig.find(ch);
            if (it == charFireConfig.end()) continue;
            for (const auto& fp : it->second) {
                bool counts = false;
                if (side == SIDE_LEFT   && (fp.direction == FIRE_UPPER_LEFT  || fp.direction == FIRE_LEFT  || fp.direction == FIRE_LOWER_LEFT))  counts = true;
                if (side == SIDE_RIGHT  && (fp.direction == FIRE_UPPER_RIGHT || fp.direction == FIRE_RIGHT || fp.direction == FIRE_LOWER_RIGHT)) counts = true;
                if (side == SIDE_BOTTOM && (fp.direction == FIRE_LOWER_LEFT  || fp.direction == FIRE_DOWN  || fp.direction == FIRE_LOWER_RIGHT)) counts = true;
                if (counts) firePoints++;
            }
        }
    }
    // 盤面上の全ひらがなについて、その発射方向がその辺を向いているものの数を合計し、25で割って正規化する
    return std::min(1.0f, (float)firePoints / (float)(GRID_SIZE * GRID_SIZE));
}

// AIの状態を毎フレーム更新する（ユーティリティスコアのみ）
void UpdateAI(const GameBoard& board) {
    // ユーティリティスコアを計算：防御密度が低い方向ほど高いスコアになる
    // 最小値0.1を設けることで、満員でも選ばれる可能性をゼロにしない
    aiState.utilLeft   = std::max(0.1f, 1.0f - CalcDefense(board, SIDE_LEFT));
    aiState.utilRight  = std::max(0.1f, 1.0f - CalcDefense(board, SIDE_RIGHT));
    aiState.utilBottom = std::max(0.1f, 1.0f - CalcDefense(board, SIDE_BOTTOM));
}

// 敵が撃破されたことをAIに通知し、適応型難易度を更新する
void NotifyEnemyKilled(float ttk) {
    // 直近で倒した数（TTK_WINDOW体）のTTKをローリングバッファで記録する
    aiState.recentTTKs[aiState.ttkIdx] = ttk;
    aiState.ttkIdx = (aiState.ttkIdx + 1) % TTK_WINDOW;
    if (aiState.ttkCount < TTK_WINDOW) aiState.ttkCount++;

    // 記録済み分の平均TTKを計算する
    float sum = 0.0f;
    int   n   = (aiState.ttkCount > 0) ? aiState.ttkCount : TTK_WINDOW;
    for (int i = 0; i < n; i++) sum += aiState.recentTTKs[i];
    float avgTTK = sum / (float)n;

    // 平均TTKに応じた目標倍率を決定する
    // 撃破が速い（< 8秒）：倍率を下げて間隔を短くする（難しくなる）
    // 撃破が遅い（> 20秒）：倍率を上げて間隔を長くする（易しくなる）
    // 普通（8〜20秒）： 倍率を標準に戻す
    float targetMult;
    if      (avgTTK < 8.0f)  targetMult = 0.85f;
    else if (avgTTK > 20.0f) targetMult = 1.3f;
    else                     targetMult = 1.0f;

    // 急激な変化を抑えるため、現在値と目標値の中間に緩やかに近づける
    aiState.spawnIntervalMult = aiState.spawnIntervalMult * 0.7f + targetMult * 0.3f;

    // 倍率を0.7〜1.5の範囲に制限する
    aiState.spawnIntervalMult = std::max(0.7f, std::min(1.5f, aiState.spawnIntervalMult));
}

// ユーティリティスコアを確率として使った重み付きランダムで、スポーン方向を決定する
// スコアが高い方向ほど選ばれやすいが、低い方向もゼロにはならない
SpawnSide GetAISpawnSide() {
    float total = aiState.utilLeft + aiState.utilRight + aiState.utilBottom;
    float r = (float)GetRandomValue(0, 10000) / 10000.0f * total;
    if (r < aiState.utilLeft)                     return SIDE_LEFT;
    if (r < aiState.utilLeft + aiState.utilRight) return SIDE_RIGHT;
    return SIDE_BOTTOM;
}

// スポーン間隔の倍率を返す
float GetAISpawnIntervalMult() {
    return aiState.spawnIntervalMult;
}
