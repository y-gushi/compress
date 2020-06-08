#include "decode.h"

struct decodenode* DeflateDecode::daddtree(struct decodenode* p, UINT32 w, UINT32 a, UINT32 b) {
    if (p == NULL) {//新しい単語がきた
        p = dalloc();//新しい単語
        p->fugo = w;//新しいノードを作る
        p->fugolen = a;
        p->val = b;
        p->left = p->right = NULL;
    }
    else if (w == p->fugo)
        p->count++;//繰り返された単語
    else if (w < p->fugo)//より小さければ、左部分木へ
        p->left = daddtree(p->left, w, a, b);
    else//大きければ、右部分木へ
        p->right = daddtree(p->right, w, a, b);
    return p;
}

void DeflateDecode::freetree(struct decodenode* p) {
    if (p != NULL) {
        freetree(p->left);
        freetree(p->right);
        free(p);
    }
}

UINT32* DeflateDecode::copy(UINT32 s) {/*sの複製を作る*/
    UINT32* p;

    p = (UINT32*)malloc(sizeof(UINT32));
    if (p != NULL) {
        p = &s;
    }
    return p;
}

struct decodenode* DeflateDecode::dalloc(void) {/*dalloc dnodeを作る*/
    return (struct decodenode*)malloc(sizeof(struct decodenode));
}

void DeflateDecode::dtreeprint(struct decodenode* p) {
    if (p != NULL) {
        dtreeprint(p->left);
        printf("%4d %4d %4d\n", p->fugo, p->fugolen, p->val);
        dtreeprint(p->right);
    }
}

struct decodenode* DeflateDecode::dsearchVal(struct decodenode* n) {
    return n->right;
}

UINT32 DeflateDecode::dsearch_node(UINT32 x, struct decodenode* node, UINT32 u)
{
    while (node != NULL) {
        if (node->fugo == x && node->fugolen == u) {
            return node->val;
        }
        else if (x < node->fugo)
            node = node->left;
        else
            node = node->right;
    }
    return 65536;
}

Hclen::Hclen() {
    hclenv = 0;
    huffHclen = 0;
    hufflen = 0;
}

void Hclen::dswap(Hclen v[], int n) {//上昇順にソート(符号長）
    int gap, i, j;
    Hclen temp;

    for (gap = n / 2; gap > 0; gap /= 2) {
        for (i = gap; i < 19; i++)
            for (j = i - gap; j >= 0 && v[j].hclenv > v[j + gap].hclenv; j -= gap) {
                temp = v[j];
                v[j] = v[j + gap];
                v[j + gap] = temp;
            }
    }
}

void Hclen::dmakehuff(Hclen h[]) {
    int min = 0, len = 0, k = 0;

    for (int i = 1; i < 8; i++) {
        for (int j = 0; j < 19; j++) {
            if (i == h[j].hufflen) {
                if (len == 0) {
                    h[j].huffHclen = 0;
                    len = i;
                    k = j;
                }
                else {
                    if (i - len == 0) {
                        h[j].huffHclen = h[k].huffHclen + 1;
                        k = j;
                    }
                    else {
                        min = i - len;
                        len = i;
                        h[j].huffHclen = (h[k].huffHclen + 1) << min;
                        k = j;
                    }
                }
            }
        }
    }
}

DeflateDecode::DeflateDecode(std::ifstream* f) {
    fr = f;
    ReadV = nullptr;
    ReadVroot = nullptr;
    readlen = 0;
    rev = 0;
    signs = nullptr;
    signSize = 0;
    BFINAL = 0;
    BTYPE = 0;
    HCLEN = 0;
    HDIST = 0;
    HLIT = 0;
    crc = 0;

    obh[0].hclenv = 16; obh[1].hclenv = 17; obh[2].hclenv = 18; obh[3].hclenv = 0; obh[4].hclenv = 8; obh[5].hclenv = 7; obh[6].hclenv = 9; obh[7].hclenv = 6;
    obh[8].hclenv = 10; obh[9].hclenv = 5; obh[10].hclenv = 11; obh[11].hclenv = 4; obh[12].hclenv = 12; obh[13].hclenv = 3; obh[14].hclenv = 13; obh[15].hclenv = 2;
    obh[16].hclenv = 14; obh[17].hclenv = 1; obh[18].hclenv = 15;
}
DeflateDecode::~DeflateDecode() {
    free(ReadV);
}

