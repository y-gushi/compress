#include "WriteData.h"
#include <iostream>

DataWrite::DataWrite(FILE* f)
{
	wf = f;
}

DataWrite::~DataWrite()
{
}

int DataWrite::writedata(UINT8* d, UINT32 len)
{
	
	if (wf) {
		std::cout << "ƒf[ƒ^‘‚«ž‚Ý" << std::endl;
		UINT32 i = 0;
		while (i < len) {
			fwrite(&d[i], sizeof(char), 1, wf);
			i++;
		}
		return 1;
	}

	return 0;
}

UINT64 DataWrite::nocompressWrite(std::ifstream* f, UINT32 len,UINT64 p,FILE *of)
{
	UINT32 i = 0;
	char data = 0;

	f->seekg(p, std::ios_base::beg);

	while (i < len) {
		data = 0;
		f->read((char*)&data, sizeof(char));
		fwrite(&data, sizeof(char), 1, wf);
		i++;
	}

	return f->tellg();
}
