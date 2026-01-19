#pragma once
#include <string>
#include <iostream>
#include <algorithm>
#include <vector>
#include <fstream>
#include <utility>

enum Color { RED, BLACK };

struct rb_node {
    std::string key;
    std::string value;
    Color color;
    rb_node *left, *right, *parent;

    rb_node(std::string k, std::string v) : key(k), value(v), color(RED), left(nullptr), right(nullptr), parent(nullptr) {}
};

class rbTree {
private:
    rb_node *root;
    rb_node *NIL; // 哨兵节点

    void leftRotate(rb_node *x) {
        rb_node *y = x->right;
        x->right = y->left;
        if (y->left != NIL) {
            y->left->parent = x;
        }
        y->parent = x->parent;
        if (x->parent == NIL) {
            root = y;
        } else if (x == x->parent->left) {
            x->parent->left = y;
        } else {
            x->parent->right = y;
        }
        y->left = x;
        x->parent = y;
    }

    void rightRotate(rb_node *y) {
        rb_node *x = y->left;
        y->left = x->right;
        if (x->right != NIL) {
            x->right->parent = y;
        }
        x->parent = y->parent;
        if (y->parent == NIL) {
            root = x;
        } else if (y == y->parent->left) {
            y->parent->left = x;
        } else {
            y->parent->right = x;
        }
        x->right = y;
        y->parent = x;
    }

