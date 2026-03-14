## ゲームについて
ゲームタイトル「もじうぉーず」  
2025年度　東京理科大学　RICORAプログラミングサークル　理大祭展示作品  
C++のライブラリであるraylibを使用した2D見下ろし型タワーディフェンスゲーム

## スクリーンショット

![スクリーンショット1](assets/1.png)
![スクリーンショット2](assets/2.png)
![スクリーンショット3](assets/3.png)

## デモプレイ

[![デモプレイ動画](https://img.youtube.com/vi/NdsWEiACGhM/0.jpg)](https://www.youtube.com/watch?v=NdsWEiACGhM)

## 動作環境
対応機種：PC（Windows上のWSL2 / Linux）  
ゲーム画面：1216×896  
（補足：プレイする際はペンとメモ用紙を用意することを推奨します）  

## 使用素材
使用したSE：効果音ラボ  
使用したBGM：魔王魂

## 実行方法

### パターン1：Windows向け配布版（推奨）

[Releases](https://github.com/RatHorse200/MojiWars/releases) から `MojiWars_windows.zip` を入手して、以下の手順で実行します。

**必要環境：** Windows（WSL2不要）

1. `MojiWars_windows.zip` を任意のフォルダに展開する
2. `MojiWars_windows.exe` をダブルクリックして起動する

---

### パターン2：Linux向け配布版

[Releases](https://github.com/RatHorse200/MojiWars/releases) から `MojiWars_linux.tar.gz` を入手して、以下の手順で実行します。

**必要環境：** Linux または Windows（WSL2）

```bash
tar -xzf MojiWars_linux.tar.gz
chmod +x MojiWars_linux
./MojiWars_linux
```

---

### パターン3：GitHubからビルドして実行（WSL2 / Linux）

**必要環境：**
- Linux または Windows（WSL2）
- raylib 6.x系

**raylibのインストール（Ubuntu / WSL2）：**
```bash
sudo apt update
sudo apt install libraylib-dev
```

バージョンが古い（6.x未満）場合は [raylib 公式](https://github.com/raysan5/raylib/releases) からソースをビルドしてください。

**ビルド・実行手順：**
```bash
git clone https://github.com/RatHorse200/MojiWars.git
cd MojiWars
make
./bin/MojiWars_linux
```

> **注意：** 現在、GitHubリポジトリ版では音楽（BGM・SE）が正常に再生されないバグがあります。

## ディレクトリ構造

```
MojiWars/
├── src/                  # ソースコード
│   ├── main.cpp
│   ├── audio.cpp / audio.h
│   ├── board.cpp / board.h
│   ├── enemy.cpp / enemy.h
│   ├── sprites.cpp / sprites.h
│   ├── title.cpp / title.h
│   ├── tutorial.cpp / tutorial.h
│   ├── char_fire.cpp / char_fire.h
│   ├── romaji.cpp / romaji.h
│   ├── constants.h
│   ├── fire_editor.cpp   # 発射位置設定ツール
│   └── viewer_editor.cpp # スケール調整ツール
├── assets/
│   ├── black_ch/         # タイトル・チュートリアル・敵の文字スプライト
│   ├── red_ch/           # ゲームプレイ用ひらがなスプライト
│   ├── music/            # BGM・SE
│   ├── char_fire.txt     # 文字ごとの弾発射位置データ
│   ├── char_scale.txt    # 文字ごとのスケールデータ
│   └── tutorial.txt      # チュートリアルテキスト
├── Makefile
└── README.md
```


## ゲーム画面説明

### スタート画面

「はじめる」を左クリック：チュートリアルへ  
「おわる」を左クリック：終了  
ESC：終了  

### チュートリアル画面

任意の箇所を左クリック：次のチュートリアル画面へ  
任意の箇所を右クリック：前のチュートリアル画面へ  
ESC：終了  

### ゲーム画面

左クリック：マス選択  
キーボード：ローマ字入力でひらがなを入力  
右クリック：文字を削除、選択取り消し  
ESC：終了   

## ゲームのチュートリアル
１．マウスの左クリックでカーソルが出る  
２．マス目にローマ字で文字を打とう  
３．文字を書く方向に弾が出る  
４．弾が敵に当たるとダメージを与える  
５．敵を全滅させ、全てのステージをクリアしよう  

マウスの右クリックで文字を消せる  
文字は敵の弾に当たると消える  
敵と敵の弾がフィールドの枠に当たるとダメージを受ける  
体力がゼロになるとゲームオーバー  
全５ステージ  

難易度選択  
かんたん；置ける文字は8文字まで、同じ文字は2個まで置ける  
むずかしい：置ける文字は5文字まで、同じ文字は1個しか置けない  

敵の種類  
ゆみ：遠くから攻撃して文字を消す　体力6 その場で静止する  
（仕様として、ゆみの発射する弾で受けるダメージは、敵ごとに10回まで。また、フィールド内の斜め方向で攻撃できる範囲にしかスポーンしない）  
けん：まっすぐ進む　体力：16　進む速度：0.25マス/s  
たて：正面から受けるダメージが低い、進むのが遅い　体力16　進む速度：0.125マス/s  

ヒント１：ゆみとたては斜めから攻撃するか、たくさんの文字で一気に倒そう  
ヒント２：強そうな平仮名は覚えておくか、紙にメモを取って必要な時に使えるようにしよう  

## ローマ字入力例
| 入力 | 結果 |
|------|------|
| a | あ |
| i | い |
| u | う |
| e | え |
| o | お |
| ka | か |
| ki | き |
| ku | く |
| ke | け |
| ko | こ |
| sa | さ |
| shi / si | し |
| su | す |
| se | せ |
| so | そ |
| ta | た |
| chi / ti | ち |
| tsu / tu | つ |
| te | て |
| to | と |
| na | な |
| ni | に |
| nu | ぬ |
| ne | ね |
| no | の |
| ha | は |
| hi | ひ |
| fu / hu | ふ |
| he | へ |
| ho | ほ |
| ma | ま |
| mi | み |
| mu | む |
| me | め |
| mo | も |
| ya | や |
| yu | ゆ |
| yo | よ |
| ra | ら |
| ri | り |
| ru | る |
| re | れ |
| ro | ろ |
| wa | わ |
| wo | を |
| nn | ん |
| ga | が |
| gi | ぎ |
| gu | ぐ |
| ge | げ |
| go | ご |
| za | ざ |
| ji / zi | じ |
| zu | ず |
| ze | ぜ |
| zo | ぞ |
| da | だ |
| di | ぢ |
| du | づ |
| de | で |
| do | ど |
| ba | ば |
| bi | び |
| bu | ぶ |
| be | べ |
| bo | ぼ |
| pa | ぱ |
| pi | ぴ |
| pu | ぷ |
| pe | ぺ |
| po | ぽ |