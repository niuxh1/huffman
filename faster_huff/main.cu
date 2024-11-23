#include <iostream>
#include <fstream>
#include <unordered_map>
#include "huffman.h"

using namespace std;

string file_path = R"(C:\Users\ASUS\Desktop\clion\faster_huff\)";

int main() {
  try {
    // 创建 huffman 对象

    huffman huff;

    // 构建字符频率映射
    unordered_map<char, int> freq_map;

    // 示例输入文件，计算字符频率
    string filename = file_path+"data_freq.txt";
    ifstream input_file(filename);
    if (!input_file) {
      throw runtime_error("Error: Could not open input file for frequency calculation!");
    }

    char ch;
    while (input_file.get(ch)) {
      freq_map[ch]++;
    }

    input_file.close();

    // 构建哈夫曼树和编码
    huff.build(freq_map);

    // 保存哈夫曼树到文件
    huff.save_tree(file_path+"hfm_tree.txt");

    // 读取哈夫曼树
    huff.read_tree(file_path+"hfm_tree.txt");

    auto start = chrono::high_resolution_clock::now();

    huff.encode(file_path+"data_source.txt", file_path+"data_encoded.txt");

    auto cpu_end = chrono::high_resolution_clock::now();

    // 编码文件 (CUDA版)
    huff.encode_cuda(file_path+"data_source.txt", file_path+"data_encoded_cuda.txt");

    auto gpu_end = chrono::high_resolution_clock::now();

    // 打印哈夫曼树结构到文件
    huff.print_tree(file_path+"print_tree.txt");

    cout << "Huffman compression completed!" << endl;

    cout<<"CPU time: "<<chrono::duration_cast<chrono::milliseconds>(cpu_end - start).count()<<"ms"<<endl;
    cout<<"GPU time: "<<chrono::duration_cast<chrono::milliseconds>(gpu_end - cpu_end).count()<<"ms"<<endl;

  } catch (const exception& e) {
    cerr << "Error: " << e.what() << endl;
    return 1;
  }
  return 0;
}
