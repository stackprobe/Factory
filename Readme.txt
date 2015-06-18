Factory
=======

Cで書いたツールとライブラリの寄せ集めです。
必要に応じて必要な機能を書き足し続けてきたものなので雑然としています。
実行形式を含むアーカイブは以下の場所から落とせます。

	http://stackprobe.dip.jp/_kit/Factory


開発環境
--------

	Windows 7 Home Premium
	Visual C++ 2008 Express Edition


ビルド手順
----------

1. 全てのファイルをローカルに展開する。

	このファイルが C:\Factory\Readme.txt となるように配置する。

2. コンソールを開く。

	OpenConsole.bat を実行する。

3. 以下のコマンドを実行する。(全てコンパイル・リンク)

	> cd build\_cx
	> rebuild
	> ff
	> cx **

4. 以下のコマンドを実行する。(改行コードをCR-LFにする)

	> fcrlf
