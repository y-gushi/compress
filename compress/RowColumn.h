#pragma once

#include "typechange.h"
#include "ChangeArrayNumber.h"
#include "shareRandW.h"
#include "TagAndItems.h"
#include <iostream>

class Ctags {
public:
    //Ctags(UINT8 *decorddata,UINT64 datalen,shareRandD *shdata);

    ArrayNumber NA;
    selection* SCT = nullptr;

    const char* Ctag = "<c ";
    const char* Vtag = "<v>";
    const char* SVend = "</sheetViews>";//13
    const char* endC = "/c>";
    const char* Ftag = "<f";
    const char* Rowtag = "<row";
    const char* sheetPr = "<sheetPr";//8
    const char* sheetPrEnd = "</sheetPr>";//10
    const char* codename = "codeName=\"";//10
    const char* dement = "<dimension ref=\"";//16
    const char* pane = "<pane";//5
    const char* Coltag = "<col ";//5文字
    const char* endtag = "</cols>";//7文字

    //row tag values
    const char* Rr = "r=\"";//3
    const char* Rspans = "spans=\"";//7
    const char* Rs = "s=\"";//3
    const char* RcustomFormat = "customFormat=\"";//14
    const char* Rht = "ht=\"";//4
    const char* RcustomHeight = "customHeight=\"";//14
    const char* RthickBot = "thickBot=\"";//10

    //cols tag values
    const char* Colmin = "min=\"";//5
    const char* Colmax = "max=\"";//5
    const char* Colswidth = "width=\"";//7
    const char* ColS = "style=\"";//7
    const char* ColcW = "customWidth=\"";//13
    const char* Colbf = "bestFit=\"";//9
    const char* Colhid = "hidden=\"";//8

    //c tag f values
    const char* Fref = "ref=\"";
    const char* Fsi = "si=\"";
    const char* Ft = "t=\"";

    const char* sheetend = "</sheetData>";//12
    const char* startSV = "<sheetView";//10

    Row* rows = nullptr;//row配列
    selection* sct = nullptr;//selection 構造体
    demention* dm = nullptr;//diment 構造体
    cols* cls = nullptr;//cols 構造体
    Pane* Panes = nullptr;

    UINT64 p = 0;//読み込み位置
    UINT8* fstr = nullptr;//最後の文字列

    UINT8* headXML = nullptr;// <sheetPrまでの文字
    UINT8* dimtopane = nullptr;// dimension />から<pane 閉じタグまでの文字
    UINT8* sFPr = nullptr;//sheetFormatPrの取得
    UINT8* MC = nullptr;//マージセル数

    UINT8* data = nullptr;//デコードデータ
    UINT64 dlen = 0;//デコードデータ長
    UINT8* coden = nullptr;//コードネーム
    shareRandD* sh = nullptr;//share配列
    UINT32 maxcol = 0;//文字数字　列max
    int sClen = 0;//diment 文字数
    int sRlen = 0;//diment 文字数
    int eClen = 0;//diment 文字数
    int eRlen = 0;//diment 文字数

    void GetCtagValue();
    void GetDiment();
    void GetSelectionPane();
    void Getrow();
    void GetSheetPr();
    void Getcols();
    C* addCtable(C* c, UINT8* tv, UINT8* sv, UINT8* si, UINT32 col, UINT8* v, F* fv);
    cols* addcolatyle(cols* cs, UINT8* min, UINT8* max, UINT8* W, UINT8* sty, UINT8* hid, UINT8* bF, UINT8* cuW);
    cols* coltalloc();
    selection* SLTaddtable(selection* s, UINT8* pv, UINT8* av, UINT8* sv);
    Row* addrows(Row* row, UINT32 r, UINT8* spanS, UINT8* spanE, UINT8* ht, UINT8* thickBot, UINT8* s, UINT8* customFormat, UINT8* customHeight);//row情報　cell情報追加
    Row* searchRow(Row* r, UINT32 newrow);
    Pane* addpanetable(Pane* p, UINT8* x, UINT8* y, UINT8* tl, UINT8* ap, UINT8* sta);
    void getfinalstr();
    void Ctableprint(C* c);
    void sheetread();
    void GetPane();
    //メモリ解放
    void Ctablefree(C* c);
    void Rowtablefree();
    void selectfree();
    void colfree();
    void panefree();
};