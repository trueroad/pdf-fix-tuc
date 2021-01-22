<!-- -*- coding: utf-8 -*- -->
# PDF 内の ToUnicod CMap を修正してコピペの文字化けを防ぐツール

[ [English](Readme.md) / 日本語 (Japanese) ]

PDF から文字をコピペするとき、
PDF によっては「見」とか「高」などの漢字が
よく似ているけど違う文字（康熙部首や CJK 部首補助などの特殊な文字）
に化けてしまう、なんてことが結構あります。
このツールはそうした文字化けする PDF を修正し、
文字化けしない PDF を生成します。

## 注意

このツールで生成した PDF から元の
PDF へ逆変換することはできません（不可逆です）。
また、気が付かないところで PDF が壊れている可能性も否定できません。
元の PDF は必ずバックアップを残しておいてください。

## 使い方

```
$ pdf-fix-tuc
Fix ToUnicode CMap in PDF 0.0.0
Copyright (C) 2021 Masamichi Hosoda. All rights reserved.
License: BSD-2-Clause

https://github.com/trueroad/pdf-fix-tuc

Usage: pdf-fix-tuc [options] [INPUT.pdf OUTPUT.pdf] ...

  -h, --help
    Print help and exit
  -V, --version
    Print version and exit
  --verbose
    Verbose

Output PDF settings (QPDF):
  --linearize
    Output linearized (web-optimized) PDF
  --object-streams=[preserve|disable|generate]   (default=preserve)
    Settings for object streams
  --newline-before-endstream
    Output newline before endstream
  --qdf
    Output QDF

$
```

## インストール

### 必要なもの

