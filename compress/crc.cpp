#include "crc.h"

crc::crc()
{
    crc32 = UINT32(0xFFFFFFFF);
    magic = UINT32(0xEDB88320);
    tablev = 0;
    /* テーブルを作成する */
    for (int i = 0; i < 256; i++) {      /* 下位8ビットそれぞれについて計算する */
        tablev = i;      /* 下位8ビットを添え字に、上位24ビットを0に初期化する */
        for (int j = 0; j < 8; j++) {
            tablev = (tablev & 1) ? (magic ^ (tablev >> 1)) : (tablev >> 1); /* 1があふれたらマジックナンバーをXORする */
        }
        table[i] = tablev;        /* 計算した値をテーブルに格納する */
    }
}

unsigned int crc::mcrc(unsigned char* a, UINT64 len)
{
    crc32 = 0xFFFFFFFF;
    /* テーブルを用いてCRC32を計算する */
    for (UINT64 i = 0; i < len; i++) {
        crc32 = table[(crc32 ^ a[i]) & 0xff] ^ (crc32 >> 8); /* 1バイト投入して更新する */
    }
    //printf("CRC:%08" PRIX32"\n", ~crc32);
    crc32 = crc32 ^ 0xFFFFFFFF;
    //std::cout << "crcHEX表示" << std::hex << crc32 << std::endl;
    return crc32;
}

void crc::rescrc()
{
    crc32 = UINT32(0xFFFFFFFF);
}
