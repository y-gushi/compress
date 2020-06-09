#include "headerRead.h"

HeaderRead::HeaderRead(char* name) {
    readfile = name; //読み込むファイルの指定
    readdata = 0;
    sig = 0;
    endpos = 0;
    curpos = 0;
    headerReaddata = 0;
    lhtest = readLHdata;
    rootLH = nullptr;
    r = nullptr;//セントラルディレクトリのルート
    ER = (ENDrecord*)malloc(sizeof(ENDrecord));
    LH = tallocLH();
    rootLH = LH;
    CD = talloc();
    saveLH = nullptr;
    saveCD = nullptr;
    scd = (CenterDerect*)malloc(sizeof(CenterDerect));
}

HeaderRead::~HeaderRead() {
    free(readLHdata);
    //free(lhtest);
    free(LH);
    free(ER);
    free(CD);
    free(saveCD);
    free(saveLH);
    //free(rootLH);
    free(scd);
}

//ファイル名でセントラルディレクトリ　データ検索
//ローカルディレクトリの位置を返す
CenterDerect* HeaderRead::searchCENTRAL(char* s) {
    bool t = false;
    CDdataes* p = saveCD;

    while (p != nullptr) {
        t = searchChara(s, p->cdd.filename, p->cdd.filenameleng);//ファイル名部分一致検索
        if (t) {
            return &p->cdd;
        }
        else {
        }
        p = p->nextCD;
    }
    return nullptr;
}

//ファイル名検索
bool HeaderRead::searchChara(char fn[], char* cdfn, UINT16 cdfnlen)
{
    int i = 0;

    //文字列の数
    while (fn[i] != '\0') {
        i++;
    }
    UINT32 isize = (UINT32)i + 1;
    char* searchfilename = (char*)malloc(isize);//検索スライド用

    int slidepos = 0;

    if (searchfilename) {
        while (slidepos < cdfnlen) {
            if (i > cdfnlen) {//ファイル名が文字列より長い
                return false;
            }
            else {
                if (slidepos == 0) {//
                    for (int j = 0; j < i; j++) {//最初に文字列にfilenameをi分入れる
                        searchfilename[j] = cdfn[slidepos];
                        slidepos++;
                    }
                    searchfilename[i] = '\0';//文字列終端を追加
                }
                else {
                    //配列を一文字づつずらす
                    for (int j = 0; j < i - 1; j++) {
                        searchfilename[j] = searchfilename[j + 1];
                    }
                    searchfilename[i - 1] = cdfn[slidepos];//最後に付け加える
                    searchfilename[i] = '\0';//文字列終端を追加
                    slidepos++;//位置移動
                }
                if (strcmp(searchfilename, fn) == 0)
                {
                    return true;
                }
            }
        }
    }

    return false;
}

CenterDerect* HeaderRead::talloc(void) {
    return (CenterDerect*)malloc(sizeof(CenterDerect));
}

LocalHeader* HeaderRead::tallocLH()
{
    return (LocalHeader*)malloc(sizeof(LocalHeader) * 1);
}

LHDataes* HeaderRead::tallocLhData()
{
    return (LHDataes*)malloc(sizeof(LHDataes));
}


LHDataes* HeaderRead::addLhData(LHDataes* ld, LocalHeader* lhd)
{
    if (!ld) {
        ld = tallocLhData();
        ld->lhdata = lhd;
        ld->nextLH = nullptr;
    }
    else {
        ld->nextLH = addLhData(ld->nextLH, lhd);
    }

    return ld;
}

CDdataes* HeaderRead::tallocCDdataes()
{
    return (CDdataes*)malloc(sizeof(CDdataes));
}

CDdataes* HeaderRead::addCDdata(CDdataes* c, CenterDerect* cddata)
{
    if (c == NULL) {
        c = tallocCDdataes();
        c->cdd = *cddata;
        c->nextCD = nullptr;
    }
    else {
        c->nextCD = addCDdata(c->nextCD, cddata);
    }

    return c;
}

