#include "headerRead.h"
/*
0 0�ʏ�i-en�j���k�I�v�V�������g�p����܂����B
          0 1�ő�i-exx / -ex�j���k�I�v�V�������g�p����܂����B
          1 0�����i-ef�j���k�I�v�V�������g�p����܂����B
          1 1�������i-es�j���k�I�v�V�������g�p����܂����B
�r�b�g3�F���̃r�b�g���ݒ肳��Ă���ꍇ�A�t�B�[���hcrc-32�A���k
               �T�C�Y�Ɣ񈳏k�T�C�Y�́A
               ���[�J���w�b�_�[�B�������l��
               ���k���ꂽ����̃f�[�^�L�q�q
               �f�[�^�B�i���FPKZIP�o�[�W����2.04g�iDOS�̂݁j
               �V�������\�b�h8���k�ł��̃r�b�g��F�����܂�
               PKZIP�̃o�[�W�����͂��̃r�b�g��F�����܂�
               ���k���@�B�j
*/
//�R���X�g���N�^
HeaderWrite::HeaderWrite() {
    zero = 0;
}

//������̒����𒲂ׂ�
int searchleng(char n[]) {
    int i = 0;
    while (n[i] != '\0') {
        i++;
    }
    return i;
}

//�f�[�^�ύX�t�@�C���l�[���̒ǉ�
changefilenames* HeaderWrite::addfilename(changefilenames* infn, char* fn)
{
    if (!infn) {
        infn = (changefilenames*)malloc(sizeof(changefilenames));
        if (infn) {
            infn->cfn = fn;
            infn->next = nullptr;
        }
    }
    else {
        infn->next = addfilename(infn->next, fn);
    }

    return infn;
}

int HeaderWrite::localwrite(FILE* fn, UINT32 t, UINT32 crc32, UINT32 asize, UINT32 bsize, UINT16 nameleng, char n[], UINT16 ver, UINT16 bitf, UINT16 method) {

    int size = 0;

    // �o�C�i���Ƃ��ď�������
    //f.write(reinterpret_cast<char*>(&a), sizeof(a));
    for (int i = 0; i < 4; i++) {
        unsigned char c = (LOCAL_HEADER >> (24 - (8 * i))) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);//�V�O�l�`���̏�������
        size++;
    }
    //�W�J�ɕK�v�ȃo�[�W���� (�ŏ��o�[�W����)
    for (int i = 0; i < 2; i++) {
        unsigned char c = (ver >> (8 * i)) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);
        size++;
    }
    //�ėp�ړI�̃r�b�g�t���O
    for (int i = 0; i < 2; i++) {
        unsigned char c = (bitf >> (8 * i)) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);//�r�b�g�t���b�O
        size++;
    }
    //���k���\�b�h
    for (int i = 0; i < 2; i++) {
        unsigned char c = (method >> (8 * i)) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);//���k�����̏�������
        size++;
    }
    //�t�@�C���̍ŏI�ύX����
    UINT16 time = t & 0xFF;
    UINT16 day = (t >> 16) & 0xFF;
    for (int i = 0; i < 2; i++) {
        unsigned char c = (time >> (8 * i)) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);//���ԂƓ��t�̏�������
        size++;
    }
    for (int i = 0; i < 2; i++) {
        unsigned char c = (day >> (8 * i)) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);//���ԂƓ��t�̏�������
        size++;
    }
    //crc 4byte �������݁i�𓀌�̒l��crc�j
    for (int i = 0; i < 4; i++) {
        unsigned char c = (crc32 >> (8 * i)) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);
        size++;
    }
    //���k�T�C�Y ��������
    for (int i = 0; i < 4; i++) {
        unsigned char c = (asize >> (8 * i)) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);
        size++;
    }
    //���k�O�T�C�Y ��������
    for (int i = 0; i < 4; i++) {
        unsigned char c = (bsize >> (8 * i)) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);
        size++;
    }
    //�t�@�C�����̒��� ��������
    for (int i = 0; i < 2; i++) {
        unsigned char c = (nameleng >> (8 * i)) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);
        size++;
    }

    //�g���t�B�[���h�̒��� 0��������
    for (int i = 0; i < 2; i++) {
        fwrite(&zero, sizeof(char), 1, fn);
        size++;
    }
    //�t�@�C�����̏�������
    for (int i = 0; i < nameleng; i++) {
        fwrite(&n[i], sizeof(char), 1, fn);
        size++;
    }

    //�g���t�B�[���h�������݂͏ȗ��i0)

    return size;
}

