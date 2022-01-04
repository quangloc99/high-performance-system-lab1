#include <algorithm>
#include <cstddef>
#include <chrono>
#include <fstream>
#include <iostream>
#include <functional>
#include <limits>
#include <random>
#include <string>
#include <vector>
#include <filesystem>

#include "omp.h"

#include "parallelized-modified-timsort.hpp"
#include "parallelized-treesort.hpp"

// some structs that having the parameters (algorithms/data) with a given name

const int RANDOM_SEED = 20211011;
const std::string RESULT_DIR = "./result";

struct SortAlgorithm {
    std::string name;
    std::function<void(std::vector<int>&)> func;
};

struct DataRange {
    std::string name;
    std::function<std::vector<int>()> get_range;
};

struct DataGenerator {
    std::string name;
    std::function<std::vector<int>(std::size_t len)> generate_data;
};

std::vector<SortAlgorithm> sort_algorithms {
    SortAlgorithm{"std::sort", [](std::vector<int>& arr) {
        std::sort(arr.begin(), arr.end());
    } },
    SortAlgorithm{"std::stable_sort", [](std::vector<int>& arr) {
        std::stable_sort(arr.begin(), arr.end());
    } },
    
    
    
    // SortAlgorithm{"parallelized-mofidifed-timsort (1 thread)", [](std::vector<int>& arr) { 
        // omp_set_num_threads(1); 
        // parallelized_modified_timsort(arr.data(), arr.data() + arr.size()); 
    // }}, 
    // SortAlgorithm{"parallelized-mofidifed-timsort (2 threads)", [](std::vector<int>& arr) { 
        // omp_set_num_threads(2); 
        // parallelized_modified_timsort(arr.data(), arr.data() + arr.size()); 
    // }}, 
    // SortAlgorithm{"parallelized-mofidifed-timsort (3 threads)", [](std::vector<int>& arr) { 
        // omp_set_num_threads(3); 
        // parallelized_modified_timsort(arr.data(), arr.data() + arr.size()); 
    // }}, 
    SortAlgorithm{"parallelized-mofidifed-timsort", [](std::vector<int>& arr) {
        // omp_set_num_threads(4); 
        parallelized_modified_timsort(arr.data(), arr.data() + arr.size());
    }},
    
    // SortAlgorithm{"parallelized-treesort (1 thread)", [](std::vector<int>& arr) { 
        // omp_set_num_threads(1); 
        // randomized_parallelized_treesort(arr.data(), arr.data() + arr.size()); 
    // }}, 
    // SortAlgorithm{"parallelized-treesort (2 threads)", [](std::vector<int>& arr) { 
        // omp_set_num_threads(2); 
        // randomized_parallelized_treesort(arr.data(), arr.data() + arr.size()); 
    // }}, 
    // SortAlgorithm{"parallelized-treesort (3 threads)", [](std::vector<int>& arr) { 
        // omp_set_num_threads(3); 
        // randomized_parallelized_treesort(arr.data(), arr.data() + arr.size()); 
    // }}, 
    SortAlgorithm{"parallelized-treesort", [](std::vector<int>& arr) {
        // omp_set_num_threads(4); 
        randomized_parallelized_treesort(arr.data(), arr.data() + arr.size());
    }},
};

std::vector<DataRange> data_ranges = {
    DataRange{"small-range", [] {  
        std::vector<int> res;  
        for (int i = 0; i <= 300; ++i) res.push_back(i);  
        return res;  
    }},  
    DataRange{"medium-range", [] {    
        std::vector<int> res;    
        for (int i = (int)1e3; i <= (int)1e5; i += 198) {    
            res.push_back(i);    
        }    
        return res;    
    }},    
    // DataRange{"large-range", [] {     
        // for (int i = (int)1e6; i <= (int)1e7; i += 500000) { 
            // res.push_back(i);     
        // }     
        // return res;     
    // }},      
    // DataRange{"one-big-n", [] { return std::vector<int>(5, 10000000); }}, 
};

