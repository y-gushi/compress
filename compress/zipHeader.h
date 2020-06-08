#pragma once

#include "typechange.h"
#include <stdlib.h>
#include <iostream>
#include <fstream>

#define LOCAL_HEADER 0x504b0304
#define CENTRALSIGNATURE 0x0504b0102
#define END_OF_CENTRAL 0x06054b50

//�w�b�_�[�p�����[�^�[
#define MAKEVERSION 0x2d //64bit�W�� �쐬�o�[�W����
#define VERSION_LH 0x14 //���[�J���w�b�_�@�W�J�ɕK�v�ȃo�[�W����2byte�i���g���G���f�B�A���j
#define BITFLAG_LH 0x06 //���[�J���w�b�_�@�r�b�g�t���b�O2byte�i���g���G���f�B�A���j 0x14
#define DEFLATE_LH_CD 0x08//���k���\�b�h�@�f�t���\�g�@2byte�i���g���G���f�B�A���j
#define NAIBU 0x0
#define GAIBU 0x0

struct CenterDerect {
    UINT64 pos;//�ǂݎ�肨���ʒu
    UINT16 version = 0;    //�쐬���ꂽ�o�[�W���� 2byte
    UINT16 minversion = 0;//�W�J�ɕK�v�ȃo�[�W���� (�ŏ��o�[�W����) 2byte
    UINT16 bitflag = 0;//�ėp�ړI�̃r�b�g�t���O 2byte
    UINT16 method = 0;//���k���\�b�h 2byte
    UINT16 time = 0;//�t�@�C���̍ŏI�ύX���� 2byte
    UINT16 day = 0;//�t�@�C���̍ŏI�ύX���t 2byte
    UINT32 crc = 0;//CRC-32 4byte
    UINT32 size = 0;//���k�T�C�Y 4byte
    UINT32 nonsize = 0;//�񈳏k�T�C�Y 4byte
    UINT16 filenameleng = 0;//�t�@�C�����̒��� (n) 2byte
    UINT16 fieldleng = 0;//�g���t�B�[���h�̒��� (m) 2byte
    UINT16 fielcomment = 0;//�t�@�C���R�����g�̒��� (k) 2byte
    UINT16 discnum = 0;//�t�@�C�����J�n����f�B�X�N�ԍ� 2byte
    UINT16 zokusei = 0;//�����t�@�C������ 2byte
    UINT32 gaibuzokusei = 0;//�O���t�@�C������ 4byte
    UINT32 localheader = 0;//���[�J���t�@�C���w�b�_�̑��΃I�t�Z�b�g 4byte
 
    char* filename = nullptr;//�t�@�C���� nbyte
    char* kakutyo = nullptr;//�g���t�B�[���h mbyte
    char* comment = nullptr;//�t�@�C���R�����g kbyte
};

struct LocalHeader {
    UINT64 pos;//�ǂݎ�肨���ʒu
    UINT16 version = 0;    //�W�J�ɕK�v�ȃo�[�W���� (�ŏ��o�[�W����) 2byte
    UINT16 bitflag = 0;//�ėp�ړI�̃r�b�g�t���O 2byte
    UINT16 method = 0;//���k���\�b�h 2byte
    UINT16 time = 0;//�t�@�C���̍ŏI�ύX���� 2byte
    UINT16 day = 0;//�t�@�C���̍ŏI�ύX���t 2byte
    UINT32 crc = 0;//CRC-32 4byte
    UINT32 size = 0;//���k�T�C�Y 4byte
    UINT32 nonsize = 0;//�񈳏k�T�C�Y 4byte
    UINT16 filenameleng = 0;//�t�@�C�����̒��� (n) 2byte
    UINT16 fieldleng = 0;//�g���t�B�[���h�̒��� (m) 2byte
    char* filename = nullptr;//�t�@�C���� nbyte
    char* kakutyo = nullptr;//�g���t�B�[���h mbyte
};

struct ENDrecord {
    UINT32 endsig = 0;

    UINT16 discnum = 0;    //���̃f�B�X�N�̐� 2byte
    UINT16 disccentral = 0;    //�Z���g�����f�B���N�g�����J�n����f�B�X�N 2byte
    UINT16 centralnum = 0; //���̃f�B�X�N��̃Z���g�����f�B���N�g�����R�[�h�̐� 2byte
    UINT16 centralsum = 0; //�Z���g�����f�B���N�g�����R�[�h�̍��v�� 2byte
    UINT32 size = 0;//�Z���g�����f�B���N�g���̃T�C�Y (�o�C�g) 4byte
    UINT32 position = 0;//�Z���g�����f�B���N�g���̊J�n�ʒu�̃I�t�Z�b�g 4byte
    UINT16 commentleng = 0; //�Z���g�����f�B���N�g�����R�[�h�̍��v�� 2byte
    char* comment = nullptr;//ZIP�t�@�C���̃R�����g nbyte
};

class ReadZipHead {
public:
    CenterDerect* CH;
    LocalHeader* LH;
    ENDrecord* EH;
    std::ifstream* fin;


    ReadZipHead(std::ifstream* f);
    ~ReadZipHead();

    UINT64 readLocal(UINT64 p);
    UINT64 readEnd();
    UINT64 readCentral();
};