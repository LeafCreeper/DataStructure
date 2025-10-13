#pragma once
#include <iostream>

template <typename T>
class Stack{
    private:
        size_t size;
        T* data;
        int top;
    public:
        Stack(size_t s): size(s), data(new T[size]), top(-1){}

        bool isFull() const {
            return top == static_cast<int>(size) - 1;
        }

        bool isEmpty() const {
            return top == -1;
        }

        void push(T x){
            if(isFull())throw(std::overflow_error("stack overflow!"));
            data[++top] = x;
            return;
        }

        T pop(){
            if(isEmpty())throw(std::overflow_error("stack underflow!"));
            return data[top--];
        }

        T readTop(){
            return data[top];
        }

        int getCurrentSize(){
            return top + 1;
        }
};

