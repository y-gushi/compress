#pragma once

#include "typechange.h"
#include <stdlib.h>

#define windowsize 12288//8192//12288
#define LEVEL 12288

class slidewndow {
public:
    UINT32 Distancelength;//一致した距離

    UINT32 distanceexval;//距離拡張ビットの値
    UINT32 distanceexbit;//距離拡張ビットのビット数

    UINT32 mileexval;//長さ符号変換用の値
    UINT32 mileexbit;//長さ拡張符号のビット数

    UINT32* milestock;//変換した値の格納 before short
    UINT32* mileRoot;
    UINT32 milestocksize;
    UINT32* distancestock;//距離の変換した値 before short
    UINT32* distanceRoot;
    UINT32 distancestocksize;
    UINT32* mile_extension_stock;//長さ拡張ビット格納 before short
    UINT32* mile_extensionRoot;
    UINT32* distance_extenshon_stock;//距離拡張ビット格納 before short
    UINT32* distance_extenshonRoot;

    UINT32 maxmilestock;//ストックの最長
    UINT64 stocknum;//ストック数

    slidewndow();
    ~slidewndow();
    UINT32 slidesearch(unsigned char* p, UINT32 len, UINT32 spos);
    UINT32 CurposSearch(unsigned char* ps, UINT32 pp, UINT32 l);
    UINT32 MileBitnumSearch(UINT32 l);//長さ拡張ビット数の参照
    UINT32 DistanceBitnumSearch(UINT32 l);//距離拡張ビット数の参照
    UINT32 DistanceExSearch(UINT32 l);//距離を符号用数値に変換
    UINT32 MileExSearch(UINT32 m);//長さを符号用数値に変換
};