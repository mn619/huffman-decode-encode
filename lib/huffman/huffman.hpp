#ifndef HUFFMAN_HPP_INCLUDED
#define HUFFMAN_HPP_INCLUDED

namespace Huffman{

    struct progress{
        void update_bar (uint64_t written, uint64_t total) {
            uint64_t percentage = (written * 100)/total;
            assert(percentage <= 100);

            printf("\033[0;32m");
            printf("\r");
            printf("[");

            for (int i = 0; i < 100; i++) {
                if(i <= percentage) {
                    printf("#");
                }
                else printf("-");
            }
            printf("] ");
            printf("%ld%%", percentage);
        }
    };
    

    struct node{
        node(): val     ((uint32_t)-1),
                cnt     (0),
                lnode   (nullptr), 
                rnode   (nullptr) {};
        
        uint32_t val;
        uint32_t cnt;
        std::string str;
        node * lnode;
        node * rnode;
    };

    struct header {
        char magic[4] = {'v', '6', '9'};
        uint32_t htoff;
        uint32_t csoff;
        uint32_t data_off;
    };

    struct huff_table_entry{
        huff_table_entry (): len(0) {};

        uint8_t len;
    };

    class Encoder{
        private:
            node * root;
            std::vector<uint32_t> cnt;
            std::vector<huff_table_entry> huff_table;
            int init();
        public:
            Encoder():  cnt         (std::vector<uint32_t>(1<<8, 0)),
                        buffer      (nullptr),
                        codes       (std::vector<std::string>(1<<8, "")) {};
                        
            std::ifstream in_file;
            std::ofstream out_file;
            char * buffer;
            uint32_t filesz;
            std::vector <std::string> codes;
            int compress(char * file_path);
    };


    class Decoder{
        private:
            std::vector<huff_table_entry> ht;
            std::map<std::string, uint8_t> m;
        public:
            Decoder():  buffer      (nullptr),
                        ht          (std::vector<huff_table_entry>(1<<8)) {};

            char * buffer;
            uint32_t filesz;
            std::ifstream in_file;
            std::ofstream out_file;
            int decode(char * file_path);
    };
}

#endif