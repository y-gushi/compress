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

	std::cout << "ローカルリード : " <<p<< std::endl;
	fin->seekg(p, std::ios_base::beg);

	LH = (LocalHeader*)malloc(sizeof(LocalHeader));

	/*ローカルヘッダシグネチャを読み込む*/
	while (sig != LOCAL_HEADER && !fin->eof()) {
		//先頭コードまで読み込む
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
		//展開に必要なバージョン
		LH->version = 0; readdata = 0;
		for (int i = 0; i < 2; i++) {
			fin->read((char*)&readdata, sizeof(char));
			readdata = ((readdata & 0xFF) << (8 * i));
			LH->version += readdata;
		}
		//汎用目的のビットフラグ
		LH->bitflag = 0; readdata = 0;
		for (int i = 0; i < 2; i++) {
			fin->read((char*)&readdata, sizeof(char));
			readdata = ((readdata & 0xFF) << (8 * i));
			LH->bitflag += readdata;
		}
		//圧縮メソッド
		LH->method = 0; readdata = 0;
		for (int i = 0; i < 2; i++) {
			fin->read((char*)&readdata, sizeof(char));
			readdata = ((readdata & 0xFF) << (8 * i));
			LH->method += readdata;
		}
		std::cout << "圧縮メソッド：" << LH->method << std::endl;
		//ファイルの最終変更時間
		LH->time = 0; readdata = 0;
		for (int i = 0; i < 2; i++) {
			fin->read((char*)&readdata, sizeof(char));
			readdata = ((readdata & 0xFF) << (8 * i));
			LH->time += readdata;
		}
		//ファイルの最終変更日付
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
		//圧縮サイズ
		LH->size = 0; readdata = 0;
		for (int i = 0; i < 4; i++) {
			fin->read((char*)&readdata, sizeof(char));
			readdata = ((readdata & 0xFF) << (8 * i));
			LH->size += readdata;
		}
		std::cout << "サイズ：" << LH->size << std::endl;
		//    非圧縮サイズ
		LH->nonsize = 0; readdata = 0;
		for (int i = 0; i < 4; i++) {
			fin->read((char*)&readdata, sizeof(char));
			readdata = ((readdata & 0xFF) << (8 * i));
			LH->nonsize += readdata;
		}
		//ファイル名の長さ (n)
		LH->filenameleng = 0; readdata = 0;
		for (int i = 0; i < 2; i++) {
			fin->read((char*)&readdata, sizeof(char));
			readdata = ((readdata & 0xFF) << (8 * i));
			LH->filenameleng += readdata;
		}
		//拡張フィールドの長さ (m)
		LH->fieldleng = 0; readdata = 0;
		for (int i = 0; i < 2; i++) {
			fin->read((char*)&readdata, sizeof(char));
			readdata = ((readdata & 0xFF) << (8 * i));
			LH->fieldleng += readdata;
		}
		//ファイル名
		UINT32 msize = ((UINT32)LH->filenameleng) + 1; readdata = 0;
		LH->filename = (char*)malloc(msize);
		if (LH->filename) {
			for (UINT16 i = 0; i < LH->filenameleng; i++) {
				fin->read((char*)&readdata, sizeof(char));
				LH->filename[i] = readdata;
			}
			LH->filename[LH->filenameleng] = '\0';
		}
		std::cout << "ファイル名：" << LH->filename << std::endl;
		//拡張フィールド
		msize = ((UINT32)LH->fieldleng) + 1; readdata = 0;
		LH->kakutyo = (char*)malloc(msize);
		for (UINT16 i = 0; i < LH->fieldleng; i++) {
			fin->read((char*)&readdata, sizeof(char));
			LH->kakutyo[i] = readdata;
		}

		LH->pos = fin->tellg();//読み終わり位置

		return LH->pos;
	}

	return UINT64(0);//圧縮データの始まり
}

UINT64 ReadZipHead::readEnd()
{
	return UINT64();
}

UINT64 ReadZipHead::readCentral()
{
	return UINT64();
}

