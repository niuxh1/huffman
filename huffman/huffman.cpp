//
// Created by ASUS on 24-11-23.
//

#include <functional>
#include <iomanip>
#include <iostream>
#include "huffman.h"

using namespace std;

void huffman::build_tree(const unordered_map<char, int> &freq) {
  priority_queue<huffman_node*,vector<huffman_node*>,compare> pq;
  for(const auto[c,f]:freq){
    pq.push(new huffman_node(c,f));
  }
  while(pq.size()>1){
    huffman_node* left=pq.top();
    pq.pop();
    huffman_node* right=pq.top();
    pq.pop();
    huffman_node* parent=new huffman_node(0,left->freq+right->freq);
    parent->left=left;
    parent->right=right;
    pq.push(parent);
  }
  root=pq.top();

}
void huffman::generate_codes(huffman_node *node, const string &code) {
  if(!node) return;
  if(!node->left&&!node->right) {
    huffman_code[node->c] = code;
    reserve_huffman_code[code] = node->c;
  }
    generate_codes(node->left, code + '0');
    generate_codes(node->right, code + '1');
}
void huffman::delete_tree(huffman_node *node) {
    if(!node) return;
    delete_tree(node->left);
    delete_tree(node->right);
    delete node;
}
huffman::huffman():root(nullptr) {

}
huffman::~huffman() {
    delete_tree(root);
}
void huffman::build(const unordered_map<char, int> &freq) {
    build_tree(freq);
    generate_codes(root, "");
    cout<<"build done!"<<endl;
}
void huffman::encode(const string &input, const string &encode) {
    ifstream input_stream(input);
    ofstream encode_stream(encode,ios::binary);
    if(!input_stream || !encode_stream) throw runtime_error("file not found");

    ostringstream oss;
    char c;
    while(input_stream.get(c)){
        oss<<huffman_code[c];
    }
    string s=oss.str();
    encode_stream<<s;
    input_stream.close();
    encode_stream.close();
    cout<<"encode done!"<<endl;
}
void huffman::decode(const string &encode, const string &decode) {
    ifstream encode_stream(encode,ios::binary);
    ofstream decode_stream(decode);
    if(!encode_stream || !decode_stream) throw runtime_error("file not found");

    ostringstream oss;
    oss<<encode_stream.rdbuf();
    string s=oss.str();
    huffman_node* node=root;
    for(char c:s){
        if(c=='0') node=node->left;
        else node=node->right;
        if(node->left==nullptr&&node->right==nullptr){
            decode_stream<<node->c;
            node=root;
        }
    }
    encode_stream.close();
    decode_stream.close();
    cout<<"decode done!"<<endl;
}
void huffman::print_tree(const std::string &output_tree) {
  std::ofstream outputStream(output_tree);
  if (!outputStream) throw std::runtime_error("Failed to open file for writing!");

  // 打印树的递归函数
  std::function<void(huffman_node*, int, std::string)> printer =
      [&](huffman_node* node, int depth, std::string prefix) {
        if (!node) return;

        // 打印当前节点
        outputStream << prefix
                     << (prefix.empty() ? "" : "-- ") // 左右分支标记
                     << (node->c ? node->c : '*') // 显示字符或 * 表示内部节点
                     << " (" << node->freq << ")" // 显示频率
                     << std::endl;

        // 递归打印左右子树
        if (node->left || node->right) {
          printer(node->left, depth + 1, prefix + "|  ");  // 左子树
          printer(node->right, depth + 1, prefix + "   "); // 右子树
        }
      };

  printer(root, 0, "");
  outputStream.close();
  std::cout << "Print tree done!" << std::endl;
}

void huffman::save_tree(const string &tree_file) {
    ofstream tree_stream(tree_file,ios::binary);
    if(!tree_stream) throw runtime_error("file not found");
    function<void(huffman_node*)> saver=[&](huffman_node* node){
        if(!node) {
            tree_stream.put('#');
            return;
        }
        tree_stream.put(node->c);
        saver(node->left);
        saver(node->right);
    };
    saver(root);
    tree_stream.close();
    cout<<"save tree done!"<<endl;
}
void huffman::read_tree(const string &tree_file) {
    ifstream  input_stream(tree_file,ios::binary);
    if(!input_stream) throw runtime_error("file not found");
    function<huffman_node*()> loader=[&]()->huffman_node*{
        char ch;
        if(!input_stream.get(ch)||ch=='#') return static_cast<huffman_node*>(nullptr);

        huffman_node* node=new huffman_node(ch,0);
        node->left=loader();
        node->right=loader();
        return node;
    };
    delete_tree(root);
    root=loader();
    input_stream.close();
    cout<<"read tree done!"<<endl;
}
