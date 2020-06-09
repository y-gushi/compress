#include "deflate.h"

deflate::deflate()
{
    fugoDataSize = 0;
    k = 0;
    limitnum = 0;

    stocktree = 0;
    limithuffstock = nullptr;
}

deflate::~deflate()
{
    free(limithuffstock);
}

tnode* deflate::talloc(void)
{
    return (struct tnode*)malloc(sizeof(struct tnode));
}

tab* deflate::ttalloc(void)
{
    return (struct tab*)malloc(sizeof(struct tab));
}

tnode* deflate::addtree(tnode* p, UINT32 n, UINT32* c)
{
    if (p == NULL) {
        p = talloc();
        p->num = n;
        p->wei = 1;
        (*c)++;
        p->left = p->right = NULL;
    }
    else if (n == p->num)
        p->wei++;
    else if (n < p->num)
        p->left = addtree(p->left, n, c);
    else
        p->right = addtree(p->right, n, c);
    return p;
}

tab* deflate::addtab(tab* t, UINT32 n, UINT32* c)
{
    if (t == NULL) {
        t = ttalloc();
        t->num = n;
        t->wei = 1;
        (*c)++;
        t->next = NULL;
    }
    else if (t->num == n)
        t->wei++;
    else
        t->next = addtab(t->next, n, c);
    return t;
}

tab* deflate::addhuftab(tab* t, tnode* p, UINT32* c, UINT32* s)
{
    if (t == NULL) {
        t = ttalloc();
        t->num = p->num;
        t->chi = p->chi;
        t->wei = 1;
        (*c)++;
        t->next = NULL;
    }
    else if (t->chi == *s)//前の符号長と一緒の場合はカウントプラス
        t->wei++;
    else
        t->next = addhuftab(t->next, p, c, s);
    return t;
}

void deflate::treeprint(tnode* p, UINT32* m)
{
    if (p != NULL) {
        treeprint(p->left, m);
        if (p->chi > * m)
            *m = p->chi;
        treeprint(p->right, m);
    }
}

void deflate::tabprint(tab* p)
{
    if (p != NULL) {
        tabprint(p->next);
    }
}

tab* deflate::makehuhu(tnode m, tab* n, UINT32* c)
{
    if (n == NULL) {
        n = ttalloc();
        n->chi = m.chi;
        n->num = (*c)++;
        n->wei = 1;
        n->next = NULL;
    }
    else
        n->next = makehuhu(m, n->next, c);

    return n;
}

tab* deflate::huhu(tnode* n[], tnode* m[], UINT32 t, UINT32 l, UINT32 lim, UINT32 llim)
{
    struct tab* s = nullptr;
    struct tab* b = nullptr;//(tab*)malloc(sizeof(tab));
    UINT32 cou = 0;//要素数確認用
    bool flag = false;
    struct tnode* zero = talloc();
    zero->wei = 0; zero->num = 0; zero->chi = 0; zero->left = zero->right = NULL;
    int stock = 350;//前の値の一時保存
    for (int i = 0; i < lim + 1; i++) {
        for (int j = 0; j < t; j++) {
            if (i == n[j]->num) {
                if (stock == n[j]->chi) {
                    s->wei++;//stockの値と一緒の場合wei++
                }
                else {
                    s = makehuhu(*n[j], s, &cou);//stockと違う場合は新しくtab作る
                    if (cou > 1)
                        s = s->next;
                    else if (cou == 1)
                        b = s;
                }
                flag = true;//一致した値があった場合Tに
                stock = n[j]->chi;//stock更新
            }
        }
        if (flag != true) {//一致した値がなかった場合0を渡す
            if (stock == -1) {
                s->wei++;//stockの値と一緒の場合wei++
            }
            else {
                s = makehuhu(*zero, s, &cou);//stockと違う場合は新しくtab作る
                if (cou > 1)s = s->next;
                else if (cou == 1)b = s;
            }
            stock = -1;
        }
        flag = false;
    }
    for (int i = 0; i < llim + 1; i++) {/*距離符号の符号長カウント*/
        for (int j = 0; j < l; j++) {
            if (i == m[j]->num) {
                if (stock == m[j]->chi) {
                    if (s) {
                        s->wei++;//stockの値と一緒の場合wei++
                    }
                }
                else {
                    s = makehuhu(*m[j], s, &cou);//stockと違う場合は新しくtab作る
                    if (cou > 1)s = s->next;
                    else if (cou == 1)b = s;
                }
                flag = true;//一致した値があった場合Tに
                stock = m[j]->chi;//stock更新
            }
        }
        if (flag != true) {//一致した値がなかった場合0を渡す
            if (stock == -1) {
                if (s) {
                    s->wei++;//stockの値と一緒の場合wei++
                }
            }
            else {
                s = makehuhu(*zero, s, &cou);//stockと違う場合は新しくtab作る
                if (cou > 1)s = s->next;
                else if (cou == 1)b = s;
            }
            stock = -1;
        }
        flag = false;
    }

    return b;
}

