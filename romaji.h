// プレイヤーのローマ字入力を正規のローマ字（ヘボン式）に変換するための辞書データと判定関数の目次ファイル
#pragma once
#include <string>
#include <map>

// ゲーム本体とエディタで辞書を共有する設定
extern std::map<std::string, std::string> romaji_to_canonical;

// ローマ字テーブルを初期化する関数
void InitRomajiTable();

// 入力した文字列が有効なローマ字入力の途中として正しいかどうか判定する関数
bool IsPrefixOfRomaji(const std::string& prefix);
