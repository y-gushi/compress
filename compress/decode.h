#pragma once

#include "typechange.h"
#include "bitstream.h"
#include "crc.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#define BASE 65521
struct decodenode { /*木のノード*/
    UINT32 fugo = 0;/*テキストへのポインタ*/
    UINT32 fugolen = 0;
    UINT32 val = 0;
    UINT32 count = 0;
    struct decodenode* left = nullptr;
    struct decodenode* right = nullptr;
};

struct DecSelect;

class Hclen {
public:
    UINT32 hclenv;
    UINT32 huffHclen;
    UINT32 hufflen;
    Hclen();
    void dswap(Hclen v[], int n);
    void dmakehuff(Hclen h[]);
};

class DeflateDecode {
private:
    std::ifstream* fr;
public:
    crc cr;
    unsigned int crc;
    unsigned char* ReadV;
    unsigned char* ReadVroot;
    unsigned char rev;//File readデータ
    UINT32 length = 0;
    UINT32 left = 0, right = 0;//符号の符号長最小値、場所　/ハフマン符号長最小値、最大値
    UINT32 count = 0;
    UINT32 k = 0;
    UINT32 stock = 0;
    UINT32 n = 1;
    UINT32 pu = 1;
    UINT32 hu = 0;
    UINT32 miles = 0;//長さ符号の値
    UINT32 len = 0;//距離付合の値
    unsigned int adler32 = 0;//アドラー読み込み値の格納

    UINT8 BFINAL;//1bit　データの最後のブロックである場合にのみ設定
    UINT8 BTYPE;//2bit ビット反転01固定ハフマン 10カスタムハフマン

    unsigned long adlerR = 1L;//adler計算値

    unsigned short HLIT;
    unsigned short HDIST;
    unsigned char HCLEN;

    Hclen obh[19];
    Hclen h;
    //std::vector<unsigned int> hufftable;//符号テーブル
    UINT32* hufftable = nullptr;
    UINT32 hufftablepos = 0;

    UINT32* lentable = nullptr;
    UINT32 lentablepos = 0;
    UINT32* hufval = nullptr;
    UINT32* lenval = nullptr;
    UINT32* lenh = nullptr;
    UINT32* lenlen = nullptr;

    unsigned int* signs;
    UINT32 signSize;

    struct decodenode* root = NULL;
    struct decodenode* lenroot = NULL;
    struct decodenode* hufhuf = NULL;

    UINT32 readlen;//読み込んだデータのサイズ
    bitstream bit;

    DeflateDecode(std::ifstream* f);
    ~DeflateDecode();
    struct decodenode* dalloc(void);
    struct decodenode* daddtree(struct decodenode* p, UINT32 w, UINT32 a, UINT32 b);
    void freetree(decodenode* p);
    UINT32* copy(UINT32 s);
    void dtreeprint(struct decodenode* p);
    struct decodenode* dsearchVal(struct decodenode* n);
    UINT32 dsearch_node(UINT32 x, struct decodenode* node, UINT32 u);
    UINT64 dataread(UINT64 position, UINT32 size);
    void headdataread();
    int haffmanread();//ヘッダー&符号の読み込み
    void milesearch(UINT32 mil);
    void distancesearch(UINT32 dis);
    void byteread();//1バイト読み込み
    DeflateDecode* memorykeep(DeflateDecode* d);//デコード構造体　クラス領域を確保
    char* copyfn(char* f);
    //DecSelect* nextadd(DecSelect* ds, char fn[], UINT64 po,char *d,UINT32 c);//nextにプラス
    DecSelect* nextadd(DecSelect* ds, DeflateDecode* decdata, char* fn);

    UINT32 searchLiteral(unsigned int n, UINT8 leng);
    void FixedRead();
    void CustomRead();
    void noCompressRead();
};

//デコードデータの構造体
struct DecSelect
{
    DeflateDecode* decorddata;//decord object
    char* Fn;// filename
    DecSelect* next;//next struct
};
