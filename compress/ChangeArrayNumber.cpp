#include "ChangeArrayNumber.h"

//配列を数字に
UINT32 ArrayNumber::RowArraytoNum(UINT8* row, int rowpl) {
    UINT32 r = 0;

    for (int i = 0; i < rowpl; i++) {
        int p = (int)pow(10, (rowpl - 1 - i));
        r += ((row[i] - 48) * p);//0: 48- 9: 57
    }
    return r;
}
//行数intを配列に
UINT8* ArrayNumber::InttoChar(UINT32 num, int* pl)
{
    UINT32 five_place = 0;//万の位
    UINT32 four_place = 0;//千の位
    UINT32 three_place = 0;//百の位
    UINT32 two_place = 0;//十の位
    UINT32 one_place = 0;//一の位

    UINT8* index = nullptr;

    if (num / 10000 > 0) {
        index = (UINT8*)malloc(6);
        if (index) {
            five_place = num / 10000;
            index[0] = five_place + '0';

            four_place = num % 10000;
            four_place = four_place / 1000;
            index[1] = four_place + '0';

            three_place = num % 10000;
            three_place = three_place % 1000;
            three_place = three_place / 100;
            index[2] = three_place + '0';

            two_place = num % 10000;
            two_place = two_place % 1000;
            two_place = two_place % 100;
            two_place = two_place / 10;
            index[3] = two_place + '0';

            one_place = num % 10000;
            one_place = one_place % 1000;
            one_place = one_place % 100;
            one_place = one_place % 10;
            index[4] = one_place + '0';

            index[5] = '\0';
            *pl = 5;
        }
    }
    else if (num / 1000 > 0) {
        index = (UINT8*)malloc(5);
        if (index) {
            four_place = num / 1000;
            index[0] = four_place + '0';

            three_place = num % 1000;
            three_place = three_place / 100;
            index[1] = three_place + '0';

            two_place = num % 1000;
            two_place = two_place % 100;
            two_place = two_place / 10;
            index[2] = two_place + '0';

            one_place = num % 1000;
            one_place = one_place % 100;
            one_place = one_place % 10;
            index[3] = one_place + '0';

            index[4] = '\0';
            *pl = 4;
        }
    }
    else if (num / 100 > 0) {
        index = (UINT8*)malloc(4);
        if (index) {
            three_place = num / 100;
            index[0] = three_place + '0';

            two_place = num % 100;
            two_place = two_place / 10;
            index[1] = two_place + '0';

            one_place = num % 100;
            one_place = one_place % 10;
            index[2] = one_place + '0';

            index[3] = '\0';
            *pl = 3;
        }
    }
    else if (num / 10 > 0) {
        index = (UINT8*)malloc(3);
        if (index) {
            two_place = num / 10;
            index[0] = two_place + '0';

            two_place = num % 10;
            index[1] = two_place + '0';

            index[2] = '\0';
            *pl = 2;
        }
    }
    else {
        index = (UINT8*)malloc(2);
        if (index) {
            index[0] = num + '0';

            index[1] = '\0';
            *pl = 1;
        }
    }

    return index;
}
//列配列を数字へ（文字数値）
UINT32 ArrayNumber::ColumnArraytoNumber(UINT8* colum, int colplace) {
    UINT32 columnumber = 0;

    for (int i = 0; i < colplace; i++) {
        columnumber += colum[i] << (8 * (colplace - i - 1));
    }
    return columnumber;
}
//列数int32を配列に
UINT8* ArrayNumber::ColumnNumtoArray(UINT32 col, int* colplace) {
    UINT8* column = nullptr;

    if (col > 23130) {
        column = (UINT8*)malloc(4);
        if (column) {
            column[0] = (col >> 16) & 0xFF;
            column[1] = (col >> 8) & 0xFF;
            column[2] = col & 0xFF;
            column[3] = '\0';
            *colplace = 3;
        }
        else {

        }

    }
    else if (col > 90) {
        column = (UINT8*)malloc(3);
        if (column) {
            column[0] = (col >> 8) & 0xFF;
            column[1] = col & 0xFF;
            column[2] = '\0';
            *colplace = 2;
        }
        else {
        }
    }
    else {
        column = (UINT8*)malloc(2);
        if (column) {
            column[0] = col;
            column[1] = '\0';
            *colplace = 1;
        }
        else {
        }
    }
    return column;
}
//列　文字数字を数字に変換 主にcols用
UINT32 ArrayNumber::ColumnCharnumtoNumber(UINT32 col) {
    UINT32 columnumber = 0;
    if (col > 23130) {
        UINT32 upper = (col >> 16) & 0xFF;
        upper = (upper - 64) * 26 * 26;
        UINT32 middle = (col >> 8) & 0xFF;
        middle = (middle - 64) * 26;
        columnumber = upper + middle + ((col & 0xFF) - 64);
    }
    else if (col > 90) {
        UINT32 upper = (col >> 8) & 0xFF;
        columnumber = (upper - 64) * 26;
        columnumber += ((col & 0xFF) - 64);
    }
    else {
        columnumber = ((col - 64) & 0xFF);
    }
    return columnumber;
}
//数字から文字数字に
UINT32 ArrayNumber::NumbertoArray(UINT32 colnumber) {
    UINT32 num = 0;
    //26*26*x=676 3桁
    if ((colnumber / (26 * 26)) > 0) {
        UINT32 up = (colnumber / (26 * 26)) + 64;
        UINT32 midremain = colnumber % (26 * 26);
        UINT32 mid = (midremain / 26) + 64;
        UINT32 dow = (midremain % 26) + 64;
        num = up + mid + dow;
    }
    else if ((colnumber / 26) > 0) {//26*x 2桁
        UINT32 mid = (colnumber / 26) + 64;
        UINT32 dow = (colnumber % 26) + 64;
        num = mid + dow;
    }
    else {
        num = colnumber + 64;
    }
    return num;
}
//intの列番号を1+
void ArrayNumber::ColumnIncliment(UINT32* c)
{
    //A=65 Z=90
    //列インクリメント
    //UINT16 colcha = *c;
    UINT32 u = ((*c) >> 16) & 0xFF;
    UINT32 m = ((*c) >> 8) & 0xFF;
    UINT32 d = (*c) & 0xFF;

    //Z超えたら繰り上げ
    if (m == 90 && u > 64 && d == 90)
        u++;
    else if (m == 90 && u < 65 && d == 90)
    {
        u = 65; m = 65; d = 65;
    }
    else if (m > 64 && d == 90)
    {
        m++; d = 65;
    }
    else if (m < 65 && d == 90)
    {
        m = 65; d = 65;
    }
    else
    {
        d++;
    }//1桁目＋

    *c = (u << 16) + (m << 8) + d;
}
//文字数値一つ減らす 列
UINT32 ArrayNumber::declimentCol(UINT32 c)
{
    //A=65 Z=90
    UINT32 up = (c >> 16) & 0xFF;
    UINT32 mid = (c >> 8) & 0xFF;
    UINT32 down = c & 0xFF;
    UINT32 ud = 0;

    if (down == 65 && mid == 65 && up > 65) {
        up--; mid = 90; down = 90;
    }
    else if (down == 65 && mid == 65 && up == 65) {
        up = 0; mid = 90; down = 90;
    }
    else if (down == 65 && mid > 65) {
        mid--;
        down = 90;
    }
    else if (down == 65 && mid == 65) {
        mid = 0; down = 90;
    }
    else
        down--;

    ud = (up << 16) + (mid << 8) + down;

    return ud;
}