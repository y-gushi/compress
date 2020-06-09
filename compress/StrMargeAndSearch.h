#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cctype>
#include "TagAndItems.h"
#include "ChangeArrayNumber.h"

class MargeaSearch {
public:
    MargeaSearch();

    ArrayNumber cn;//数値　配列変換
    UINT8* tripleStrmarge(UINT8* left, UINT8* center, UINT8* right);//３つの文字配列を結合
    ColorAnSize* splitColor(UINT8* cstr);//色　サイズ　分割
    UINT8* margeRowTag(UINT32 r, UINT8* t, int* len);//行数値と文字列の結合
    UINT8* margeRowAndCol(UINT32 row, UINT32 col, UINT8* tag, int* strlen);//列行結合　数値同士
    int seachcharactors(UINT8* str, UINT8* data, UINT32 dataleng, int startpos);//文字列検索
    UINT8* charmarge(UINT8* a, UINT8* b, int al, int bl);//2 文字列結合
    int charlengsearch(UINT8* c);
};