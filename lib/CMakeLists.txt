add_library(
    huffman
    huffman/huffman.hpp
    huffman/huffman.cpp
    )

target_include_directories(huffman PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/huffman")