std::mt19937 rng(RANDOM_SEED);

std::uniform_int_distribution<int> val_dist(
        std::numeric_limits<int>::min(),
        std::numeric_limits<int>::max());
std::vector<DataGenerator> data_generators {
    DataGenerator{"random", [](std::size_t n) { 
        std::vector<int> ans(n); 
        for (auto& i: ans) { 
            i = val_dist(rng); 
        } 
        return ans; 
    }}, 
    DataGenerator{"sorted", [](std::size_t n) -> std::vector<int> {  
        if (n == 0) return {};  
        std::uniform_int_distribution<int> diff_dist(0, 10);  
        std::vector<int> ans(n);  
        for (auto& i: ans) {  
            i = diff_dist(rng);  
        }  
        ans[0] = val_dist(rng);  
        for (std::size_t i = 1; i < n; ++i) {  
            ans[i] += ans[i - 1];  
        }  
        return ans;  
    }},  
    DataGenerator{"reversed-sorted", [](std::size_t n) { 
        auto ans = data_generators[1].generate_data(n);  // a little bit hack but acceptable. 
        std::reverse(ans.begin(), ans.end()); 
        return ans; 
    }}, 
    DataGenerator{"same-value", [](std::size_t n) { 
        return std::vector<int>(n, val_dist(rng)); 
    }}, 
};

template<class F, class T>
std::chrono::duration<double, std::milli> measure_time(const F& func, T& data) {
    auto t1 = std::chrono::high_resolution_clock::now();
    func(data);
    auto t2 = std::chrono::high_resolution_clock::now();
    return t2 - t1;
}


void test_print(const SortAlgorithm& algo, std::vector<int> data) {
    std::cout << algo.name << std::endl;
    std::cout << "\tInput data: " << std::endl;
    std::cout << "\t";
    for (auto i: data) std::cout << i << ' ' ;
    std::cout << std::endl;
    algo.func(data);
    std::cout << "\tOutput data: " << std::endl;
    std::cout << "\t";
    for (auto i: data) std::cout << i << ' ';
    bool sorted = std::is_sorted(data.begin(), data.end());
    std::cout << std::endl;
    std::cout << "\tIs sorted?: " << std::boolalpha << sorted << std::endl;
}

int main(int argc, char** args) {
    std::string base_name = "";
    if (argc > 1) {
        base_name = std::string(args[1]) + '-';
    }
    std::filesystem::create_directory(RESULT_DIR);
    for (auto& range_gen: data_ranges) {
        for (auto& generator: data_generators) {
            std::string output_filename = RESULT_DIR + "/" + base_name + range_gen.name + "-" + generator.name + ".csv";
            std::ofstream out(output_filename);
            out << "n";
            for (auto& algo: sort_algorithms) {
                out << "," << algo.name;
            }
            out << std::endl;
            std::cout << output_filename << std::endl;
            auto range = range_gen.get_range();
            for (auto n: range) {
                std::cout <<  output_filename << "; n = " << n << ": " << std::flush;
                out << n;
                auto org_seq = generator.generate_data(n);
                for (auto& algo: sort_algorithms) {
                    // std::cout << "\t\t" << algo.name; 
                    auto cpy_seq = org_seq;
                    auto run_time = measure_time(algo.func, cpy_seq);
                    bool sorted = std::is_sorted(cpy_seq.begin(), cpy_seq.end());
                    if (sorted) {
                        // std::cout << "OK!" << std::endl; 
                        out << "," <<run_time.count();
                        continue;
                    }
                    std::cout << "FAILED!" << std::endl;
                    std::ofstream data_out("data.txt");
                    for (auto i: org_seq) data_out << i << ' ';
                    data_out << std::endl;
                    for (auto i: cpy_seq) data_out << i << ' ';
                    exit(0);
                }
                out << std::endl;
                std::cout << "OK!" << std::endl;
            }
        }
    }
    return 0;
}
