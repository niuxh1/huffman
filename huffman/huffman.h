//
// Created by ASUS on 24-11-23.
//

#ifndef HUFFMAX_TREE__HUFFMAN_H_
#define HUFFMAX_TREE__HUFFMAN_H_

#include <string>
#include <unordered_map>
#include <queue>
#include <vector>
#include <fstream>
#include <sstream>

using namespace std;

class huffman_node{
public:
    huffman_node(char c,int freq):c(c),freq(freq),left(nullptr),right(nullptr){}
    char c;
    int freq;
    huffman_node *left, *right;
};

class compare{
public:
    bool operator()(huffman_node *a, huffman_node *b){
        return a->freq > b->freq;
    }
};

class huffman {
private:
    huffman_node *root;
    unordered_map<char, string> huffman_code;
    unordered_map<string ,char> reserve_huffman_code;

    void build_tree(const unordered_map<char, int> &freq);
   void generate_codes(huffman_node *node, const string &code);
    void delete_tree(huffman_node *node);
public:
    huffman();
    ~huffman();

    void build(const unordered_map<char, int> &freq);
    void encode(const string& input,const string& encode);
    void decode(const string& encode,const string& decode);
    void print_tree(const string & output_tree);
    void save_tree(const string & tree_file);
    void read_tree(const string & tree_file);;
};

#endif //HUFFMAX_TREE__HUFFMAN_H_
