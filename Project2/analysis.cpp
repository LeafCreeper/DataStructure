#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <utility>
#include <sstream>
#include <random>
#include <algorithm>
#include <numeric>

#include "avl_tree.h"
#include "rb_tree.h"

// 解析一行数据，格式为 "key value"
std::pair<std::string, std::string> parse_line(const std::string& line) {
    std::stringstream ss(line);
    std::string key, value;
    ss >> key;
    std::getline(ss, value);
    if (!value.empty() && value[0] == ' ') {
        value.erase(0, 1);
    }
    return {key, value};
}

// 读取文件中的所有单词（用于插入/删除）
std::vector<std::pair<std::string, std::string>> read_words_for_insert(const std::string& filename) {
    std::vector<std::pair<std::string, std::string>> words;
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return words;
    }
    std::string line;
    std::getline(file, line); // Skip header
    while (std::getline(file, line)) {
        if (!line.empty()) {
            words.push_back(parse_line(line));
        }
    }
    return words;
}

std::vector<std::string> read_words_for_delete(const std::string& filename) {
    std::vector<std::string> words;
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return words;
    }
    std::string line;
    std::getline(file, line); // Skip header
    while (std::getline(file, line)) {
        if (!line.empty()) {
            words.push_back(line);
        }
    }
    return words;
}


// 模板函数：执行批量操作并记录性能
template<typename TreeType>
void perform_bulk_operation(TreeType& tree, const std::string& tree_name, const std::string& operation, const std::string& filename, std::ofstream& csv_file) {
    std::cout << "--- Analyzing " << operation << " for " << tree_name << " from " << filename << " ---" << std::endl;
    
    if (operation == "insert" || operation == "init") {
        auto words = read_words_for_insert(filename);
        if (words.empty()) return;

        size_t total_ops = 0;
        auto batch_start_time = std::chrono::high_resolution_clock::now();

        for (size_t i = 0; i < words.size(); ++i) {
            tree.insert(words[i].first, words[i].second);
            total_ops++;

            if ((i + 1) % 100 == 0) {
                auto batch_end_time = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::microseconds>(batch_end_time - batch_start_time);
                csv_file << tree_name << "," << operation << "," << total_ops << "," << duration.count() << "\n";
                std::cout << "  Processed " << total_ops << " words. Last 100 took " << duration.count() << " us." << std::endl;
                batch_start_time = std::chrono::high_resolution_clock::now();
            }
        }
    } else if (operation == "delete") {
        auto words = read_words_for_delete(filename);
        if (words.empty()) return;

        size_t total_ops = 0;
        auto batch_start_time = std::chrono::high_resolution_clock::now();

        for (size_t i = 0; i < words.size(); ++i) {
            tree.remove(words[i]);
            total_ops++;

            if ((i + 1) % 100 == 0) {
                auto batch_end_time = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::microseconds>(batch_end_time - batch_start_time);
                csv_file << tree_name << "," << operation << "," << total_ops << "," << duration.count() << "\n";
                std::cout << "  Processed " << total_ops << " words. Last 100 took " << duration.count() << " us." << std::endl;
                batch_start_time = std::chrono::high_resolution_clock::now();
            }
        }
    }
     std::cout << "--- Analysis complete for " << operation << " on " << tree_name << " ---" << std::endl << std::endl;
}

// 函数：执行搜索性能分析
template<typename TreeType>
void perform_search_analysis(TreeType& tree, const std::string& tree_name, const std::vector<std::pair<std::string, std::string>>& all_words, const std::vector<int>& N_values, std::ofstream& csv_file) {
    std::cout << "--- Analyzing Search for " << tree_name << " ---" << std::endl;
    std::vector<std::string> search_keys;
    for(const auto& p : all_words) {
        search_keys.push_back(p.first);
    }

    std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());

    for (int N : N_values) {
        if (N > search_keys.size()) continue;

        // 随机抽样N个词
        std::vector<std::string> sampled_keys;
        std::sample(search_keys.begin(), search_keys.end(), std::back_inserter(sampled_keys), N, rng);

        auto start_time = std::chrono::high_resolution_clock::now();
        for (const auto& key : sampled_keys) {
            tree.search(key);
        }
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

        csv_file << tree_name << ",search," << N << "," << duration.count() << "\n";
        std::cout << "  Searching for " << N << " random words took " << duration.count() << " us." << std::endl;
    }
    std::cout << "--- Search analysis complete for " << tree_name << " ---" << std::endl << std::endl;
}


int main() {
    std::ofstream csv_file("performance_analysis_random.csv");
    if (!csv_file.is_open()) {
        std::cerr << "Error: Could not open performance_analysis.csv for writing." << std::endl;
        return 1;
    }

    // 写入CSV文件头
    csv_file << "TreeType,Operation,ItemsProcessed,Time_microseconds\n";

    // --- 1. 初始化分析 ---
    avlTree avl_init;
    rbTree rbt_init;
    // perform_bulk_operation(avl_init, "AVL", "init", "init.txt", csv_file);
    // perform_bulk_operation(rbt_init, "Red-Black", "init", "init.txt", csv_file);

    perform_bulk_operation(avl_init, "AVL", "init", "init_random.txt", csv_file);
    perform_bulk_operation(rbt_init, "Red-Black", "init", "init_random.txt", csv_file);
    // 这两行是测试随机化时用的

    // --- 2. 删除分析 ---
    // 注意：这里的删除是基于上面初始化后的树
    perform_bulk_operation(avl_init, "AVL", "delete", "delete.txt", csv_file);
    perform_bulk_operation(rbt_init, "Red-Black", "delete", "delete.txt", csv_file);

    // --- 3. 插入分析 ---
    // 注意：这里的插入是基于删除操作后的树
    perform_bulk_operation(avl_init, "AVL", "insert", "insert.txt", csv_file);
    perform_bulk_operation(rbt_init, "Red-Black", "insert", "insert.txt", csv_file);

    // --- 4. 查询分析 ---
    // 使用一个全新的、完整的树进行查询性能测试，以保证查询的公平性
    std::cout << "\n--- Re-initializing trees for fair search analysis ---" << std::endl;
    avlTree avl_search_tree;
    rbTree rbt_search_tree;
    auto all_words = read_words_for_insert("init.txt");
    for(const auto& p : all_words) {
        avl_search_tree.insert(p.first, p.second);
        rbt_search_tree.insert(p.first, p.second);
    }
    std::cout << "--- Initialization for search complete ---" << std::endl << std::endl;

    std::vector<int> N_values = {1, 5, 10, 30, 50, 70, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};
    perform_search_analysis(avl_search_tree, "AVL", all_words, N_values, csv_file);
    perform_search_analysis(rbt_search_tree, "Red-Black", all_words, N_values, csv_file);


    csv_file.close();
    std::cout << "\nPerformance analysis complete. Results saved to performance_analysis.csv" << std::endl;

    return 0;
}
