#include<iostream>
#include<queue>
#include <stdint.h>
#include<cstring>
#include<fstream>
#include<algorithm>
#include <assert.h>
#include<map>
#include "huffman.hpp"

int        Huffman::Encoder::init() {
    if(!in_file) {
        printf("no input file\n");
        return 1;
    }
    struct compare {
        bool operator()(node * a, node * b)
        {
            return a->cnt > b->cnt;
        }
    };

    std::priority_queue<node *, std::vector<node *>, compare> pq;
    std::queue<node *> q;
    
    in_file.seekg(0, std::ios::end);
	filesz = in_file.tellg();

	buffer = (char *)malloc(filesz);

    in_file.seekg(0, std::ios::beg);

	in_file.read(buffer, filesz);

    for (size_t i = 0; i < filesz; i++)
        cnt[0xff & buffer[i]]++;
    
    // create leaf node
    for (int i = 0; i < (1<<8); i++) {
        if(cnt[i] == 0) continue;

        node * leaf_node = new node;
        leaf_node->val = i;
        leaf_node->cnt = cnt[i];
        
        pq.push(leaf_node);
    }

    while (pq.size() > 1) {
        node * a = pq.top();
        pq.pop();

        node * b = pq.top();
        pq.pop();

        node * new_root = new node;
        new_root->cnt = a->cnt + b->cnt;
        new_root->lnode = a;
        new_root->rnode = b;

        pq.push(new_root);
    }

    root = pq.top();
    pq.pop();


    // Write the string on nodes
    q.push(root);

    while (!q.empty()) {
        node * tree_node = q.front();
        q.pop();

        if (nullptr == tree_node->lnode && nullptr == tree_node->rnode) {
            codes[tree_node->val] = tree_node->str;
            continue;
        }

        if (nullptr != tree_node->lnode) {
            tree_node->lnode->str = tree_node->str;
            tree_node->lnode->str.push_back('0');
            q.push(tree_node->lnode);
        }

        if (nullptr != tree_node->rnode) {
            tree_node->rnode->str = tree_node->str;
            tree_node->rnode->str.push_back('1');
            q.push(tree_node->rnode);
        }
    }

    return 0;
}

int     Huffman::Encoder::compress(char *  file_path){
    // Fill buffer, create tree
    in_file.open(file_path, std::ios::in | std::ios::binary);

    if (!in_file.good()) {
        return 1;
    }

    init();
    std::string out_file_name = file_path;
    out_file_name += ".compressed";
    header hdr;
    huff_table_entry htent;
    uint8_t byte = 0, bitcnt = 0;
    progress prg;

    out_file.open(out_file_name, std::ios::out | std::ios::binary);

    // Write header

    hdr.htoff = sizeof(header);

    hdr.csoff = sizeof(header) + sizeof(huff_table_entry)*(1<<8);

    uint32_t code_string_size_bit = 0;
    for(int i = 0; i < (1<<8); i++)
        code_string_size_bit += codes[i].size();
    
    hdr.data_off = hdr.csoff + (code_string_size_bit + 7)/8;

    out_file.write((char *)&hdr, sizeof(header));

    // Write huffman table entries
    for (int i = 0; i < (1<<8); i++) {
        htent.len = codes[i].size();
        out_file.write((char *)&htent, sizeof(huff_table_entry));
    }

    // Write codes string
    for (int i = 0; i < (1<<8); i++) {
        if (codes[i].size() == 0) {
            continue;
        }

        for (char c: codes[i]) {
            byte <<= 1;
            if (c == '1') {
                byte |= 1;
            }
            bitcnt++;

            if (bitcnt == 8) {
                out_file.write((char *)&byte, 1); 
                bitcnt = 0;
            }
        }
    }

    // Write last byte of codes string
    if(bitcnt > 0) {
        byte <<= 8 - bitcnt;
        out_file.write((char *)&byte, 1);
    }

    bitcnt = 0;
    for (size_t i = 0; i < filesz; i++) {
        // Update progress bar at every 500KB
        if(i%(1024*512) == 0) {
            prg.update_bar(i, filesz);
        }

        for(char c: codes[0xff & buffer[i]]) {
            byte <<= 1;
            byte |= (c == '1');

            bitcnt++;
            if (bitcnt == 8) {
                out_file.write((char *)&byte, 1);
                byte = 0;
                bitcnt = 0;
            }
        }
    }

    // Write last byte of data
    if (bitcnt > 0) {
        byte <<= 8 - bitcnt;
        out_file.write((char *)&byte, 1);
    }

    in_file.close();
    out_file.close();
    free(buffer);
    return 0;
};


