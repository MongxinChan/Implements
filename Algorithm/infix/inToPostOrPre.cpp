#include <iostream>
#include <stack>
#include <string>
#include <algorithm>
#include <cctype>

using namespace std;

// 获取运算符优先级的辅助函数
int getPrecedence(char op) {
    if (op == '^') return 3;
    else if (op == '*' || op == '/') return 2;
    else if (op == '+' || op == '-') return 1;
    else return -1;
}

// 检查中缀表达式格式是否合法的函数
bool isValidInfix(const string& s) {
    if (s.empty()) return false;

    int parens = 0;               // 记录括号的平衡度
    bool lastWasOperator = false; // 记录上一个字符是否是运算符
    bool lastWasOpenParen = false;// 记录上一个字符是否是左括号

    for (int i = 0; i < s.length(); i++) {
        char c = s[i];

        bool isOperand = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
        bool isOperator = (c == '+' || c == '-' || c == '*' || c == '/' || c == '^');

        // 1. 检查非法字符
        if (!isOperand && !isOperator && c != '(' && c != ')') {
            cout << "格式错误：包含非法字符 '" << c << "'" << endl;
            return false; 
        }

        // 2. 检查括号匹配与空括号
        if (c == '(') {
            parens++;
            lastWasOpenParen = true;
            lastWasOperator = false;
        } else if (c == ')') {
            parens--;
            if (parens < 0) {
                cout << "格式错误：右括号过多或位置错误" << endl;
                return false; 
            }
            if (lastWasOpenParen) {
                cout << "格式错误：存在空括号 ()" << endl;
                return false; 
            }
            lastWasOperator = false;
            lastWasOpenParen = false;
        }

        // 3. 检查运算符的使用规则
        if (isOperator) {
            if (lastWasOperator) {
                cout << "格式错误：存在连续的运算符" << endl;
                return false; 
            }
            if (i == 0 || i == s.length() - 1) {
                cout << "格式错误：表达式不能以运算符开头或结尾" << endl;
                return false; 
            }
            if (lastWasOpenParen) {
                cout << "格式错误：左括号后不能紧跟运算符" << endl;
                return false; 
            }
            lastWasOperator = true;
            lastWasOpenParen = false;
        } else if (isOperand) {
            lastWasOperator = false;
            lastWasOpenParen = false;
        }
    }

    if (parens != 0) {
        cout << "格式错误：括号未闭合（左括号过多）" << endl;
        return false;
    }

    return true; // 所有检查通过
}

// 1. 中缀表达式转后缀表达式
string infixToPostfix(string s) {
    stack<char> st;
    string result;

    for (int i = 0; i < s.length(); i++) {
        char c = s[i];

        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')) {
            result += c;
        } else if (c == '(') {
            st.push('(');
        } else if (c == ')') {
            while (!st.empty() && st.top() != '(') {
                result += st.top();
                st.pop();
            }
            if (!st.empty()) st.pop(); 
        } else {
            while (!st.empty() && getPrecedence(st.top()) >= getPrecedence(c)) {
                result += st.top();
                st.pop();
            }
            st.push(c);
        }
    }

    while (!st.empty()) {
        result += st.top();
        st.pop();
    }
    return result;
}

// 2. 中缀表达式转前缀表达式
string infixToPrefix(string s) {
    reverse(s.begin(), s.end());

    for (int i = 0; i < s.length(); i++) {
        if (s[i] == '(') s[i] = ')';
        else if (s[i] == ')') s[i] = '(';
    }

    stack<char> st;
    string result;

    for (int i = 0; i < s.length(); i++) {
        char c = s[i];

        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')) {
            result += c;
        } else if (c == '(') {
            st.push('(');
        } else if (c == ')') {
            while (!st.empty() && st.top() != '(') {
                result += st.top();
                st.pop();
            }
            if (!st.empty()) st.pop();
        } else {
            while (!st.empty() && getPrecedence(st.top()) > getPrecedence(c)) {
                result += st.top();
                st.pop();
            }
            st.push(c);
        }
    }

    while (!st.empty()) {
        result += st.top();
        st.pop();
    }

    reverse(result.begin(), result.end());
    return result;
}

int main() {
    string infix_expr;
    
    // 使用 while 循环，如果输入错误可以让用户重新输入
    while (true) {
        cout << "请输入中缀表达式 (输入 'q' 退出): ";
        getline(cin, infix_expr);

        // 退出条件
        if (infix_expr == "q" || infix_expr == "Q") {
            cout << "程序退出。" << endl;
            break;
        }

        // 预处理：去除用户输入中可能包含的空格
        infix_expr.erase(remove_if(infix_expr.begin(), infix_expr.end(), ::isspace), infix_expr.end());

        // 如果用户只输入了空格，跳过
        if (infix_expr.empty()) continue;

        // 格式验证
        if (!isValidInfix(infix_expr)) {
            cout << "请检查您的表达式并重新输入！\n" << endl;
            continue; // 格式有问题，跳过本次转换，重新开始循环
        }

        // 格式正确，执行转换
        cout << "---------------------------------" << endl;
        cout << "有效的表达式: " << infix_expr << endl;
        
        string postfix_expr = infixToPostfix(infix_expr);
        cout << "后缀表达式:   " << postfix_expr << endl;
        
        string prefix_expr = infixToPrefix(infix_expr);
        cout << "前缀表达式:   " << prefix_expr << endl;
        cout << "---------------------------------\n" << endl;
    }

    return 0;
}