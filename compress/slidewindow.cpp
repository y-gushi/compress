#include "slidewindow.h"

slidewndow::slidewndow()
{
    Distancelength = 0;
    distanceexval = 0;//距離拡張ビット
    distanceexbit = 0;//距離拡張ビットのビット数
    mileexval = 0;//長さ拡張ビットの値
    mileexbit = 0;//長さ拡張ビットのビット数
    SearchPositionStock = nullptr;//検索一致位置の候補
    Rootsearchposition = nullptr;
    SearchMileStock = nullptr;//長さ候補のストック
    Rootsearchmilestock = nullptr;
    maxmilestock = 0;//ストックの最長
    stocknum = 0;//ストック数
    milestock = nullptr;
    distancestock = nullptr;
    mile_extension_stock = nullptr;
    distance_extenshon_stock = nullptr;
    mileRoot = nullptr;
    distanceRoot = nullptr;
    mile_extensionRoot = nullptr;
    distance_extenshonRoot = nullptr;
    milestocksize = 0;
    distancestocksize = 0;
}

slidewndow::~slidewndow()
{
    
}

UINT32 slidewndow::slidesearch(unsigned char* p, UINT32 len, UINT32 spos)
{
    UINT32 fl = 0;
    UINT32 m = 0;//長さ格納
    UINT32 l = 0;//距離格納
    UINT32 i = spos;
    milestocksize = 0;
    distancestocksize = 0;

    milestock = (UINT32*)malloc(sizeof(UINT32) * LEVEL);
    distancestock = (UINT32*)malloc(sizeof(UINT32) * LEVEL);
    mile_extension_stock = (UINT32*)malloc(sizeof(UINT32) * LEVEL);
    distance_extenshon_stock = (UINT32*)malloc(sizeof(UINT32) * LEVEL);
    mileRoot = milestock;
    distanceRoot = distancestock;
    mile_extensionRoot = mile_extension_stock;
    distance_extenshonRoot = distance_extenshon_stock;
    UINT32 exdis = 0;//確認用
    UINT32 exmil = 0;//確認用

    if (milestock) {
        while (i < len && milestocksize < LEVEL-1) {
            if (i == 0) {
                *milestock = p[0];
                milestock++;
                milestocksize++;
                i++;
            }
            else {
                fl = CurposSearch(p, i, len);
                if (fl > 0) {//一致した文字列があった場合
                    m = MileExSearch((fl & 0xFFFFFFFF));//長さを符号よう数値に変換
                    l = DistanceExSearch(Distancelength);//距離を符号よう数値に変換
                    i += fl;//距離ぶんポインタ移動
                    *milestock = m;//長さを配列に
                    milestock++;
                    milestocksize++;
                    if (m >= 265 && m < 285) {
                        *mile_extension_stock = mileexval;//長さ拡張ビットの値を入れる
                        mile_extension_stock++;
                        exmil++;
                    }
                    if (l >= 4) {
                        *distance_extenshon_stock = distanceexval & 0xFFFFFFFF;//距離拡張の値を入れる
                        distance_extenshon_stock++;
                        exdis++;
                    }
                    *distancestock = l;//距離を配列に
                    distancestock++;
                    distancestocksize++;
                }
                else {//一致した文字列がなかった場合
                    *milestock = p[i];
                    milestock++;//一致する文字列が無ければ文字を配列に
                    milestocksize++;
                    i++;
                }
            }
        }
        *milestock = 256;//終了数値
        milestock++;
        milestocksize++;
    }
    milestock = mileRoot;
    distancestock = distanceRoot;
    mile_extension_stock = mile_extensionRoot;
    distance_extenshon_stock = distance_extenshonRoot;

    return i;
}

