#include <iostream>
#include "huffman.h"

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <stdexcept>
#include <chrono>

string file_name = R"(C:\Users\ASUS\Desktop\clion\huffman\)";
std::unordered_map<char, int> buildFrequencyMap(const std::string& filename) {
  // 打开文件
  std::ifstream file(filename);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open file: " + filename);
  }

  // 用于存储频率的哈希表
  std::unordered_map<char, int> freqMap;

  char ch;
  while (file.get(ch)) { // 逐字符读取
    // 检查是否为有效字符（ASCII 范围 32 到 126，或换行符 '\n'）
    if ((ch >= 32 && ch <= 126) || ch == '\n') {
      freqMap[ch]++;
    }
  }

  file.close();
  return freqMap;
}



int main() {
  auto start = std::chrono::high_resolution_clock::now();
  try {
    // 构建字符频率映射

    std::string filename = "data_freq.txt";
    std::unordered_map<char, int> freqMap = buildFrequencyMap(filename);

    huffman huff;

    huff.build(freqMap);


    huff.save_tree(file_name+"hfm_tree.txt");
    huff.read_tree(file_name+"hfm_tree.txt");

    huff.encode(file_name+"data_source.txt", file_name+"data_encoded.txt");
    huff.decode(file_name+"data_encoded.txt", file_name+"data_decoded.txt");

    huff.print_tree(file_name+"print_tree.txt");

    cout<<"huffman all done!"<<endl;

  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << '\n';
  }
  auto end = std::chrono::high_resolution_clock::now();

  // 计算时间差
  std::chrono::duration<double> duration = end - start;

  std::cout << "huffman time: " << duration.count() << "s" << std::endl;

  return 0;
}
