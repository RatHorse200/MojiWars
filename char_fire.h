// ひらがなごとの弾の発射位置・方向設定を管理するデータ構造と、操作関数の目次ファイル
#pragma once
#include <string>
#include <vector>
#include <map>

// 弾が飛ぶ七方向
enum FireDirection {
    FIRE_UPPER_LEFT  = 0,
    FIRE_LEFT        = 1,
    FIRE_LOWER_LEFT  = 2,
    FIRE_DOWN        = 3,
    FIRE_LOWER_RIGHT = 4,
    FIRE_RIGHT       = 5,
    FIRE_UPPER_RIGHT = 6,
};

// ゲーム本体とエディタで共有する設定
extern const char* FIRE_DIR_NAMES[7];

// 一つの弾をマスのどこからどの方向へ撃つかを表す構造体
struct FirePoint {
    int relX, relY;  
    int direction;
};

// ゲーム本体とエディタで共有する設定
extern std::map<std::string, std::vector<FirePoint>> charFireConfig;

void LoadCharFireConfig();
void SaveCharFireConfig();
void GetFireVelocity(int direction, float speed, float& dx, float& dy);
