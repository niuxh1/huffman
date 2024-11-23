# huffman
# v1.0
一个简单的huffman实现，你可以修改main中的file_name实现修改文件地址，其中data_src是资源文件,data_freq是频率统计文件，data_decode是编码后输出文件，data_encode是01压缩文件，hfm_tree是树资源文件，print_hfm更直观的展示了树结构

# v2.0
发现Huffman树非常适合并行计算，故使用cuda进行加速，成功运行，效果一半，已经发布，证明huffman不适合并行计算
