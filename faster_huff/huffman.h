#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <unordered_map>
#include <string>
#include <queue>
#include <fstream>
#include <iostream>
#include <functional>
#include <sstream>
#include <vector>
#include <chrono>
#include <thread>
#include <future>
#include <cuda_runtime.h>

using namespace std;

__global__ void encode_kernel(char *input, char *output, int *huffman_codes, int n) {
  int idx = blockIdx.x * blockDim.x + threadIdx.x;  // 计算线程在块中的全局索引
  int threadId = threadIdx.x + blockIdx.x * blockDim.x;

  // 每个线程处理多个字符，假设每个线程处理 4 个字符
  const int num_per_thread = 4;

  if (threadId < n) {
    for (int i = 0; i < num_per_thread; i++) {
      int index = threadId * num_per_thread + i;
      if (index < n) {
        output[index] = huffman_codes[(unsigned char)input[index]];
      }
    }
  }
}

void encode_cuda(const char *input, char *output, int *huffman_codes, int n) {
  // 分配设备内存
  char *d_input, *d_output;
  int *d_huffman_codes;

  cudaMalloc((void **)&d_input, n * sizeof(char));
  cudaMalloc((void **)&d_output, n * sizeof(char));
  cudaMalloc((void **)&d_huffman_codes, 256 * sizeof(int));  // 假设有 256 种字符

  // 将数据从主机复制到设备
  cudaMemcpy(d_input, input, n * sizeof(char), cudaMemcpyHostToDevice);
  cudaMemcpy(d_huffman_codes, huffman_codes, 256 * sizeof(int), cudaMemcpyHostToDevice);

  // 计算适当的线程数和块数
  int blockSize = 256;  // 每个线程块 256 线程
  int numBlocks = (n + blockSize * 4 - 1) / (blockSize * 4);  // 每个线程处理多个字符，块数调整为处理多个字符的数量

  // 启动CUDA内核
  encode_kernel<<<numBlocks, blockSize>>>(d_input, d_output, d_huffman_codes, n);

  // 检查是否有错误
  cudaDeviceSynchronize();
  cudaError_t err = cudaGetLastError();
  if (err != cudaSuccess) {
    std::cerr << "CUDA kernel failed: " << cudaGetErrorString(err) << std::endl;
  }

  // 将结果从设备复制回主机
  cudaMemcpy(output, d_output, n * sizeof(char), cudaMemcpyDeviceToHost);

  // 释放设备内存
  cudaFree(d_input);
  cudaFree(d_output);
  cudaFree(d_huffman_codes);
}




class huffman {
public:
  struct huffman_node {
    char c;
    int freq;
    huffman_node *left, *right;
    huffman_node(char ch, int frequency) : c(ch), freq(frequency), left(nullptr), right(nullptr) {}
  };

  huffman();
  ~huffman();

  void build(const std::unordered_map<char, int>& freq);
  void encode(const std::string& input, const std::string& encode);
  void decode(const std::string& encode, const std::string& decode);
  void print_tree(const std::string& output_tree);
  void save_tree(const std::string& tree_file);
  void read_tree(const std::string& tree_file);
  void generate_codes(huffman_node* node, const std::string& code);

  void encode_cuda(const std::string& input, const std::string& output);
  void build_tree(const std::unordered_map<char, int>& freq);

private:
  huffman_node* root;
  std::unordered_map<char, std::string> huffman_code;
  std::unordered_map<std::string, char> reserve_huffman_code;

  void delete_tree(huffman_node* node);
};

class Timer {
public:
  Timer() {
    start_time = std::chrono::high_resolution_clock::now();
  }

  ~Timer() {
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> duration = end_time - start_time;
    std::cout << "Time taken: " << duration.count() << " seconds\n";
  }

private:
  std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
};

#endif
void huffman::build_tree(const unordered_map<char, int>& freq) {
  priority_queue<huffman_node*, vector<huffman_node*>, function<bool(huffman_node*, huffman_node*)>> pq([](huffman_node* a, huffman_node* b) {
    return a->freq > b->freq;
  });

  for (const auto& [c, f] : freq) {
    pq.push(new huffman_node(c, f));
  }

  while (pq.size() > 1) {
    huffman_node* left = pq.top();
    pq.pop();
    huffman_node* right = pq.top();
    pq.pop();
    huffman_node* parent = new huffman_node(0, left->freq + right->freq);
    parent->left = left;
    parent->right = right;
    pq.push(parent);
  }
  root = pq.top();
}

void huffman::generate_codes(huffman_node* node, const string& code) {
  if (!node) return;
  if (!node->left && !node->right) {
    huffman_code[node->c] = code;
    reserve_huffman_code[code] = node->c;
  }
  generate_codes(node->left, code + '0');
  generate_codes(node->right, code + '1');
}

void huffman::delete_tree(huffman_node* node) {
  if (!node) return;
  delete_tree(node->left);
  delete_tree(node->right);
  delete node;
}

huffman::huffman() : root(nullptr) {}

huffman::~huffman() {
  delete_tree(root);
}

void huffman::build(const unordered_map<char, int>& freq) {
  build_tree(freq);
  generate_codes(root, "");
  cout << "Build done!" << endl;
}

