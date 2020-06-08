#include "encode.h"

encoding::encoding() {
    maxhh = 0;
    tc = 0;//�c���[�̗v�f��
    tcc = 0;//�c���[�̗v�f���@�R�s�[�p
    lc = 0;//�����l�c���[�̗v�f��
    lcc = 0;//�����l�c���[�̗v�f���@�R�s�[�p
    maxlen = 0;//�����̒����̍ő�l
    lmaxlen = 0;//���������̒����̍ő�l
    k = 0;//�g���r�b�g�̊m�F�p
    mc = 0;//�����̒l�̃J�E���g
    l = 0;//�����̒l�̃J�E���g
    lb = 0;//�����̊g���l�̃J�E���g

    encoderv = nullptr;
    rvp = nullptr;//�Q�ƃ��[�g
    fir = nullptr;
    lir = nullptr;

    htc = 18;
    datalen = 0;

    so = nullptr;//�|�C���^ �e�[�u�� so[tc]�@��`�ɕϐ��g���Ȃ�
    sso = nullptr;//�|�C���^ �e�[�u���@�e���l�Q�Ɨp�@sso[tc]�@��`�ɕϐ��g���Ȃ�
    lev = nullptr;//�|�C���^ �e�[�u���i�����jlev[lc]�@��`�ɕϐ��g���Ȃ�
    llev = nullptr;//�|�C���^
    huh = nullptr;//�|�C���^ �����̕����e�[�u���@huh[hcc]
    huhc = nullptr;//�|�C���^ �����̕����e�[�u���@�e���l�Q�Ɨp�@huhc[hccc]

    hh = nullptr;//�����̕����e�[�u��
    hhh = nullptr;
    hhc = nullptr;//�����̕����J�E���g�p�|�C���^
    hcc = 0;//���l�̎�ނ̃J�E���g
    hccc = 0;
}

encoding::~encoding() {
    free(encoderv);
}

//deflate�̐擪3�r�b�g�̏������ݏ���
void encoding::headerwrite(unsigned char HL, unsigned char HD, unsigned char BI, unsigned char BT) {
    //deflate�̃w�b�_�[��񏑂�����
    bits.BigendIn(BI & 0x01, 1);//1bit �ŏI�u���b�N���ǂ���
    bits.BigendIn(BT & 0x02, 2);//2bit �r�b�g���]01�Œ�n�t�}�� 10�J�X�^���n�t�}��
    bits.BigendIn(HL & 0x1F, 5);//���������̐�

    bits.BigendIn(HD & 0x1F, 5);//���������̐�
    bits.BigendIn((htc - 4) & 0x0F, 4);//HCLEN�p�̐�
    for (int i = 0; i < htc; i++) {//��������
        if (bits.bitpos > 7) {
            bits.LittleendOut(8);//���ʂ���o��
            *encoderv = bits.bitoutvalue & 0xFF;
            encoderv++;
            datalen++;
        }
        bits.BigendIn(hht[i], 3);//���ʃr�b�g��擪�ɗ���
    }
}

