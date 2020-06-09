#include "SearchItemNUm.h"

searchItemNum::searchItemNum(Items* itemstruct, Ctags* cs) {
    its = itemstruct;
    Cels = cs;
    Mstr = MargeaSearch();
    rootMat = (MatchColrs*)malloc(sizeof(MatchColrs));
    rootMat = nullptr;
}

searchItemNum::~searchItemNum()
{
    free(its);
    free(Cels);
    free(incolumn);
}

bool searchItemNum::searchitemNumber(UINT8* uniq) {
    Row* sr = (Row*)malloc(sizeof(Row));
    sr = Cels->rows;
    Items* Item = (Items*)malloc(sizeof(Items));
    Item = its;
    UINT8* nr = nullptr;//文字入力行　配列
    int result = 0;
    const char* SaT[] = { "s", "4" };
    bool incellflag = false;
    UINT8* matchItem = nullptr;

    sr = Cels->rows;
    while (sr) {//品番検索
        Item = its;
        while (Item) {
            if (sr->cells) {
                if (sr->cells->si) {
                    result = strcmp((const char*)sr->cells->si, (const char*)Item->itn);//品番　シートデータの比較
                    if (result == 0) {
                        matchItem = Item->itn;//一致品ばんコピー
                        C* cn = (C*)malloc(sizeof(C));
                        cn = sr->cells;
                        while (cn->next) {
                            cn = cn->next;
                        }
                        inputColum = cn->col;//現最終列
                        changenum.ColumnIncliment(&inputColum);//最終列　+1
                        nr = changenum.InttoChar(sr->r, &result);//入力行
                        incolumn = changenum.ColumnNumtoArray(inputColum, &result);//最終列　配列変換
                        startR = sr->r;//品番の行
                        incellflag = true;
                        break;
                    }
                }
            }
            Item = Item->next;
        }
        if (incellflag)
            break;
        sr = sr->next;
    }

    if (!incellflag)
        return false;//品番一致なし
    else
        Cels->addcelldata(nr, incolumn, (UINT8*)SaT[0], (UINT8*)SaT[1], uniq, nullptr, nullptr);//最初の一回に変更

    Item = its;

    while (Item) {//item 品番一致　全カラー
        sr = Cels->rows;
        result = strcmp((const char*)matchItem, (const char*)Item->itn);//品番　シートデータの比較
        if (result == 0) {
            colorsearch(sr, Item, Item->itn);
        }
        Item = Item->next;
    }

    return incellflag;
}

