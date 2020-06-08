#pragma once

#include <stdio.h>
#include "typechange.h"
#include <stdlib.h>
#include <math.h>

class ArrayNumber {
public:
    UINT8* InttoChar(UINT32 num, int* pl);//行数intを配列に

    UINT32 ColumnArraytoNumber(UINT8* colum, int colplace);//列配列を数字へ（文字数値）

    UINT8* ColumnNumtoArray(UINT32 col, int* colplace);//列数int32を配列に

    UINT32 ColumnCharnumtoNumber(UINT32 col);//列　文字数字を数字に変換 主にcols用

    void ColumnIncliment(UINT32* c);//intの列番号を1+

    UINT32 NumbertoArray(UINT32 colnumber);//数字から文字数字に

    UINT32 declimentCol(UINT32 c);//文字数値一つ減らす

    UINT32 RowArraytoNum(UINT8* row, int rowpl);//行配列を数字に
};