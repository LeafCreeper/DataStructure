#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include "stack.h"

using std::vector, std::string;

vector<string> parse(string expr){
    vector<string> result;
    string num;
    string word;

    for(char c : expr){
        // 如果是数字，连续处理
        if(isdigit(c)){
            num+=c;
            continue;
        }

        if(!isdigit(c) && num.size() != 0){
            result.push_back(num);
            num.clear();
        }

        if(isspace(c))continue;

        if(isalpha(c)){
            word += c;
            continue;
        }

        if(!isalpha(c) && word.size() != 0){
            result.push_back(word);
            word.clear();
        }
        
        result.push_back(string(1,c));
    }

    if(!num.empty())result.push_back(num);
    if(!word.empty()) result.push_back(word);


    return result;
}

bool isNum(const std::string& op) {
    static const std::regex pattern(R"(^[+-]?\d+$)");
    return std::regex_match(op, pattern);
}


bool isOperator(string op){
    return op == "+" || op == "-" || op == "*" || op == "/" || op == "mod";
}

int getOpPriority(string op){
    if(!isOperator(op))throw(std::invalid_argument("Not a Operator!!!"));
    if (op == "+" || op == "-") return 0;
    if (op == "*" || op == "/" || op == "mod") return 1;
    return -1;
}

int classify(string x){
    if(isNum(x))return 0;
    if(isOperator(x))return 1;
    if(x == "(")return 2;
    if(x == ")")return 3; 
    return -1;
}

// Conversion rules 
// Operands are immediately placed onto the output and operators are placed onto the stack . 
// When the operator on the top of the stack that has  higher priority than the input operator, it is popped. 
// If we see a right parenthesis, then we pop the stack,  writing symbols until we encounter a left parenthesis.  
// If we read the end of input, we pop the stack until it is  empty.

vector<string> infix2postfix(vector<string> expr_infix){
    Stack<string> operators = Stack<string>(100);
    vector<string> output;

    for(string token : expr_infix){
        switch (classify(token))
        {
        case 0: // 数字
            output.push_back(token);
            break;
        
        case 1: // 操作符
            while (!operators.isEmpty() && isOperator(operators.readTop()) &&
                getOpPriority(operators.readTop()) >= getOpPriority(token)) {
                output.push_back(operators.pop());
            }
            operators.push(token);
            break;


        case 2: // 左括号
            operators.push(token);
            break;

        case 3: // 右括号
            while(operators.readTop()!="("){
                output.push_back(operators.pop());
            }
            operators.pop();
            break;
        }
    }

    while(!operators.isEmpty()){
        output.push_back(operators.pop());
    }
    
    return output;
}

int postfix2result(vector<string> expr_postfix){
    Stack<int> numbers = Stack<int>(100);
    for(string token : expr_postfix){
        if(isNum(token)){
            numbers.push(std::stoi(token));
            continue;
        }
        
        if(token == "+"){
            numbers.push(numbers.pop() + numbers.pop());
            continue;
        }

        if(token == "-"){
            int b = numbers.pop();
            int a = numbers.pop();
            numbers.push(a-b);
            continue;
        }

        if(token == "*"){
            numbers.push(numbers.pop() * numbers.pop());
            continue;
        }
        
        if(token == "/"){
            int b = numbers.pop();
            int a = numbers.pop();
            if(b == 0) throw std::runtime_error("Division by zero!");
            numbers.push(a/b);
            continue;
        }

        if(token == "mod"){
            int b = numbers.pop();
            int a = numbers.pop();
            if(b == 0) throw std::runtime_error("Mod by zero!");
            numbers.push(a%b);
            continue;
        }
    }

    if(numbers.getCurrentSize()==1){
        return numbers.pop();
    }else throw("invalid expression!");
}

int main() {
    string expr = "(3 + 5) * (6 - 2) / 2 mod ( 100 mod 13 * 7)";
    vector<string> infix = parse(expr);
    vector<string> postfix = infix2postfix(infix);

    std::cout << "Postfix: ";
    for (auto& t : postfix) std::cout << t << " ";
    std::cout << "\nResult: " << postfix2result(postfix) << std::endl;
}