void encoding::huhtable(struct tnode* a[], UINT8 b) {
    //initialize
    for (int i = 0; i < 19; i++) {
        hht[i] = 0;
    }
    htc = 18;
    //�����̕���������������
    //16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15�̕���
    for (int i = 0; i < 19; i++) {
        for (int j = 0; j < b; j++) {
            if (i == a[j]->num) {//�ԍ�����v����
                switch (a[j]->num) {
                case 16:hht[0] = a[j]->chi; break;//���������i�[
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
    //�v�f����1�v���X�@HCLEN�p�@-4
    htc += 1;
}
//�f�[�^���k�J�n
void encoding::compress(unsigned char* data, UINT64 dataleng) {
    //�u���b�N�������@�𓀎��@hex 1fff�@dec 8191
    //�����p�^�[������

    size_t comsize = size_t(dataleng) / 10;
    comsize = comsize * 7;

    encoderv = (unsigned char*)malloc(comsize);
    rvp = encoderv;
    UINT32 readpos = 0;

    //long long int length=dataleng;//�f�[�^�� unsigned �s��
    //����
    while ((dataleng & 0xFFFFFFFF) > readpos) {
        //������
        l = 0;
        lb = 0;
        k = 0;
        mc = 0;

        readpos = s.slidesearch(data, dataleng & 0xFFFFFFFF, readpos);

        makeSign();

        //�J�X�^���n�t�}��������

        headerwrite(((sso[tcc - 1]->num) - 256) & 0xFF, (llev[lcc - 1]->num) & 0xFF, 0, 2);

        while (hhh != NULL) {//�����̕����e�[�u���̕�����
            for (UINT32 i = 0; i < hccc; i++) {
                if (hhh->num == huhc[i]->num) {
                    bits.LittleendIn(huhc[i]->wei, huhc[i]->chi);//���������ʃr�b�g��擪�ɓ����
                    if (hhh->wei > 1) {//1���傫���ꍇ�͊g������
                        switch (hhh->num) {
                        case 16:bits.BigendIn((hhh->wei) - 3, 2);//2�r�b�g�̊g���i0=3...3=6)
                            break;
                        case 17:bits.BigendIn((hhh->wei) - 3, 3);//3�r�b�g�̊g���i3~10)
                            break;
                        case 18:bits.BigendIn((hhh->wei) - 11, 7);//7�r�b�g�̊g���i11~138)
                            break;
                        default:break;
                        }
                    }
                    while (bits.bitpos > 7) {
                        bits.LittleendOut(8);//��ʃr�b�g��擪�ɒl�𓾂�
                        //���k�l��ǉ�
                        *encoderv = bits.bitoutvalue & 0xFF;
                        encoderv++;
                        datalen++;
                    }
                }
            }
            hhh = hhh->next;
        }

        //�f�[�^������
        for (UINT32 i = 0; i < s.milestocksize; i++) {//s.milestocksize
            for (UINT32 j = 0; j < tcc; j++) {
                if (s.mileRoot[i] == sso[j]->num) {//�e�[�u�������v�����l���Q��
                    if (sso[j]->num > 256) {
                        bits.LittleendIn(sso[j]->wei, sso[j]->chi);//���ʃr�b�g��擪��chi�r�b�g�ǉ�
                        k = s.MileBitnumSearch(sso[j]->num);//�g���r�b�g�����̊m�F

                        if (k < 300 && k > 0) {//�g���r�b�g���������ꍇ
                            bits.BigendIn(s.mile_extensionRoot[mc], k);//�����̊g���l����ʃr�b�g��擪��k�r�b�g�ǉ�
                            mc++;

                            while (bits.bitpos > 7) {
                                bits.LittleendOut(8);//��ʃr�b�g��擪�ɒl�𓾂�
                                //���k�l��ǉ�
                                *encoderv = bits.bitoutvalue & 0xFF;
                                encoderv++;
                                datalen++;
                            }
                        }
                        while (bits.bitpos > 7) {
                            bits.LittleendOut(8);//��ʃr�b�g��擪�ɒl�𓾂�
                            //���k�l��ǉ�
                            *encoderv = bits.bitoutvalue & 0xFF;
                            encoderv++;
                            datalen++;
                        }

                        //���������̌���
                        for (UINT32 t = 0; t < lcc; t++) {
                            if (llev[t]->num == s.distanceRoot[l]) {
                                bits.LittleendIn(llev[t]->wei, llev[t]->chi);//�����e�[�u�������v�����l�̕�����ǉ�
                                k = s.DistanceBitnumSearch(s.distanceRoot[l]);//�����g���r�b�g���𓾂�
                                l++;
                                break;
                            }
                            while (bits.bitpos > 7) {
                                bits.LittleendOut(8);//��ʃr�b�g��擪�ɒl�𓾂�
                                //���k�l��ǉ�
                                *encoderv = bits.bitoutvalue & 0xFF;
                                encoderv++;
                                datalen++;
                            }
                        }
                        if (k < 300 && k > 0) {//�g���r�b�g���������ꍇ
                            //printf("�����g���r�b�g��->%3d,�l->%3d\n", k, s.distance_extenshonRoot[lb]);
                            bits.BigendIn(s.distance_extenshonRoot[lb], k);//�����̊g���̒l����ʃr�b�g��擪��k�r�b�g�ǉ�
                            lb++;

                            while (bits.bitpos > 7) {
                                bits.LittleendOut(8);//��ʃr�b�g��擪�ɒl�𓾂�
                                           //���k�l��ǉ�
                                *encoderv = bits.bitoutvalue & 0xFF;
                                encoderv++;
                                datalen++;
                            }
                        }

                    }
                    else {
                        bits.LittleendIn(sso[j]->wei, sso[j]->chi);//���ʃr�b�g��擪��chi�r�b�g�ǉ�
                        while (bits.bitpos > 7) {
                            bits.LittleendOut(8);//��ʃr�b�g��擪�ɒl�𓾂�
                            //���k�l��ǉ�
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

        //huh��� 
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

        //tab���
        defl.tabfree(hh);
        //defl.tabfree(hhh);
        //hh = nullptr;
        //hhh = nullptr;
        //defl.limithuffstock=nullptr;//���������p�������J��
        free(defl.limithuffstock);
    }
    //�������J��
    s.mileRoot = nullptr;
    s.distanceRoot = nullptr;
    s.mile_extensionRoot = nullptr;
    s.distance_extenshonRoot = nullptr;
    //sso = nullptr;
    //free(so);
    //llev = nullptr;
    //free(lev);

    bits.BigendIn(0, 1);//1bit �ŏI�u���b�N���ǂ���
    bits.BigendIn(0, 2);

    while (bits.bitpos > 0) {//�c��r�b�g����������
        if (bits.bitpos > 7)
            bits.LittleendOut(8);//�c�肪8�r�b�g��葽���ꍇ��8�r�b�g�����
        else {
            bits.LittleendIn(0, (8 - bits.bitpos));//�c��8��菭�Ȃ��ꍇ�͎c��r�b�g0�����
            bits.LittleendOut(8);
        }
        *encoderv = bits.bitoutvalue & 0xFF;
        encoderv++;
        datalen++;
    }
    //�Ō� 0 0 len 0 nlen ffff
    //1 1  custom 256 - 279        7bit        0000000 �`
    //                                      0010111
    bits.LittleendIn(0, 16);
    bits.LittleendIn(0xffff, 16);

    while (bits.bitpos > 7) {//�c��r�b�g����������
        bits.LittleendOut(8);//��ʃr�b�g��擪�ɒl�𓾂�
        *encoderv = bits.bitoutvalue & 0xFF;
        encoderv++;
        datalen++;
    }

    bits.BigendIn(1, 1);//1bit �ŏI�u���b�N���ǂ���
    bits.BigendIn(1, 2);
    bits.BigendIn(0, 7);

    while (bits.bitpos > 0) {//�c��r�b�g����������
        if (bits.bitpos > 7)
            bits.LittleendOut(8);//�c�肪8�r�b�g��葽���ꍇ��8�r�b�g�����
        else {
            bits.LittleendIn(0, (8 - bits.bitpos));//�c��8��菭�Ȃ��ꍇ�͎c��r�b�g0�����
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
    //�o���񐔂��e�[�u���ɃJ�E���g���ē����
    for (UINT32 i = 0; i < s.milestocksize; i++) {
        fir = defl.addtab(fir, s.mileRoot[i], &tc);
    }

    //�����̏o���񐔂��e�[�u���ɃJ�E���g���Ă����
    for (UINT32 i = 0; i < s.distancestocksize; i++) {
        lir = defl.addtab(lir, s.distanceRoot[i], &lc);
    }

    tcc = tc;
    lcc = lc;

    UINT32 tnodesize = sizeof(tnode);
    UINT32 ssize = tnodesize * tc;
    UINT32 lsize = tnodesize * lc;

    so = (tnode**)malloc(ssize);//�|�C���^ �e�[�u�� so[tc]�@��`�ɕϐ��g���Ȃ�
    sso = (tnode**)malloc(ssize);//�|�C���^ �e�[�u���@�e���l�Q�Ɨp�@sso[tc]�@��`�ɕϐ��g���Ȃ�
    lev = (tnode**)malloc(lsize);//�|�C���^ �e�[�u���i�����jlev[lc]�@��`�ɕϐ��g���Ȃ�
    llev = (tnode**)malloc(lsize);//�|�C���^


    //���e�����E�����e�[�u�����c���[�z��ɓ����
    for (UINT32 i = 0; i < tc; i++) {
        so[i] = sso[i] = defl.talloc();
        sso[i] = so[i] = defl.tabcopy(so[i], *fir);
        fir = fir->next;
    }
    //�����l�e�[�u�����c���[�z��ɓ����
    for (UINT32 i = 0; i < lc; i++) {
        lev[i] = llev[i] = defl.talloc();
        llev[i] = lev[i] = defl.tabcopy(lev[i], *lir);
        lir = lir->next;
    }
    //�n�t�}�������̒��������߂�
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

    //�n�t�}�������̒��������߂�i�����j
    while (lc > 1) {
        defl.quicksort(lev, 0, lc - 1);
        lev[lc - 2] = defl.treemake(lev[lc - 1], lev[lc - 2]);
        lev[lc - 1] = NULL;
        lc--;
    }
    defl.treeprint(so[0], &maxlen);//�ő啄�����𓾂�
    defl.treeprint(lev[0], &lmaxlen);//�ő啄�����𓾂�

    defl.shellsort(sso, tcc);//���l�Ńc���[���l�������ɕ��ׂ�
    defl.shellsort(llev, lcc);//���l�Ńc���[���l�������ɕ��ׂ�i�����j
    defl.makehuff(sso, maxlen + 1, tcc);//�n�t�}������������U��@(sso, maxlen + 1, tcc)
    defl.makehuff(llev, lmaxlen + 1, lcc);//�n�t�}������������U��

    //�����̕������̕��������X�g�����
    hh = defl.huhu(sso, llev, tcc, lcc, sso[tcc - 1]->num, llev[lcc - 1]->num);
    hh = defl.runlen(hh);//���������O�X�����ϊ�

    hhh = hh;//�n�t�}�������̕����z���hhh���Q��

    //�����̕������l�̃J�E���g�@hh��NULL�ɂȂ�
    while (hh != NULL) {
        hhc = defl.addtab(hhc, hh->num, &hcc);
        hh = hh->next;
    }
    hccc = hcc;

    UINT32 hsize = tnodesize * hcc;
    huh = (tnode**)malloc(hsize);//�|�C���^ �����̕����e�[�u���@huh[hcc]
    huhc = (tnode**)malloc(hsize);//�|�C���^ �����̕����e�[�u���@�e���l�Q�Ɨp�@huhc[hccc]


    if (huh && huhc) {
        for (UINT32 i = 0; i < hcc; i++) {//�e�[�u�����c���[�z��ɓ����
            huh[i] = huhc[i] = defl.talloc();
            huhc[i] = huh[i] = defl.tabcopy(huh[i], *hhc);
            hhc = hhc->next;
        }

        UINT32 limstocksize = tnodesize * 4;
        defl.limithuffstock = (tnode**)malloc(limstocksize);

        while (hcc > 1) {//�����̕����̒��������߂�
            defl.quicksort(huh, 0, hcc - 1);
            huh[hcc - 2] = defl.limitedtreemake(huh[hcc - 1], huh[hcc - 2], huff_of_huff_limit);
            huh[hcc - 1] = NULL;
            hcc--;
        }

        //������������
        if (defl.limitnum > 1) {
            while (defl.limitnum > 1) {//2�̏ꍇ�\�[�g�G���[�ɂȂ�
                //std::cout << "�c���[�X�g�b�N�P�ȏ�" << std::endl;
                defl.quicksort(defl.limithuffstock, 0, defl.limitnum - 1);
                defl.limithuffstock[defl.limitnum - 2] = defl.treemake(defl.limithuffstock[defl.limitnum - 1], defl.limithuffstock[defl.limitnum - 2]);
                defl.limitnum--;
            }
            huh[0] = defl.treemake(defl.limithuffstock[0], huh[0]);
        }
        else if (defl.limitnum == 1) {
            huh[0] = defl.treemake(defl.limithuffstock[0], huh[0]);
        }
        defl.limithuffstock = nullptr;//���������p�������J��
        defl.treeprint(huh[0], &maxhh);//�ő啄�����𓾂�
        defl.shellsort(huhc, hccc);//���l�Ńc���[���l�������ɕ��ׂ�
        defl.makehuff(huhc, maxhh + 1, hccc);//�n�t�}������������U��
        huhtable(huhc, hccc);//�����̕��������X�g����������
    }
}

void encoding::write(FILE* f) {
    for (UINT32 i = 0; i < datalen; i++) {
        fwrite(&rvp[i], sizeof(char), 1, f);
    }
}