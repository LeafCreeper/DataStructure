// Before we implement the matrix multiplication, we need to implement a efficient matrix class first
// which supports matrix partitioning without cloning the matrix memory

#pragma once
#include <memory>
#include <cassert>
#include <cstring>
#include <algorithm>

template<typename T>
class Matrix{
    private:
        std::shared_ptr<T[]> data_; // smart prt to continous mem
        size_t rows_;
        size_t cols_;
        size_t ld_; // how many unit do two rows in the matrix seperate, for partitioning use
        size_t offset_;

    public:
        Matrix(): data_(nullptr), rows_(0), cols_(0), ld_(0), offset_(0){}
        
        Matrix(size_t rows, size_t cols)
            : data_(new T[rows*cols](), std::default_delete<T[]>()),
            rows_(rows), cols_(cols), ld_(cols), offset_(0){}

        Matrix(std::shared_ptr<T[]> data,
               size_t rows, size_t cols, size_t ld, size_t offset)
            : data_(std::move(data)),rows_(rows),cols_(cols),ld_(ld),offset_(offset){}

        inline T& operator()(size_t i, size_t j) noexcept{
            assert(i < rows_ && j < cols_);
            return data_.get()[offset_ + i * ld_ + j];
        }
        

        inline const T& operator()(size_t i, size_t j) const noexcept {
           assert(i < rows_ && j < cols_);
           return data_.get()[offset_ + i * ld_ + j];
        }

        size_t rows() const noexcept{ return rows_; }
        size_t cols() const noexcept{ return cols_; }
        size_t ld()   const noexcept { return ld_; }

        T* data() noexcept { return data_.get() + offset_;}
        const T* data() const noexcept { return data_.get() + offset_;}

        // 用固定值填满矩阵
        void fill(const T& value){
            for(size_t i = 0; i < rows_; ++i){
                std::fill_n(data_.get() + offset_ + i*ld_, cols_, value);
            }
        }

        // 从另一个同尺寸的矩阵里复制数据
        void copy_from(const Matrix<T>& other){
            assert(rows_ == other.rows_ && cols_ == other.cols_);
            for(size_t i = 0; i < rows_; ++i){
                std::memcpy(data_.get() + offset_ + i*ld_,
                            other.data_.get() + other.offset_ + i*other.ld_,
                            cols_*sizeof(T));
            }
        }

        // 子矩阵
        Matrix SubMatrix(size_t i0, size_t j0, size_t h, size_t w) const {
            assert(i0 + h <= rows_ && j0 + w <= cols_);
            return Matrix(data_, h, w, ld_, offset_ + i0 * ld_ + j0);
        }

        // 重载加减运算符
        inline Matrix operator+(const Matrix& other) const{
            assert(rows_ == other.rows_ && cols_ == other.cols_);
            Matrix result(rows_, cols_);

            for(size_t i = 0; i < rows_; ++i){
                for(size_t j = 0; j < cols_; ++j){
                    result(i,j) = (*this)(i,j) + other(i,j);
                }
            }
            return result;
        }

        inline Matrix operator-(const Matrix& other) const{
            assert(rows_ == other.rows_ && cols_ == other.cols_);
            Matrix result(rows_, cols_);

            for(size_t i = 0; i < rows_; ++i){
                for(size_t j = 0; j < cols_; ++j){
                    result(i,j) = (*this)(i,j) - other(i,j);
                }
            }
            return result;
        }

        // 判断当前矩阵是否拥有独立内存（非子矩阵）
        bool owns_memory() const noexcept {
            return offset_ == 0 && ld_ == cols_;
        }
};