#pragma once

class bitstream {
public:
    unsigned char stocks[64] = { 0 };
    unsigned int bitoutvalue;
    unsigned short bitstartpos;//���ݓǂݍ��񂾎��̈ʒu
    unsigned short bitendpos;//�ǂݍ��񂾌�̈ʒu
    unsigned short stockValue;
    unsigned short bitpos;//�ǂݍ��݈ʒu�̋���

    bitstream();
    void BigendOut(int setValue);
    void LittleendOut(int setValueR);

    void LittleendIn(int bi, int n);
    void BigendIn(int bir, int nr);
};