#include "encode.h"

encoding::encoding() {
    maxhh = 0;
    tc = 0;//ツリーの要素数
    tcc = 0;//ツリーの要素数　コピー用
    lc = 0;//距離値ツリーの要素数
    lcc = 0;//距離値ツリーの要素数　コピー用
    maxlen = 0;//符号の長さの最大値
    lmaxlen = 0;//距離符号の長さの最大値
    k = 0;//拡張ビットの確認用
    mc = 0;//長さの値のカウント
    l = 0;//距離の値のカウント
    lb = 0;//距離の拡張値のカウント

    encoderv = nullptr;
    rvp = nullptr;//参照ルート
    fir = nullptr;
    lir = nullptr;

    htc = 18;
    datalen = 0;

    so = nullptr;//ポインタ テーブル so[tc]　定義に変数使えない
    sso = nullptr;//ポインタ テーブル　各数値参照用　sso[tc]　定義に変数使えない
    lev = nullptr;//ポインタ テーブル（距離）lev[lc]　定義に変数使えない
    llev = nullptr;//ポインタ
    huh = nullptr;//ポインタ 符号の符号テーブル　huh[hcc]
    huhc = nullptr;//ポインタ 符号の符号テーブル　各数値参照用　huhc[hccc]

    hh = nullptr;//符号の符号テーブル
    hhh = nullptr;
    hhc = nullptr;//符号の符号カウント用ポインタ
    hcc = 0;//数値の種類のカウント
    hccc = 0;
}

encoding::~encoding() {
    free(encoderv);
}

//deflateの先頭3ビットの書き込み準備
void encoding::headerwrite(unsigned char HL, unsigned char HD, unsigned char BI, unsigned char BT) {
    //deflateのヘッダー情報書き込み
    bits.BigendIn(BI & 0x01, 1);//1bit 最終ブロックかどうか
    bits.BigendIn(BT & 0x02, 2);//2bit ビット反転01固定ハフマン 10カスタムハフマン
    bits.BigendIn(HL & 0x1F, 5);//長さ符号の数

    bits.BigendIn(HD & 0x1F, 5);//距離符号の数
    bits.BigendIn((htc - 4) & 0x0F, 4);//HCLEN用の数
    for (int i = 0; i < htc; i++) {//書き込む
        if (bits.bitpos > 7) {
            bits.LittleendOut(8);//下位から出す
            *encoderv = bits.bitoutvalue & 0xFF;
            encoderv++;
            datalen++;
        }
        bits.BigendIn(hht[i], 3);//下位ビットを先頭に流す
    }
}

