#include "./parallelized-modified-timsort.hpp"

#include <cstring>
#include <utility>
#include <algorithm>
#include <tuple>
#include "omp.h"

static inline void parallelized_modified_timsort_inner(int* begin, int* end, int* buff);
void parallelized_modified_timsort(int* begin, int* end) {
    int* buff = new int[end - begin];
#pragma omp parallel
#pragma omp single
    {
        parallelized_modified_timsort_inner(begin, end, buff);
    }
    
    delete[] buff;
}

static inline void merge3(int* begin_a, int* end_a, int* begin_b, int* end_b,
        int *begin_c, int* end_c, int *result);
static inline void insertion_sort(int* begin, int* end);
static inline std::pair<int*, int*> find_run(int* begin, int* end);

static inline void parallelized_modified_timsort_inner(int* begin, int* end, int* buff) {
    if (end - begin <= (long int)TIMSORT_TO_INSERTION_SORT_THREADHOLD) {
        insertion_sort(begin, end);
        return ;
    }
    int *run_begin, *run_end;
    std::tie(run_begin, run_end) = find_run(begin, end);
#pragma omp task
    parallelized_modified_timsort_inner(begin, run_begin, buff);
#pragma omp task
    parallelized_modified_timsort_inner(run_end, end, buff + (run_end - begin));
#pragma omp taskwait
    merge3(
        begin, run_begin,
        run_begin, run_end,
        run_end, end,
        buff
    );
    memcpy(begin, buff, sizeof(*begin) * (end - begin));
}

static void insertion_sort(int* begin, int* end) {
    for (int* it = begin + 1; it < end; ++it) {
        int val = *it;
        int* prv;
        for (prv = it - 1; prv >= begin && *prv > val; --prv) {
            prv[1] = *prv;
        }
        prv[1] = val;
    }
}

static void merge3(
    int* begin_a, int* end_a,
    int* begin_b, int* end_b,
    int* begin_c, int* end_c,
    int* result
) {
    while (begin_a != end_a || begin_b != end_b || begin_c != end_c) {
        int** opt = NULL;
        if (begin_a != end_a) {
            opt = &begin_a;
        }
        if (begin_b != end_b && (opt == NULL || **opt > *begin_b)) {
            opt = &begin_b;
        }
        if (begin_c != end_c && (opt == NULL || **opt > *begin_c)) {
            opt = &begin_c;
        }
        *result++ = *(*opt)++;
    }
}

static inline std::pair<int*, int*> find_run(int* begin, int* end) {
    if (end - begin <= 1) {
        return std::make_pair(begin, end);
    }
    int* mid = begin + (end - begin) / 2;
    int *run_begin = mid, *run_end = mid + 1;
    if (*run_begin > *run_end) {
        while (run_begin > begin && run_begin[-1] > *run_begin) {
            --run_begin;
        }
        while (run_end < end && run_end[-1] > *run_end) {
            ++run_end;
        }
        std::reverse(run_begin, run_end);
    } else {
        while (run_begin > begin && run_begin[-1] <= *run_begin) {
            --run_begin;
        }
        while (run_end < end && run_end[-1] <= *run_end) {
            ++run_end;
        }
    }
    return std::make_pair(run_begin, run_end);
}

