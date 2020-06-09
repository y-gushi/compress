#include "headerRead.h"
/*
0 0通常（-en）圧縮オプションが使用されました。
          0 1最大（-exx / -ex）圧縮オプションが使用されました。
          1 0高速（-ef）圧縮オプションが使用されました。
          1 1超高速（-es）圧縮オプションが使用されました。
ビット3：このビットが設定されている場合、フィールドcrc-32、圧縮
               サイズと非圧縮サイズは、
               ローカルヘッダー。正しい値が
               圧縮された直後のデータ記述子
               データ。（注：PKZIPバージョン2.04g（DOSのみ）
               新しいメソッド8圧縮でこのビットを認識します
               PKZIPのバージョンはこのビットを認識します
               圧縮方法。）
*/
//コンストラクタ
HeaderWrite::HeaderWrite() {
    zero = 0;
}

//文字列の長さを調べる
int searchleng(char n[]) {
    int i = 0;
    while (n[i] != '\0') {
        i++;
    }
    return i;
}

//データ変更ファイルネームの追加
changefilenames* HeaderWrite::addfilename(changefilenames* infn, char* fn)
{
    if (!infn) {
        infn = (changefilenames*)malloc(sizeof(changefilenames));
        if (infn) {
            infn->cfn = fn;
            infn->next = nullptr;
        }
    }
    else {
        infn->next = addfilename(infn->next, fn);
    }

    return infn;
}

int HeaderWrite::localwrite(FILE* fn, UINT32 t, UINT32 crc32, UINT32 asize, UINT32 bsize, UINT16 nameleng, char n[], UINT16 ver, UINT16 bitf, UINT16 method) {

    int size = 0;

    // バイナリとして書き込む
    //f.write(reinterpret_cast<char*>(&a), sizeof(a));
    for (int i = 0; i < 4; i++) {
        unsigned char c = (LOCAL_HEADER >> (24 - (8 * i))) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);//シグネチャの書き込み
        size++;
    }
    //展開に必要なバージョン (最小バージョン)
    for (int i = 0; i < 2; i++) {
        unsigned char c = (ver >> (8 * i)) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);
        size++;
    }
    //汎用目的のビットフラグ
    for (int i = 0; i < 2; i++) {
        unsigned char c = (bitf >> (8 * i)) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);//ビットフラッグ
        size++;
    }
    //圧縮メソッド
    for (int i = 0; i < 2; i++) {
        unsigned char c = (method >> (8 * i)) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);//圧縮方式の書き込み
        size++;
    }
    //ファイルの最終変更時間
    UINT16 time = t & 0xFF;
    UINT16 day = (t >> 16) & 0xFF;
    for (int i = 0; i < 2; i++) {
        unsigned char c = (time >> (8 * i)) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);//時間と日付の書き込み
        size++;
    }
    for (int i = 0; i < 2; i++) {
        unsigned char c = (day >> (8 * i)) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);//時間と日付の書き込み
        size++;
    }
    //crc 4byte 書き込み（解凍後の値をcrc）
    for (int i = 0; i < 4; i++) {
        unsigned char c = (crc32 >> (8 * i)) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);
        size++;
    }
    //圧縮サイズ 書き込み
    for (int i = 0; i < 4; i++) {
        unsigned char c = (asize >> (8 * i)) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);
        size++;
    }
    //圧縮前サイズ 書き込み
    for (int i = 0; i < 4; i++) {
        unsigned char c = (bsize >> (8 * i)) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);
        size++;
    }
    //ファイル名の長さ 書き込み
    for (int i = 0; i < 2; i++) {
        unsigned char c = (nameleng >> (8 * i)) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);
        size++;
    }

    //拡張フィールドの長さ 0書き込み
    for (int i = 0; i < 2; i++) {
        fwrite(&zero, sizeof(char), 1, fn);
        size++;
    }
    //ファイル名の書き込み
    for (int i = 0; i < nameleng; i++) {
        fwrite(&n[i], sizeof(char), 1, fn);
        size++;
    }

    //拡張フィールド書き込みは省略（0)

    return size;
}

