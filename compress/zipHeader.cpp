#include "zipHeader.h"

ReadZipHead::ReadZipHead(std::ifstream *f)
{
	fin = f;
	CH = nullptr;
	LH = nullptr;
	EH = nullptr;
}

ReadZipHead::~ReadZipHead()
{
	free(CH);
	free(LH);
	free(EH);
}

UINT64 ReadZipHead::readLocal(UINT64 p)
{
	UINT32 readdata = 0;
	UINT32 sig = 0;

	std::cout << "���[�J�����[�h : " <<p<< std::endl;
	fin->seekg(p, std::ios_base::beg);

	LH = (LocalHeader*)malloc(sizeof(LocalHeader));

	/*���[�J���w�b�_�V�O�l�`����ǂݍ���*/
	while (sig != LOCAL_HEADER && !fin->eof()) {
		//�擪�R�[�h�܂œǂݍ���
		readdata = 0;
		fin->read((char*)&readdata, sizeof(char));
		sig = sig << 8;
		sig += readdata & 0xFF;
		std::cout << sig;
		if (sig == CENTRALSIGNATURE) {
			fin->seekg(0, std::ios_base::end);
			break;
		}
	}
	if (!fin->eof()) {
		//�W�J�ɕK�v�ȃo�[�W����
		LH->version = 0; readdata = 0;
		for (int i = 0; i < 2; i++) {
			fin->read((char*)&readdata, sizeof(char));
			readdata = ((readdata & 0xFF) << (8 * i));
			LH->version += readdata;
		}
		//�ėp�ړI�̃r�b�g�t���O
		LH->bitflag = 0; readdata = 0;
		for (int i = 0; i < 2; i++) {
			fin->read((char*)&readdata, sizeof(char));
			readdata = ((readdata & 0xFF) << (8 * i));
			LH->bitflag += readdata;
		}
		//���k���\�b�h
		LH->method = 0; readdata = 0;
		for (int i = 0; i < 2; i++) {
			fin->read((char*)&readdata, sizeof(char));
			readdata = ((readdata & 0xFF) << (8 * i));
			LH->method += readdata;
		}
		std::cout << "���k���\�b�h�F" << LH->method << std::endl;
		//�t�@�C���̍ŏI�ύX����
		LH->time = 0; readdata = 0;
		for (int i = 0; i < 2; i++) {
			fin->read((char*)&readdata, sizeof(char));
			readdata = ((readdata & 0xFF) << (8 * i));
			LH->time += readdata;
		}
		//�t�@�C���̍ŏI�ύX���t
		LH->day = 0; readdata = 0;
		for (int i = 0; i < 2; i++) {
			fin->read((char*)&readdata, sizeof(char));
			readdata = ((readdata & 0xFF) << (8 * i));
			LH->day += readdata;
		}
		//CRC-32
		LH->crc = 0; readdata = 0;
		for (int i = 0; i < 4; i++) {
			fin->read((char*)&readdata, sizeof(char));
			readdata = ((readdata & 0xFF) << (8 * i));
			LH->crc += readdata;
		}
		//���k�T�C�Y
		LH->size = 0; readdata = 0;
		for (int i = 0; i < 4; i++) {
			fin->read((char*)&readdata, sizeof(char));
			readdata = ((readdata & 0xFF) << (8 * i));
			LH->size += readdata;
		}
		std::cout << "�T�C�Y�F" << LH->size << std::endl;
		//    �񈳏k�T�C�Y
		LH->nonsize = 0; readdata = 0;
		for (int i = 0; i < 4; i++) {
			fin->read((char*)&readdata, sizeof(char));
			readdata = ((readdata & 0xFF) << (8 * i));
			LH->nonsize += readdata;
		}
		//�t�@�C�����̒��� (n)
		LH->filenameleng = 0; readdata = 0;
		for (int i = 0; i < 2; i++) {
			fin->read((char*)&readdata, sizeof(char));
			readdata = ((readdata & 0xFF) << (8 * i));
			LH->filenameleng += readdata;
		}
		//�g���t�B�[���h�̒��� (m)
		LH->fieldleng = 0; readdata = 0;
		for (int i = 0; i < 2; i++) {
			fin->read((char*)&readdata, sizeof(char));
			readdata = ((readdata & 0xFF) << (8 * i));
			LH->fieldleng += readdata;
		}
		//�t�@�C����
		UINT32 msize = ((UINT32)LH->filenameleng) + 1; readdata = 0;
		LH->filename = (char*)malloc(msize);
		if (LH->filename) {
			for (UINT16 i = 0; i < LH->filenameleng; i++) {
				fin->read((char*)&readdata, sizeof(char));
				LH->filename[i] = readdata;
			}
			LH->filename[LH->filenameleng] = '\0';
		}
		std::cout << "�t�@�C�����F" << LH->filename << std::endl;
		//�g���t�B�[���h
		msize = ((UINT32)LH->fieldleng) + 1; readdata = 0;
		LH->kakutyo = (char*)malloc(msize);
		for (UINT16 i = 0; i < LH->fieldleng; i++) {
			fin->read((char*)&readdata, sizeof(char));
			LH->kakutyo[i] = readdata;
		}

		LH->pos = fin->tellg();//�ǂݏI���ʒu

		return LH->pos;
	}

	return UINT64(0);//���k�f�[�^�̎n�܂�
}

UINT64 ReadZipHead::readEnd()
{
	return UINT64();
}

UINT64 ReadZipHead::readCentral()
{
	return UINT64();
}