int     Huffman::Decoder::decode(char   * file_path){
    in_file.open(file_path, std::ios::in | std::ios::binary);

    if (!in_file.good()) {
        return 1;
    }
    std::string out_file_name = file_path;
    out_file_name += ".out";

    out_file.open(out_file_name, std::ios::out | std::ios::binary);

    huff_table_entry htent;
    uint8_t byte;
    progress pgr;

    node *  cur;
    uint32_t cur_len = 0;

    int htind = 0;

    root = new node;
    cur = root;

    in_file.seekg(0, std::ios::end);
	filesz = in_file.tellg();
    
    in_file.seekg(0, std::ios::beg);
    in_file.read((char *)&hdr, sizeof(header));

    datasz = filesz - hdr.data_off;

    if (0 != memcmp((char *)&hdr.magic, "V69F", 4)) {
        printf("Wrong magic bytes. probably wrong file.\n");

        clean();
        return 1;
    }
    
    for (int i = 0; i < (1<<8); i++) {
        in_file.seekg(hdr.htoff + i*sizeof(huff_table_entry));
        in_file.read((char *)&htent, sizeof(huff_table_entry));

        ht[i] = htent;
    }

    // read code strings
    free(buffer);
    in_file.seekg(hdr.csoff);
    cssize = hdr.data_off - hdr.csoff;
    buffer = (char *)malloc(cssize);
    in_file.read(buffer, cssize);

    // create map
    while(ht[htind].len == 0 && htind < (1<<8)) htind++;

    if (htind == (1<<8)) {
        printf("No encoded bytes found\n");
        clean();
        return 1;
    }

    for(int i = 0, bits = 0; i < cssize; i++) {
        byte = (uint8_t)buffer[i];
        for(int j = 7; j >= 0; j--) {
            if(byte>>j&1) {
                if (nullptr == cur->rnode) 
                    cur->rnode = new node;
                cur = cur->rnode;
                cur_len++;
            }
            else {
                if (nullptr == cur->lnode) 
                    cur->lnode = new node;
                cur = cur->lnode;
                cur_len++;
            }

            if(cur_len == ht[htind].len) {
                assert(cur->val == (uint32_t)-1);
                cur->val = htind;
                htind++;
                cur_len = 0;
                cur = root;
                while(ht[htind].len == 0 && htind < (1<<8)) htind++;
            }

            if (htind == (1<<8)) {
                break;
            }
        }

        if (htind == (1<<8)) {
            break;
        }
    }

    cur = root;

    free(buffer);
    buffer = (char *) malloc(datasz);
    in_file.seekg(hdr.data_off);
    in_file.read(buffer, datasz);

    for (int i = 0; i < datasz; i++) {
        // Update progress bar at 500KB
        if(i%(1024*512) == 0){
            pgr.update_bar(i, datasz);
        }

        byte = static_cast<uint8_t>(buffer[i]);
        for(int j = 7; j >= 0; j--) {
            if(byte>>j&1) {
                if (nullptr == cur->rnode) {
                    printf("%d Data not encoded correctly\n", i);
                    clean();
                    return 1;
                }
                cur = cur->rnode;
            }
            else {
                if (nullptr == cur->lnode) {
                    printf("%d Data not encoded correctly\n", i);
                    clean();
                    return 1;
                }
                cur = cur->lnode;
            }

            if (nullptr == cur->lnode && nullptr == cur->rnode) {
                uint8_t val = static_cast<uint8_t>(cur->val);
                out_file.write((char *)&val, 1);
                cur = root;
            }
        }
    }

    clean();
    return 0;

};