#pragma once

#include <cstddef>
#include <random>
#include <algorithm>

const std::size_t TREE_SORT_SEQUENTIAL_COUNT_THRESHOLD = 64;
const std::size_t SEQUENTIAL_FLATTEN_THRESHOLD = 64;

struct SimpleBinarySearchTree {
    int value;
    std::size_t tree_size;
    SimpleBinarySearchTree *left_child, *right_child;
    SimpleBinarySearchTree(int val);
    // no destructor: the algorithm must clean the tree itself
    void update_size();
};

// 2 build function "will" shuffle the data
SimpleBinarySearchTree* build_binary_search_tree(int* begin, int* end);
void flatten_and_delete_tree(SimpleBinarySearchTree* root, int* result);

void parallelized_treesort(int* begin, int* end);

inline void randomized_parallelized_treesort(int* begin, int* end) {
    static std::mt19937 rng;
    std::shuffle(begin, end, rng);
    return parallelized_treesort(begin, end);
}

