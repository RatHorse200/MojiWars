// ひらがなごとの弾の発射位置・方向設定の読み書きと、飛ぶ速度の計算を行うファイル
#include "char_fire.h"
#include <fstream>
#include <sstream>
#include <cmath>

// 弾が飛んでいく七つの方向の名前
const char* FIRE_DIR_NAMES[7] = {
    "upper-left", "left", "lower-left", "down", "lower-right", "right", "upper-right"
};

// 読み込んだ設定を記憶する辞書
// 一つのひらがなが複数の発射ポイント(FirePoint)を持っている
std::map<std::string, std::vector<FirePoint>> charFireConfig;

static const char* FIRE_CONFIG_FILE = "assets/char_fire.txt"; // 設定を保存、読み込みをするテキストファイル

// char_fire.txtから設定を読み込む関数
void LoadCharFireConfig() {
    charFireConfig.clear();
    std::ifstream f(FIRE_CONFIG_FILE);
    if (!f.is_open()) return;
    std::string line;
    while (std::getline(f, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream ss(line);
        std::string ch; int relX, relY, dir; // ひらがな、x座標、y座標、発射方向を抜き出す
        if (!(ss >> ch >> relX >> relY >> dir)) continue;
        charFireConfig[ch].push_back({relX, relY, dir});
    }
}

// 現在の設定をchar_fire.txtに保存する関数
void SaveCharFireConfig() {
    std::ofstream f(FIRE_CONFIG_FILE);
    f << "# ひらがなから弾が発射される座標と方向についてまとめたテキストファイルです。\n";
    f << "# フォーマット：ひらがな　x座標　y座標　方向\n";
    f << "方向: 0=左上 1=左 2=左下 3=下 4=右下 5=右 6=右上\n";
    f << "左上を基準（0, 0）とした相対的なピクセル座標 (値は0からCELL_SIZE-1の間となる)\n";
    for (auto& [ch, pts] : charFireConfig) {
        for (auto& p : pts) {
            f << ch << " " << p.relX << " " << p.relY << " " << p.direction << "\n";
        }
    }
}

// 弾の飛ぶ速度を計算する関数
// 実際に画面上で1フレームあたりX座標とY座標がどれだけ変化するかを計算する
void GetFireVelocity(int direction, float speed, float& dx, float& dy) {
    float diagSpeed = speed / sqrtf(2.0f);
    switch (direction) {
        case FIRE_UPPER_LEFT:  dx = -diagSpeed; dy = -diagSpeed; break;
        case FIRE_LEFT:        dx = -speed;     dy = 0.0f;       break;
        case FIRE_LOWER_LEFT:  dx = -diagSpeed; dy = diagSpeed; break;
        case FIRE_DOWN:        dx = 0.0f;       dy = speed;      break;
        case FIRE_LOWER_RIGHT: dx = diagSpeed;  dy = diagSpeed; break;
        case FIRE_RIGHT:       dx = speed;      dy = 0.0f;       break;
        case FIRE_UPPER_RIGHT: dx = diagSpeed;  dy = -diagSpeed; break;
        default:               dx = speed;      dy = 0.0f;       break;
    }
}