tab* deflate::runmake(tab* s, UINT32 c, UINT32 n)
{
    if (s == NULL) {
        s = ttalloc();
        s->num = c;
        s->wei = n;
        s->chi = 0;
        s->next = NULL;
    }
    else {
        s->next = runmake(s->next, c, n);
    }
    return s;
}

tab* deflate::runlen(tab* a)
{
    struct tab* b = NULL;
    int stock = 0;//前の符号長を保存
    while (a != NULL) {
        if (a->chi != 0) {//直前値の繰り返し
            if (a->wei < 4) {//a->wei 連続回数　が3までの数の場合は1つづつ追加
                while (a->wei > 0) {
                    b = runmake(b, a->chi, 1);//一つづつtab作る num->符号長 n繰り返し数
                    a->wei--;
                }
            }
            else {//a->wei 連続回数　が4以上の数の場合は1つ追加後繰り返し拡張
                b = runmake(b, a->chi, 1);//一つ作る
                a->wei--;
                while (a->wei > 0) {//繰り返しの回数
                    if (a->wei < 3) {
                        b = runmake(b, a->chi, 1);//一つづつtab作る
                        a->wei--;
                    }
                    else if (a->wei < 7 && a->wei>2) {//連続回数が3〜6の場合　符号長値 8 が 12 個 (1 + 6 + 5) に拡張されます。
                        b = runmake(b, 16, a->wei);
                        a->wei -= a->wei;
                    }
                    else if (a->wei > 6) {//連続回数が7以上の場合
                        b = runmake(b, 16, 6);
                        a->wei -= 6;
                    }
                }
            }
        }
        else {
            while (a->wei > 0) {//0の場合直前に値を置く必要がない
                if (a->wei < 3) {
                    b = runmake(b, a->chi, 1);
                    a->wei--;//一つ引く*/
                }
                else if (a->wei < 11 && a->wei>2) {//連続回数が3〜10の場合
                    b = runmake(b, 17, a->wei);
                    a->wei -= a->wei;
                }
                else if (10 < a->wei && a->wei < 139) {//連続回数が138以下の場合
                    b = runmake(b, 18, a->wei);
                    a->wei -= a->wei;
                }
                else if (a->wei > 138) {//連続回数が138より大きい場合
                    b = runmake(b, 18, 138);
                    a->wei -= 138;
                }
            }
        }
        stock = a->wei;
        a = a->next;
    }
    return b;
}

void deflate::treeplas(tnode* p)
{
    if (p != NULL) {
        treeplas(p->left);
        p->chi++;
        treeplas(p->right);
    }
}

tnode* deflate::tabcopy(tnode* p, tab s)
{
    p = talloc();
    p->num = s.num;
    p->wei = s.wei;
    p->chi = 0;
    p->left = p->right = NULL;
    return p;
}

void deflate::quicksort(tnode* v[], int left, int right)
{
    int i, last;

    if (left >= right)
        return;//配列の要素が2より少ないときは何もしない
    swap(v, left, (left + right) / 2);
    last = left;
    for (i = left + 1; i <= right; i++)
        if (v[i]->wei > v[left]->wei)
            swap(v, ++last, i);
    swap(v, left, last);
    quicksort(v, left, last - 1);
    quicksort(v, last + 1, right);
}

