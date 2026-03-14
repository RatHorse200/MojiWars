// プレイヤーのローマ字入力を正規のローマ字（ヘボン式）に変換するための辞書データと、入力途中の判定処理を管理するファイル
#include "romaji.h"

std::map<std::string, std::string> romaji_to_canonical;

// ローマ字テーブルを初期化する関数
void InitRomajiTable() {
    // 母音
    romaji_to_canonical["a"] = "a";
    romaji_to_canonical["i"] = "i";
    romaji_to_canonical["u"] = "u";
    romaji_to_canonical["e"] = "e";
    romaji_to_canonical["o"] = "o";

    // か行
    romaji_to_canonical["ka"] = "ka";
    romaji_to_canonical["ki"] = "ki";
    romaji_to_canonical["ku"] = "ku";
    romaji_to_canonical["ke"] = "ke";
    romaji_to_canonical["ko"] = "ko";

    // さ行
    romaji_to_canonical["sa"]  = "sa";
    romaji_to_canonical["si"]  = "shi";
    romaji_to_canonical["shi"] = "shi";
    romaji_to_canonical["su"]  = "su";
    romaji_to_canonical["se"]  = "se";
    romaji_to_canonical["so"]  = "so";

    // た行
    romaji_to_canonical["ta"]  = "ta";
    romaji_to_canonical["ti"]  = "chi";
    romaji_to_canonical["chi"] = "chi";
    romaji_to_canonical["tu"]  = "tsu";
    romaji_to_canonical["tsu"] = "tsu";
    romaji_to_canonical["te"]  = "te";
    romaji_to_canonical["to"]  = "to";

    // な行
    romaji_to_canonical["na"] = "na";
    romaji_to_canonical["ni"] = "ni";
    romaji_to_canonical["nu"] = "nu";
    romaji_to_canonical["ne"] = "ne";
    romaji_to_canonical["no"] = "no";

    // は行
    romaji_to_canonical["ha"] = "ha";
    romaji_to_canonical["hi"] = "hi";
    romaji_to_canonical["hu"] = "fu";
    romaji_to_canonical["fu"] = "fu";
    romaji_to_canonical["he"] = "he";
    romaji_to_canonical["ho"] = "ho";

    // ま行
    romaji_to_canonical["ma"] = "ma";
    romaji_to_canonical["mi"] = "mi";
    romaji_to_canonical["mu"] = "mu";
    romaji_to_canonical["me"] = "me";
    romaji_to_canonical["mo"] = "mo";

    // や行
    romaji_to_canonical["ya"] = "ya";
    romaji_to_canonical["yu"] = "yu";
    romaji_to_canonical["yo"] = "yo";

    // ら行
    romaji_to_canonical["ra"] = "ra";
    romaji_to_canonical["ri"] = "ri";
    romaji_to_canonical["ru"] = "ru";
    romaji_to_canonical["re"] = "re";
    romaji_to_canonical["ro"] = "ro";

    // わ行
    romaji_to_canonical["wa"] = "wa";
    romaji_to_canonical["wo"] = "wo";
    romaji_to_canonical["nn"] = "n";

    // 濁音 が行
    romaji_to_canonical["ga"] = "ga";
    romaji_to_canonical["gi"] = "gi";
    romaji_to_canonical["gu"] = "gu";
    romaji_to_canonical["ge"] = "ge";
    romaji_to_canonical["go"] = "go";

    // 濁音 ざ行
    romaji_to_canonical["za"] = "za";
    romaji_to_canonical["zi"] = "ji";
    romaji_to_canonical["ji"] = "ji";
    romaji_to_canonical["zu"] = "zu";
    romaji_to_canonical["ze"] = "ze";
    romaji_to_canonical["zo"] = "zo";

    // 濁音 だ行
    romaji_to_canonical["da"] = "da";
    romaji_to_canonical["di"] = "di";
    romaji_to_canonical["du"] = "du";
    romaji_to_canonical["de"] = "de";
    romaji_to_canonical["do"] = "do";

    // 濁音 ば行
    romaji_to_canonical["ba"] = "ba";
    romaji_to_canonical["bi"] = "bi";
    romaji_to_canonical["bu"] = "bu";
    romaji_to_canonical["be"] = "be";
    romaji_to_canonical["bo"] = "bo";

    // 半濁音 ぱ行
    romaji_to_canonical["pa"] = "pa";
    romaji_to_canonical["pi"] = "pi";
    romaji_to_canonical["pu"] = "pu";
    romaji_to_canonical["pe"] = "pe";
    romaji_to_canonical["po"] = "po";
}

// 入力した文字列が有効なローマ字入力の途中として正しいかどうか判定する関数
bool IsPrefixOfRomaji(const std::string& prefix) {
    for (const auto& pair : romaji_to_canonical) {
        if (pair.first.substr(0, prefix.length()) == prefix) {
            return true;
        }
    }
    return false;
}