void encoding::huhtable(struct tnode* a[], UINT8 b) {
    //initialize
    for (int i = 0; i < 19; i++) {
        hht[i] = 0;
    }
    htc = 18;
    //符号の符号長を書き込む
    //16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15の並び
    for (int i = 0; i < 19; i++) {
        for (int j = 0; j < b; j++) {
            if (i == a[j]->num) {//番号が一致した
                switch (a[j]->num) {
                case 16:hht[0] = a[j]->chi; break;//符号長を格納
                case 17:hht[1] = a[j]->chi; break;
                case 18:hht[2] = a[j]->chi; break;
                case 0:hht[3] = a[j]->chi; break;
                case 8:hht[4] = a[j]->chi; break;
                case 7:hht[5] = a[j]->chi; break;
                case 9:hht[6] = a[j]->chi; break;
                case 6:hht[7] = a[j]->chi; break;
                case 10:hht[8] = a[j]->chi; break;
                case 5:hht[9] = a[j]->chi; break;
                case 11:hht[10] = a[j]->chi; break;
                case 4:hht[11] = a[j]->chi; break;
                case 12:hht[12] = a[j]->chi; break;
                case 3:hht[13] = a[j]->chi; break;
                case 13:hht[14] = a[j]->chi; break;
                case 2:hht[15] = a[j]->chi; break;
                case 14:hht[16] = a[j]->chi; break;
                case 1:hht[17] = a[j]->chi; break;
                case 15:hht[18] = a[j]->chi; break;
                }
            }
        }
    }
    while (hht[htc] == 0) {
        htc--;
    }
    //要素数は1プラス　HCLEN用　-4
    htc += 1;
}
//データ圧縮開始
void encoding::compress(unsigned char* data, UINT64 dataleng) {
    //ブロック符号数　解凍時　hex 1fff　dec 8191
    //同じパターン検索

    size_t comsize = size_t(dataleng) / 10;
    comsize = comsize * 7;

    encoderv = (unsigned char*)malloc(comsize);
    rvp = encoderv;
    UINT32 readpos = 0;

    //long long int length=dataleng;//データ長 unsigned 不可
    //分割
    while ((dataleng & 0xFFFFFFFF) > readpos) {
        //初期化
        l = 0;
        lb = 0;
        k = 0;
        mc = 0;

        readpos = s.slidesearch(data, dataleng & 0xFFFFFFFF, readpos);

        makeSign();

        //カスタムハフマン符号化

        headerwrite(((sso[tcc - 1]->num) - 256) & 0xFF, (llev[lcc - 1]->num) & 0xFF, 0, 2);

        while (hhh != NULL) {//符号の符号テーブルの符号化
            for (UINT32 i = 0; i < hccc; i++) {
                if (hhh->num == huhc[i]->num) {
                    bits.LittleendIn(huhc[i]->wei, huhc[i]->chi);//符号を下位ビットを先頭に入れる
                    if (hhh->wei > 1) {//1より大きい場合は拡張あり
                        switch (hhh->num) {
                        case 16:bits.BigendIn((hhh->wei) - 3, 2);//2ビットの拡張（0=3...3=6)
                            break;
                        case 17:bits.BigendIn((hhh->wei) - 3, 3);//3ビットの拡張（3~10)
                            break;
                        case 18:bits.BigendIn((hhh->wei) - 11, 7);//7ビットの拡張（11~138)
                            break;
                        default:break;
                        }
                    }
                    while (bits.bitpos > 7) {
                        bits.LittleendOut(8);//上位ビットを先頭に値を得る
                        //圧縮値を追加
                        *encoderv = bits.bitoutvalue & 0xFF;
                        encoderv++;
                        datalen++;
                    }
                }
            }
            hhh = hhh->next;
        }

        //データ符号化
        for (UINT32 i = 0; i < s.milestocksize; i++) {//s.milestocksize
            for (UINT32 j = 0; j < tcc; j++) {
                if (s.mileRoot[i] == sso[j]->num) {//テーブルから一致した値を参照
                    if (sso[j]->num > 256) {
                        bits.LittleendIn(sso[j]->wei, sso[j]->chi);//下位ビットを先頭にchiビット追加
                        k = s.MileBitnumSearch(sso[j]->num);//拡張ビットすうの確認

                        if (k < 300 && k > 0) {//拡張ビットがあった場合
                            bits.BigendIn(s.mile_extensionRoot[mc], k);//長さの拡張値を上位ビットを先頭にkビット追加
                            mc++;

                            while (bits.bitpos > 7) {
                                bits.LittleendOut(8);//上位ビットを先頭に値を得る
                                //圧縮値を追加
                                *encoderv = bits.bitoutvalue & 0xFF;
                                encoderv++;
                                datalen++;
                            }
                        }
                        while (bits.bitpos > 7) {
                            bits.LittleendOut(8);//上位ビットを先頭に値を得る
                            //圧縮値を追加
                            *encoderv = bits.bitoutvalue & 0xFF;
                            encoderv++;
                            datalen++;
                        }

                        //距離符号の検索
                        for (UINT32 t = 0; t < lcc; t++) {
                            if (llev[t]->num == s.distanceRoot[l]) {
                                bits.LittleendIn(llev[t]->wei, llev[t]->chi);//距離テーブルから一致した値の符号を追加
                                k = s.DistanceBitnumSearch(s.distanceRoot[l]);//距離拡張ビット数を得る
                                l++;
                                break;
                            }
                            while (bits.bitpos > 7) {
                                bits.LittleendOut(8);//上位ビットを先頭に値を得る
                                //圧縮値を追加
                                *encoderv = bits.bitoutvalue & 0xFF;
                                encoderv++;
                                datalen++;
                            }
                        }
                        if (k < 300 && k > 0) {//拡張ビットがあった場合
                            //printf("距離拡張ビット数->%3d,値->%3d\n", k, s.distance_extenshonRoot[lb]);
                            bits.BigendIn(s.distance_extenshonRoot[lb], k);//距離の拡張の値を上位ビットを先頭にkビット追加
                            lb++;

                            while (bits.bitpos > 7) {
                                bits.LittleendOut(8);//上位ビットを先頭に値を得る
                                           //圧縮値を追加
                                *encoderv = bits.bitoutvalue & 0xFF;
                                encoderv++;
                                datalen++;
                            }
                        }

                    }
                    else {
                        bits.LittleendIn(sso[j]->wei, sso[j]->chi);//下位ビットを先頭にchiビット追加
                        while (bits.bitpos > 7) {
                            bits.LittleendOut(8);//上位ビットを先頭に値を得る
                            //圧縮値を追加
                            *encoderv = bits.bitoutvalue & 0xFF;
                            encoderv++;
                            datalen++;
                        }
                    }
                }
            }
        }
        free(s.milestock);
        free(s.distancestock);
        free(s.mile_extension_stock);
        free(s.distance_extenshon_stock);

        //huh解放 
        for (int i = 0; i < hcc; i++)
            free(huhc[i]);
        //for (int i = 0; i < hcc; i++)
        //    free(huh[i]);
        //free(huh);

        for (int i = 0; i < tc; i++)
            free(sso[i]);
        //for (int i = 0; i < tc; i++)
        //    free(so[i]);
        //free(so);

        for (int i = 0; i < lcc; i++)
            free(llev[i]);
        //for (int i = 0; i < lcc; i++)
        //    free(lev[i]);
        //free(lev);

        //tab解放
        defl.tabfree(hh);
        //defl.tabfree(hhh);
        //hh = nullptr;
        //hhh = nullptr;
        //defl.limithuffstock=nullptr;//長さ制限用メモリ開放
        free(defl.limithuffstock);
    }
    //メモリ開放
    s.mileRoot = nullptr;
    s.distanceRoot = nullptr;
    s.mile_extensionRoot = nullptr;
    s.distance_extenshonRoot = nullptr;
    //sso = nullptr;
    //free(so);
    //llev = nullptr;
    //free(lev);

    bits.BigendIn(0, 1);//1bit 最終ブロックかどうか
    bits.BigendIn(0, 2);

    while (bits.bitpos > 0) {//残りビットを書き込む
        if (bits.bitpos > 7)
            bits.LittleendOut(8);//残りが8ビットより多い場合は8ビット入れる
        else {
            bits.LittleendIn(0, (8 - bits.bitpos));//残り8より少ない場合は残りビット0入れる
            bits.LittleendOut(8);
        }
        *encoderv = bits.bitoutvalue & 0xFF;
        encoderv++;
        datalen++;
    }
    //最後 0 0 len 0 nlen ffff
    //1 1  custom 256 - 279        7bit        0000000 ～
    //                                      0010111
    bits.LittleendIn(0, 16);
    bits.LittleendIn(0xffff, 16);

    while (bits.bitpos > 7) {//残りビットを書き込む
        bits.LittleendOut(8);//上位ビットを先頭に値を得る
        *encoderv = bits.bitoutvalue & 0xFF;
        encoderv++;
        datalen++;
    }

    bits.BigendIn(1, 1);//1bit 最終ブロックかどうか
    bits.BigendIn(1, 2);
    bits.BigendIn(0, 7);

    while (bits.bitpos > 0) {//残りビットを書き込む
        if (bits.bitpos > 7)
            bits.LittleendOut(8);//残りが8ビットより多い場合は8ビット入れる
        else {
            bits.LittleendIn(0, (8 - bits.bitpos));//残り8より少ない場合は残りビット0入れる
            bits.LittleendOut(8);
        }
        *encoderv = bits.bitoutvalue & 0xFF;
        encoderv++;
        datalen++;
    }
    encoderv = rvp;
}

