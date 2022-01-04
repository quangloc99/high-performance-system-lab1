#include "parallelized-treesort.hpp"

#include <algorithm>
#include <utility>
#include <tuple>

static inline std::pair<int*, int*> three_way_partition(int* begin, int* end, int pivot);

SimpleBinarySearchTree::SimpleBinarySearchTree(int val)
    : value(val)
    , tree_size(1)
    , left_child(NULL)
    , right_child(NULL)
{}

void SimpleBinarySearchTree::update_size() {
    tree_size = 1;
    if (left_child) {
        tree_size += left_child->tree_size;
    }
    if (right_child) {
        tree_size += right_child->tree_size;
    }
}

static SimpleBinarySearchTree* build_inner(int* begin, int* end) {
    if (begin == end) {
        return NULL;
    }
    int pivot = *begin;
    auto root = new SimpleBinarySearchTree(pivot);
    int *begin_eq, *end_eq;
    std::tie(begin_eq, end_eq) = three_way_partition(begin + 1, end, pivot);
    // a very lazy way to choose middle
    int* mid = begin_eq + (end_eq - begin_eq) / 2;
    if (end - begin <= (long int)TREE_SORT_SEQUENTIAL_COUNT_THRESHOLD) {
        root->left_child = build_inner(begin + 1, mid);
        root->right_child = build_inner(mid, end);
    } else {
        SimpleBinarySearchTree *lc, *rc;
#pragma omp task shared(lc)
        lc = build_inner(begin + 1, mid);
#pragma omp task shared(rc)
        rc = build_inner(mid, end);
#pragma omp taskwait
        root->left_child = lc;
        root->right_child = rc;
    }
    root->update_size();
    return root;
}

SimpleBinarySearchTree* build_binary_search_tree(int* begin, int* end) {
    SimpleBinarySearchTree* root;
#pragma omp parallel shared(root)
#pragma omp single
    root = build_inner(begin, end);
    return root;
}

static void flatten_and_delete_tree_inner(SimpleBinarySearchTree* root, int* result) {
    if (root == NULL) {
        return ;
    }
    std::size_t left_size = root->left_child ? root->left_child->tree_size : 0;
    result[left_size] = root->value;
    if (root->tree_size <= (long int)SEQUENTIAL_FLATTEN_THRESHOLD) {
        flatten_and_delete_tree_inner(root->left_child, result);
        flatten_and_delete_tree_inner(root->right_child, result + left_size + 1);
    } else {
#pragma omp task
        flatten_and_delete_tree_inner(root->left_child, result);
#pragma omp task
        flatten_and_delete_tree_inner(root->right_child, result + left_size + 1);
#pragma omp taskwait
    }
    delete root;
}

void flatten_and_delete_tree(SimpleBinarySearchTree* root, int* result) {
#pragma omp parallel
#pragma omp single
    flatten_and_delete_tree_inner(root, result);
}

void parallelized_treesort(int* begin, int* end) {
    auto root = build_binary_search_tree(begin, end);
    flatten_and_delete_tree(root, begin);
}

static inline std::pair<int*, int*> three_way_partition(int* begin, int *end, int pivot) {
    int* begin_eq = begin;
    int* end_eq = begin;
    int* begin_greater = end;
    while (end_eq != begin_greater) {
        if (*end_eq == pivot) {
            ++end_eq;
            continue;
        }
        if (*end_eq < pivot) {
            std::swap(*begin_eq, *end_eq);
            ++begin_eq;
            ++end_eq;
            continue;
        }
        --begin_greater;
        std::swap(*end_eq, *begin_greater);
    }
    return std::make_pair(begin_eq, end_eq);
}
    
