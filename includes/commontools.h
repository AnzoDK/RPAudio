#pragma once
#include <iostream>
//Does not work with char**
class RPtools
{
public:
    static int c_arrLength(char* arr);
    static int Syncsafe_to_int(uint8_t* syncSafe,int bytes=4);
    static int Mp3FrequencyTable(int code);
    static int mp3ModeTable(int code);
    static unsigned short huffman_table[];
};
