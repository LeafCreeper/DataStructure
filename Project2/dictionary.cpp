#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility> 
#include <sstream> 
#include <limits> 

#include "avl_tree.h"
#include "rb_tree.h"

// 使用模板函数来处理两种树的共性操作
template<typename TreeType>
void process_file(TreeType& tree, const std::string& filename, const std::string& operation_type);

// 解析一行数据，格式为 "key value"
std::pair<std::string, std::string> parse_line(const std::string& line);

// 打印范围查询的结果
void print_range_results(const std::vector<std::pair<std::string, std::string>>& results);

void show_menu();

int main() {
    avlTree avl;
    rbTree rbt;

    // 初始化
    std::cout << "Step 1: Initializing trees with init.txt" << std::endl;
    process_file(avl, "init.txt", "insert");
    avl.preorderPrint("avlt.txt");

    process_file(rbt, "init.txt", "insert");
    rbt.preorderPrint("rbt.txt");

    // 删除
    std::cout << "\nStep 2: Deleting words from delete.txt" << std::endl;
    process_file(avl, "delete.txt", "delete");
    avl.preorderPrint("avlt.txt");

    process_file(rbt, "delete.txt", "delete");
    rbt.preorderPrint("rbt.txt");

    // 插入
    std::cout << "\nStep 3: Inserting new words from insert.txt" << std::endl;
    process_file(avl, "insert.txt", "insert");
    avl.preorderPrint("avlt.txt");

    process_file(rbt, "insert.txt", "insert");
    rbt.preorderPrint("rbt.txt");


    // 用户查询 
    char choice;
    do {
        show_menu();
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // 清除缓冲区

        if (choice == '1') {
            std::cout << "Enter word to search: ";
            std::string word;
            std::getline(std::cin, word);

            // AVL Tree Search
            std::string meaning_avl = avl.search(word);
            std::cout << "\n[AVL Tree] Result: " << (meaning_avl.empty() ? "Not Found" : meaning_avl) << std::endl;

            // Red-Black Tree Search
            std::string meaning_rbt = rbt.search(word);
            std::cout << "\n[Red-Black Tree] Result: " << (meaning_rbt.empty() ? "Not Found" : meaning_rbt) << std::endl;

        } else if (choice == '2') {
            std::string start_key, end_key;
            std::cout << "Enter start word of range: ";
            std::getline(std::cin, start_key);
            std::cout << "Enter end word of range: ";
            std::getline(std::cin, end_key);

            // AVL Tree Range Search
            auto results_avl = avl.searchRange(start_key, end_key);
            std::cout << "\n[AVL Tree] Found " << results_avl.size() << " results:" << std::endl;
            print_range_results(results_avl);

            // Red-Black Tree Range Search
            auto results_rbt = rbt.searchRange(start_key, end_key);
            std::cout << "\n[Red-Black Tree] Found " << results_rbt.size() << " results:" << std::endl;
            print_range_results(results_rbt);

        } else if (choice != '3') {
            std::cout << "Invalid choice. Please try again." << std::endl;
        }
        std::cout << "-------------------------------------------------" << std::endl;
    } while (choice != '3');

    std::cout << "Exiting program." << std::endl;

    return 0;
}

void show_menu() {
    std::cout << "\n--- MyDict Menu ---" << std::endl;
    std::cout << "1. Search for a single word" << std::endl;
    std::cout << "2. Search for words in a range" << std::endl;
    std::cout << "3. Exit" << std::endl;
    std::cout << "Enter your choice: ";
}

std::pair<std::string, std::string> parse_line(const std::string& line) {
    std::stringstream ss(line);
    std::string key, value;
    ss >> key;
    // 读取剩余部分作为 value
    std::getline(ss, value);
    // 去除 value 前面的空格
    if (!value.empty() && value[0] == ' ') {
        value.erase(0, 1);
    }
    return {key, value};
}

template<typename TreeType>
void process_file(TreeType& tree, const std::string& filename, const std::string& operation_type) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return;
    }

    std::string line;
    // 跳过第一行标题 (e.g., "INSERT" or "DELETE")
    if (!std::getline(file, line)) {
        return; // 文件为空或读取失败
    }

    std::vector<std::pair<std::string, std::string>> data_to_insert;
    std::vector<std::string> data_to_delete;

    // 先将文件内容读入内存，以更精确地计时
    if (operation_type == "insert") {
        while (std::getline(file, line)) {
            if (!line.empty()) {
                data_to_insert.push_back(parse_line(line));
            }
        }
    } else { // delete
        while (std::getline(file, line)) {
            if (!line.empty()) {
                data_to_delete.push_back(line);
            }
        }
    }
    file.close();

    size_t total_ops = (operation_type == "insert") ? data_to_insert.size() : data_to_delete.size();
    if (total_ops == 0) {
        std::cout << "No data to process in " << filename << std::endl;
        return;
    }

    std::cout << "Processing " << total_ops << " " << operation_type << " operations..." << std::endl;

    for (size_t i = 0; i < total_ops; ++i) {
        if (operation_type == "insert") {
            tree.insert(data_to_insert[i].first, data_to_insert[i].second);
        } else {
            tree.remove(data_to_delete[i]);
        }
    }

    std::cout << "Finished processing " << total_ops << " operations." << std::endl;
}

void print_range_results(const std::vector<std::pair<std::string, std::string>>& results) {
    if (results.empty()) {
        std::cout << "  (No words found in this range)" << std::endl;
        return;
    }
    for (const auto& p : results) {
        std::cout << "  " << p.first << ": " << p.second << std::endl;
    }
}