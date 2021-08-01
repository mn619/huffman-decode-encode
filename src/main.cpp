#include<iostream>
#include<fstream>
#include<vector>
#include<cstring>
#include<assert.h>
#include <huffman.cpp>

int     main(int argc, char * argv[]) {
    if (argc < 3) {
        printf("Wrong arguments : try zip {options} {file_name}\noptions:\n\t-e : encode\n\t-d : decode\n");
        return 0;
    }

    if (0 == strcmp(argv[1], "-e")){
        Huffman::Encoder h;
        h.compress(argv[2]);  
        return 0;      
    }
    else if(0 == strcmp(argv[1], "-d")) {
        Huffman::Decoder d;
        d.decode(argv[2]);
        return 0;
    }
    
    printf("Wrong arguments : try zip {options} {file_name}\noptions:\n\t-e : encode\n\t-d : decode\n");
    return 0;
}