void DeflateDecode::headdataread() {

    while (bit.bitpos < 8) {
        fr->read((char*)&rev, 1);
        bit.BigendIn((rev & 0xFF), 8);
    }

    bit.BigendOut(1);
    BFINAL = bit.bitoutvalue;
    bit.LittleendOut(2);
    BTYPE = bit.bitoutvalue;
    std::cout << "圧縮タイプ　" << short(BTYPE) << std::endl;
    if (BTYPE == 2) {
        bit.LittleendOut(5);
        HLIT = bit.bitoutvalue;//HLIT
        fr->read((char*)&rev, 1);
        bit.BigendIn(rev, 8);
        bit.LittleendOut(5);

        HDIST = bit.bitoutvalue;
        fr->read((char*)&rev, 1);
        bit.BigendIn(rev, 8);
        bit.LittleendOut(4);

        HCLEN = bit.bitoutvalue;
    }

}

void DeflateDecode::milesearch(UINT32 mil) {
    switch (mil) {
    case 257:miles = 3; break;
    case 258:miles = 4; break;
    case 259:miles = 5; break;
    case 260:miles = 6; break;
    case 261:miles = 7; break;
    case 262:miles = 8; break;
    case 263:miles = 9; break;
    case 264:miles = 10; break;
    case 265:while (bit.bitpos < 1) byteread(); bit.LittleendOut(1); miles = 11 + bit.bitoutvalue; break;
    case 266:while (bit.bitpos < 1) byteread(); bit.LittleendOut(1); miles = 13 + bit.bitoutvalue; break;
    case 267:while (bit.bitpos < 1) byteread(); bit.LittleendOut(1); miles = 15 + bit.bitoutvalue; break;
    case 268:while (bit.bitpos < 1) byteread(); bit.LittleendOut(1); miles = 17 + bit.bitoutvalue; break;
    case 269:while (bit.bitpos < 2) byteread(); bit.LittleendOut(2); miles = 19 + bit.bitoutvalue; break;
    case 270:while (bit.bitpos < 2) byteread(); bit.LittleendOut(2); miles = 23 + bit.bitoutvalue; break;
    case 271:while (bit.bitpos < 2) byteread(); bit.LittleendOut(2); miles = 27 + bit.bitoutvalue; break;
    case 272:while (bit.bitpos < 2) byteread(); bit.LittleendOut(2); miles = 31 + bit.bitoutvalue; break;
    case 273:while (bit.bitpos < 3) byteread(); bit.LittleendOut(3); miles = 35 + bit.bitoutvalue; break;
    case 274:while (bit.bitpos < 3) byteread(); bit.LittleendOut(3); miles = 43 + bit.bitoutvalue; break;
    case 275:while (bit.bitpos < 3) byteread(); bit.LittleendOut(3); miles = 51 + bit.bitoutvalue; break;
    case 276:while (bit.bitpos < 3) byteread(); bit.LittleendOut(3); miles = 59 + bit.bitoutvalue; break;
    case 277:while (bit.bitpos < 4) byteread(); bit.LittleendOut(4); miles = 67 + bit.bitoutvalue; break;
    case 278:while (bit.bitpos < 4) byteread(); bit.LittleendOut(4); miles = 83 + bit.bitoutvalue; break;
    case 279:while (bit.bitpos < 4) byteread(); bit.LittleendOut(4); miles = 99 + bit.bitoutvalue; break;
    case 280:while (bit.bitpos < 4) byteread(); bit.LittleendOut(4); miles = 115 + bit.bitoutvalue; break;
    case 281:while (bit.bitpos < 5) byteread(); bit.LittleendOut(5); miles = 131 + bit.bitoutvalue; break;
    case 282:while (bit.bitpos < 5) byteread(); bit.LittleendOut(5); miles = 163 + bit.bitoutvalue; break;
    case 283:while (bit.bitpos < 5) byteread(); bit.LittleendOut(5); miles = 195 + bit.bitoutvalue; break;
    case 284:while (bit.bitpos < 5) byteread(); bit.LittleendOut(5); miles = 227 + bit.bitoutvalue; break;
    case 285:miles = 258; break;
        //case 285:while (bit.bitpos < 16) byteread(); bit.LittleendOut(16); miles = 0 + bit.bitoutvalue; break;
    default:break;
    }
}

