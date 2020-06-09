#pragma once
#include "bitstream.h"
#include "slidewindow.h"
#include "deflate.h"
#include "nodeandtab.h"
#include <stdio.h>

#define huff_of_huff_limit 5
#define mile_literal_limit 13

class encoding {
public:

    bitstream bits;

    unsigned short hht[19] = { 0 };
    UINT8 htc;//要素カウントよう
    UINT32 datalen;

    struct tab* fir;
    struct tab* lir;

    struct tnode** so;
    struct tnode** sso;

    struct tnode** lev;
    struct tnode** llev;

    struct tab* hh;//符号の符号テーブル
    struct tab* hhh;
    struct tnode** huh;//ポインタ 符号の符号テーブル　huh[hcc]　要素数定義に変数使えない
    struct tnode** huhc;//ポインタ 符号の符号テーブル　各数値参照用　huhc[hccc]　要素数定義に変数使えない

    UINT32 maxhh;

    unsigned char* encoderv;
    unsigned char* rvp;

    UINT8 hccc;
    struct tab* hhc;//符号の符号カウント用ポインタ
    UINT32 hcc;//数値の種類のカウント

    UINT32 tc;//ツリーの要素数
    UINT32 tcc;//ツリーの要素数　コピー用
    UINT32 lc;//距離値ツリーの要素数
    UINT32 lcc;//距離値ツリーの要素数　コピー用
    UINT32 maxlen;//符号の長さの最大値
    UINT32 lmaxlen;//距離符号の長さの最大値
    UINT32 k;//拡張ビットの確認用
    UINT32 mc;//長さの値のカウント
    UINT32 l;//距離の値のカウント
    UINT32 lb;//距離の拡張値のカウント

    encoding();
    ~encoding();
    void huhtable(struct tnode* a[], UINT8 b);
    UINT32* litrals(UINT32 d);
    void fixedcompress(slidewndow* s);
    void headerwrite(unsigned char HLIT, unsigned char HDIST, unsigned char BI, unsigned char BT);
    void makeSign(slidewndow* s, deflate* defl);
    void compress(unsigned char* data, UINT64 dataleng);
    void write(FILE* f);


};