#pragma once

#include "typechange.h"
#include "bitstream.h"
#include "nodeandtab.h"
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <inttypes.h>

#define limsize 4
class deflate {
public:

    int fugoDataSize;

    bitstream bit;

    int k;

    struct tnode** limithuffstock;//長さ制限ハフマンのストック

    int limitnum;

    int stocktree;//格納数

    deflate();
    ~deflate();

    struct tnode* talloc(void);
    struct tab* ttalloc(void);
    struct tnode* addtree(struct tnode* p, UINT32 n, UINT32* c);
    struct tab* addtab(struct tab* t, UINT32 n, UINT32* c);
    struct tab* addhuftab(struct tab* t, struct tnode* p, UINT32* c, UINT32* s);
    void treeprint(struct tnode* p, UINT32* m);
    void tabprint(struct tab* p);
    struct tab* makehuhu(struct tnode m, struct tab* n, UINT32* c);
    struct tab* huhu(struct tnode* n[], struct tnode* m[], UINT32 t, UINT32 l, UINT32 lim, UINT32 llim);
    struct tab* runmake(struct tab* s, UINT32 c, UINT32 n);
    struct tab* runlen(struct tab* a);
    //ツリー全体のchiに1プラス
    void treeplas(struct tnode* p);
    //tnodeの参照を作る
    struct tnode* tabcopy(struct tnode* p, struct tab s);
    //qsort:v[left]...v[rigth]を降順にソートする
    void quicksort(struct tnode* v[], int left, int right);

    //数値で昇順にソートする
    void shellsort(struct tnode* v[], int n);
    //符号で昇順にソートする
    void shellsortFugo(struct tnode* v[], int n);
    void swap(struct tnode* v[], int i, int j);
    //ハフマン符号長さを決める
    struct tnode* treemake(struct tnode* r, struct tnode* l);
    //ハフマン符号を割り振る
    void makehuff(struct tnode* h[], UINT32 f, UINT32 l);
    //ハフマン符号長さを決める(長さ制限付き）
    struct tnode* limitedtreemake(struct tnode* r, struct tnode* l, UINT32 lim);
    //tab解放
    void tabfree(struct tab* t);
    //node解放
    void nodefree(struct tnode* tn);

};
