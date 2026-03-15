# ゲーム本編および各種開発用ツール（スケール調整、発射位置設定）をコンパイル・起動するためのビルド設定ファイル
# 2D見下ろし型タワーディフェンスゲーム - Makefile
# raylibを使用

# コンパイラ設定
CXX = g++
CXXFLAGS = -Wall -std=c++17 -O2 # -Wall:すべての警告を出す -02:プログラムの動作を高速化する最適化を行う

# ソースディレクトリ
SRCDIR = src

# raylib のリンク設定
LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# ターゲット
TARGET = bin/MojiWars_linux # ゲームの実行ファイル名
SOURCES = $(SRCDIR)/main.cpp $(SRCDIR)/romaji.cpp $(SRCDIR)/sprites.cpp $(SRCDIR)/board.cpp $(SRCDIR)/enemy.cpp $(SRCDIR)/title.cpp $(SRCDIR)/tutorial.cpp $(SRCDIR)/char_fire.cpp $(SRCDIR)/audio.cpp $(SRCDIR)/ai.cpp # ゲームを構成する全ての.cppファイル

# デフォルトターゲット
all: $(TARGET) # 単にmakeと入力されたら、メインゲームを作る作業を始める

# 実行ファイルの生成
$(TARGET): $(SOURCES)
	mkdir -p bin
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES) $(LDFLAGS)

# 実行　make runと入力されたら、ゲームをビルドした後に実行できる
run: $(TARGET)
	./$(TARGET)

# 文字サイズを調整するための開発用ツールを作るための手順 make run_viewerと入力してこのツールをコンパイルして起動する
viewer: viewer_editor
viewer_editor: $(SRCDIR)/viewer_editor.cpp
	$(CXX) $(CXXFLAGS) -o viewer_editor $(SRCDIR)/viewer_editor.cpp $(LDFLAGS)

run_viewer: viewer_editor
	./viewer_editor

# ひらがなから発射される弾の設定をするための開発用ツールを作るための手順　make run_fireと入力してこのツールをコンパイルして起動する
fire_editor: $(SRCDIR)/fire_editor.cpp $(SRCDIR)/char_fire.cpp $(SRCDIR)/sprites.cpp
	$(CXX) $(CXXFLAGS) -o fire_editor $(SRCDIR)/fire_editor.cpp $(SRCDIR)/char_fire.cpp $(SRCDIR)/sprites.cpp $(LDFLAGS)

run_fire: fire_editor
	./fire_editor

# クリーンアップ make cleanで作られた実行ファイルや中間ファイルをすべて削除して、フォルダを綺麗な状態に戻す
clean:
	rm -f $(TARGET) viewer_editor fire_editor

# 再ビルド rebuild
rebuild: clean all

# デバッグビルド バグ取り専用のモードでビルドする
debug: CXXFLAGS += -g -DDEBUG
debug: clean $(TARGET)

# これらはファイル名ではなくコマンドの名前であることを明示するための安全対策
.PHONY: all run clean rebuild debug viewer run_viewer fire_editor run_fire
