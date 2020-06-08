#pragma once

#include "ChangeArrayNumber.h"
#include "CRC.h"
#include <string.h>

struct Si {
    UINT8* Ts = nullptr;
    Si* next = nullptr;
};

class shareRandD {
public:
    int sicount = 0;//r タグの数
    int siunique = 0;//siの数
    int sicount_place = 0;
    int siunique_place = 0;
    int mycount = 0;//string index 0始り
    const char* tagT = "<t";
    const char* tagSi = "<si>";
    const char* esi = "</si>";//5
    const char* esst = "</sst>";//6
    UINT8* sd;
    UINT64 sdlen;
    UINT8* countstr = nullptr;
    UINT8* uniqstr = nullptr;
    int writeleng = 0;
    UINT8** Sharestr = nullptr;
    Si** sis = nullptr;//Si配列
    int Tcount = 0;//Si 構造体中　t 数

    ArrayNumber st;
    crc Crc;

    shareRandD(UINT8* d, UINT64 l);
    ~shareRandD();
    void getSicount();//unique and count get
    UINT8* writeshare(UINT8* instr, int instrlen);//share str plus
    Si* addSitable(Si* s, UINT8* str);
    void ReadShare();//si<t>文字列配列へ保存
};
