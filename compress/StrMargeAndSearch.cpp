#include "StrMargeAndSearch.h"
#include "SearchItemNUm.h"

MargeaSearch::MargeaSearch()
{
}
//３つの文字配列を結合
UINT8* MargeaSearch::tripleStrmarge(UINT8* left, UINT8* center, UINT8* right)
{
    int i = 0;
    int j = 0;
    int t = 0;

    while (left[i] != '\0')
        i++;
    while (center[j] != '\0')
        j++;
    while (right[t] != '\0')
        t++;
    UINT32 msize = (UINT32)i + (UINT32)j + (UINT32)t;
    UINT32 mlsize = msize + 1;
    UINT8* c = (UINT8*)malloc(sizeof(UINT8) * mlsize);

    if (c) {
        for (int h = 0; h < i; h++)
        {
            c[h] = left[h];
        }

        for (int k = 0; k < j; k++)
        {
            c[k + i] = center[k];
        }

        for (int l = 0; l < t; l++)
        {
            c[l + i + j] = right[l];
        }

        c[msize] = '\0';

        return c;
    }

    return nullptr;
}
//色　サイズ　分割
ColorAnSize* MargeaSearch::splitColor(UINT8* cstr) {
    //色　サイズ　分割
    int sp = 0; int spl = 0; int sc = 0;
    UINT32 widec = 0; UINT32 widec_t = 0;
    int i = 0;
    int strlen = 0;
    bool flag = false;
    int skakko = 0;
    int ekakko = 0;
    //UINT8** cands = nullptr;
    ColorAnSize* cas = (ColorAnSize*)malloc(sizeof(ColorAnSize));

    while (cstr[strlen] != '\0')
        strlen++;
    UINT32 msize = (UINT32)strlen + 1;

    UINT8* split = (UINT8*)malloc(msize);
    UINT8* splitspace = (UINT8*)malloc(msize);
    UINT8* scol = (UINT8*)malloc(msize);
    UINT8* ssiz = (UINT8*)malloc(msize);

    if (split && scol && ssiz) {
        if (strlen > 6) {
            i = 0;
            while (cstr[spl] != '\0') {//【　】とばす 3byte
                widec = 0;
                if (spl > 2) {
                    widec = (cstr[spl - 2] << 16);
                    widec_t = (cstr[spl - 1] << 8);
                    widec += widec_t + cstr[spl];
                    widec = widec & (0xFFFFFF);
                }

                if (widec == 0xe38090) {
                    flag = true;
                    skakko = spl - 2;
                    break;
                }
                spl++;
            }
        }
        if (flag) {
            while (i < skakko) {
                split[i] = cstr[i];
                i++;
            }
            while (widec != 0xe38091) {
                widec = 0;
                widec = (cstr[spl] << 16);
                widec_t = (cstr[spl + 1] << 8);
                widec += widec_t + cstr[spl + 2];
                widec = widec & (0xFFFFFF);
                spl++;
            }
            spl += 2;
            while (cstr[spl] != '\0') {
                split[i] = cstr[spl];
                i++; spl++;
            }
            flag = false;
        }
        else {
            while (cstr[i] != '\0') {
                widec = 0;
                split[i] = cstr[i];
                i++;
            }
        }
        split[i] = '\0';

        //全角すぺーす　E38080 削除
        i = 0;
        int j = 0;
        while (split[i] != '\0') {
            widec = 0;
            if (i > 2) {
                widec = (cstr[i - 2] << 16);
                widec += (cstr[i - 1] << 8);
                widec += cstr[i];
                widec = widec & (0xFFFFFF);
            }
            if (widec == 0xe38080) {//全角スペース
                //2個前戻る 代入
                j -= 3;
            }
            else {
                splitspace[j] = split[i];
            }
            j++; i++;
        }
        splitspace[j] = '\0';

        searchItemNum itemchange(nullptr, nullptr);

        spl = 0;

        sc = strlen;
        while (splitspace[spl] != '\0') {//サイズ終わり位置
            if (splitspace[spl] == 'c' && splitspace[spl + 1] == 'm')
                break;
            spl++;
        }

        int colorend = 0;//カラー終わり位置

        if (splitspace[spl - 1] == 'F') {
            cas->size = (UINT8*)malloc(2);// サイズF
            cas->size[0] = 'F'; cas->size[1] = '\0';
            colorend = spl - 1;
        }
        else {
            int pos = spl;//サイズ終わり位置
            spl--;
            while (std::isdigit(splitspace[spl])) {//数字の間
                spl--;
            }
            colorend = spl + 1;
            sc = pos - (spl + 1);
            msize = (UINT32)sc + 1;
            cas->size = (UINT8*)malloc(msize);
            //サイズ取得
            spl++;
            sc = 0;
            while (spl < pos) {
                if (!std::isspace(splitspace[spl]))
                    cas->size[sc] = splitspace[spl];
                sc++; spl++;
            }
            cas->size[sc] = '\0';
        }

        //カラー取得
        spl = 0;
        while (spl < colorend) {// ()とばす
            if (splitspace[spl] == '(') {
                while (splitspace[spl - 1] != ')')
                    spl++;
            }

            else if (!isspace(splitspace[spl]) && splitspace[spl] != '#' && splitspace[spl] != ')') {//スペース & #
                scol[sp] = splitspace[spl]; sp++;
            }
            if (spl != colorend)
                spl++;
        }
        scol[sp] = '\0';

        while (splitspace[sp - 1] == 128 || splitspace[sp - 1] == 227) {// スペースとばす
            sp--;
        }

        msize = (UINT32)sp + 1;
        cas->color = (UINT8*)malloc(msize);

        if (cas->color) {

            for (int k = 0; k < sp; k++)
                cas->color[k] = scol[k];
            cas->color[sp] = '\0';

            free(scol); free(split); free(ssiz); free(splitspace);
        }
    }

    return cas;
}
//行数値と文字列の結合
UINT8* MargeaSearch::margeRowTag(UINT32 r, UINT8* t, int* len)
{
    unsigned char* row_num = nullptr;
    int row_place = 0;
    //行番号を配列に
    row_num = cn.InttoChar(r, &row_place);

    int i = 0;
    while (t[i] != '\0')
        i++;
    row_num = charmarge(t, row_num, i, row_place);

    *len = i + row_place;

    return row_num;
}
//文字列検索
int MargeaSearch::seachcharactors(UINT8* str, UINT8* data, UINT32 dataleng, int startpos)
{
    int i = 0;
    while (str[i] != '\0')
        i++;//文字列の数
    UINT32 msize = (UINT32)i + 1;
    UINT8* sp = (UINT8*)malloc(msize);//検索スライド用

    UINT32 p = (UINT32)startpos;

    if (sp) {
        while (p < dataleng) {
            for (int j = 0; j < i - 1; j++) {
                sp[j] = sp[j + 1];
            }
            sp[i - 1] = data[p];//最後に付け加える
            sp[i] = '\0';//文字列終端を追加
            p++;//位置移動

            int result = strncmp((const char*)sp, (const char*)str, i);
            if (result == 0) {
                free(sp);
                return p;
            }

        }
    }

    return -1;
}
//2 文字列結合
UINT8* MargeaSearch::charmarge(UINT8* a, UINT8* b, int al, int bl)
{
    UINT32 msize = (UINT32)al + (UINT32)bl + 1;
    UINT8* c = (UINT8*)malloc(msize);

    if (c) {
        for (int i = 0; i < al; i++)
        {
            c[i] = a[i];
        }

        for (int i = 0; i < bl; i++)
        {
            c[i + al] = b[i];
        }

        c[al + bl] = '\0';

        return c;
    }

    return nullptr;
}
//列行結合　数値同士
UINT8* MargeaSearch::margeRowAndCol(UINT32 row, UINT32 col, UINT8* tag, int* strlen)
{
    unsigned char* column = nullptr;

    unsigned char* rowandcol = nullptr;
    unsigned char* tagandrc = nullptr;

    int col_place = 0;
    //列番号をchar配列に
    column = cn.ColumnNumtoArray(col, &col_place);

    unsigned char* row_num = nullptr;
    int row_place = 0;

    //行番号を配列に
    row_num = cn.InttoChar(row, &row_place);

    //行列結合
    rowandcol = charmarge(column, row_num, col_place, row_place);

    int i = 0;
    int j = 0;

    while (tag[i] != '\0')
        i++;

    while (rowandcol[j] != '\0')
        j++;

    //tag 結合
    tagandrc = charmarge(tag, rowandcol, i, j);
    //結合後の文字数
    *strlen = i + j;

    return tagandrc;
}
//文字数カウント
int MargeaSearch::charlengsearch(UINT8* c)
{
    int i = 0;
    while (c[i] != '\0')
        i++;
    return i;
}
//一致する文字列を逆から探す　次の位置を返す なければ　nullptr
UINT8* seachReverse(UINT8* c, UINT8* readdata, UINT64 dataleng, UINT32 startpos)
{
    UINT32 i = 0;
    //文字列の数
    while (c[i] != '\0')
        i++;
    UINT32 msize = i + 1;
    UINT8* searchpos = (UINT8*)malloc(msize);//検索スライド用

    int datapos = startpos;

    if (searchpos) {
        while (datapos > 0) {
            for (int j = i - 1; j > 0; j--) {
                searchpos[j] = searchpos[j - 1];
            }
            searchpos[0] = readdata[datapos];//最後に付け加える
            searchpos[i] = '\0';//文字列終端を追加
            datapos--;//位置移動
            msize = (UINT32)i - 1;

            int result = strncmp((char const*)searchpos, (char const*)c, msize);
            if (result == 0) {
                datapos += i + 1;
                for (int k = 0; k < 6; k++) {
                    searchpos[i + k] = readdata[datapos];
                    datapos++;
                }
                searchpos[i + 6] = '\0';
                return searchpos;
            }
        }
    }

    return nullptr;
}