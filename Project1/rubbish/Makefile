# 编译器
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Iinclude -Isrc

# 源文件和目标
SRC = src/main.cpp
TARGET = sorter

# 默认目标
all: $(TARGET)

# 链接生成可执行文件
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^

# 清理
clean:
	rm -f $(TARGET)
