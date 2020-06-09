#pragma once

#include "typechange.h"
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include "decode.h"
#include "SearchItemNUm.h"

#define LOCAL_HEADER 0x504b0304
/*local file header　シグネチャ 0x04034b50  0x504b0304*/
#define CENTRALSIGNATURE 0x0504b0102
/*central directory header シグネチャ 0x02014b50 "PK\01\02"  0x0504b0102*/
#define END_OF_CENTRAL 0x06054b50
#define REND 0x504b0506
/*end of central dir signature シグネチャ 0x06054b50 "PK\05\06"  0x504b0506*/
#define MAKEVERSION 0x2d //64bit標準
#define VERSION_LH 0x14 //ローカルヘッダ　展開に必要なバージョン2byte（リトルエンディアン）
#define BITFLAG_LH 0x06 //ローカルヘッダ　ビットフラッグ2byte（リトルエンディアン） 0x14
#define DEFLATE_LH_CD 0x08//圧縮メソッド　デフレ―ト　2byte（リトルエンディアン）
#define NAIBU 0x0
#define GAIBU 0x0

int searchleng(char n[]);

/*全てのヘッダ内の複数バイトの値は、リトルエンディアンで格納される。*/
struct CenterDerect {

    UINT64 curpos = 0;
    UINT16 version = 0;    //作成されたバージョン 2byte
    UINT16 minversion = 0;//展開に必要なバージョン (最小バージョン) 2byte
    UINT16 bitflag = 0;//汎用目的のビットフラグ 2byte
    UINT16 method = 0;//圧縮メソッド 2byte
    UINT16 time = 0;//ファイルの最終変更時間 2byte
    UINT16 day = 0;//ファイルの最終変更日付 2byte
    UINT32 crc = 0;//CRC-32 4byte
    UINT32 size = 0;//圧縮サイズ 4byte
    UINT32 nonsize = 0;//非圧縮サイズ 4byte
    UINT16 filenameleng = 0;//ファイル名の長さ (n) 2byte
    UINT16 fieldleng = 0;//拡張フィールドの長さ (m) 2byte
    UINT16 fielcomment = 0;//ファイルコメントの長さ (k) 2byte
    UINT16 discnum = 0;//ファイルが開始するディスク番号 2byte
    UINT16 zokusei = 0;//内部ファイル属性 2byte
    UINT32 gaibuzokusei = 0;//外部ファイル属性 4byte
    UINT32 localheader = 0;//ローカルファイルヘッダの相対オフセット 4byte

    char* filename = nullptr;
    char* kakutyo = nullptr;//拡張フィールド mbyte
    char* comment = nullptr;//ファイルコメント kbyte
};

struct ENDrecord {
    UINT32 endsig = 0;

    UINT16 discnum = 0;    //このディスクの数 2byte
    UINT16 disccentral = 0;    //セントラルディレクトリが開始するディスク 2byte
    UINT16 centralnum = 0; //このディスク上のセントラルディレクトリレコードの数 2byte
    UINT16 centralsum = 0; //セントラルディレクトリレコードの合計数 2byte
    UINT32 size = 0;//セントラルディレクトリのサイズ (バイト) 4byte
    UINT32 position = 0;//セントラルディレクトリの開始位置のオフセット 4byte
    UINT16 commentleng = 0; //セントラルディレクトリレコードの合計数 2byte
    char* comment = nullptr;//ZIPファイルのコメント nbyte
};

struct LocalHeader {
    UINT16 version = 0;    //展開に必要なバージョン (最小バージョン) 2byte
    UINT16 bitflag = 0;//汎用目的のビットフラグ 2byte
    UINT16 method = 0;//圧縮メソッド 2byte
    UINT16 time = 0;//ファイルの最終変更時間 2byte
    UINT16 day = 0;//ファイルの最終変更日付 2byte
    UINT32 crc = 0;//CRC-32 4byte
    UINT32 size = 0;//圧縮サイズ 4byte
    UINT32 nonsize = 0;//非圧縮サイズ 4byte
    UINT16 filenameleng = 0;//ファイル名の長さ (n) 2byte
    UINT16 fieldleng = 0;//拡張フィールドの長さ (m) 2byte
    char* filename = nullptr;//ファイル名 nbyte
    char* kakutyo = nullptr;//拡張フィールド mbyte
    UINT64 pos;//データの始まる位置
};

struct changefilenames {
    char* cfn = nullptr;
    changefilenames* next = nullptr;
};

struct filelist {
    char* cfn = nullptr;
    int hash = 0;
    filelist* next = nullptr;
};

struct LHDataes {
    LocalHeader* lhdata;
    LHDataes* nextLH;
};

struct CDdataes {
    CenterDerect cdd;
    CDdataes* nextCD;
};

class HeaderRead
{
public:
    UINT32 readdata;//一時読み込みデータ
    char headerReaddata;
    UINT32 sig;//シグネチャ判定
    char* readfile;  //読み込むファイルの指定

    ENDrecord* ER;//endシグネチャの構造体
    LocalHeader* LH;//localシグネチャの構造体
    CenterDerect* CD;
    CenterDerect* scd;
    UINT64 readpos = 0;//現在のファイル位置

    LocalHeader* rootLH;//検索ルート
    CenterDerect* r;//検索用ルート

    LHDataes* saveLH;//ローカルヘッダデータ保存用
    CDdataes* saveCD;//セントラルヘッダー保存用

    UINT64 endpos = 0; //ファイルの終端位置
    UINT64 curpos = 0; //ファイルの現在位置

    UINT32 filenum = 0;//読み込み数

    filelist* cdfn = nullptr;

    unsigned char* readLHdata = (unsigned char*)malloc(sizeof(unsigned char) * 100);//crcテスト用
    unsigned char* lhtest = (unsigned char*)malloc(sizeof(unsigned char) * 1l);

    HeaderRead(char* name);
    ~HeaderRead();
    void endread();//エンドヘッダー情報読み取り
    void localread(UINT64 pos);
    void centerread(UINT64 pos, UINT32 size, UINT16 n);
    CenterDerect* centeroneread(UINT64 pos, UINT32 size, UINT16 n, char* fn);

    CenterDerect* searchCENTRAL(char* s);//ファイル名でセントラルディレクトリの検索

    bool searchChara(char fn[], char* cdfn, UINT16 cdfnlen);//ファイル名でローカルディレクトリの検索
    CenterDerect* talloc();
    LocalHeader* tallocLH();
    LHDataes* tallocLhData();
    LHDataes* addLhData(LHDataes* ld, LocalHeader* lhd);
    CDdataes* tallocCDdataes();
    CDdataes* addCDdata(CDdataes* c, CenterDerect* cddata);
    filelist* addfn(filelist* f, char* fn, int h);
};

/*ヘッダー情報の作成*/
class HeaderWrite {
public:
    char zero = 0;
    unsigned int cdsize = 0;
    UINT32 recordsum = 0;
    unsigned int endsize = 0;

    /*書き込み用関数*/
    HeaderWrite();
    void centralwrite(FILE* fn, CenterDerect cd);
    void eocdwrite(FILE* fn, UINT16 discnum, UINT16 cdnum, UINT16 disccdsum, UINT16 cdsum, UINT64 pos, UINT32 size);
    changefilenames* addfilename(changefilenames* infn, char* fn);
    int localwrite(FILE* fn, UINT32 t, UINT32 crc32, UINT32 asize, UINT32 bsize, UINT16 nameleng, char n[], UINT16 ver, UINT16 bitf, UINT16 method);
};