void deflate::shellsort(tnode* v[], int n)
{
    int gap, i, j;
    struct tnode* h = talloc();

    for (gap = n / 2; gap > 0; gap /= 2)
        for (i = gap; i < n; i++)
            for (j = i - gap; j >= 0 && v[j]->num > v[j + gap]->num; j -= gap) {
                h = v[j];
                v[j] = v[j + gap];
                v[j + gap] = h;
            }
}

void deflate::shellsortFugo(tnode* v[], int n)
{
    int gap, i, j;
    struct tnode* h = talloc();

    for (gap = n / 2; gap > 0; gap /= 2)
        for (i = gap; i < n; i++)
            for (j = i - gap; j >= 0 && v[j]->wei > v[j + gap]->wei; j -= gap) {
                h = v[j];
                v[j] = v[j + gap];
                v[j + gap] = h;
            }
}

void deflate::swap(tnode* v[], int i, int j)
{
    struct tnode* temp = talloc();
    temp = v[i];
    v[i] = v[j];
    v[j] = temp;
    
}

tnode* deflate::treemake(tnode* r, tnode* l)
{
    struct tnode* p = talloc();

    p->num = -1;
    p->wei = r->wei + l->wei;
    p->chi = (r->chi > l->chi ? r->chi : l->chi);
    p->left = p->right = NULL;

    if (r->chi < l->chi) {//chi大きい方が左
        p->right = r;
        p->left = l;
    }
    else if (r->chi > l->chi) {//chi大きい方が右
        p->right = l;
        p->left = r;
    }
    else {
        if (r->wei < l->wei) {//chiが一緒の場合　重み小さい方が左
            p->right = l;
            p->left = r;
        }
        else if (r->wei > l->wei) {
            p->right = r;
            p->left = l;
        }
        else {
            if (r->num < l->num) {//重みとchiが一緒の場合　数値大きい方が左
                p->right = r;
                p->left = l;
            }
            else {//数値大きい方が左
                p->right = l;
                p->left = r;
            }
        }
    }
    treeplas(p);
    return p;
}

void deflate::makehuff(tnode* h[], UINT32 f, UINT32 l)
{
    int min = 0, len = 0, k = 0;

    for (int i = 1; i < f; i++) {
        for (int j = 0; j < l; j++) {
            if (i == h[j]->chi) {
                if (len == 0) {
                    h[j]->wei = 0;
                    len = i;
                    k = j;
                }
                else {
                    if (i - len == 0) {
                        h[j]->wei = h[k]->wei + 1;
                        k = j;
                    }
                    else {
                        min = i - len;
                        len = i;
                        h[j]->wei = (h[k]->wei + 1) << min;
                        k = j;
                    }
                }
            }
        }
    }
}

tnode* deflate::limitedtreemake(tnode* r, tnode* l, UINT32 lim)
{
    if (r->chi > lim) {
        limithuffstock[limitnum] = r;
        limitnum++;
        return l;
    }
    else if (l->chi > lim) {
        limithuffstock[limitnum] = l;
        limitnum++;
        return r;
    }

    struct tnode* p = talloc();
    p->num = -1;
    p->wei = r->wei + l->wei;
    p->chi = r->chi > l->chi ? r->chi : l->chi;
    p->left = p->right = NULL;

    if (r->chi < l->chi) {//chi大きい方が左
        p->right = r;
        p->left = l;
    }
    else if (r->chi > l->chi) {//chi大きい方が右
        p->right = l;
        p->left = r;
    }
    else {
        if (r->wei < l->wei) {//chiが一緒の場合　重み小さい方が左
            p->right = l;
            p->left = r;
        }
        else if (r->wei > l->wei) {
            p->right = r;
            p->left = l;
        }
        else {
            if (r->num < l->num) {//重みとchiが一緒の場合　数値大きい方が左
                p->right = r;
                p->left = l;
            }
            else {//数値大きい方が左
                p->right = l;
                p->left = r;
            }
        }
    }
    treeplas(p);
    return p;
}

void deflate::tabfree(tab* t)
{
    struct tab* q;
    while (t != NULL) {
        q = t->next;  /* 次へのポインタを保存 */
        free(t);
        t = q;
    }
}

void deflate::nodefree(tnode* tn)
{
    if (tn != NULL) {
        nodefree(tn->left);
        nodefree(tn->right);
        free(tn);
    }
}
