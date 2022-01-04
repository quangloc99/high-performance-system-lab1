#pragma once

#include <cstddef>

const size_t TIMSORT_TO_INSERTION_SORT_THREADHOLD = 64;

void parallelized_modified_timsort(int* begin, int* end);