void DeflateDecode::distancesearch(UINT32 dis) {
    switch (dis) {
    case 0:len = 1; break;
    case 1:len = 2; break;
    case 2:len = 3; break;
    case 3:len = 4; break;
    case 4:while (bit.bitpos < 1) byteread(); bit.LittleendOut(1); len = 5 + bit.bitoutvalue; break;
    case 5:while (bit.bitpos < 1) byteread(); bit.LittleendOut(1); len = 7 + bit.bitoutvalue; break;
    case 6:while (bit.bitpos < 2) byteread(); bit.LittleendOut(2); len = 9 + bit.bitoutvalue; break;
    case 7:while (bit.bitpos < 2) byteread(); bit.LittleendOut(2); len = 13 + bit.bitoutvalue; break;
    case 8:while (bit.bitpos < 3) byteread(); bit.LittleendOut(3); len = 17 + bit.bitoutvalue; break;
    case 9:while (bit.bitpos < 3) byteread(); bit.LittleendOut(3); len = 25 + bit.bitoutvalue; break;
    case 10:while (bit.bitpos < 4) byteread(); bit.LittleendOut(4); len = 33 + bit.bitoutvalue; break;
    case 11:while (bit.bitpos < 4) byteread(); bit.LittleendOut(4); len = 49 + bit.bitoutvalue; break;
    case 12:while (bit.bitpos < 5) byteread(); bit.LittleendOut(5); len = 65 + bit.bitoutvalue; break;
    case 13:while (bit.bitpos < 5) byteread(); bit.LittleendOut(5); len = 97 + bit.bitoutvalue; break;
    case 14:while (bit.bitpos < 6) byteread(); bit.LittleendOut(6); len = 129 + bit.bitoutvalue; break;
    case 15:while (bit.bitpos < 6) byteread(); bit.LittleendOut(6); len = 193 + bit.bitoutvalue; break;
    case 16:while (bit.bitpos < 7) byteread(); bit.LittleendOut(7); len = 257 + bit.bitoutvalue; break;
    case 17:while (bit.bitpos < 7) byteread(); bit.LittleendOut(7); len = 385 + bit.bitoutvalue; break;
    case 18:while (bit.bitpos < 8) byteread(); bit.LittleendOut(8); len = 513 + bit.bitoutvalue; break;
    case 19:while (bit.bitpos < 8) byteread(); bit.LittleendOut(8); len = 769 + bit.bitoutvalue; break;
    case 20:while (bit.bitpos < 9) byteread(); bit.LittleendOut(9); len = 1025 + bit.bitoutvalue; break;
    case 21:while (bit.bitpos < 9) byteread(); bit.LittleendOut(9); len = 1537 + bit.bitoutvalue; break;
    case 22:while (bit.bitpos < 10) byteread(); bit.LittleendOut(10); len = 2049 + bit.bitoutvalue; break;
    case 23:while (bit.bitpos < 10) byteread(); bit.LittleendOut(10); len = 3073 + bit.bitoutvalue; break;
    case 24:while (bit.bitpos < 11) byteread(); bit.LittleendOut(11); len = 4097 + bit.bitoutvalue; break;
    case 25:while (bit.bitpos < 11) byteread(); bit.LittleendOut(11); len = 6145 + bit.bitoutvalue; break;
    case 26:while (bit.bitpos < 12) byteread(); bit.LittleendOut(12); len = 8193 + bit.bitoutvalue; break;
    case 27:while (bit.bitpos < 12) byteread(); bit.LittleendOut(12); len = 12289 + bit.bitoutvalue; break;
    case 28:while (bit.bitpos < 13) byteread(); bit.LittleendOut(13); len = 16385 + bit.bitoutvalue; break;
    case 29:while (bit.bitpos < 13) byteread(); bit.LittleendOut(13); len = 24577 + bit.bitoutvalue; break;
    case 30:while (bit.bitpos < 14) byteread(); bit.LittleendOut(14); len = 32769 + bit.bitoutvalue; break;
    case 31:while (bit.bitpos < 14) byteread(); bit.LittleendOut(14); len = 49153 + bit.bitoutvalue; break;
    default:break;
    }
}