void HeaderWrite::centralwrite(FILE* fn, CenterDerect cd) {

    for (int i = 0; i < 4; i++) {
        unsigned char c = (CENTRALSIGNATURE >> (24 - (8 * i))) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);//�V�O�l�`���̏�������
        cdsize++;
    }

    for (int i = 0; i < 2; i++) {
        unsigned char c = (cd.version >> (8 * i)) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);
        cdsize++;
    }
    //���o�ɕK�v�ȃo�[�W�����i�ŏ��j//�ǂݎ��ł�2d
    for (int i = 0; i < 2; i++) {
        unsigned char c = (cd.minversion >> (8 * i)) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);//2�o�C�g
        cdsize++;
    }
    //std::cout << "���o�ɕK�v�ȃo�[�W����
    for (int i = 0; i < 2; i++) {
        unsigned char c = (cd.bitflag >> (8 * i)) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);
        cdsize++;
    }
    //    ���k���@
    for (int i = 0; i < 2; i++) {
        unsigned char c = (cd.method >> (8 * i)) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);//���k�����̏�������2�o�C�g�@���[�J���w�b�_�Ɠ���
        cdsize++;
    }
    //    �t�@�C���̍ŏI�ύX����
    for (int i = 0; i < 2; i++) {
        unsigned char c = (cd.time >> (8 * i)) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);//����2�o�C�g�̏�������
        cdsize++;
    }
    //    �t�@�C���̍ŏI�ύX��
    for (int i = 0; i < 2; i++) {
        unsigned char c = (cd.day >> (8 * i)) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);//���t2�o�C�g�̏�������
        cdsize++;
    }
    //crc
    for (int i = 0; i < 4; i++) {
        unsigned char c = (cd.crc >> (8 * i)) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);//crc 4byte �������݁i���[�J���t�@�C���̒l��crc�H�j
        cdsize++;
    }
    //    ���k�T�C�Y
    for (int i = 0; i < 4; i++) {
        unsigned char c = (cd.size >> (8 * i)) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);//���k�T�C�Y ��������
        cdsize++;
    }

    //    �񈳏k�T�C�Y
    for (int i = 0; i < 4; i++) {
        unsigned char c = (cd.nonsize >> (8 * i)) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);;//���k�O�T�C�Y ��������
        cdsize++;
    }

    //    �t�@�C�����̒����in)
    for (int i = 0; i < 2; i++) {
        unsigned char c = (cd.filenameleng >> (8 * i)) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);//�t�@�C�����̒���2�o�C�g ��������
        cdsize++;
    }
    // �ǉ��t�B�[���h���im�j
    for (int i = 0; i < 2; i++) {
        unsigned char c = (cd.fieldleng >> (8 * i)) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);
        cdsize++;
    }
    //�t�@�C���̃R�����g���ik�j
    for (int i = 0; i < 2; i++) {
        unsigned char c = (cd.fielcomment >> (8 * i)) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);
        cdsize++;
    }

    //    �t�@�C�����n�܂�f�B�X�N�ԍ�
    for (int i = 0; i < 2; i++) {
        unsigned char c = (cd.discnum >> (8 * i)) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);
        cdsize++;
    }
    //�����t�@�C�������i�R�s�[�j2�o�C�g ��������
    for (int i = 0; i < 2; i++) {
        unsigned char c = (cd.zokusei >> (8 * i)) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);
        cdsize++;
    }
    //�O���t�@�C�������i�R�s�[�j4�o�C�g ��������
    for (int i = 0; i < 4; i++) {
        unsigned char c = (cd.gaibuzokusei >> (8 * i)) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);
        cdsize++;
    }
    //���[�J���w�b�_���΃I�t�Z�b�g4�o�C�g ��������
    for (int i = 0; i < 4; i++) {
        unsigned char c = (cd.localheader >> (8 * i)) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);
        cdsize++;
    }

    //�t�@�C�����̏�������
    for (int i = 0; i < cd.filenameleng; i++) {
        fwrite(&cd.filename[i], sizeof(char), 1, fn);
        cdsize++;
    }
    //�g���t�B�[���h
    if (cd.fieldleng > 0) {
        for (int i = 0; i < cd.fieldleng; i++) {
            fwrite(&cd.kakutyo[i], sizeof(char), 1, fn);
            cdsize++;
        }
    }
    //�t�@�C���R�����g�@�Ȃ�
    if (cd.fielcomment > 0) {
        for (int i = 0; i < cd.fielcomment; i++) {
            fwrite(&cd.comment[i], sizeof(char), 1, fn);
            cdsize++;
        }
    }
    recordsum++;
}

void HeaderWrite::eocdwrite(FILE* fn, UINT16 discnum, UINT16 cdnum, UINT16 disccdsum, UINT16 cdsum, UINT64 pos, UINT32 size) {
    //�V�O�l�`���̏�������
    for (int i = 0; i < 4; i++) {
        unsigned char c = (END_OF_CENTRAL >> (8 * i)) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);
        endsize++;
    }
    //���̃f�B�X�N�̐��i�R�s�[�j ��������
    for (int i = 0; i < 2; i++) {
        unsigned char c = (discnum >> (8 * i)) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);
        endsize++;
    }
    //�Z���g�����f�B���N�g�����J�n����f�B�X�N ��������
    for (int i = 0; i < 2; i++) {
        unsigned char c = (cdnum >> (8 * i)) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);
        endsize++;
    }
    //���̃f�B�X�N��̃Z���g�����f�B���N�g�����R�[�h�̐�  ��������
    for (int i = 0; i < 2; i++) {
        unsigned char c = (disccdsum >> (8 * i)) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);
        endsize++;
    }
    //�Z���g�����f�B���N�g�����R�[�h�̍��v�� ��������
    for (int i = 0; i < 2; i++) {
        unsigned char c = (cdsum >> (8 * i)) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);
        endsize++;
    }
    //�Z���g�����f�B���N�g���̃T�C�Y (�o�C�g)  ��������
    for (int i = 0; i < 4; i++) {
        unsigned char c = (size >> (8 * i)) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);
        endsize++;
    }
    //�Z���g�����f�B���N�g���̈ʒu
    for (int i = 0; i < 4; i++) {
        unsigned char c = (pos >> (8 * i)) & 0xFF;
        fwrite(&c, sizeof(char), 1, fn);
        endsize++;
    }

    //ZIP�t�@�C���̃R�����g�̒��� (n)  �������� 0��������
    fwrite(&zero, sizeof(char), 1, fn);
    fwrite(&zero, sizeof(char), 1, fn);
    endsize += 2;

    //ZIP�t�@�C���̃R�����g�@����Ȃ�
}