void HeaderWrite::centralwrite(FILE* fn, CenterDerect cd) {

    for (int i = 0; i < 4; i++) {
        unsigned char c = (CENTRALSIGNATURE >> (24 - (8 * i))) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);//シグネチャの書き込み
        cdsize++;
    }

    for (int i = 0; i < 2; i++) {
        unsigned char c = (cd.version >> (8 * i)) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);
        cdsize++;
    }
    //抽出に必要なバージョン（最小）//読み取りでは2d
    for (int i = 0; i < 2; i++) {
        unsigned char c = (cd.minversion >> (8 * i)) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);//2バイト
        cdsize++;
    }
    //std::cout << "抽出に必要なバージョン
    for (int i = 0; i < 2; i++) {
        unsigned char c = (cd.bitflag >> (8 * i)) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);
        cdsize++;
    }
    //    圧縮方法
    for (int i = 0; i < 2; i++) {
        unsigned char c = (cd.method >> (8 * i)) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);//圧縮方式の書き込み2バイト　ローカルヘッダと同じ
        cdsize++;
    }
    //    ファイルの最終変更時刻
    for (int i = 0; i < 2; i++) {
        unsigned char c = (cd.time >> (8 * i)) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);//時間2バイトの書き込み
        cdsize++;
    }
    //    ファイルの最終変更日
    for (int i = 0; i < 2; i++) {
        unsigned char c = (cd.day >> (8 * i)) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);//日付2バイトの書き込み
        cdsize++;
    }
    //crc
    for (int i = 0; i < 4; i++) {
        unsigned char c = (cd.crc >> (8 * i)) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);//crc 4byte 書き込み（ローカルファイルの値をcrc？）
        cdsize++;
    }
    //    圧縮サイズ
    for (int i = 0; i < 4; i++) {
        unsigned char c = (cd.size >> (8 * i)) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);//圧縮サイズ 書き込み
        cdsize++;
    }

    //    非圧縮サイズ
    for (int i = 0; i < 4; i++) {
        unsigned char c = (cd.nonsize >> (8 * i)) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);;//圧縮前サイズ 書き込み
        cdsize++;
    }

    //    ファイル名の長さ（n)
    for (int i = 0; i < 2; i++) {
        unsigned char c = (cd.filenameleng >> (8 * i)) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);//ファイル名の長さ2バイト 書き込み
        cdsize++;
    }
    // 追加フィールド長（m）
    for (int i = 0; i < 2; i++) {
        unsigned char c = (cd.fieldleng >> (8 * i)) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);
        cdsize++;
    }
    //ファイルのコメント長（k）
    for (int i = 0; i < 2; i++) {
        unsigned char c = (cd.fielcomment >> (8 * i)) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);
        cdsize++;
    }

    //    ファイルが始まるディスク番号
    for (int i = 0; i < 2; i++) {
        unsigned char c = (cd.discnum >> (8 * i)) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);
        cdsize++;
    }
    //内部ファイル属性（コピー）2バイト 書き込み
    for (int i = 0; i < 2; i++) {
        unsigned char c = (cd.zokusei >> (8 * i)) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);
        cdsize++;
    }
    //外部ファイル属性（コピー）4バイト 書き込み
    for (int i = 0; i < 4; i++) {
        unsigned char c = (cd.gaibuzokusei >> (8 * i)) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);
        cdsize++;
    }
    //ローカルヘッダ相対オフセット4バイト 書き込み
    for (int i = 0; i < 4; i++) {
        unsigned char c = (cd.localheader >> (8 * i)) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);
        cdsize++;
    }

    //ファイル名の書き込み
    for (int i = 0; i < cd.filenameleng; i++) {
        fwrite(&cd.filename[i], sizeof(char), 1, fn);
        cdsize++;
    }
    //拡張フィールド
    if (cd.fieldleng > 0) {
        for (int i = 0; i < cd.fieldleng; i++) {
            fwrite(&cd.kakutyo[i], sizeof(char), 1, fn);
            cdsize++;
        }
    }
    //ファイルコメント　なし
    if (cd.fielcomment > 0) {
        for (int i = 0; i < cd.fielcomment; i++) {
            fwrite(&cd.comment[i], sizeof(char), 1, fn);
            cdsize++;
        }
    }
    recordsum++;
}

void HeaderWrite::eocdwrite(FILE* fn, UINT16 discnum, UINT16 cdnum, UINT16 disccdsum, UINT16 cdsum, UINT64 pos, UINT32 size) {
    //シグネチャの書き込み
    for (int i = 0; i < 4; i++) {
        unsigned char c = (END_OF_CENTRAL >> (8 * i)) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);
        endsize++;
    }
    //このディスクの数（コピー） 書き込み
    for (int i = 0; i < 2; i++) {
        unsigned char c = (discnum >> (8 * i)) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);
        endsize++;
    }
    //セントラルディレクトリが開始するディスク 書き込み
    for (int i = 0; i < 2; i++) {
        unsigned char c = (cdnum >> (8 * i)) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);
        endsize++;
    }
    //このディスク上のセントラルディレクトリレコードの数  書き込み
    for (int i = 0; i < 2; i++) {
        unsigned char c = (disccdsum >> (8 * i)) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);
        endsize++;
    }
    //セントラルディレクトリレコードの合計数 書き込み
    for (int i = 0; i < 2; i++) {
        unsigned char c = (cdsum >> (8 * i)) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);
        endsize++;
    }
    //セントラルディレクトリのサイズ (バイト)  書き込み
    for (int i = 0; i < 4; i++) {
        unsigned char c = (size >> (8 * i)) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);
        endsize++;
    }
    //セントラルディレクトリの位置
    for (int i = 0; i < 4; i++) {
        unsigned char c = (pos >> (8 * i)) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);
        endsize++;
    }

    //ZIPファイルのコメントの長さ (n)  書き込み 0書き込み
    fwrite(&zero, sizeof(char), 1, fn);
    fwrite(&zero, sizeof(char), 1, fn);
    endsize += 2;

    //ZIPファイルのコメント　入れない
}