void DeflateDecode::byteread()
{
    fr->read((char*)&rev, 1);
    bit.BigendIn((rev & 0xFF), 8);
}

char* DeflateDecode::copyfn(char* f) {
    char* p = (char*)malloc(30);
    p = f;
    return p;
}

DeflateDecode* DeflateDecode::memorykeep(DeflateDecode* d)
{
    DeflateDecode* p = (DeflateDecode*)malloc(sizeof(DeflateDecode));
    p = d;

    return p;
}

DecSelect* DeflateDecode::nextadd(DecSelect* ds, DeflateDecode* decdata, char* fn)
{
    if (ds == nullptr) {
        ds = (DecSelect*)malloc(sizeof(DecSelect));
        if (ds) {
            ds->decorddata = memorykeep(decdata); //memorykeep(fn, po);
            ds->Fn = copyfn(fn);
            ds->next = nullptr;
        }
    }
    else {
        ds->next = nextadd(ds->next, decdata, fn);
    }

    return ds;
}

UINT32 DeflateDecode::searchLiteral(unsigned int n, UINT8 leng)
{
    int a = 0;
    if (n >= 48 && n <= 191 && leng == 8) {
        a = n - 48;
        return a;
    }
    else if (n >= 400 && n <= 511 && leng == 9) {
        a = n - 256;
        return a;
    }
    else if (n >= 0 && n <= 23 && leng == 7) {
        a = n + 256;
        return a;
    }
    else if (n >= 192 && n <= 195 && leng == 8) {
        a = n + 88;
        return a;
    }
    else {
        return 65536;
    }
}

void DeflateDecode::FixedRead()
{
    UINT8 dlen = 7;//初期符号長をセット
    k = 0;
    miles = 0;
    len = 0;

    while (k != 256) {
        while (bit.bitpos < 10) {
            fr->read((char*)&rev, 1);
            bit.BigendIn(rev & 0xFF, 8);
        }
        if (dlen > 7) {
            unsigned int f = bit.bitoutvalue << 1;
            bit.BigendOut(1);
            bit.bitoutvalue = f + bit.bitoutvalue;
        }
        else { bit.BigendOut(7); }

        k = searchLiteral(bit.bitoutvalue, dlen);

        if (k == 65536) { dlen++; }
        else if (k > 256 && k < 65536) {

            milesearch(k);

            while (bit.bitpos < 5) {
                byteread();
            }

            bit.BigendOut(5);

            distancesearch(bit.bitoutvalue);
            int ml = 0;
            UINT32 lpos = 0;
            lpos = readlen - len;

            if (miles > len) {
                ml = miles - len;
                for (UINT32 h = 0; h < miles / len; h++) {
                    for (UINT32 j = 0; j < len; j++) {
                        *ReadV = *(ReadVroot + lpos + j);
                        //std::cout << *ReadV << ",";
                        ReadV++;
                        readlen++;
                    }
                }
                if (ml % len != 0) {
                    for (UINT32 i = 0; i < miles % len; i++) {
                        *ReadV = *(ReadVroot + lpos + i);
                        //std::cout << *ReadV << ",";
                        ReadV++;
                        readlen++;
                    }
                }
            }
            else {
                for (UINT32 i = 0; i < miles; i++) {
                    *ReadV = *(ReadVroot + lpos + i);
                    //std::cout << *ReadV << ",";
                    ReadV++;
                    readlen++;
                }
            }
            dlen = 7;
        }
        else if (k < 256 && k >= 0) {
            *ReadV = k;
            std::cout << *ReadV << ",";
            ReadV++;
            readlen++;
            dlen = 7;
        }
    }
}