* C++11 コンパイラ（g++ 4.9 以降等）
* [libqpdf](http://qpdf.sourceforge.net/)
* pkg-config 等
* Autoconf 2.69 以降
* Automake

パッケージを使って準備したい場合には、以下が便利でしょう。

* Debian / Ubuntu
    + libqpdf-dev
* Fedora
    + qpdf-devel
* Cygwin
    + libqpdf-devel

### ビルド・インストール方法

```
$ git clone https://github.com/trueroad/pdf-fix-tuc.git
$ cd pdf-fix-tuc
$ ./autogen.sh
$ mkdir build
$ cd build
$ ../configure
$ make
$ make install
```

## しくみ

### PDF のテキスト抽出（コピペ）

多くの PDF は文字を表現する際に、
Unicode のような文字コードではなく
CID (Character ID) や GID (Glyph ID) と呼ばれる
フォント内のグリフ（字形）を識別するコードが用いられています。

PDF を作るときには、使用するフォントの cmap テーブルを使って
Unicode のコードポイントを CID/GID へ変換し、それを PDF 内に記述します。
例えば「見」という漢字は Unicode では U+898B と表現されます。
この CID/GID が何になるかはフォントによって違うのですが、
[原ノ味フォント](https://github.com/trueroad/HaranoAjiFonts)
のような
[Adobe-Japan1](https://github.com/adobe-type-tools/Adobe-Japan1)
に従っているフォントの場合、対応する CID は CID+1887 になります。
つまり、PDF にする前は U+898B と表現されていた「見」を PDF にすると、
PDF 内部では CID+1887 として記録されます。
この PDF を表示や印刷する際には、CID+1887 を元に
フォントから必要なグリフを取り出して表示や印刷をします。

一方で、PDF からテキスト抽出（文字をコピペしたり検索したり等）する際には、
CID+1887 だけでは元の文字コードがわかりません。
そこで CID/GID から Unicode コードポイントへ変換できる
ToUnicode CMap という変換テーブルが用いられます。
多くの PDF では PDF 生成時に使用するフォント毎に ToUnicode CMap が生成され
PDF 内部に埋め込まれるため、これによってテキスト抽出が可能になっています。

### 文字化けの原因

「見」などをコピペすると文字化けする PDF は、
この ToUnicode CMap の生成方法に問題がある、
つまり PDF を作るのに使ったツールに問題があります。
ですが、メジャーどころも含めて多くのツールが同じ問題を抱えており、
こうしたツールで生成した PDF は同じ文字化けを起こしてしまいます
（こうした問題に配慮してコピペが文字化けしない PDF 生成ツールもあります）。

問題の一端は cmap テーブルの構造にあります。
cmap テーブルは Unicode コードポイントから CID/GID
へ変換するためのテーブルです。
これを逆変換するテーブルを作れば ToUnicode CMap にすることができます。
しかし、cmap テーブルは 1 対 1 対応になっていないため、
簡単に逆変換することができません。
「見」の場合、U+898B → CID+1887 というマッピングの他に、
U+2F92 → CID+1887 というマッピングが書かれています。
CID+1887 から逆変換しようとすると U+898B にすればよいのか
U+2F92 にすればよいのか、わからなくて困ってしまいます。
「見」の他にも、複数の Unicode コードポイントが一つの CID/GID
にマッピングされているものはたくさんあり、n 対 1 対応になっています。
なぜ 1 対 1 対応になっていないのかというと、Unicode
は見た目が同じでも由来が異なれば異なるコードポイントを割り当てるのに対して、
CID/GID はフォントの仕組みなので見た目（字形）が同じであれば
同じ ID を割り当てる、という方針の違いがあるからと言えると思います。

こうした n 対 1 対応がある、ということを考慮せず逆変換しようとして
CID+1887 に対応する Unicode コードポイントを
cmap テーブルから探すとどうなるでしょうか。
恐らく cmap テーブルを上から順番に検索して最初に見つかった CID+1887
に対応する Unicode コードポイントを返すでしょう。
cmap テーブルは Unicode コードポイントの昇順に並んでいますから、
最初に見つかるのは番号の小さい U+2F92 の方ということになります。
というわけで、元々は cmap テーブルで U+898B → CID+1887 を使ったのに
ToUnicode CMap には CID+1887 → U+2F92 が書かれてしまい、
元々 U+898B だった「見」が違う文字である U+2F92 に化けてしまう、
ということになります。

この U+2F92 というのは何かというと、
Unicode では 'KANGXI RADICAL SEE' という名前がついていて
'Kangxi Radicals' というブロックに入っています。
Kangxi Radicals というのは康熙部首（康煕部首）と呼ばれるもので、
普通の漢字ではなく部首を表す特殊な文字が収められているブロックになります。
特殊な文字ですから普通は使いませんし、
これらが収録されていないフォントもあります。
収録されていないフォントで表示しようとすると他のフォントで代替されたりして、
そこだけ変な表示になってしまいます。
また、普通の「見」で検索をかけると U+898B を探すことになり、
U+2F92 は違う文字なのでヒットしなくなってしまいます。

一方の U+898B は 'CJK Unified Ideographs' つまり
CJK 統合漢字のブロックに入っている普通の漢字です。
この問題のもう一つの一端は、
見た目が同じ文字に複数の Unicode
コードポイントが割り当てられているのは仕方ないにしても、
普通は使わない文字の方が小さい番号、
普通に使う漢字の方が大きい番号になっており、
逆変換で単純に小さい番号の方を採用すると化けてしまう、
というところにあります。

### 文字化けしない PDF 生成ツール

cmap テーブルの逆変換に頼らずに
元の Unicode コードポイントと CID/GID の関係を覚えておいて
ToUnicode CMap を生成するような PDF 生成ツールであれば、
コピペで文字化けしない PDF ができます
（ただし、一つの PDF で同じフォントに対して
U+2F92 と U+898B の両方が使われていたら区別できませんので、
どちらかに寄せなければならなくなります）。

また、cmap テーブルの逆変換で ToUnicode CMap を生成するような
PDF 生成ツールでも、n 対 1 対応を考慮して、
闇雲に最初に見つかった番号の小さい Unicode コードポイントを返すのではなく、
複数の候補の中から何らかの基準で優先順位をつけて、
よりふさわしい Unicode コードポイントを返すようにすれば、
ほとんどの場合でコピペが文字化けしなくなるハズです。

例えば XeTeX で使われる xdvipdfmx が PDF を生成する場合は、
xdvipdfmx の入力ファイル (.xdv) は既に CID/GID しか記載されておらず
元の Unicode コードポイントが失われた状態になっているため、
後者の方法でコピペが文字化けしない PDF を生成しています
（他にもフォントが Adobe-Japan1 の場合は ToUnicode CMap を埋め込まない
という動作でもコピペの文字化けを防いでおり、これと同様になるよう
PDF から Adobe-Japan1 フォントの ToUnicode CMap を削除するツール[
pdf-rm-tuc
](https://github.com/trueroad/pdf-rm-tuc)
もあります）。
優先度の決定は Unicode のコードポイントの範囲を区切って行っています。
ソースコードに入っている[
tt_cmap.c
](https://github.com/texjporg/tex-jp-build/blob/master/source/texk/dvipdfm-x/tt_cmap.c)
にある `is_PUA_or_presentation` 関数を見ると
以下のようになっていてその範囲がわかります。

```c
/* Soft-hyphen (U+00AD) to lower its priority... added here for convenience */
static int is_PUA_or_presentation (unsigned int uni)
{
  /* Some of CJK Radicals Supplement and Kangxi Radicals
   * are commonly double encoded, lower the priority.
   * CJK Compatibility Ideographs & Supplement added.
   */
  return  ((uni >= 0x2E80 && uni <= 0x2EF3) || (uni >= 0x2F00 && uni <= 0x2FD5) ||
           (uni >= 0xE000 && uni <= 0xF8FF) || (uni >= 0xFB00 && uni <= 0xFB4F) ||
           (uni >= 0xF900 && uni <= 0xFAFF) || (uni >= 0x2F800 && uni <= 0x2FA1F) ||
           (uni >= 0xF0000 && uni <= 0xFFFFD) || (uni >= 0x100000 && uni <= 0x10FFFD) ||
           (uni == 0x00AD));
}
```

「見」が化ける先の U+2F92 は康熙部首のブロックに入っており、
これを判定する `(uni >= 0x2F00 && uni <= 0x2FD5)`
に含まれているので優先度が低くなり採用されません。
その結果、ToUnicode CMap には正しい方の
CID+1887 → U+898B
が記載されるためコピペが文字化けしなくなるというわけです。
他にも康煕部首と同様の部首が収められている
'CJK Radicals Supplement' CJK 部首補助のブロックや、
'CJK Compatibility Ideographs' CJK 互換漢字、
'CJK Compatibility Ideographs Supplement' CJK 互換漢字補助、
'Private Use Area' 私用領域、
などが除外されるようになっています。

### 本ツールの動作

本ツールは、これまでに説明したようなコピペで文字化けする PDF の
ToUnicode CMap に手を入れてコピペが文字化けしないようにします。
具体的には ToUnicode CMap に 
CID+1887 → U+2F92
のような変換先が優先度の低い Unicode になっているマッピングを見つけたら、
変換先を優先度が高い Unicode に書き換え
CID+1887 → U+898B
のようにした PDF を出力します。

この書き換えを行うテーブルは自動生成したものをソースの[
tounicode_table.cc
](https://github.com/trueroad/pdf-fix-tuc/blob/master/src/tounicode_table.cc)
に入れてあります。これを自動生成するスクリプトもソースの[
build_table.py
](https://github.com/trueroad/pdf-fix-tuc/blob/master/src/build_table.py)
に同梱してあります（ただし、普通にビルドしても自動生成は発動しません）。
自動生成の方法は、[
Adobe-Japan1
](https://github.com/adobe-type-tools/Adobe-Japan1)
で配布されている漢字グリフの一覧表[
aj17-kanji.txt
](https://github.com/adobe-type-tools/Adobe-Japan1/blob/master/aj17-kanji.txt)
を使い、
同じ CID に複数の Unicode コードポイントが割り当てられてるものについて、
xdvipdfmx と同じ方法で優先度の低い Unicode コードポイントを判定し、
同じグリフ内で優先度が低い Unicode コードポイントを優先度が低くない
Unicode コードポイントへ書き換える、
というテーブルを生成しています。
ただし、今のところ BMP から
BMP 外へ書き換えるようなテーブルは生成しないようにしています。

Adobe-Japan1 を元にしてテーブルを生成しているため
非 Adobe-Japan1 フォントではうまくいかなくなる可能性もありますが、
ほとんどの日本語フォントは非 Adobe-Japan1 であっても
漢字グリフは Adobe-Japan1 のグリフセットをベースにしていると思いますので、
大抵は大丈夫だろうと思っています。

## ライセンス

Copyright (C) 2021 Masamichi Hosoda. All rights reserved.

License: BSD-2-Clause

[LICENSE](./LICENSE) をご覧ください。