    void insertFixup(rb_node *z) {
        while (z->parent->color == RED) {
            if (z->parent == z->parent->parent->left) {
                rb_node *y = z->parent->parent->right; // Uncle
                if (y->color == RED) { // Case 1
                    z->parent->color = BLACK;
                    y->color = BLACK;
                    z->parent->parent->color = RED;
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->right) { // Case 2
                        z = z->parent;
                        leftRotate(z);
                    }
                    // Case 3
                    z->parent->color = BLACK;
                    z->parent->parent->color = RED;
                    rightRotate(z->parent->parent);
                }
            } else {
                rb_node *y = z->parent->parent->left; // Uncle
                if (y->color == RED) { // Case 1
                    z->parent->color = BLACK;
                    y->color = BLACK;
                    z->parent->parent->color = RED;
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->left) { // Case 2
                        z = z->parent;
                        rightRotate(z);
                    }
                    // Case 3
                    z->parent->color = BLACK;
                    z->parent->parent->color = RED;
                    leftRotate(z->parent->parent);
                }
            }
        }
        root->color = BLACK;
    }

    void rbTransplant(rb_node* u, rb_node* v) {
        if (u->parent == NIL) {
            root = v;
        } else if (u == u->parent->left) {
            u->parent->left = v;
        } else {
            u->parent->right = v;
        }
        v->parent = u->parent;
    }

    rb_node* treeMinimum(rb_node* x) {
        while (x->left != NIL) {
            x = x->left;
        }
        return x;
    }

    void deleteFixup(rb_node* x) {
        while (x != root && x->color == BLACK) {
            if (x == x->parent->left) {
                rb_node* w = x->parent->right; // Sibling
                if (w->color == RED) { // Case 1
                    w->color = BLACK;
                    x->parent->color = RED;
                    leftRotate(x->parent);
                    w = x->parent->right;
                }
                if (w->left->color == BLACK && w->right->color == BLACK) { // Case 2
                    w->color = RED;
                    x = x->parent;
                } else {
                    if (w->right->color == BLACK) { // Case 3
                        w->left->color = BLACK;
                        w->color = RED;
                        rightRotate(w);
                        w = x->parent->right;
                    }
                    // Case 4
                    w->color = x->parent->color;
                    x->parent->color = BLACK;
                    w->right->color = BLACK;
                    leftRotate(x->parent);
                    x = root;
                }
            } else {
                rb_node* w = x->parent->left; // Sibling
                if (w->color == RED) { // Case 1
                    w->color = BLACK;
                    x->parent->color = RED;
                    rightRotate(x->parent);
                    w = x->parent->left;
                }
                if (w->right->color == BLACK && w->left->color == BLACK) { // Case 2
                    w->color = RED;
                    x = x->parent;
                } else {
                    if (w->left->color == BLACK) { // Case 3
                        w->right->color = BLACK;
                        w->color = RED;
                        leftRotate(w);
                        w = x->parent->left;
                    }
                    // Case 4
                    w->color = x->parent->color;
                    x->parent->color = BLACK;
                    w->left->color = BLACK;
                    rightRotate(x->parent);
                    x = root;
                }
            }
        }
        x->color = BLACK;
    }

    rb_node* searchNode(const std::string& key) {
        rb_node* current = root;
        while (current != NIL && key != current->key) {
            if (key < current->key) {
                current = current->left;
            } else {
                current = current->right;
            }
        }
        return current;
    }

    void destroyTree(rb_node* node) {
        if (node != NIL) {
            destroyTree(node->left);
            destroyTree(node->right);
            delete node;
        }
    }

    
    void preorderPrintRecursive(std::ofstream& out, rb_node* x, int level, int child) {
        out << "level=" << level << " child=" << child << " ";
        if (x != NIL) {
            out << x->key << "(" << (x->color == RED ? "RED" : "BLACK") << ")" << std::endl;
            preorderPrintRecursive(out, x->left, level + 1, 0);
            preorderPrintRecursive(out, x->right, level + 1, 1);
        } else {
            out << "null" << std::endl;
        }
    }

    void searchRangeRecursive(rb_node* x, const std::string& startKey, const std::string& endKey, std::vector<std::pair<std::string, std::string>>& result) {
        if (x == NIL) {
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
    rbTree() {
        NIL = new rb_node("", "");
        NIL->color = BLACK;
        root = NIL;
    }

    ~rbTree() {
        destroyTree(root);
        delete NIL;
    }

    void insert(const std::string& key, const std::string& value) {
        rb_node* z = new rb_node(key, value);
        z->left = NIL;
        z->right = NIL;

        rb_node* y = NIL;
        rb_node* x = root;

        while (x != NIL) {
            y = x;
            if (z->key < x->key) {
                x = x->left;
            } else if (z->key > x->key) {
                x = x->right;
            } else {
                // Key already exists, update value and clean up
                x->value = value;
                delete z;
                return;
            }
        }

        z->parent = y;
        if (y == NIL) {
            root = z;
        } else if (z->key < y->key) {
            y->left = z;
        } else {
            y->right = z;
        }

        insertFixup(z);
    }

    void remove(const std::string& key) {
        rb_node* z = searchNode(key);
        if (z == NIL) {
            return; // Key not found
        }

        rb_node* y = z;
        rb_node* x;
        Color y_original_color = y->color;

        if (z->left == NIL) {
            x = z->right;
            rbTransplant(z, z->right);
        } else if (z->right == NIL) {
            x = z->left;
            rbTransplant(z, z->left);
        } else {
            y = treeMinimum(z->right);
            y_original_color = y->color;
            x = y->right;
            if (y->parent == z) {
                x->parent = y;
            } else {
                rbTransplant(y, y->right);
                y->right = z->right;
                y->right->parent = y;
            }
            rbTransplant(z, y);
            y->left = z->left;
            y->left->parent = y;
            y->color = z->color;
        }
        delete z;
        if (y_original_color == BLACK) {
            deleteFixup(x);
        }
    }

    std::string search(const std::string& key) {
        rb_node* result = searchNode(key);
        if (result != NIL) {
            return result->value;
        }
        return "";
    }


    void preorderPrint(const std::string& filename) {
        std::ofstream outFile(filename);
        if (!outFile.is_open()) {
            std::cerr << "Error opening file for writing: " << filename << std::endl;
            return;
        }
        if (root != NIL) {
            outFile << "level=0 child=0 " << root->key << "(" << (root->color == RED ? "RED" : "BLACK") << ")" << std::endl;
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