void DeflateDecode::CustomRead()
{
    count = 1;
    k = 0;
    miles = 0;
    len = 0;
    UINT32 fugoleng = 0;

    while (k != 256) {
        //読み込み済みデータの数計算

        while (bit.bitpos < count) {
            fr->read((char*)&rev, 1);
            bit.BigendIn((rev & 0xFF), 8);
        }
        if (count > 1) {
            unsigned int f = bit.bitoutvalue << 1;
            bit.BigendOut(1);
            bit.bitoutvalue = f + bit.bitoutvalue;
        }
        else { bit.BigendOut(1); }
        k = dsearch_node(bit.bitoutvalue, root, count);
        if (k == 65536) {
            count++;
        }
        else if (k > 256 && k < 65536) {
            milesearch(k);
            count = 1;
            //printf("一致長さ：%3d、値：%3d\n",miles,k);
            k = 65536;
            while (k == 65536) {
                while (bit.bitpos < count) {
                    fr->read((char*)&rev, 1);
                    bit.BigendIn((rev & 0xFF), 8);
                }
                if (count > 1) {
                    int f = bit.bitoutvalue << 1;
                    bit.BigendOut(1);
                    bit.bitoutvalue = f + bit.bitoutvalue;
                }
                else { bit.BigendOut(1); }

                k = dsearch_node(bit.bitoutvalue, lenroot, count);
                if (k == 65536) {
                    count++;
                }
                else {
                    distancesearch(k);
                    fugoleng++;
                    //printf("一致距離：%3d、値：%3d\n", len,k);
                    int ml = 0;
                    UINT32 lpos = 0;
                    lpos = readlen - len;

                    if (miles > len) {
                        ml = miles - len;
                        for (UINT32 h = 0; h < miles / len; h++) {
                            for (UINT32 j = 0; j < len; j++) {
                                *ReadV = *(ReadVroot + lpos + j);
                                ReadV++;
                                readlen++;
                                //lpos++;
                            }
                        }
                        if (ml % len != 0) {
                            for (UINT32 i = 0; i < miles % len; i++) {
                                *ReadV = *(ReadVroot + lpos + i);
                                ReadV++;
                                readlen++;
                            }
                        }
                    }
                    else {
                        for (UINT32 i = 0; i < miles; i++) {
                            *ReadV = *(ReadVroot + lpos + i);
                            ReadV++;
                            readlen++;
                        }
                    }
                    count = 1;
                }
            }
        }
        else if (k >= 0 && k < 256) {
            //printf("一致riteral 値：%3d\n", k);
            *ReadV = k;
            ReadV++;
            readlen++;
            count = 1;
            fugoleng++;
        }
    }
    freetree(lenroot);
    //lenroot=nullptr;
    //free(lenroot);
    freetree(root);
    //root=nullptr;
    //free(root);
    free(signs);
    free(lentable);
    free(lenlen);
    free(lenval);
    free(hufftable);
    free(hufval);
    free(lenh);
    //std::cout << "解凍　符号数 : " << fugoleng << std::endl;
}

void DeflateDecode::noCompressRead()
{
    unsigned int len = 0;
    unsigned int nlen = 0;

    //4バイト分読み込む
    while (bit.bitpos < 32) {
        byteread();
    }

    //残りbit処理　バイト境界までスキップ
    if (bit.bitpos % 8 != 0) {
        bit.LittleendOut(bit.bitpos % 8);

    }

    if (bit.bitpos >= 32) {
        //LEN 2BYTE
        bit.LittleendOut(16);
        len = bit.bitoutvalue;
        std::cout << "nocompress len : " << len << std::endl;
        //NLEN LENの2の補数
        bit.LittleendOut(16);//65536
        nlen = bit.bitoutvalue;
        //std::cout << "nocompress nlen : " << nlen << std::endl;
    }

    for (UINT32 i = 0; i < len; i++) {
        fr->read((char*)&rev, 1);
        *ReadV = rev & 0xFF;
        ReadV++;
        readlen++;
        //std::cout << "nocompress val : " << rev << std::endl;
    }

}

