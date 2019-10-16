Factory
=======

Cで書いたツールとライブラリの寄せ集めです。
必要に応じて必要な機能を書き足し続けてきたものなので雑然としています。
実行形式を含むアーカイブは以下の場所から落とせます。

	http://stackprobe.dip.jp/_kit/Factory


開発環境
--------

Windows 10 (Pro | Home)
Visual C++ 2010 Express

システムドライブ = C


一時ファイルなど
----------------

一部のプログラムが以下のパス名を一時ディレクトリとして使用します。

	C:\1, C:\2, C:\3, ... C:\999

	★ビルド手順中の fcrlf がこのディレクトリを生成し、zz がこれらのディレクトリを削除します。


開発環境では以下のディレクトリを使用しています。@ 2019.xx.xx
突然現れる謎のパスはだいたいこいつらです。

	C:\app         = フリーソフト等、外部アプリ置き場
	C:\app2        = フリーソフト等、外部アプリ置き場２
	C:\app\Kit     = https://github.com/stackprobe/Kit/tree/master/
	C:\appdata     = APP_DATA や ProgramData みたいな場所
	C:\Dat         = C:\Devで使う共通データ
	C:\Dev         = いろいろな開発環境
	C:\etc         = 第１の物置
	C:\Factory     = ここ
	C:\home        = 第３の物置
	C:\huge        = 第５の物置 (GitHub関係専用)
	C:\huge\GitHub = リポジトリ置き場
	C:\pleiades    = Pleiades All in One Java? Full Edition?
	C:\prv         = subst S: C:\prv
	C:\temp        = 一時ファイル用、ログオン時に空にする。
	C:\tmp         = 一時ファイル用、クリアしない。
	C:\var         = 第２の物置
	C:\var2        = 第６の物置
	S:\            = 第４の物置 (元はネットワークドライブ、使わなくなって形骸化)


ビルド手順
----------

-1. Windows 10 Pro 又は Windows 7 Professional 環境を用意する。

	-1.1. アップデートを全て適用する。

0. Visual C++ 2010 Express をインストールする。

	0.1. アップデートを全て適用する。

1. 全てのファイルをローカルに展開する。

	このファイルが C:\Factory\Readme.txt となるように配置する。

2. コンソールを開く。

	2.1. OpenConsole.bat を実行する。

3. 以下のコマンドを実行する。-- 全てのソースをコンパイル・リンクする。

	> cd build\_cx
	> rebuild
	> ff
	> cx **

4. 以下のコマンドを実行する。-- 改行コードをCR-LFにする。

	> fcrlf

5. 以下のコマンドを実行する。-- 一時ファイルを削除する。

	> zz


補足
----

/J コンパイラオプションにより char は unsigned char になります。

習慣で unsigned を多用しています。