void huffman::encode(const string& input, const string& encode) {
  ifstream input_stream(input);
  ofstream encode_stream(encode, ios::binary);
  if (!input_stream || !encode_stream) throw runtime_error("File not found");

  ostringstream oss;
  char c;
  while (input_stream.get(c)) {
    oss << huffman_code[c];
  }
  string s = oss.str();
  encode_stream << s;
  input_stream.close();
  encode_stream.close();
  cout << "Encode done!" << endl;
}

void huffman::decode(const string& encode, const string& decode) {
  ifstream encode_stream(encode, ios::binary);
  ofstream decode_stream(decode);
  if (!encode_stream || !decode_stream) throw runtime_error("File not found");

  ostringstream oss;
  oss << encode_stream.rdbuf();
  string s = oss.str();
  huffman_node* node = root;
  for (char c : s) {
    if (c == '0') node = node->left;
    else node = node->right;
    if (node->left == nullptr && node->right == nullptr) {
      decode_stream << node->c;
      node = root;
    }
  }
  encode_stream.close();
  decode_stream.close();
  cout << "Decode done!" << endl;
}

void huffman::print_tree(const string& output_tree) {
  ofstream outputStream(output_tree);
  if (!outputStream) throw runtime_error("Failed to open file for writing!");

  function<void(huffman_node*, int, string)> printer = [&](huffman_node* node, int depth, string prefix) {
    if (!node) return;
    outputStream << prefix << (prefix.empty() ? "" : "-- ") << (node->c ? node->c : '*') << " (" << node->freq << ")" << endl;
    if (node->left || node->right) {
      printer(node->left, depth + 1, prefix + "|  ");
      printer(node->right, depth + 1, prefix + "   ");
    }
  };

  printer(root, 0, "");
  outputStream.close();
  cout << "Print tree done!" << endl;
}

void huffman::save_tree(const string& tree_file) {
  ofstream tree_stream(tree_file, ios::binary);
  if (!tree_stream) throw runtime_error("File not found");

  function<void(huffman_node*)> saver = [&](huffman_node* node) {
    if (!node) {
      tree_stream.put('#');
      return;
    }
    tree_stream.put(node->c);
    saver(node->left);
    saver(node->right);
  };
  saver(root);
  tree_stream.close();
  cout << "Save tree done!" << endl;
}

void huffman::read_tree(const string& tree_file) {
  ifstream input_stream(tree_file, ios::binary);
  if (!input_stream) throw runtime_error("File not found");

  function<huffman_node*()> loader = [&]() -> huffman_node* {
    char ch;
    if (!input_stream.get(ch) || ch == '#') return nullptr;
    huffman_node* node = new huffman_node(ch, 0);
    node->left = loader();
    node->right = loader();
    return node;
  };
  delete_tree(root);
  root = loader();
  input_stream.close();
  cout << "Read tree done!" << endl;
}

void huffman::encode_cuda(const std::string& input, const std::string& output) {
  ifstream input_stream(input, ios::binary);
  ofstream output_stream(output, ios::binary);
  if (!input_stream || !output_stream) throw runtime_error("File not found");

  // 读取文件内容到内存
  ostringstream oss;
  char ch;
  while (input_stream.get(ch)) {
    oss << ch;
  }
  string data = oss.str();

  // 分配内存并将数据传输到GPU
  char* d_input, * d_output;
  int* d_huffman_codes;
  int* huffman_codes = new int[256];  // 创建一个大小为 256 的数组，用于存储哈夫曼编码

  // 填充 huffman_codes 数组
  for (int i = 0; i < 256; ++i) {
    if (huffman_code.find(i) != huffman_code.end()) {
      huffman_codes[i] = stoi(huffman_code[i], nullptr, 2);  // 将哈夫曼编码字符串转换为整数
    } else {
      huffman_codes[i] = 0;  // 如果字符没有对应的编码，填充为 0
    }

  }

  cudaMalloc((void**)&d_input, data.size() * sizeof(char));
  cudaMalloc((void**)&d_output, data.size() * sizeof(char));
  cudaMalloc((void**)&d_huffman_codes, 256 * sizeof(int));  // 为哈夫曼编码表分配内存
  cudaMemcpy(d_input, data.c_str(), data.size() * sizeof(char), cudaMemcpyHostToDevice);
  cudaMemcpy(d_huffman_codes, huffman_codes, 256 * sizeof(int), cudaMemcpyHostToDevice);

  // 启动CUDA内核
  int block_size = 256;
  int num_blocks = (data.size() + block_size - 1) / block_size;
  encode_kernel<<<num_blocks, block_size>>>(d_input, d_output, d_huffman_codes, data.size());

  // 从GPU拷贝数据回CPU并保存结果
  char* encoded_data = new char[data.size()];
  cudaMemcpy(encoded_data, d_output, data.size() * sizeof(char), cudaMemcpyDeviceToHost);
  output_stream.write(encoded_data, data.size());

  // 释放CUDA内存
  cudaFree(d_input);
  cudaFree(d_output);
  cudaFree(d_huffman_codes);
  delete[] huffman_codes;
  delete[] encoded_data;
}
