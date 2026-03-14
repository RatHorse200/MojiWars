// ゲーム内のbgmやseの再生・管理を行うオーディオ機能の目次ファイル
#pragma once
#include "raylib.h"

enum BGMTrack { BGM_NONE = 0, BGM_TITLE, BGM_GAME }; // bgmの種類に番号をふる

void InitGameAudio();   // オーディオを初期化する関数
void CloseGameAudio();  // ゲーム終了直前に読み込んでいたすべての音楽と効果音のデータをメモリから消去する関数
void UpdateGameAudio();   // 毎フレーム、再生中のbgmを更新する関数
void SetBGM(BGMTrack track);    // bgmを切り替える関数

void PlaySE_Click();    // 左クリック音を再生する関数
void PlaySE_Place();    // ひらがな配置音を再生する関数
void PlaySE_StageClear();   // ステージクリア音を再生する関数
void PlaySE_GameClear();    // ゲームクリア音を再生する関数
void PlaySE_GameOver();     // ゲームオーバー音を再生する関数
void PlaySE_FieldDamage();  // フィールドのダメージ音を再生する関数
