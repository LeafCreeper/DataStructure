#pragma once
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <fstream>
#include <utility>

struct node {
    std::string key;
    std::string value;
    node* left;
    node* right;
    int height;

    node(std::string k, std::string v)
        : key(k), value(v), left(nullptr), right(nullptr), height(1){}

};

class avlTree {

private:
    node* root;


    node* searchNode(std::string k){
        node* x = root;
        while(x != nullptr && x->key != k){
            if(x->key < k){
                x = x->right;
            }else x = x->left;
        }
        return x;
    }

    int get_bf(node* x){
        if (x == nullptr) return 0;
        int lh = x->left ? x->left->height : 0;
        int rh = x->right ? x->right->height : 0;
        return lh - rh;
    }

    int get_height(node* x){
        if(x == nullptr) return 0;
        else return x->height;
    }

    // node* detect_unbalance(std::string k){
    //     node* x = root;
    //     int x_bf = get_bf(x);
    //     while(x_bf <= 1 && x_bf >= -1 && x->key != k && x != nullptr){
    //         x_bf = get_bf(x);
    //         if(x->key < k){
    //             x = x->right;
    //         }else x = x->left;
    //     }
    //     if(x == nullptr || x->key == k) return nullptr;
    //     return x; //返回第一个失衡的位置
    // }
    // 这个函数非常的愚蠢，完全不应该写，插入的过程中回溯地找就可以了

    enum RotateType {
        BALANCED = 0,
        LL = 1,
        LR = 2,
        RR = 3,
        RL = 4
    };


    RotateType get_unbalance_type_insert(node* x, const std::string& key){
        int bf = get_bf(x);

        if (bf <= 1 && bf >= -1) return BALANCED;

        if (bf > 1) {  // 左重
            return (key < x->left->key) ? LL : LR;
        } 
        else {        // 右重
            return (key > x->right->key) ? RR : RL;
        }
    }

    
    node* L_rotation(node* y){
        node* x = y->right;
        node* T = x->left;

        x->left = y;
        y->right = T;

        y->height = std::max(get_height(y->left), get_height(y->right)) + 1;
        x->height = std::max(get_height(x->left), get_height(x->right)) + 1;

        return x;
    }

    node* R_rotation(node* y){
        node* x = y->left;
        node* T = x->right;
        x->right = y;
        y->left = T;

        y->height = std::max(get_height(y->left), get_height(y->right)) + 1;
        x->height = std::max(get_height(x->left), get_height(x->right)) + 1;

        return x;
    }
        
    node* insertNode(node* x, const std::string& key, const std::string& value) {
        // 1. 普通 BST 插入
        if (x == nullptr) {
            return new node(key, value);
        }

        if (key < x->key) {
            x->left = insertNode(x->left, key, value);
        }
        else if (key > x->key) {
            x->right = insertNode(x->right, key, value);
        }
        else {
            // key 已存在 → 更新中文意思，modify一下
            x->value = value;
            return x;
        }

        // 2. 更新高度
        x->height = 1 + std::max(get_height(x->left), get_height(x->right));

        RotateType r = get_unbalance_type_insert(x, key);

        switch (r) {
            case LL:
                return R_rotation(x);

            case LR:
                x->left = L_rotation(x->left);
                return R_rotation(x);

            case RR:
                return L_rotation(x);

            case RL:
                x->right = R_rotation(x->right);
                return L_rotation(x);

            case BALANCED:
            default:
                return x;
        }
    }

    node* findMin(node* x) {
        while (x->left != nullptr) x = x->left;
        return x;
    }

    node* deleteNode(node* x, std::string k) {
        if (x == nullptr) return x;

        // 1. 标准BST删除
        if (k < x->key) {
            x->left = deleteNode(x->left, k);
        } else if (k > x->key) {
            x->right = deleteNode(x->right, k);
        } else {
            // 找到要删除的节点
            if (x->left == nullptr || x->right == nullptr) {
                node* temp = x->left ? x->left : x->right;
                if (temp == nullptr) { // 没有子节点
                    temp = x;
                    x = nullptr;
                } else { // 一个子节点
                    *x = *temp; 
                }
                delete temp;
            } else {
                // 两个子节点
                node* temp = findMin(x->right);
                x->key = temp->key;
                x->value = temp->value;
                x->right = deleteNode(x->right, temp->key);
            }
        }

        if (x == nullptr) return x;

        // 2. 更新高度
        x->height = 1 + std::max(get_height(x->left), get_height(x->right));

        // 3. 获取平衡因子并进行旋转
        int bf = get_bf(x);

        // 左重
        if (bf > 1) {
            if (get_bf(x->left) >= 0) { // LL
                return R_rotation(x);
            } else { // LR
                x->left = L_rotation(x->left);
                return R_rotation(x);
            }
        }
        // 右重
        if (bf < -1) {
            if (get_bf(x->right) <= 0) { // RR
                return L_rotation(x);
            } else { // RL
                x->right = R_rotation(x->right);
                return L_rotation(x);
            }
        }

        return x;
    }

    void destroyTree(node* x) {
        if (x) {
            destroyTree(x->left);
            destroyTree(x->right);
            delete x;
        }
    }

    
    void preorderPrintRecursive(std::ofstream& out, node* x, int level, int child) {
        out << "level=" << level << " child=" << child << " ";
        if (x != nullptr) {
            out << x->key << "(height=" << x->height << ")" << std::endl;
            preorderPrintRecursive(out, x->left, level + 1, 0);
            preorderPrintRecursive(out, x->right, level + 1, 1);
        } else {
            out << "null" << std::endl;
        }
    }

    void searchRangeRecursive(node* x, const std::string& startKey, const std::string& endKey, std::vector<std::pair<std::string, std::string>>& result) {
        if (x == nullptr) {
            return;
        }
        if (startKey < x->key) {
            searchRangeRecursive(x->left, startKey, endKey, result);
        }
        if (x->key >= startKey && x->key <= endKey) {
            result.push_back({x->key, x->value});
        }
        if (endKey > x->key) {
            searchRangeRecursive(x->right, startKey, endKey, result);
        }
    }


public:
    avlTree() : root(nullptr){};
    ~avlTree() {
        destroyTree(root);
    }

    void insert(const std::string& key, const std::string& value) {
        root = insertNode(root, key, value);
    }

    void remove(const std::string& key) {
        root = deleteNode(root, key);
    }

    std::string search(const std::string& key) {
        node* result = searchNode(key);
        if (result) {
            return result->value;
        }
        return ""; // 或者抛出异常
    }

    
    void preorderPrint(const std::string& filename) {
        std::ofstream outFile(filename);
        if (!outFile.is_open()) {
            std::cerr << "Error opening file for writing: " << filename << std::endl;
            return;
        }
        if (root != nullptr) {
            outFile << "level=0 child=0 " << root->key << "(height=" << root->height << ")" << std::endl;
            preorderPrintRecursive(outFile, root->left, 1, 0);
            preorderPrintRecursive(outFile, root->right, 1, 1);
        }
        outFile.close();
    }

    std::vector<std::pair<std::string, std::string>> searchRange(const std::string& startKey, const std::string& endKey) {
        std::vector<std::pair<std::string, std::string>> result;
        searchRangeRecursive(root, startKey, endKey, result);
        return result;
    }



};