void encoding::makeSign() {
    tc = 0;
    lc = 0;
    hcc = 0;
    defl.limitnum = 0;
    maxlen = 0;
    lmaxlen = 0;
    fir = nullptr;
    lir = nullptr;
    hh = nullptr;
    //出現回数をテーブルにカウントして入れる
    for (UINT32 i = 0; i < s.milestocksize; i++) {
        fir = defl.addtab(fir, s.mileRoot[i], &tc);
    }

    //距離の出現回数をテーブルにカウントしていれる
    for (UINT32 i = 0; i < s.distancestocksize; i++) {
        lir = defl.addtab(lir, s.distanceRoot[i], &lc);
    }

    tcc = tc;
    lcc = lc;

    UINT32 tnodesize = sizeof(tnode);
    UINT32 ssize = tnodesize * tc;
    UINT32 lsize = tnodesize * lc;

    so = (tnode**)malloc(ssize);//ポインタ テーブル so[tc]　定義に変数使えない
    sso = (tnode**)malloc(ssize);//ポインタ テーブル　各数値参照用　sso[tc]　定義に変数使えない
    lev = (tnode**)malloc(lsize);//ポインタ テーブル（距離）lev[lc]　定義に変数使えない
    llev = (tnode**)malloc(lsize);//ポインタ


    //リテラル・長さテーブルをツリー配列に入れる
    for (UINT32 i = 0; i < tc; i++) {
        so[i] = sso[i] = defl.talloc();
        sso[i] = so[i] = defl.tabcopy(so[i], *fir);
        fir = fir->next;
    }
    //距離値テーブルをツリー配列に入れる
    for (UINT32 i = 0; i < lc; i++) {
        lev[i] = llev[i] = defl.talloc();
        llev[i] = lev[i] = defl.tabcopy(lev[i], *lir);
        lir = lir->next;
    }
    //ハフマン符号の長さを決める
    while (tc > 1) {
        defl.quicksort(so, 0, tc - 1);
        so[tc - 2] = defl.treemake(so[tc - 1], so[tc - 2]);
        so[tc - 1] = NULL;
        tc--;
    }

    //free(fir);
    //free(lir);
    defl.tabfree(fir);
    defl.tabfree(lir);

    //ハフマン符号の長さを決める（距離）
    while (lc > 1) {
        defl.quicksort(lev, 0, lc - 1);
        lev[lc - 2] = defl.treemake(lev[lc - 1], lev[lc - 2]);
        lev[lc - 1] = NULL;
        lc--;
    }
    defl.treeprint(so[0], &maxlen);//最大符号長を得る
    defl.treeprint(lev[0], &lmaxlen);//最大符号長を得る

    defl.shellsort(sso, tcc);//数値でツリー数値を昇順に並べる
    defl.shellsort(llev, lcc);//数値でツリー数値を昇順に並べる（距離）
    defl.makehuff(sso, maxlen + 1, tcc);//ハフマン符号を割り振る　(sso, maxlen + 1, tcc)
    defl.makehuff(llev, lmaxlen + 1, lcc);//ハフマン符号を割り振る

    //符号の符号長の符号長リストを作る
    hh = defl.huhu(sso, llev, tcc, lcc, sso[tcc - 1]->num, llev[lcc - 1]->num);
    hh = defl.runlen(hh);//ランレングス符号変換

    hhh = hh;//ハフマン符号の符号配列はhhhを参照

    //符号の符号数値のカウント　hhはNULLになる
    while (hh != NULL) {
        hhc = defl.addtab(hhc, hh->num, &hcc);
        hh = hh->next;
    }
    hccc = hcc;

    UINT32 hsize = tnodesize * hcc;
    huh = (tnode**)malloc(hsize);//ポインタ 符号の符号テーブル　huh[hcc]
    huhc = (tnode**)malloc(hsize);//ポインタ 符号の符号テーブル　各数値参照用　huhc[hccc]


    if (huh && huhc) {
        for (UINT32 i = 0; i < hcc; i++) {//テーブルをツリー配列に入れる
            huh[i] = huhc[i] = defl.talloc();
            huhc[i] = huh[i] = defl.tabcopy(huh[i], *hhc);
            hhc = hhc->next;
        }

        UINT32 limstocksize = tnodesize * 4;
        defl.limithuffstock = (tnode**)malloc(limstocksize);

        while (hcc > 1) {//符号の符号の長さを決める
            defl.quicksort(huh, 0, hcc - 1);
            huh[hcc - 2] = defl.limitedtreemake(huh[hcc - 1], huh[hcc - 2], huff_of_huff_limit);
            huh[hcc - 1] = NULL;
            hcc--;
        }

        //長さ制限処理
        if (defl.limitnum > 1) {
            while (defl.limitnum > 1) {//2個の場合ソートエラーになる
                //std::cout << "ツリーストック１個以上" << std::endl;
                defl.quicksort(defl.limithuffstock, 0, defl.limitnum - 1);
                defl.limithuffstock[defl.limitnum - 2] = defl.treemake(defl.limithuffstock[defl.limitnum - 1], defl.limithuffstock[defl.limitnum - 2]);
                defl.limitnum--;
            }
            huh[0] = defl.treemake(defl.limithuffstock[0], huh[0]);
        }
        else if (defl.limitnum == 1) {
            huh[0] = defl.treemake(defl.limithuffstock[0], huh[0]);
        }
        defl.limithuffstock = nullptr;//長さ制限用メモリ開放
        defl.treeprint(huh[0], &maxhh);//最大符号長を得る
        defl.shellsort(huhc, hccc);//数値でツリー数値を昇順に並べる
        defl.makehuff(huhc, maxhh + 1, hccc);//ハフマン符号を割り振る
        huhtable(huhc, hccc);//符号の符号長リストを書き込む
    }
}

void encoding::write(FILE* f) {
    for (UINT32 i = 0; i < datalen; i++) {
        fwrite(&rvp[i], sizeof(char), 1, f);
    }
}