void searchItemNum::colorsearch(Row* inrow, Items* IT, UINT8* itn) {
    const char* sizetable[] = { "090","100","110","120","130","140","150","160","F" };
    const char* style[] = { "27","28","29" };
    UINT8 middle[] = "28";
    Row* color = inrow;// = (Row*)malloc(sizeof(Row))
    UINT8* nextColor = (UINT8*)malloc(1); nextColor = nullptr;
    UINT8* nextSize = (UINT8*)malloc(1); nextSize = nullptr;
    UINT8* nowColor = (UINT8*)malloc(1); nowColor = nullptr;
    UINT8* nowSize = (UINT8*)malloc(1); nowSize = nullptr;
    UINT8* beforeColor = (UINT8*)malloc(1); beforeColor = nullptr;
    UINT8* beforeSize = (UINT8*)malloc(1); beforeSize = nullptr;
    ColorAnSize* CandS;
    Items* ite = (Items*)malloc(sizeof(Items));
    ite = IT;
    bool b = false;
    bool n = false;
    int result = 0;
    UINT32 i = 0;//セル文字数
    int matchflag = 0;//文字列　部分一致判定
    unitC sear;//全角変換

    while (color) {//スタート位置まで移動
        if (color->r == startR)
            break;
        color = color->next;
    }

    if (color && color->next) {
        color = color->next;//品番次の行
    }

    while (color) {
        if (color->cells) {
            if (color->cells->si) {//文字列　存在
                i = 0;
                while (color->cells->si[i] != '\0')//文字数カウント
                    i++;

                //全角変換
                ite->col = sear.changenumber(ite->col);
                color->cells->si = sear.changeKana(color->cells->si);//かな変換
                color->cells->si = sear.changenumber(color->cells->si);

                ite->col = sear.slipNum(ite->col);//連続数字　削除

                matchflag = Mstr.seachcharactors(ite->col, color->cells->si, i, 0);//セルカラーとアイテムの比較　部分検索
                if (matchflag != -1) {
                    CandS = Mstr.splitColor(color->cells->si);//カラーとサイズの分割
                    if (CandS->color && CandS->size) {
                        nowColor = CandS->color; nowSize = CandS->size;
                    }
                    else {
                        nowColor = nullptr; nowSize = nullptr;
                    }
                    if (color->next) {
                        if (color->next->cells) {
                            if (color->next->cells->si) {
                                i = 0;
                                while (color->next->cells->si[i] != '\0')//文字数カウント
                                    i++;
                                //次の色全角半角変換
                                color->next->cells->si = sear.changeKana(color->next->cells->si);//かな変換
                                color->next->cells->si = sear.changenumber(color->next->cells->si);
                                matchflag = Mstr.seachcharactors(ite->col, color->next->cells->si, i, 0);//セルカラーとアイテムの比較　部分検索
                                if (matchflag != -1) {
                                    ColorAnSize* nCandnS = Mstr.splitColor(color->next->cells->si);//カラーとサイズの分割
                                    nextColor = nCandnS->color;
                                    nextSize = nCandnS->size;
                                }
                                else {
                                    nextColor = nullptr;
                                    nextSize = nullptr;
                                }
                            }
                            else {
                                nextColor = nullptr; nextSize = nullptr;
                            }
                        }
                        else {
                            nextColor = nullptr; nextSize = nullptr;
                        }
                    }
                    else {
                        nextColor = nullptr; nextSize = nullptr;
                    }
                    if (nowColor && nowSize) {
                        result = strcmp((const char*)nowColor, (const char*)ite->col);//カラー比較
                        if (result == 0) {//カラー一致

                            for (int i = 0; i < 9; i++) {//サイズループ
                                result = strcmp((const char*)nowSize, (const char*)sizetable[i]);
                                if (result == 0) {//サイズ一致

                                    UINT8* sizeMatch = (UINT8*)malloc(1); sizeMatch = nullptr;//サイズ保存用
                                    switch (i) {//該当サイズ
                                    case 0:sizeMatch = ite->s90; break;
                                    case 1:sizeMatch = ite->s100; break;
                                    case 2:sizeMatch = ite->s110; break;
                                    case 3:sizeMatch = ite->s120; break;
                                    case 4:sizeMatch = ite->s130; break;
                                    case 5:sizeMatch = ite->s140; break;
                                    case 6:sizeMatch = ite->s150; break;
                                    case 7:sizeMatch = ite->s160; break;
                                    case 8:sizeMatch = ite->sF; break;
                                    default:break;
                                    }
                                    if (sizeMatch) {
                                        rootMat = addmatches(rootMat, itn, nowColor);//カラー、サイズ一致で保存

                                        if (beforeColor) {
                                            result = strcmp((const char*)nowColor, (const char*)beforeColor);
                                            if (result == 0) {
                                                b = true;
                                            }
                                            else {
                                                b = false;
                                            }
                                        }
                                        else {
                                            b = false;
                                        }
                                        if (nextColor) {
                                            result = strcmp((const char*)nowColor, (const char*)nextColor);
                                            if (result == 0) {
                                                n = true;
                                            }
                                            else {
                                                n = false;
                                            }
                                        }
                                        else {
                                            n = false;
                                        }
                                        if (b && n) {//真ん中　書き込み
                                            UINT8* nr = changenum.InttoChar(color->r, &result);
                                            Cels->addcelldata(nr, incolumn, nullptr, (UINT8*)style[1], sizeMatch, nullptr, nullptr);
                                        }
                                        else if (b && !n) {//した　書き込み
                                            UINT8* nr = changenum.InttoChar(color->r, &result);
                                            Cels->addcelldata(nr, incolumn, nullptr, (UINT8*)style[2], sizeMatch, nullptr, nullptr);
                                        }
                                        else {//上　書き込み
                                            UINT8* nr = changenum.InttoChar(color->r, &result);
                                            Cels->addcelldata(nr, incolumn, nullptr, (UINT8*)style[0], sizeMatch, nullptr, nullptr);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

            }
            if (color->cells->si) {
                beforeColor = nowColor;
                beforeSize = nowSize;
            }
            else {
                beforeColor = nullptr;
                beforeSize = nullptr;
            }
        }
        color = color->next;
    }
}

MatchColrs* searchItemNum::addmatches(MatchColrs* m, UINT8* i, UINT8* c)
{
    if (!m) {
        m = (MatchColrs*)malloc(sizeof(MatchColrs));
        if (m) {
            m->itemnum = i;
            m->color = c;
            m->next = nullptr;
        }
    }
    else {
        m->next = addmatches(m->next, i, c);
    }
    return m;
}

Items* searchItemNum::addItems(Items* base, Items* itm)
{
    if (!base) {
        base = (Items*)malloc(sizeof(Items));
        if (base) {
            base->itn = itm->itn;
            base->col = itm->col;
            base->next = nullptr;
        }
    }
    else {
        base->next = addItems(base->next, itm);
    }

    return base;
}

char* searchItemNum::CharChenge(UINT8* cc) {
    char* col = (char*)cc;

    std::string st = std::string(col);
    //stockcolor = splitcolor;
    //次を参照
    //Unicodeへ変換後の文字列長を得る
    int lenghtUnicode = MultiByteToWideChar(CP_UTF8, 0, st.c_str(), st.size() + 1, NULL, 0);

    //必要な分だけUnicode文字列のバッファを確保
    wchar_t* bufUnicode = new wchar_t[lenghtUnicode];

    //string str((istreambuf_iterator<char>(splitcolor)), istreambuf_iterator<char>());
    MultiByteToWideChar(CP_UTF8, 0, st.c_str(), st.size() + 1, bufUnicode, lenghtUnicode);

    //ShiftJISへ変換後の文字列長を得る
    int lengthSJis = WideCharToMultiByte(CP_THREAD_ACP, 0, bufUnicode, -1, NULL, 0, NULL, NULL);

    //必要な分だけShiftJIS文字列のバッファを確保
    char* bufShiftJis = new char[lengthSJis];

    //UnicodeからShiftJISへ変換
    WideCharToMultiByte(CP_THREAD_ACP, 0, bufUnicode, lenghtUnicode + 1, bufShiftJis, lengthSJis, NULL, NULL);

    std::string strSJis(bufShiftJis);
    return bufShiftJis;
}