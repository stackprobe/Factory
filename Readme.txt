Factory
=======

Cで書いたツールとライブラリの寄せ集めです。
必要に応じて必要な機能を書き足し続けてきたものなので雑然としています。
実行形式を含むアーカイブは以下の場所から落とせます。

	http://stackprobe.dip.jp/_kit/Factory


開発環境
--------

Windows 7 Home Premium
Visual C++ 2010 Express Edition (たぶん Visual C++ 2008 Express Edition でもOK)

システムドライブ = C


一時ファイルなど
----------------

一部のプログラムが以下のパス名を一時ファイルとして使用します。

	C:\1, C:\2, C:\3, ... C:\999

	★ビルド手順中の fcrlf がこのパスを生成し、zz がこれらのパスを削除します。


習慣的に以下のディレクトリを使用します。

	C:\app       = フリーソフト等、外部アプリ置き場
	C:\app\Kit   = https://github.com/stackprobe/Kit/tree/master/
	C:\appdata   = APP_DATA
	C:\temp      = 一時ファイル用、ログオン時に空にする。
	C:\tmp       = 一時ファイル用、クリアしない。


ビルド手順
----------

1. 全てのファイルをローカルに展開する。

	このファイルが C:\Factory\Readme.txt となるように配置する。

2. コンソールを開く。

	OpenConsole.bat を実行する。

3. 全てコンパイル・リンク（以下のコマンドを実行する）

	> cd build\_cx
	> rebuild
	> ff
	> cx **

4. 改行コードをCR-LFにする（以下のコマンドを実行する）

	> fcrlf

5. 一時ファイルを削除する（以下のコマンドを実行する）

	> zz


補足
----

本リポジトリ内では /J コンパイラオプションにより char は unsigned char になります。
