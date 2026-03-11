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

配布用ZIPファイル（`MojiWars_win.zip`）を入手して、以下の手順で実行します。

**必要環境：** Windows（WSL2不要）

1. `MojiWars.zip` を任意のフォルダに展開する
2. `MojiWars/MojiWars.exe` をダブルクリックして起動する

---

### パターン2：GitHubから実行（WSL2 / Linux）

**必要環境：**
- Linux または Windows（WSL2）
- raylib 6.x系

**raylibのインストール（Ubuntu / WSL2）：**
```bash
sudo apt update
sudo apt install libraylib-dev
```

バージョンが古い（6.x未満）場合は [raylib 公式](https://github.com/raysan5/raylib/releases) からソースをビルドしてください。

**実行手順：**
```bash
git clone https://github.com/RatHorse200/MojiWars.git
cd MojiWars
./game
```

> **注意：** 現在、GitHubリポジトリ版では音楽（BGM・SE）が正常に再生されないバグがあります。

## ディレクトリファイルの階層構造

```
Game/
├── assets/
│   ├── black_ch/         # ゲームプレイ用以外（タイトル画面、チュートリアル画面、敵）の文字スプライト
│   │   ├── exclamation.png
│   │   ├── ellipsis.png
│   │   └── ...
│   ├── music/            # ゲームのSEとBGM
│   │   ├── maou_bgm_cyber44.ogg
│   │   ├── maou_bgm_cyber45.ogg
│   │   └── ...
│   ├── red_ch/           # ゲームプレイ用ひらがなスプライト
│   │   ├── a.png
│   │   ├── i.png
│   │   └── ...
│   ├── char_fire.txt     # どの文字がどの位置から弾を発射するかをまとめたテキストファイル(char_fire.cppのプログラムで作成)
│   ├── char_scale.txt    # どの文字がどれくらいのスケールで伸び縮みや縮小しているかをまとめたテキストファイル(char_scale.cppのプログラムで作成)
│   ├── tutorial.txt      # チュートリアルの文章が全文書かれてある
│   ├── 1.png             # スクリーンショット1
│   ├── 2.png             # スクリーンショット2
│   ├── 3.png             # スクリーンショット3
│   └── demo.mp4          # デモプレイ動画
├── audio.cpp
├── audio.h
├── board.cpp
├── board.h
├── char_fire.cpp
├── char_fire.h
├── constants.h
├── enemy.cpp
├── enemy.h
├── fire_editor.cpp
├── main.cpp
├── Makefile
├── README.md
├── romaji.cpp
├── romaji.h
├── sprites.cpp
├── sprites.h
├── title.cpp
├── title.h
├── tutorial.cpp
├── tutorial.h
└── viewer_editor.cpp
```

```
music/
├── maou_bgm_cyber44.ogg    # タイトル画面とチュートリアル画面のBGM
├── maou_bgm_cyber45.ogg    # ゲームプレイ中のBGM
├── ひらめく2.mp3            # ステージクリア時のSE
├── 歓声と拍手.mp3           # ゲームクリア時のSE
├── 間抜け4.mp3              # ゲームオーバー時のSE
├── 決定ボタンを押す2.mp3　　 # 左クリック時のSE
└── 決定ボタンを押す50.mp3    # 文字を書いて置く時のSE
```

```
assets/black_ch/
├── 0.png
├── 1.png
├── 2.png
├── 3.png
├── 4.png
├── 5.png
├── 6.png
├── 7.png
├── 8.png
├── 9.png
├── a.png
├── arrow_down.png
├── ba.png
├── be.png
├── bi.png
├── bo.png
├── bu.png
├── chi.png
├── chouon.png
├── da.png
├── de.png
├── di.png
├── do.png
├── du.png
├── e.png
├── ellipsis.png
├── exclamation.png
├── fu.png
├── ga.png
├── ge.png
├── gi.png
├── go.png
├── gu.png
├── ha.png
├── he.png
├── hi.png
├── hidari.png
├── ho.png
├── i.png
├── ji.png
├── ka.png
├── ke.png
├── ki.png
├── ko.png
├── ku.png
├── ma.png
├── me.png
├── mi.png
├── migi.png
├── mo.png
├── mu.png
├── n.png
├── na.png
├── ne.png
├── ni.png
├── no.png
├── nu.png
├── o.png
├── pa.png
├── pe.png
├── pi.png
├── po.png
├── pu.png
├── ra.png
├── re.png
├── ri.png
├── ro.png
├── ru.png
├── sa.png
├── se.png
├── shi.png
├── so.png
├── su.png
├── ta.png
├── te.png
├── to.png
├── tsu.png
├── u.png
├── wa.png
├── wo.png
├── xa.png
├── xe.png
├── xi.png
├── xo.png
├── xtu.png
├── xu.png
├── xya.png
├── xyo.png
├── xyu.png
├── ya.png
├── yo.png
├── yu.png
├── za.png
├── ze.png
├── zo.png
└── zu.png
```

```
assets/red_ch/
├── a.png     # 'a' を入力したとき表示
├── ba.png    # 'ba' を入力したとき表示
├── be.png    # 'be' を入力したとき表示
├── bi.png    # 'bi' を入力したとき表示
├── bo.png    # 'bo' を入力したとき表示
├── bu.png    # 'bu' を入力したとき表示
├── chi.png   # 'chi' または 'ti' を入力したとき表示
├── da.png    # 'da' を入力したとき表示
├── de.png    # 'de' を入力したとき表示
├── di.png    # 'di' を入力したとき表示
├── do.png    # 'do' を入力したとき表示
├── du.png    # 'du' を入力したとき表示
├── e.png     # 'e' を入力したとき表示
├── fu.png    # 'fu' または 'hu' を入力したとき表示
├── ga.png    # 'ga' を入力したとき表示
├── ge.png    # 'ge' を入力したとき表示
├── gi.png    # 'gi' を入力したとき表示
├── go.png    # 'go' を入力したとき表示
├── gu.png    # 'gu' を入力したとき表示
├── ha.png    # 'ha' を入力したとき表示
├── he.png    # 'he' を入力したとき表示
├── hi.png    # 'hi' を入力したとき表示
├── ho.png    # 'ho' を入力したとき表示
├── i.png     # 'i' を入力したとき表示
├── ji.png    # 'ji' または 'zi' を入力したとき表示
├── ka.png    # 'ka' を入力したとき表示
├── ke.png    # 'ke' を入力したとき表示
├── ki.png    # 'ki' を入力したとき表示
├── ko.png    # 'ko' を入力したとき表示
├── ku.png    # 'ku' を入力したとき表示
├── ma.png    # 'ma' を入力したとき表示
├── me.png    # 'me' を入力したとき表示
├── mi.png    # 'mi' を入力したとき表示
├── mo.png    # 'mo' を入力したとき表示
├── mu.png    # 'mu' を入力したとき表示
├── n.png     # 'nn' を入力したとき表示
├── na.png    # 'na' を入力したとき表示
├── ne.png    # 'ne' を入力したとき表示
├── ni.png    # 'ni' を入力したとき表示
├── no.png    # 'no' を入力したとき表示
├── nu.png    # 'nu' を入力したとき表示
├── o.png     # 'o' を入力したとき表示
├── pa.png    # 'pa' を入力したとき表示
├── pe.png    # 'pe' を入力したとき表示
├── pi.png    # 'pi' を入力したとき表示
├── po.png    # 'po' を入力したとき表示
├── pu.png    # 'pu' を入力したとき表示
├── ra.png    # 'ra' を入力したとき表示
├── re.png    # 're' を入力したとき表示
├── ri.png    # 'ri' を入力したとき表示
├── ro.png    # 'ro' を入力したとき表示
├── ru.png    # 'ru' を入力したとき表示
├── sa.png    # 'sa' を入力したとき表示
├── se.png    # 'se' を入力したとき表示
├── shi.png   # 'shi' または 'si' を入力したとき表示
├── so.png    # 'so' を入力したとき表示
├── su.png    # 'su' を入力したとき表示
├── ta.png    # 'ta' を入力したとき表示
├── te.png    # 'te' を入力したとき表示
├── to.png    # 'to' を入力したとき表示
├── tsu.png   # 'tsu' または 'tu' を入力したとき表示
├── u.png     # 'u' を入力したとき表示
├── wa.png    # 'wa' を入力したとき表示
├── wo.png    # 'wo' を入力したとき表示
├── ya.png    # 'ya' を入力したとき表示
├── yo.png    # 'yo' を入力したとき表示
├── yu.png    # 'yu' を入力したとき表示
├── za.png    # 'za' を入力したとき表示
├── ze.png    # 'ze' を入力したとき表示
├── zo.png    # 'zo' を入力したとき表示
└── zu.png    # 'zu' を入力したとき表示
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