UINT32 slidewndow::CurposSearch(unsigned char* ps, UINT32 pp, UINT32 l)
{
    UINT32 pstock = 0;//一致した場所
    UINT32 slide = pp;//読み込みポジションスライド
    UINT32 f = 0;
    UINT32 wp = 0;//長さカウント
    UINT32 i = 0;
    stocknum = 0;
    maxmilestock = 0;
    UINT32 maxlenstock = 0;

    if (pp > windowsize) {//maxlen 12288 8191
        f = pp - windowsize;
    }

    while (f < pp) {
        if (ps[f] == ps[pp]) {//swと読み込みが一致
            pstock = f;
            while (ps[f] == ps[slide] && ((l - 1) > slide) && wp < 258) {//長さの最大値が258
                f++;
                slide++;
                wp++;
                if (f >= pp) {
                    f = pstock;
                }
            }
            if (wp > 2) {
                if (wp > 199) {
                    Distancelength = (pp - pstock);//一致した距離
                    return wp;
                }
                if (maxmilestock < wp) {
                    maxmilestock = wp;
                    maxlenstock = pstock;
                }
                i++;
            }
            slide = pp;//ポジションスライダ初期化
            f = pstock;
            wp = 0;//長さ初期化
        }
        f++;
    }
    if (i > 0) {//イッチがあった場合
        Distancelength = (pp - maxlenstock);//一致した距離
        return maxmilestock;
    }

    return 0;
}

UINT32 slidewndow::MileBitnumSearch(UINT32 l) {//拡張ビット数の参照
    if (l >= 257 && l <= 264)
        return 0;
    else if (l >= 265 && l <= 268)
        return 1;
    else if (l >= 269 && l <= 272)
        return 2;
    else if (l >= 273 && l <= 276)
        return 3;
    else if (l >= 277 && l <= 280)
        return 4;
    else if (l >= 281 && l <= 284)
        return 5;
    else if (l == 285)
        return 0;

    return 300;
}

UINT32 slidewndow::DistanceBitnumSearch(UINT32 l)
{
    if (l >= 0 && l <= 3)
        return 0;
    else if (l >= 4 && l <= 5)
        return 1;
    else if (l >= 6 && l <= 7)
        return 2;
    else if (l >= 8 && l <= 9)
        return 3;
    else if (l >= 10 && l <= 11)
        return 4;
    else if (l >= 12 && l <= 13)
        return 5;
    else if (l >= 14 && l <= 15)
        return 6;
    else if (l >= 16 && l <= 17)
        return 7;
    else if (l >= 18 && l <= 19)
        return 8;
    else if (l >= 20 && l <= 21)
        return 9;
    else if (l >= 22 && l <= 23)
        return 10;
    else if (l >= 24 && l <= 25)
        return 11;
    else if (l >= 26 && l <= 27)
        return 12;
    else if (l >= 28 && l <= 29)
        return 13;
    else if (l >= 30 && l <= 31)
        return 14;
    else
        return 300;
}

UINT32 slidewndow::DistanceExSearch(UINT32 l)
{
    if (l == 1)
        return 0;
    else if (l == 2)
        return 1;
    else if (l == 3)
        return 2;
    else if (l == 4)
        return 3;
    else if (l >= 5 && l < 7) { distanceexval = l - 5; distanceexbit = 1; return 4; }
    else if (l >= 7 && l < 9) { distanceexval = l - 7; distanceexbit = 1; return 5; }
    else if (l >= 9 && l < 13) { distanceexval = l - 9; distanceexbit = 2; return 6; }
    else if (l >= 13 && l < 17) { distanceexval = l - 13; distanceexbit = 2; return 7; }
    else if (l >= 17 && l < 25) { distanceexval = l - 17; distanceexbit = 3; return 8; }
    else if (l >= 25 && l < 33) { distanceexval = l - 25; distanceexbit = 3; return 9; }
    else if (l >= 33 && l < 49) { distanceexval = l - 33; distanceexbit = 4; return 10; }
    else if (l >= 49 && l < 65) { distanceexval = l - 49; distanceexbit = 4; return 11; }
    else if (l >= 65 && l < 97) { distanceexval = l - 65; distanceexbit = 5; return 12; }
    else if (l >= 97 && l < 129) { distanceexval = l - 97; distanceexbit = 5; return 13; }
    else if (l >= 129 && l < 193) { distanceexval = l - 129; distanceexbit = 6; return 14; }
    else if (l >= 193 && l < 257) { distanceexval = l - 193; distanceexbit = 6; return 15; }
    else if (l >= 257 && l < 385) { distanceexval = l - 257; distanceexbit = 7; return 16; }
    else if (l >= 385 && l < 513) { distanceexval = l - 385; distanceexbit = 7; return 17; }
    else if (l >= 513 && l < 769) { distanceexval = l - 513; distanceexbit = 8; return 18; }
    else if (l >= 769 && l < 1025) { distanceexval = l - 769; distanceexbit = 8; return 19; }
    else if (l >= 1025 && l < 1537) { distanceexval = l - 1025; distanceexbit = 9; return 20; }
    else if (l >= 1537 && l < 2049) { distanceexval = l - 1537; distanceexbit = 9; return 21; }
    else if (l >= 2049 && l < 3073) { distanceexval = l - 2049; distanceexbit = 10; return 22; }
    else if (l >= 3073 && l < 4097) { distanceexval = l - 3073; distanceexbit = 10; return 23; }
    else if (l >= 4097 && l < 6145) { distanceexval = l - 4097; distanceexbit = 11; return 24; }
    else if (l >= 6145 && l < 8193) { distanceexval = l - 6145; distanceexbit = 11; return 25; }
    else if (l >= 8193 && l < 12289) { distanceexval = l - 8193; distanceexbit = 12; return 26; }
    else if (l >= 12289 && l < 16385) { distanceexval = l - 12289; distanceexbit = 12; return 27; }
    else if (l >= 16385 && l < 24577) { distanceexval = l - 16385; distanceexbit = 13; return 28; }
    else if (l >= 24577 && l < 32769) { distanceexval = l - 24577; distanceexbit = 13; return 29; }
    else if (l >= 32769 && l < 49153) { distanceexval = l - 32769; distanceexbit = 14; return 30; }
    else if (l >= 49153 && l < 65537) { distanceexval = l - 49153; distanceexbit = 14; return 31; }
    return 300;
}

