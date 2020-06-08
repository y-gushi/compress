#include "bitstream.h"

bitstream::bitstream()
{
    for (int i = 0; i < 64; i++) {
        stocks[i] = 0;
    }
    bitoutvalue = 0;
    bitstartpos = 0;
    bitendpos = 0;
    bitpos = 0;
    stockValue = 0;
}

void bitstream::BigendOut(int setValue)//�z����Bit������o��
{
    stockValue = bitoutvalue;
    bitoutvalue = 0;
    for (int i = 0; i < setValue; i++) {
        bitoutvalue += stocks[bitstartpos] << (setValue - 1 - i);
        bitstartpos++;
        //std::cout << "bitstartpos:" << bitstartpos << "  setValue:" << setValue << std::endl;
        if (bitstartpos > 63) { bitstartpos = 0; }
    }
    if (bitendpos < bitstartpos) { bitpos = bitendpos + (64 - bitstartpos); }
    else { bitpos = bitendpos - bitstartpos; }
}

void bitstream::LittleendOut(int setValueR)//�z�񉺈�Bit������o���A���]�����l���Z�b�g
{
    stockValue = bitoutvalue;
    bitoutvalue = 0;
    for (int i = 0; i < setValueR; i++) {
        bitoutvalue += stocks[bitstartpos] << i;
        bitstartpos++;
        if (bitstartpos > 63) { bitstartpos = 0; }
    }
    if (bitendpos < bitstartpos) { bitpos = bitendpos + (64 - bitstartpos); }
    else { bitpos = bitendpos - bitstartpos; }
}

void bitstream::LittleendIn(int bi, int n)
{
    for (int j = 0; j < n; j++) {
        stocks[bitendpos] = (bi >> ((n - 1) - j)) & 0x01;
        bitendpos++;
        if (bitendpos > 63) { bitendpos = 0; }
    }
    if (bitendpos < bitstartpos) { bitpos = bitendpos + (64 - bitstartpos); }
    else { bitpos = bitendpos - bitstartpos; }
}

void bitstream::BigendIn(int bir, int nr)//���Bit�����Bit�Ɂi�r�b�O�G���f�B�A���j
{
    for (int j = 0; j < nr; j++) {
        stocks[bitendpos] = (bir >> j) & 0x01;
        bitendpos++;
        //std::cout << "bitendpos:" << bitendpos<<"  bir:"<<bir << std::endl;
        if (bitendpos > 63) { bitendpos = 0; }//stocks = stockp;
    }
    if (bitendpos < bitstartpos) { bitpos = bitendpos + (64 - bitstartpos); }
    else { bitpos = bitendpos - bitstartpos; }
}