filelist* HeaderRead::addfn(filelist* f, char* fn, int h)
{
    if (!f) {
        f = (filelist*)malloc(sizeof(filelist));
        if (f) {
            UINT32 msize = (UINT32)h + 1;
            f->cfn = (char*)malloc(msize);
            f->cfn = fn;
            f->hash = h;
            f->next = nullptr;
        }
    }
    else {
        f->next = addfn(f, fn, h);
    }
    return f;
}


void HeaderRead::endread() {
    std::ifstream fin(readfile, std::ios::in | std::ios::binary);
    if (!fin) {
        std::cout << "ファイル ID.txt が開けません";
    }

    fin.seekg(0, std::ios_base::end);//ファイル終端位置取得
    UINT64 endpos = 0;
    endpos = fin.tellg();

    curpos = endpos;
    curpos -= 1;
    fin.seekg(curpos, std::ios_base::beg);

    /*終端コード検索　ファイル終わりから*/
    while (curpos > 0) {

        fin.read((char*)&readdata, sizeof(char));
        sig = sig << 8;
        sig += readdata;

        if (sig == END_OF_CENTRAL) {
            curpos += 4;//シグネチャ分ポインタ戻す
            fin.seekg(curpos);

            //このディスクの数 2byte
            ER->discnum = 0;
            for (int i = 0; i < 2; i++) {
                fin.read((char*)&readdata, sizeof(char));
                readdata = ((readdata & 0xFF) << (8 * i));
                ER->discnum += readdata;
            }

            //セントラルディレクトリが開始するディスク 2byte
            ER->disccentral = 0;
            for (int i = 0; i < 2; i++) {
                fin.read((char*)&readdata, sizeof(char));
                readdata = ((readdata & 0xFF) << (8 * i));
                ER->disccentral += readdata;
            }

            //このディスク上のセントラルディレクトリレコードの数 2byte
            ER->centralnum = 0;
            for (int i = 0; i < 2; i++) {
                fin.read((char*)&readdata, sizeof(char));
                readdata = ((readdata & 0xFF) << (8 * i));
                ER->centralnum += readdata;
            }

            //セントラルディレクトリレコードの合計数 2byte
            ER->centralsum = 0;
            for (int i = 0; i < 2; i++) {
                readdata = 0;
                fin.read((char*)&readdata, sizeof(char));
                readdata = ((readdata & 0xFF) << (8 * i));
                ER->centralsum += readdata;
            }

            //セントラルディレクトリのサイズ (バイト) 4byte
            ER->size = 0;
            for (int i = 0; i < 4; i++) {
                fin.read((char*)&readdata, sizeof(char));
                readdata = (readdata & 0xFF) << (8 * i);
                ER->size += readdata;
            }

            //セントラルディレクトリの開始位置のオフセット 4byte
            ER->position = 0;
            for (int i = 0; i < 4; i++) {
                readdata = 0;
                fin.read((char*)&readdata, sizeof(char));
                readdata = (readdata & 0xFF) << (8 * i);
                ER->position += readdata;
            }
            readpos = ER->position;

            //セントラルディレクトリレコードの合計数 2byte
            ER->commentleng = 0;
            for (int i = 0; i < 2; i++) {
                fin.read((char*)&readdata, sizeof(char));
                readdata = ((readdata & 0xFF) << (8 * i));
                ER->commentleng += readdata;
            }

            //ZIPファイルのコメント nbyte
            UINT32 msize = ((UINT32)ER->commentleng) + 1;
            ER->comment = (char*)malloc(msize);
            for (int i = 0; i < ER->commentleng; i++) {
                fin.read((char*)&readdata, sizeof(char));
                ER->comment[i] = (readdata & 0xFF);
            }
            fin.close();
            break;
        }
        curpos--;
        fin.seekg(curpos);
    }

}
//ローカルディレクトリのヘッダー情報
void HeaderRead::localread(UINT64 pos) {
    std::ifstream fin(readfile, std::ios::in | std::ios::binary);
    if (!fin) {
    }

    fin.seekg(pos, std::ios_base::beg);
    sig = 0;


    /*ローカルヘッダシグネチャを読み込む*/
    while (sig != LOCAL_HEADER) {
        //先頭コードまで読み込む
        fin.read((char*)&readdata, sizeof(char));
        sig = sig << 8;
        sig += readdata & 0xFF;
    }

    LH->version = 0;
    for (int i = 0; i < 2; i++) {
        fin.read((char*)&readdata, sizeof(char));
        readdata = ((readdata & 0xFF) << (8 * i));
        LH->version += readdata;
    }

    //汎用目的のビットフラグ
    LH->bitflag = 0;
    for (int i = 0; i < 2; i++) {
        fin.read((char*)&readdata, sizeof(char));
        readdata = ((readdata & 0xFF) << (8 * i));
        LH->bitflag += readdata;
    }

    //圧縮メソッド
    LH->method = 0;
    for (int i = 0; i < 2; i++) {
        fin.read((char*)&readdata, sizeof(char));
        readdata = ((readdata & 0xFF) << (8 * i));
        LH->method += readdata;
    }

    //ファイルの最終変更時間
    LH->time = 0;
    for (int i = 0; i < 2; i++) {
        fin.read((char*)&readdata, sizeof(char));
        readdata = ((readdata & 0xFF) << (8 * i));
        LH->time += readdata;
    }

    //ファイルの最終変更日付
    LH->day = 0;
    for (int i = 0; i < 2; i++) {
        fin.read((char*)&readdata, sizeof(char));
        readdata = ((readdata & 0xFF) << (8 * i));
        LH->day += readdata;
    }

    //CRC-32
    LH->crc = 0;
    for (int i = 0; i < 4; i++) {
        fin.read((char*)&readdata, sizeof(char));
        readdata = ((readdata & 0xFF) << (8 * i));
        LH->crc += readdata;
    }

    //圧縮サイズ
    LH->size = 0;
    for (int i = 0; i < 4; i++) {
        fin.read((char*)&readdata, sizeof(char));
        readdata = ((readdata & 0xFF) << (8 * i));
        LH->size += readdata;
    }

    //    非圧縮サイズ
    LH->nonsize = 0;
    for (int i = 0; i < 4; i++) {
        fin.read((char*)&readdata, sizeof(char));
        readdata = ((readdata & 0xFF) << (8 * i));
        LH->nonsize += readdata;
    }

    //ファイル名の長さ (n)
    LH->filenameleng = 0;
    for (int i = 0; i < 2; i++) {
        fin.read((char*)&readdata, sizeof(char));
        readdata = ((readdata & 0xFF) << (8 * i));
        LH->filenameleng += readdata;
    }

    //拡張フィールドの長さ (m)
    LH->fieldleng = 0;
    for (int i = 0; i < 2; i++) {
        fin.read((char*)&readdata, sizeof(char));
        readdata = ((readdata & 0xFF) << (8 * i));
        LH->fieldleng += readdata;
    }

    //ファイル名
    UINT32 msize = ((UINT32)LH->filenameleng) + 1;
    LH->filename = (char*)malloc(msize);
    if (LH->filename) {
        for (UINT16 i = 0; i < LH->filenameleng; i++) {
            fin.read((char*)&readdata, sizeof(char));
            LH->filename[i] = readdata;
        }
        LH->filename[LH->filenameleng] = '\0';
    }

    //拡張フィールド
    msize = ((UINT32)LH->fieldleng) + 1;
    LH->kakutyo = (char*)malloc(msize);
    if (LH->kakutyo) {
        for (UINT16 i = 0; i < LH->fieldleng; i++) {
            fin.read((char*)&readdata, sizeof(char));
            LH->kakutyo[i] = readdata;
        }
    }

    LH->pos = fin.tellg();//圧縮データの始まり
    fin.close();
}
//セントラルディレクトリのヘッダー情報
void HeaderRead::centerread(UINT64 pos, UINT32 size, UINT16 n) {

    std::ifstream fin(readfile, std::ios::in | std::ios::binary);

    if (!fin) {
        std::cout << "not file open" << std::endl;
    }

    int j = 0;

    fin.seekg(pos, std::ios_base::beg);

    while (j < n) {

        //while (s < size) {
            /*先頭コードまで読み込む*/
        for (int i = 0; i < 4; i++) {
            fin.read((char*)&readdata, sizeof(char));
            sig = sig << 8;
            sig += readdata & 0xFF;
        }

        if (sig == CENTRALSIGNATURE) { //中央ディレクトリファイルヘッダーの署名= 0x02014b50
            //std::cout << "中央ディレクトリ出力　j:" << j << std::endl;
            //CD->version = 0;
            //mac 62づつ増える？　windows 45づつ増える？
            //属性　GetFileAttributes?
            CD->version = 0;
            for (int i = 0; i < 2; i++) {
                fin.read((char*)&readdata, sizeof(char));//作成者
                readdata = ((readdata & 0xFF) << (8 * i));
                CD->version += readdata;
            }

            //抽出に必要なバージョン（最小）
            CD->minversion = 0;
            for (int i = 0; i < 2; i++) {
                fin.read((char*)&readdata, sizeof(char));
                readdata = ((readdata & 0xFF) << (8 * i));
                CD->minversion += readdata;
            }

            CD->bitflag = 0;
            for (int i = 0; i < 2; i++) {
                fin.read((char*)&readdata, sizeof(char));//汎用ビットフラグ
                readdata = ((readdata & 0xFF) << (8 * i));
                CD->bitflag += readdata;
            }

            CD->method = 0;
            for (int i = 0; i < 2; i++) {//    圧縮方法
                fin.read((char*)&readdata, sizeof(char));
                readdata = ((readdata & 0xFF) << (8 * i));
                CD->method += readdata;
            }

            CD->time = 0;
            for (int i = 0; i < 2; i++) {//    ファイルの最終変更時刻
                fin.read((char*)&readdata, sizeof(char));
                readdata = ((readdata & 0xFF) << (8 * i));
                CD->time += readdata;
            }

            CD->day = 0;
            for (int i = 0; i < 2; i++) {//    ファイルの最終変更日
                fin.read((char*)&readdata, sizeof(char));
                readdata = ((readdata & 0xFF) << (8 * i));
                CD->day += readdata;
            }

            //crc
            CD->crc = 0;
            for (int i = 0; i < 4; i++) {
                fin.read((char*)&readdata, sizeof(char));
                readdata = ((readdata & 0xFF) << (8 * i));
                CD->crc += readdata;
            }

            //    圧縮サイズ
            CD->size = 0;
            for (int i = 0; i < 4; i++) {
                fin.read((char*)&readdata, sizeof(char));
                readdata = ((readdata & 0xFF) << (8 * i));
                CD->size += readdata;
            }

            //    非圧縮サイズ
            CD->nonsize = 0;
            for (int i = 0; i < 4; i++) {
                fin.read((char*)&readdata, sizeof(char));
                readdata = ((readdata & 0xFF) << (8 * i));
                CD->nonsize += readdata;
            }

            //    ファイル名の長さ（n)
            CD->filenameleng = 0;
            for (int i = 0; i < 2; i++) {
                fin.read((char*)&readdata, sizeof(char));
                readdata = ((readdata & 0xFF) << (8 * i));
                CD->filenameleng += readdata;
            }

            CD->fieldleng = 0;
            for (int i = 0; i < 2; i++) {//    追加フィールド長（m）
                fin.read((char*)&readdata, sizeof(char));
                readdata = ((readdata & 0xFF) << (8 * i));
                CD->fieldleng += readdata;
            }

            CD->fielcomment = 0;
            for (int i = 0; i < 2; i++) {//    ファイルのコメント長（k）
                fin.read((char*)&readdata, sizeof(char));
                readdata = ((readdata & 0xFF) << (8 * i));
                CD->fielcomment += readdata;
            }

            CD->discnum = 0;
            for (int i = 0; i < 2; i++) {//    ファイルが始まるディスク番号
                fin.read((char*)&readdata, sizeof(char));
                readdata = (readdata << (8 * i));
                CD->discnum += readdata;
            }

            CD->zokusei = 0;
            for (int i = 0; i < 2; i++) {//        内部ファイル属性
                fin.read((char*)&readdata, sizeof(char));
                readdata = ((readdata & 0xFF) << (8 * i));
                CD->zokusei += readdata;
            }

            CD->gaibuzokusei = 0;
            for (int i = 0; i < 4; i++) {//    外部ファイル属性
                fin.read((char*)&readdata, sizeof(char));
                readdata = ((readdata & 0xFF) << (8 * i));
                CD->gaibuzokusei += readdata;
            }

            CD->localheader = 0;
            for (int i = 0; i < 4; i++) {//    ローカルファイルヘッダの相対オフセット
                fin.read((char*)&readdata, sizeof(char));
                readdata = ((readdata & 0xFF) << (8 * i));
                CD->localheader += readdata;
            }

            UINT32 msize = ((UINT32)CD->filenameleng) + 1;
            CD->filename = (char*)malloc(msize);
            for (int i = 0; i < CD->filenameleng; i++) {//    ファイル名
                fin.read((char*)&headerReaddata, sizeof(char));
                CD->filename[i] = headerReaddata;
            }
            CD->filename[CD->filenameleng] = '\0';

            msize = ((UINT32)CD->fieldleng) + 1;
            if (CD->fieldleng > 0) {
                CD->kakutyo = (char*)malloc(msize);
                for (int i = 0; i < CD->fieldleng; i++) {//    拡張フィールド
                    fin.read((char*)&readdata, sizeof(char));
                    CD->kakutyo[i] = readdata & 0xFF;
                }
            }

            msize = ((UINT32)CD->fielcomment) + 1;
            if (CD->fielcomment > 0) {
                CD->comment = (char*)malloc(msize);
                for (int i = 0; i < CD->fielcomment; i++) {//ファイルコメント
                    fin.read((char*)&readdata, sizeof(char));
                    CD->comment[i] = readdata & 0xFF;
                }
            }

            sig = 0;
            //データを保持する
            cdfn = addfn(cdfn, CD->filename, CD->filenameleng);
        }
        j++;
    }
    fin.close();
}

