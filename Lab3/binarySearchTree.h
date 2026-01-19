#include <iostream>

template <typename T>
class binarySearchTree
{
private:
    struct treeNode
    {
        T val;
        treeNode* left;
        treeNode* right;

        treeNode(const T& v): val(v), left(nullptr), right(nullptr){}
    };

    treeNode* root = nullptr;

    void deleteTree(treeNode* node){
        if(node != nullptr){
            deleteTree(node->left);
            deleteTree(node->right);
            delete node;
        }
    }

    
public:
    binarySearchTree(): root(nullptr){}
    ~binarySearchTree(){ deleteTree(root); }

    void insert(const T& k){
        if(root == nullptr){
            root = new treeNode(k);
            return;
        }

        treeNode* parent=nullptr;
        treeNode* son = root;
        
        while (son != nullptr)
        {
            parent = son;
            if(k < parent->val)son = parent->left;
            else son = parent->right;
        }

        treeNode* newNode = new treeNode(k);
        if (k < parent->val) parent->left = newNode;
        else parent->right = newNode;

        return;
    }

    treeNode* search(const T& k){
        // 迭代版本
        treeNode* current = root;
        while(current != nullptr && current->val != k){
            if(k < current->val)current = current->left;
            else current = current->right;
        }
        return current;
    }

    treeNode* search_recursion(treeNode* x, const T& k){
        // 递归版本
        if(x == nullptr)return nullptr;
        if(x->val == k)return x;

        if(k < x->val)return search_recursion(x->left,k);
        else return search_recursion(x->right,k);

    }

    treeNode* tree_minimum(treeNode* node = nullptr){
        if(node == nullptr) return nullptr;
        treeNode* current = node;
        while(current->left) current = current->left;
        return current;
    }

    treeNode* tree_maximum(treeNode* node = nullptr){
        if(node == nullptr) return nullptr;
        treeNode* current = node;
        while(current->right) current = current->right;
        return current;
    }

    treeNode* predecessor(treeNode* node){
        if (!node) return nullptr;

        if(node->left){
            node = node->left;
            while(node->right)node = node->right;
            return node;
        }
        else{
            treeNode* current = root;
            treeNode* predecessor = nullptr;

            T k = node->val;
            if (!search(k)) return nullptr; //如果不加这一行，即使不存在也会返回潜在predecessor

            while(current && current->val != k){
                if(k > current->val){
                    predecessor = current;
                    current = current->right;
                }else current = current->left;
            }

            return predecessor;
        }
    }

    treeNode* successor(treeNode* node){
        if (!node) return nullptr;
        

        if(node->right){
            node = node->right;
            while(node->left)node = node->left;
            return node;
        }
        else{
            treeNode* current = root;
            treeNode* successor = nullptr;

            T k = node->val;
            if (!search(k)) return nullptr; //如果不加这一行，即使不存在也会返回潜在predecessor

            while(current && current->val != k){
                if(k < current->val){
                    successor = current;
                    current = current->left;
                }else current = current->right;
            }

            return successor;
        }
    }

    void inOrderWalk(treeNode* current){
        if(current){
            inOrderWalk(current->left);
            std::cout << current->val << ' ';
            inOrderWalk(current->right);
        }
    }

    void preOrderWalk(treeNode* current){
        if(current){
            std::cout << current->val << ' ';
            preOrderWalk(current->left);
            preOrderWalk(current->right);
        }
    }

    void postOrderWalk(treeNode* current){
        if(current){
            postOrderWalk(current->left);
            postOrderWalk(current->right);
            std::cout << current->val << ' ';
        }
    }


    // void remove(const T& x){
    //     treeNode* parent = nullptr;
    //     treeNode* current = root;
    //     while(current != nullptr || current->val == x){
    //         parent = current;
    //         if(x < current->val)current = current->left;
    //         else current = current->right;
    //     }
    //     if(current == nullptr)return;

    //     int n_sons = (current->left != nullptr) + (current->right != nullptr);

    //     switch (n_sons){
    //         case 0:
    //             if(current == root)root = nullptr;
    //             else{
    //                 if(parent.left == current)parent.left = nullptr;
    //                 else parent.right = nullptr;
    //             }
    //             delete current;

    //             break;
            
    //         case 1:
    //             if(current == root) {
    //                 root = (current->left) ? current->left : current->right;
    //                 delete current;
    //                 break;
    //             }

    //             treeNode* child = (current->left) ? current->left : current->right;
                
    //             if(parent->left == current) {
    //                 parent->left = child;
    //             } else {
    //                 parent->right = child;
    //             }
                
    //             delete current;
    //             break;

    //         case 2:
                



    //     }

};