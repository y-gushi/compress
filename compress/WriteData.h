#pragma once
#include <fstream>
#include "typechange.h"

class DataWrite {
public:
	FILE* wf;

	DataWrite(FILE* f);
	~DataWrite();

	int writedata(UINT8* d,UINT32 len);
	UINT64 nocompressWrite(std::ifstream* f,UINT32 len,UINT64 p,FILE *of);
};