CenterDerect* HeaderRead::centeroneread(UINT64 pos, UINT32 size, UINT16 n, char* fn)
{
    std::ifstream fin(readfile, std::ios::in | std::ios::binary);

    bool flag = false;

    if (!fin) {
        std::cout << "not file open" << std::endl;
    }

    fin.seekg(pos, std::ios_base::beg);

    /*先頭コードまで読み込む*/
    for (int i = 0; i < 4; i++) {
        fin.read((char*)&readdata, sizeof(char));
        sig = sig << 8;
        sig += readdata & 0xFF;
    }

    if (sig == CENTRALSIGNATURE) {
        scd->version = 0;
        for (int i = 0; i < 2; i++) {
            fin.read((char*)&readdata, sizeof(char));//作成者
            readdata = ((readdata & 0xFF) << (8 * i));
            scd->version += readdata;
        }

        scd->minversion = 0;
        for (int i = 0; i < 2; i++) {
            fin.read((char*)&readdata, sizeof(char));
            readdata = ((readdata & 0xFF) << (8 * i));
            scd->minversion += readdata;
        }

        scd->bitflag = 0;
        for (int i = 0; i < 2; i++) {
            fin.read((char*)&readdata, sizeof(char));//汎用ビットフラグ
            readdata = ((readdata & 0xFF) << (8 * i));
            scd->bitflag += readdata;
        }

        scd->method = 0;
        for (int i = 0; i < 2; i++) {//    圧縮方法
            fin.read((char*)&readdata, sizeof(char));
            readdata = ((readdata & 0xFF) << (8 * i));
            scd->method += readdata;
        }

        scd->time = 0;
        for (int i = 0; i < 2; i++) {//    ファイルの最終変更時刻
            fin.read((char*)&readdata, sizeof(char));
            readdata = ((readdata & 0xFF) << (8 * i));
            scd->time += readdata;
        }

        scd->day = 0;
        for (int i = 0; i < 2; i++) {//    ファイルの最終変更日
            fin.read((char*)&readdata, sizeof(char));
            readdata = ((readdata & 0xFF) << (8 * i));
            scd->day += readdata;
        }

        //crc
        scd->crc = 0;
        for (int i = 0; i < 4; i++) {
            fin.read((char*)&readdata, sizeof(char));
            readdata = ((readdata & 0xFF) << (8 * i));
            scd->crc += readdata;
        }

        //    圧縮サイズ
        scd->size = 0;
        for (int i = 0; i < 4; i++) {
            fin.read((char*)&readdata, sizeof(char));
            readdata = ((readdata & 0xFF) << (8 * i));
            scd->size += readdata;
        }

        //    非圧縮サイズ
        scd->nonsize = 0;
        for (int i = 0; i < 4; i++) {
            fin.read((char*)&readdata, sizeof(char));
            readdata = ((readdata & 0xFF) << (8 * i));
            scd->nonsize += readdata;
        }

        //    ファイル名の長さ（n)
        scd->filenameleng = 0;
        for (int i = 0; i < 2; i++) {
            fin.read((char*)&readdata, sizeof(char));
            readdata = ((readdata & 0xFF) << (8 * i));
            scd->filenameleng += readdata;
        }

        scd->fieldleng = 0;
        for (int i = 0; i < 2; i++) {//    追加フィールド長（m）
            fin.read((char*)&readdata, sizeof(char));
            readdata = ((readdata & 0xFF) << (8 * i));
            scd->fieldleng += readdata;
        }

        scd->fielcomment = 0;
        for (int i = 0; i < 2; i++) {//ファイルのコメント長（k）
            fin.read((char*)&readdata, sizeof(char));
            readdata = ((readdata & 0xFF) << (8 * i));
            scd->fielcomment += readdata;
        }

        scd->discnum = 0;
        for (int i = 0; i < 2; i++) {//ファイルが始まるディスク番号
            fin.read((char*)&readdata, sizeof(char));
            readdata = (readdata << (8 * i));
            scd->discnum += readdata;
        }

        scd->zokusei = 0;
        for (int i = 0; i < 2; i++) {//内部ファイル属性
            fin.read((char*)&readdata, sizeof(char));
            readdata = ((readdata & 0xFF) << (8 * i));
            scd->zokusei += readdata;
        }

        scd->gaibuzokusei = 0;
        for (int i = 0; i < 4; i++) {//    外部ファイル属性
            fin.read((char*)&readdata, sizeof(char));
            readdata = ((readdata & 0xFF) << (8 * i));
            scd->gaibuzokusei += readdata;
        }

        scd->localheader = 0;
        for (int i = 0; i < 4; i++) {//    ローカルファイルヘッダの相対オフセット
            fin.read((char*)&readdata, sizeof(char));
            readdata = ((readdata & 0xFF) << (8 * i));
            scd->localheader += readdata;
        }

        UINT32 msize = ((UINT32)scd->filenameleng) + 1;
        scd->filename = (char*)malloc(msize);
        for (int i = 0; i < scd->filenameleng; i++) {//    ファイル名
            fin.read((char*)&headerReaddata, sizeof(char));
            scd->filename[i] = headerReaddata;
        }
        scd->filename[scd->filenameleng] = '\0';
        flag = searchChara(fn, scd->filename, scd->filenameleng);

        if (scd->fieldleng > 0) {
            msize = ((UINT32)scd->fieldleng) + 1;
            scd->kakutyo = (char*)malloc(msize);
            for (int i = 0; i < scd->fieldleng; i++) {//    拡張フィールド
                fin.read((char*)&readdata, sizeof(char));
                scd->kakutyo[i] = readdata & 0xFF;
            }
        }

        if (scd->fielcomment) {
            msize = ((UINT32)scd->fielcomment) + 1;
            scd->comment = (char*)malloc(msize);
            for (int i = 0; i < scd->fielcomment; i++) {//ファイルコメント
                fin.read((char*)&readdata, sizeof(char));
                scd->comment[i] = readdata & 0xFF;
            }
        }

        sig = 0;

        readpos = fin.tellg();
    }
    filenum++;

    fin.close();

    if (flag)
        return scd;

    return nullptr;
}