UINT32 slidewndow::MileExSearch(UINT32 m)
{
    if (m == 3)return 257;
    else if (m == 4)return 258;
    else if (m == 5)return 259;
    else if (m == 6)return 260;
    else if (m == 7)return 261;
    else if (m == 8)return 262;
    else if (m == 9)return 263;
    else if (m == 10)return 264;
    else if (m >= 11 && m <= 12) { mileexval = m - 11; mileexbit = 1; return 265; }
    else if (m >= 13 && m <= 14) { mileexval = m - 13; mileexbit = 1; return 266; }
    else if (m >= 15 && m <= 16) { mileexval = m - 15; mileexbit = 1; return 267; }
    else if (m >= 17 && m <= 18) { mileexval = m - 17; mileexbit = 1; return 268; }
    else if (m >= 19 && m <= 22) { mileexval = m - 19; mileexbit = 2; return 269; }
    else if (m >= 23 && m <= 26) { mileexval = m - 23; mileexbit = 2; return 270; }
    else if (m >= 27 && m <= 30) { mileexval = m - 27; mileexbit = 2; return 271; }
    else if (m >= 31 && m <= 34) { mileexval = m - 31; mileexbit = 2; return 272; }
    else if (m >= 35 && m <= 42) { mileexval = m - 35; mileexbit = 3; return 273; }
    else if (m >= 43 && m <= 50) { mileexval = m - 43; mileexbit = 3; return 274; }
    else if (m >= 51 && m <= 58) { mileexval = m - 51; mileexbit = 3; return 275; }
    else if (m >= 59 && m <= 66) { mileexval = m - 59; mileexbit = 3; return 276; }
    else if (m >= 67 && m <= 82) { mileexval = m - 67; mileexbit = 4; return 277; }
    else if (m >= 83 && m <= 98) { mileexval = m - 83; mileexbit = 4; return 278; }
    else if (m >= 99 && m <= 114) { mileexval = m - 99; mileexbit = 4; return 279; }
    else if (m >= 115 && m <= 130) { mileexval = m - 115; mileexbit = 4; return 280; }
    else if (m >= 131 && m <= 162) { mileexval = m - 131; mileexbit = 5; return 281; }
    else if (m >= 163 && m <= 194) { mileexval = m - 163; mileexbit = 5; return 282; }
    else if (m >= 195 && m <= 226) { mileexval = m - 195; mileexbit = 5; return 283; }
    else if (m >= 227 && m <= 257) { mileexval = m - 227; mileexbit = 5; return 284; }
    else if (m == 258)return 285;
    //else if (m >= 258 && m <= 65793){ mileexval = m; mileexbit = 16; return 285; }
    //16bit拡張ビット0 ～ 65535
    return 300;
}
