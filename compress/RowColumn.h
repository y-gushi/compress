#pragma once

#include "typechange.h"
#include "ChangeArrayNumber.h"
#include "shareRandW.h"
#include "TagAndItems.h"
#include <iostream>

class Ctags {
public:
    Ctags(UINT8 *decorddata,UINT64 datalen,shareRandD *shdata);

    ~Ctags();

    ArrayNumber NA;
    selection* SCT = nullptr;

    const char* Ctag = "<c";
    const char* Vtag = "<v>";
    const char* endVtag = "</v>";
    const char* SVend = "</sheetViews>";//13
    const char* endC = "</c>";
    const char* Ftag = "<f";
    const char* endFtag = "</f>";
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

    Row* rows = nullptr;
    selection* sct = nullptr;
    demention* dm = nullptr;
    cols* cls = nullptr;
    Pane* Panes = nullptr;

    UINT64 p = 0;
    UINT8* fstr = nullptr;

    UINT8* headXML = nullptr;// <sheetPrまでの文字
    UINT8* dimtopane = nullptr;// dimension />から<pane 閉じタグまでの文字
    UINT8* sFPr = nullptr;//sheetFormatPrの取得
    UINT8* MC = nullptr;//マージセル数

    UINT8* data;//デコードデータ 解放　デコードで
    UINT8* wd;//データ更新用
    UINT64 dlen = 0;//デコードデータ長
    UINT8* coden;//コードネーム
    shareRandD *sh;//share配列
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
    void getcolv();
    
    C* addCtable(C* c, UINT8* tv, UINT8* sv, UINT8* si, UINT32 col, UINT8* v, F* fv);
    cols* addcolatyle(cols* cs, UINT8* min, UINT8* max, UINT8* W, UINT8* sty, UINT8* hid, UINT8* bF, UINT8* cuW);
    cols* coltalloc();
    selection* SLTaddtable(selection* s, UINT8* pv, UINT8* av, UINT8* sv);
    Row* addrows(Row* row, UINT32 r, UINT8* spanS, UINT8* spanE, UINT8* ht, UINT8* thickBot, UINT8* s, UINT8* customFormat, UINT8* customHeight);//row情報　cell情報追加
    Row* searchRow(Row* r, UINT32 newrow);
    Pane* addpanetable(Pane* p, UINT8* x, UINT8* y, UINT8* tl, UINT8* ap, UINT8* sta);
    F* getFtag();
    void getfinalstr();
    
    void getCtag();
    UINT8* getvalue();
    UINT8* getVorftag(const char* tag,UINT32 taglen,UINT32 *size);
    UINT8* getSi(UINT8* v,UINT32 vl);
    
    void Ctablefree(C* c);
    void Rowtablefree();
    void selectfree();
    void colfree();
    void panefree();
    void Ctableprint(C* c);
    void sheetread();
    void GetPane();

    const char* closetag = "\"/>";
    int writep = 0;

    void addcelldata(UINT8* row, UINT8* col, UINT8* t, UINT8* s, UINT8* v, F* f, UINT8* si);//cel挿入 rowspan変更
    void writesheetdata();
    void writecols();
    void writeDiment();
    void writeSelection();
    void writecells();
    void writec(C* ctag, UINT8* ROW);
    void writefinal();

    UINT8* StrInit();
};