UINT64 DeflateDecode::dataread(UINT64 position, UINT32 size) {

    
    fr->seekg(position, std::ios::beg);

    UINT32 msize = size + 1000;
    ReadV = (unsigned char*)malloc(sizeof(UINT8) * msize);
    if (!ReadV) {
        return 0;
    }
    else {
        ReadVroot = ReadV;

        do {
            headdataread();

            if (BTYPE == 0)
            {
                //圧縮なし
                noCompressRead();

            }
            else if (BTYPE == 1) {
                //固定ハフマン
                FixedRead();
            }
            else if (BTYPE == 2) {
                //カスタムハフマン

                haffmanread();

                CustomRead();

            }
            else if (BTYPE == 3) {
                headdataread();
            }

        } while (BFINAL != 1);




        ReadV = ReadVroot;

        UINT64 po = fr->tellg();

        //fr->close();

        crc = cr.mcrc(ReadV, readlen);

        return po;
    }
    return 0;
}

int DeflateDecode::haffmanread() {

    //初期化
    signs = (unsigned int*)malloc(sizeof(unsigned int) * 400);
    if (!signs) {}
    //signsRoot = signs;

    signSize = 0;

    for (int i = 0; i < 19; i++) {
        obh[i].huffHclen = 0;
        obh[i].hufflen = 0;
    }
    n = 1;
    pu = 1;
    hu = 0;
    length = 0;
    left = 15;
    right = 0;
    stock = 0;
    root = nullptr;
    lenroot = nullptr;
    hufhuf = nullptr;
    k = 0;

    obh[0].hclenv = 16; obh[1].hclenv = 17; obh[2].hclenv = 18; obh[3].hclenv = 0; obh[4].hclenv = 8; obh[5].hclenv = 7; obh[6].hclenv = 9; obh[7].hclenv = 6;
    obh[8].hclenv = 10; obh[9].hclenv = 5; obh[10].hclenv = 11; obh[11].hclenv = 4; obh[12].hclenv = 12; obh[13].hclenv = 3; obh[14].hclenv = 13; obh[15].hclenv = 2;
    obh[16].hclenv = 14; obh[17].hclenv = 1; obh[18].hclenv = 15;


    for (int i = 0; i < HCLEN + 4; i++) {
        while (bit.bitpos < 4) {
            fr->read((char*)&rev, 1);
            bit.BigendIn(rev & 0xFF, 8);
        }
        if (bit.bitoutvalue != 0)
            length++;
        bit.LittleendOut(3);
        obh[i].hufflen = bit.bitoutvalue;
    }
    //並べ替え
    h.dswap(obh, HCLEN + 4);
    h.dmakehuff(obh);//ハフマン符号化

    for (int i = 0; i < 19; i++) {
        if (obh[i].hufflen != 0)
            hufhuf = daddtree(hufhuf, obh[i].huffHclen, obh[i].hufflen, obh[i].hclenv);
    }
    for (int i = 0; i < 19; i++) {
        if (obh[i].hufflen != 0 && obh[i].hclenv != 0) {
            if (obh[i].hclenv < left)
                left = obh[i].hclenv;
            else if (obh[i].hclenv > right)
                right = obh[i].hclenv;
        }
    }

    count = 2;
    UINT32 tablesize = UINT32(HLIT) + 257 + UINT32(HDIST) + 1;
    while (signSize < (tablesize)) {
        while (bit.bitpos < 8) {
            fr->read((char*)&rev, 1);
            bit.BigendIn(int(rev) & 0xFF, 8);
        }
        if (count > 2) {
            unsigned int f = bit.bitoutvalue << 1;
            bit.BigendOut(1);
            bit.bitoutvalue = f + bit.bitoutvalue;
        }
        else { bit.BigendOut(2); }

        k = dsearch_node(bit.bitoutvalue, hufhuf, count);
        if (k == 65536) {
            count++;
        }
        else if (k == 16) {
            if (bit.bitpos < 3) {
                fr->read((char*)&rev, 1);
                bit.BigendIn(rev, 8);
            }
            bit.LittleendOut(2);
            for (unsigned int j = 0; j < (bit.bitoutvalue + 3); j++) {
                signs[signSize] = stock; signSize++;
            }
            count = 2;
        }
        else if (k == 17) {
            if (bit.bitpos < 3) {
                fr->read((char*)&rev, 1);
                bit.BigendIn(rev, 8);
            }
            bit.LittleendOut(3);
            for (unsigned int j = 0; j < (bit.bitoutvalue + 3); j++) {
                signs[signSize] = 0; signSize++;
            }
            stock = 0; count = 2;
        }
        else if (k == 18) {
            while (bit.bitpos < 7) {
                fr->read((char*)&rev, 1);
                bit.BigendIn(rev, 8);
            }
            bit.LittleendOut(7);
            if ((signSize + bit.bitoutvalue) >= (tablesize)) {
                for (int j = 0; j < (signed int)((tablesize) - signSize); j++) {
                    signs[signSize] = 0; signSize++;
                }
            }
            else {
                for (unsigned int j = 0; j < (bit.bitoutvalue + 11); j++) {
                    signs[signSize] = 0; signSize++;
                }
            }
            stock = 0; count = 2;
        }
        else {
            signs[signSize] = k; signSize++;
            stock = k;
            count = 2;
        }
    }

    UINT32 msize = (UINT32)HLIT + 257;

    hufftable = (UINT32*)malloc(sizeof(UINT32) * msize);
    if (!hufftable)
        return 0;
    hufval = (UINT32*)malloc(sizeof(UINT32) * msize);
    if (!hufval)
        return 0;
    lenh = (UINT32*)malloc(sizeof(UINT32) * msize);
    if (!lenh)
        return 0;

    UINT32 customsize = signSize - msize;
    lentable = (UINT32*)malloc(sizeof(UINT32) * customsize);
    if (!lentable)
        return 0;
    lenval = (UINT32*)malloc(sizeof(UINT32) * customsize);
    if (!lenval)
        return 0;
    lenlen = (UINT32*)malloc(sizeof(UINT32) * customsize);
    if (!lenlen)
        return 0;

    hufftablepos = 0;
    lentablepos = 0;

    //符号テーブルの作成
    if (hufftable && hufval && lenh) {
        while (n <= 16) {
            for (UINT32 j = 0; j < msize; j++) {
                if (signs[j] == n && hufftablepos == 0) {
                    hufftable[hufftablepos] = hu;
                    hufval[hufftablepos] = j;
                    lenh[hufftablepos] = n;
                    pu = n;
                    hufftablepos++;
                }
                else if (signs[j] == n && hufftablepos != 0) {
                    hu = (hu + 1);
                    hu = hu << (n - pu);
                    hufftable[hufftablepos] = hu;
                    hufval[hufftablepos] = j;
                    lenh[hufftablepos] = n;
                    pu = n;
                    hufftablepos++;
                }
            }
            n++;
        }

        for (unsigned int i = 0; i < msize; i++) {
            if (hufftable)
                root = daddtree(root, hufftable[i], lenh[i], hufval[i]);
        }
    }
    else {
        //std::cout << "not keep memory" << std::endl;
        return 0;
    }
    n = 1;
    pu = 1;
    hu = 0;

    //lenテーブルの作成
    if (lentable && lenlen && lenval) {
        while (n <= 16) {
            for (UINT32 j = msize; j < signSize; j++) {
                if (signs[j] == n && lentablepos == 0) {
                    lentable[lentablepos] = hu;
                    lenval[lentablepos] = j - msize;
                    lenlen[lentablepos] = n;
                    pu = n;
                    lentablepos++;
                }
                else if (signs[j] == n && lentablepos != 0) {
                    hu = (hu + 1) << (n - pu);
                    lentable[lentablepos] = hu;
                    lenval[lentablepos] = j - msize;
                    lenlen[lentablepos] = n;
                    pu = n;
                    lentablepos++;
                }
            }
            n++;
        }
        
        for (unsigned int i = 0; i < customsize; i++) {
            lenroot = daddtree(lenroot, lentable[i], lenlen[i], lenval[i]);
        }
    }

    return 1;
}