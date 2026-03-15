// ゲームのbgmとseの読み込み、再生、切り替え、メモリ解放を行うファイル
#include "audio.h"
#include "raylib.h"

static Music bgmTitle = {};     // タイトル画面とチュートリアル画面のbgm
static Music bgmGame = {};      // ゲームプレイ中のbgm
static BGMTrack currentBGM  = BGM_NONE;     // 現在再生中のbgmトラック

static Sound seClick = {};    // 左クリック音
static Sound sePlace = {};    // ひらがな配置音
static Sound seStageClear = {};    // ステージクリア音
static Sound seGameClear = {};    // ゲームクリア音
static Sound seGameOver = {};    // ゲームオーバー音
static Sound seFieldDamage = {};    // フィールドのダメージ音

// オーディオを初期化する関数
void InitGameAudio() {
    SetAudioStreamBufferSizeDefault(4096 * 8);  // ゲームの処理が重くなった瞬間に音がぷつっと途切れるのを防ぐため、オーディオバッファを大きくする
    InitAudioDevice();

    bgmTitle = LoadMusicStream("assets/music/maou_bgm_cyber44.ogg");
    bgmGame = LoadMusicStream("assets/music/maou_bgm_cyber45.ogg");
    bgmTitle.looping = true;
    bgmGame.looping = true;
    SetMusicVolume(bgmTitle, 0.15f);     // BGMの音量を小さくする
    SetMusicVolume(bgmGame, 0.15f);     // BGMの音量を小さくする

    seClick = LoadSound("assets/music/se_click.ogg");
    sePlace = LoadSound("assets/music/se_place.ogg");
    seStageClear = LoadSound("assets/music/se_stage_clear.ogg");
    seGameClear = LoadSound("assets/music/se_game_clear.ogg");
    seGameOver = LoadSound("assets/music/se_game_over.ogg");
    seFieldDamage = LoadSound("assets/music/se_field_damage.ogg");
    SetSoundVolume(sePlace, 2.0f);       // ひらがな配置音を大きくする
    SetSoundVolume(seFieldDamage, 0.5f); // フィールドダメージ音の音量
}

// ゲーム終了直前に読み込んでいたすべての音楽と効果音のデータをメモリから消去する関数
void CloseGameAudio() {
    StopMusicStream(bgmTitle);  // 再生中のbgmを止める
    StopMusicStream(bgmGame);   // 再生中のbgmを止める
    UnloadMusicStream(bgmTitle);
    UnloadMusicStream(bgmGame);
    UnloadSound(seClick);
    UnloadSound(sePlace);
    UnloadSound(seStageClear);
    UnloadSound(seGameClear);
    UnloadSound(seGameOver);
    UnloadSound(seFieldDamage);
    CloseAudioDevice();     // オーディオデバイスとの通信を遮断する
}

// 毎フレーム、再生中のbgmを更新する関数
void UpdateGameAudio() {
    if (currentBGM == BGM_TITLE) UpdateMusicStream(bgmTitle);
    else if (currentBGM == BGM_GAME) UpdateMusicStream(bgmGame);
}

// BGMを切り替える関数
void SetBGM(BGMTrack track) {
    if (track == currentBGM) return;

    if      (currentBGM == BGM_TITLE) StopMusicStream(bgmTitle);
    else if (currentBGM == BGM_GAME) StopMusicStream(bgmGame);

    currentBGM = track;

    if      (track == BGM_TITLE) PlayMusicStream(bgmTitle);
    else if (track == BGM_GAME) PlayMusicStream(bgmGame);
}

// 各瞬間に効果音を鳴らす関数
void PlaySE_Click() { StopSound(seClick); PlaySound(seClick); }     // 左クリック音を再生する関数
void PlaySE_Place() { StopSound(sePlace); PlaySound(sePlace); }     // ひらがな配置音を再生する関数
void PlaySE_StageClear() { StopSound(seStageClear); PlaySound(seStageClear); }      // ステージクリア音を再生する関数
void PlaySE_GameClear() { StopSound(seGameClear); PlaySound(seGameClear); }     // ゲームクリア音を再生する関数
void PlaySE_GameOver() { StopSound(seGameOver); PlaySound(seGameOver); }     // ゲームオーバー音を再生する関数
void PlaySE_FieldDamage() { StopSound(seFieldDamage); PlaySound(seFieldDamage); }     // フィールドのダメージ音を再生する関数
