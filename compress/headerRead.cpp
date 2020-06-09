#include "headerRead.h"

HeaderRead::HeaderRead(char* name) {
    readfile = name; //�ǂݍ��ރt�@�C���̎w��
    readdata = 0;
    sig = 0;
    endpos = 0;
    curpos = 0;
    headerReaddata = 0;
    lhtest = readLHdata;
    rootLH = nullptr;
    r = nullptr;//�Z���g�����f�B���N�g���̃��[�g
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

//�t�@�C�����ŃZ���g�����f�B���N�g���@�f�[�^����
//���[�J���f�B���N�g���̈ʒu��Ԃ�
CenterDerect* HeaderRead::searchCENTRAL(char* s) {
    bool t = false;
    CDdataes* p = saveCD;

    while (p != nullptr) {
        t = searchChara(s, p->cdd.filename, p->cdd.filenameleng);//�t�@�C����������v����
        if (t) {
            return &p->cdd;
        }
        else {
        }
        p = p->nextCD;
    }
    return nullptr;
}

//�t�@�C��������
bool HeaderRead::searchChara(char fn[], char* cdfn, UINT16 cdfnlen)
{
    int i = 0;

    //������̐�
    while (fn[i] != '\0') {
        i++;
    }
    UINT32 isize = (UINT32)i + 1;
    char* searchfilename = (char*)malloc(isize);//�����X���C�h�p

    int slidepos = 0;

    if (searchfilename) {
        while (slidepos < cdfnlen) {
            if (i > cdfnlen) {//�t�@�C�������������蒷��
                return false;
            }
            else {
                if (slidepos == 0) {//
                    for (int j = 0; j < i; j++) {//�ŏ��ɕ������filename��i�������
                        searchfilename[j] = cdfn[slidepos];
                        slidepos++;
                    }
                    searchfilename[i] = '\0';//������I�[��ǉ�
                }
                else {
                    //�z����ꕶ���Â��炷
                    for (int j = 0; j < i - 1; j++) {
                        searchfilename[j] = searchfilename[j + 1];
                    }
                    searchfilename[i - 1] = cdfn[slidepos];//�Ō�ɕt��������
                    searchfilename[i] = '\0';//������I�[��ǉ�
                    slidepos++;//�ʒu�ړ�
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
        std::cout << "�t�@�C�� ID.txt ���J���܂���";
    }

    fin.seekg(0, std::ios_base::end);//�t�@�C���I�[�ʒu�擾
    UINT64 endpos = 0;
    endpos = fin.tellg();

    curpos = endpos;
    curpos -= 1;
    fin.seekg(curpos, std::ios_base::beg);

    /*�I�[�R�[�h�����@�t�@�C���I��肩��*/
    while (curpos > 0) {

        fin.read((char*)&readdata, sizeof(char));
        sig = sig << 8;
        sig += readdata;

        if (sig == END_OF_CENTRAL) {
            curpos += 4;//�V�O�l�`�����|�C���^�߂�
            fin.seekg(curpos);

            //���̃f�B�X�N�̐� 2byte
            ER->discnum = 0;
            for (int i = 0; i < 2; i++) {
                fin.read((char*)&readdata, sizeof(char));
                readdata = ((readdata & 0xFF) << (8 * i));
                ER->discnum += readdata;
            }

            //�Z���g�����f�B���N�g�����J�n����f�B�X�N 2byte
            ER->disccentral = 0;
            for (int i = 0; i < 2; i++) {
                fin.read((char*)&readdata, sizeof(char));
                readdata = ((readdata & 0xFF) << (8 * i));
                ER->disccentral += readdata;
            }

            //���̃f�B�X�N��̃Z���g�����f�B���N�g�����R�[�h�̐� 2byte
            ER->centralnum = 0;
            for (int i = 0; i < 2; i++) {
                fin.read((char*)&readdata, sizeof(char));
                readdata = ((readdata & 0xFF) << (8 * i));
                ER->centralnum += readdata;
            }

            //�Z���g�����f�B���N�g�����R�[�h�̍��v�� 2byte
            ER->centralsum = 0;
            for (int i = 0; i < 2; i++) {
                readdata = 0;
                fin.read((char*)&readdata, sizeof(char));
                readdata = ((readdata & 0xFF) << (8 * i));
                ER->centralsum += readdata;
            }

            //�Z���g�����f�B���N�g���̃T�C�Y (�o�C�g) 4byte
            ER->size = 0;
            for (int i = 0; i < 4; i++) {
                fin.read((char*)&readdata, sizeof(char));
                readdata = (readdata & 0xFF) << (8 * i);
                ER->size += readdata;
            }

            //�Z���g�����f�B���N�g���̊J�n�ʒu�̃I�t�Z�b�g 4byte
            ER->position = 0;
            for (int i = 0; i < 4; i++) {
                readdata = 0;
                fin.read((char*)&readdata, sizeof(char));
                readdata = (readdata & 0xFF) << (8 * i);
                ER->position += readdata;
            }
            readpos = ER->position;

            //�Z���g�����f�B���N�g�����R�[�h�̍��v�� 2byte
            ER->commentleng = 0;
            for (int i = 0; i < 2; i++) {
                fin.read((char*)&readdata, sizeof(char));
                readdata = ((readdata & 0xFF) << (8 * i));
                ER->commentleng += readdata;
            }

            //ZIP�t�@�C���̃R�����g nbyte
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
//���[�J���f�B���N�g���̃w�b�_�[���
void HeaderRead::localread(UINT64 pos) {
    std::ifstream fin(readfile, std::ios::in | std::ios::binary);
    if (!fin) {
    }

    fin.seekg(pos, std::ios_base::beg);
    sig = 0;


    /*���[�J���w�b�_�V�O�l�`����ǂݍ���*/
    while (sig != LOCAL_HEADER) {
        //�擪�R�[�h�܂œǂݍ���
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

    //�ėp�ړI�̃r�b�g�t���O
    LH->bitflag = 0;
    for (int i = 0; i < 2; i++) {
        fin.read((char*)&readdata, sizeof(char));
        readdata = ((readdata & 0xFF) << (8 * i));
        LH->bitflag += readdata;
    }

    //���k���\�b�h
    LH->method = 0;
    for (int i = 0; i < 2; i++) {
        fin.read((char*)&readdata, sizeof(char));
        readdata = ((readdata & 0xFF) << (8 * i));
        LH->method += readdata;
    }

    //�t�@�C���̍ŏI�ύX����
    LH->time = 0;
    for (int i = 0; i < 2; i++) {
        fin.read((char*)&readdata, sizeof(char));
        readdata = ((readdata & 0xFF) << (8 * i));
        LH->time += readdata;
    }

    //�t�@�C���̍ŏI�ύX���t
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

    //���k�T�C�Y
    LH->size = 0;
    for (int i = 0; i < 4; i++) {
        fin.read((char*)&readdata, sizeof(char));
        readdata = ((readdata & 0xFF) << (8 * i));
        LH->size += readdata;
    }

    //    �񈳏k�T�C�Y
    LH->nonsize = 0;
    for (int i = 0; i < 4; i++) {
        fin.read((char*)&readdata, sizeof(char));
        readdata = ((readdata & 0xFF) << (8 * i));
        LH->nonsize += readdata;
    }

    //�t�@�C�����̒��� (n)
    LH->filenameleng = 0;
    for (int i = 0; i < 2; i++) {
        fin.read((char*)&readdata, sizeof(char));
        readdata = ((readdata & 0xFF) << (8 * i));
        LH->filenameleng += readdata;
    }

    //�g���t�B�[���h�̒��� (m)
    LH->fieldleng = 0;
    for (int i = 0; i < 2; i++) {
        fin.read((char*)&readdata, sizeof(char));
        readdata = ((readdata & 0xFF) << (8 * i));
        LH->fieldleng += readdata;
    }

    //�t�@�C����
    UINT32 msize = ((UINT32)LH->filenameleng) + 1;
    LH->filename = (char*)malloc(msize);
    if (LH->filename) {
        for (UINT16 i = 0; i < LH->filenameleng; i++) {
            fin.read((char*)&readdata, sizeof(char));
            LH->filename[i] = readdata;
        }
        LH->filename[LH->filenameleng] = '\0';
    }

    //�g���t�B�[���h
    msize = ((UINT32)LH->fieldleng) + 1;
    LH->kakutyo = (char*)malloc(msize);
    if (LH->kakutyo) {
        for (UINT16 i = 0; i < LH->fieldleng; i++) {
            fin.read((char*)&readdata, sizeof(char));
            LH->kakutyo[i] = readdata;
        }
    }

    LH->pos = fin.tellg();//���k�f�[�^�̎n�܂�
    fin.close();
}
//�Z���g�����f�B���N�g���̃w�b�_�[���
void HeaderRead::centerread(UINT64 pos, UINT32 size, UINT16 n) {

    std::ifstream fin(readfile, std::ios::in | std::ios::binary);

    if (!fin) {
        std::cout << "not file open" << std::endl;
    }

    int j = 0;

    fin.seekg(pos, std::ios_base::beg);

    while (j < n) {

        //while (s < size) {
            /*�擪�R�[�h�܂œǂݍ���*/
        for (int i = 0; i < 4; i++) {
            fin.read((char*)&readdata, sizeof(char));
            sig = sig << 8;
            sig += readdata & 0xFF;
        }

        if (sig == CENTRALSIGNATURE) { //�����f�B���N�g���t�@�C���w�b�_�[�̏���= 0x02014b50
            //std::cout << "�����f�B���N�g���o�́@j:" << j << std::endl;
            //CD->version = 0;
            //mac 62�Â�����H�@windows 45�Â�����H
            //�����@GetFileAttributes?
            CD->version = 0;
            for (int i = 0; i < 2; i++) {
                fin.read((char*)&readdata, sizeof(char));//�쐬��
                readdata = ((readdata & 0xFF) << (8 * i));
                CD->version += readdata;
            }

            //���o�ɕK�v�ȃo�[�W�����i�ŏ��j
            CD->minversion = 0;
            for (int i = 0; i < 2; i++) {
                fin.read((char*)&readdata, sizeof(char));
                readdata = ((readdata & 0xFF) << (8 * i));
                CD->minversion += readdata;
            }

            CD->bitflag = 0;
            for (int i = 0; i < 2; i++) {
                fin.read((char*)&readdata, sizeof(char));//�ėp�r�b�g�t���O
                readdata = ((readdata & 0xFF) << (8 * i));
                CD->bitflag += readdata;
            }

            CD->method = 0;
            for (int i = 0; i < 2; i++) {//    ���k���@
                fin.read((char*)&readdata, sizeof(char));
                readdata = ((readdata & 0xFF) << (8 * i));
                CD->method += readdata;
            }

            CD->time = 0;
            for (int i = 0; i < 2; i++) {//    �t�@�C���̍ŏI�ύX����
                fin.read((char*)&readdata, sizeof(char));
                readdata = ((readdata & 0xFF) << (8 * i));
                CD->time += readdata;
            }

            CD->day = 0;
            for (int i = 0; i < 2; i++) {//    �t�@�C���̍ŏI�ύX��
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

            //    ���k�T�C�Y
            CD->size = 0;
            for (int i = 0; i < 4; i++) {
                fin.read((char*)&readdata, sizeof(char));
                readdata = ((readdata & 0xFF) << (8 * i));
                CD->size += readdata;
            }

            //    �񈳏k�T�C�Y
            CD->nonsize = 0;
            for (int i = 0; i < 4; i++) {
                fin.read((char*)&readdata, sizeof(char));
                readdata = ((readdata & 0xFF) << (8 * i));
                CD->nonsize += readdata;
            }

            //    �t�@�C�����̒����in)
            CD->filenameleng = 0;
            for (int i = 0; i < 2; i++) {
                fin.read((char*)&readdata, sizeof(char));
                readdata = ((readdata & 0xFF) << (8 * i));
                CD->filenameleng += readdata;
            }

            CD->fieldleng = 0;
            for (int i = 0; i < 2; i++) {//    �ǉ��t�B�[���h���im�j
                fin.read((char*)&readdata, sizeof(char));
                readdata = ((readdata & 0xFF) << (8 * i));
                CD->fieldleng += readdata;
            }

            CD->fielcomment = 0;
            for (int i = 0; i < 2; i++) {//    �t�@�C���̃R�����g���ik�j
                fin.read((char*)&readdata, sizeof(char));
                readdata = ((readdata & 0xFF) << (8 * i));
                CD->fielcomment += readdata;
            }

            CD->discnum = 0;
            for (int i = 0; i < 2; i++) {//    �t�@�C�����n�܂�f�B�X�N�ԍ�
                fin.read((char*)&readdata, sizeof(char));
                readdata = (readdata << (8 * i));
                CD->discnum += readdata;
            }

            CD->zokusei = 0;
            for (int i = 0; i < 2; i++) {//        �����t�@�C������
                fin.read((char*)&readdata, sizeof(char));
                readdata = ((readdata & 0xFF) << (8 * i));
                CD->zokusei += readdata;
            }

            CD->gaibuzokusei = 0;
            for (int i = 0; i < 4; i++) {//    �O���t�@�C������
                fin.read((char*)&readdata, sizeof(char));
                readdata = ((readdata & 0xFF) << (8 * i));
                CD->gaibuzokusei += readdata;
            }

            CD->localheader = 0;
            for (int i = 0; i < 4; i++) {//    ���[�J���t�@�C���w�b�_�̑��΃I�t�Z�b�g
                fin.read((char*)&readdata, sizeof(char));
                readdata = ((readdata & 0xFF) << (8 * i));
                CD->localheader += readdata;
            }

            UINT32 msize = ((UINT32)CD->filenameleng) + 1;
            CD->filename = (char*)malloc(msize);
            for (int i = 0; i < CD->filenameleng; i++) {//    �t�@�C����
                fin.read((char*)&headerReaddata, sizeof(char));
                CD->filename[i] = headerReaddata;
            }
            CD->filename[CD->filenameleng] = '\0';

            msize = ((UINT32)CD->fieldleng) + 1;
            if (CD->fieldleng > 0) {
                CD->kakutyo = (char*)malloc(msize);
                for (int i = 0; i < CD->fieldleng; i++) {//    �g���t�B�[���h
                    fin.read((char*)&readdata, sizeof(char));
                    CD->kakutyo[i] = readdata & 0xFF;
                }
            }

            msize = ((UINT32)CD->fielcomment) + 1;
            if (CD->fielcomment > 0) {
                CD->comment = (char*)malloc(msize);
                for (int i = 0; i < CD->fielcomment; i++) {//�t�@�C���R�����g
                    fin.read((char*)&readdata, sizeof(char));
                    CD->comment[i] = readdata & 0xFF;
                }
            }

            sig = 0;
            //�f�[�^��ێ�����
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

    /*�擪�R�[�h�܂œǂݍ���*/
    for (int i = 0; i < 4; i++) {
        fin.read((char*)&readdata, sizeof(char));
        sig = sig << 8;
        sig += readdata & 0xFF;
    }

    if (sig == CENTRALSIGNATURE) {
        scd->version = 0;
        for (int i = 0; i < 2; i++) {
            fin.read((char*)&readdata, sizeof(char));//�쐬��
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
            fin.read((char*)&readdata, sizeof(char));//�ėp�r�b�g�t���O
            readdata = ((readdata & 0xFF) << (8 * i));
            scd->bitflag += readdata;
        }

        scd->method = 0;
        for (int i = 0; i < 2; i++) {//    ���k���@
            fin.read((char*)&readdata, sizeof(char));
            readdata = ((readdata & 0xFF) << (8 * i));
            scd->method += readdata;
        }

        scd->time = 0;
        for (int i = 0; i < 2; i++) {//    �t�@�C���̍ŏI�ύX����
            fin.read((char*)&readdata, sizeof(char));
            readdata = ((readdata & 0xFF) << (8 * i));
            scd->time += readdata;
        }

        scd->day = 0;
        for (int i = 0; i < 2; i++) {//    �t�@�C���̍ŏI�ύX��
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

        //    ���k�T�C�Y
        scd->size = 0;
        for (int i = 0; i < 4; i++) {
            fin.read((char*)&readdata, sizeof(char));
            readdata = ((readdata & 0xFF) << (8 * i));
            scd->size += readdata;
        }

        //    �񈳏k�T�C�Y
        scd->nonsize = 0;
        for (int i = 0; i < 4; i++) {
            fin.read((char*)&readdata, sizeof(char));
            readdata = ((readdata & 0xFF) << (8 * i));
            scd->nonsize += readdata;
        }

        //    �t�@�C�����̒����in)
        scd->filenameleng = 0;
        for (int i = 0; i < 2; i++) {
            fin.read((char*)&readdata, sizeof(char));
            readdata = ((readdata & 0xFF) << (8 * i));
            scd->filenameleng += readdata;
        }

        scd->fieldleng = 0;
        for (int i = 0; i < 2; i++) {//    �ǉ��t�B�[���h���im�j
            fin.read((char*)&readdata, sizeof(char));
            readdata = ((readdata & 0xFF) << (8 * i));
            scd->fieldleng += readdata;
        }

        scd->fielcomment = 0;
        for (int i = 0; i < 2; i++) {//�t�@�C���̃R�����g���ik�j
            fin.read((char*)&readdata, sizeof(char));
            readdata = ((readdata & 0xFF) << (8 * i));
            scd->fielcomment += readdata;
        }

        scd->discnum = 0;
        for (int i = 0; i < 2; i++) {//�t�@�C�����n�܂�f�B�X�N�ԍ�
            fin.read((char*)&readdata, sizeof(char));
            readdata = (readdata << (8 * i));
            scd->discnum += readdata;
        }

        scd->zokusei = 0;
        for (int i = 0; i < 2; i++) {//�����t�@�C������
            fin.read((char*)&readdata, sizeof(char));
            readdata = ((readdata & 0xFF) << (8 * i));
            scd->zokusei += readdata;
        }

        scd->gaibuzokusei = 0;
        for (int i = 0; i < 4; i++) {//    �O���t�@�C������
            fin.read((char*)&readdata, sizeof(char));
            readdata = ((readdata & 0xFF) << (8 * i));
            scd->gaibuzokusei += readdata;
        }

        scd->localheader = 0;
        for (int i = 0; i < 4; i++) {//    ���[�J���t�@�C���w�b�_�̑��΃I�t�Z�b�g
            fin.read((char*)&readdata, sizeof(char));
            readdata = ((readdata & 0xFF) << (8 * i));
            scd->localheader += readdata;
        }

        UINT32 msize = ((UINT32)scd->filenameleng) + 1;
        scd->filename = (char*)malloc(msize);
        for (int i = 0; i < scd->filenameleng; i++) {//    �t�@�C����
            fin.read((char*)&headerReaddata, sizeof(char));
            scd->filename[i] = headerReaddata;
        }
        scd->filename[scd->filenameleng] = '\0';
        flag = searchChara(fn, scd->filename, scd->filenameleng);

        if (scd->fieldleng > 0) {
            msize = ((UINT32)scd->fieldleng) + 1;
            scd->kakutyo = (char*)malloc(msize);
            for (int i = 0; i < scd->fieldleng; i++) {//    �g���t�B�[���h
                fin.read((char*)&readdata, sizeof(char));
                scd->kakutyo[i] = readdata & 0xFF;
            }
        }

        if (scd->fielcomment) {
            msize = ((UINT32)scd->fielcomment) + 1;
            scd->comment = (char*)malloc(msize);
            for (int i = 0; i < scd->fielcomment; i++) {//�t�@�C���R�����g
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