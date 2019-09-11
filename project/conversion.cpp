#include "conversion.h"
#include <QObject>
#include <QGuiApplication>
#include <QtMath>
#include <QDir>
#include <iostream>
#include "math.h"
#include <stack>
#include <map>
#include <string>

using namespace std;

Conversion::Conversion()
{

}

/**
 * @brief Conversion::infixToPostfix converts infix string to postfix
 * @param expression entered expression in infix form
 * @return string transformed to postfix
 */
string Conversion::infixToPostfix(string expression)
{
    stack<char> stack1;
    string postfix = "";
    map<char, int> precedence = { {'+',1}, {'-',1}, {'*',2}, {'/',2}, {'^',3}, {'S',4}, {'C',4}, {'T',4}, {'K',4}, {'L',4}, {'Q',4} };
    for (unsigned i = 0; i < expression.size(); i++)
    {
        if(isalpha(expression[i]) && precedence.find(expression[i]) == precedence.end() && expression[i] != 'x' && expression[i] != 'y' && expression[i] != 'z'){
            return "";
        }
        if (isdigit(expression[i]) || (expression[i] == '.') || (expression[i] == '-' && (i == 0 || expression[i - 1] == '(')) || (expression[i] =='x') || (expression[i]=='y') ||(expression[i]=='z')){
            postfix += expression[i];
        } else if (expression[i] == '(') {
            stack1.push(expression[i]);
        } else if (expression[i] == ')') {
            postfix += " ";
            while (!stack1.empty())
            {
                if (stack1.top() == '(')
                {
                    stack1.pop();
                    break;
                }else{
                    postfix += stack1.top();
                    stack1.pop();
                }
            }
        } else if (!(precedence.find(expression[i]) == precedence.end())) {
            postfix += " ";
            while (!stack1.empty())
            {
                if (stack1.top() != '(' &&  precedence.at(expression[i]) <= precedence.at(stack1.top()) && (precedence.at(expression[i]) != '^' || precedence.at(stack1.top()) != '^'))
                {
                    postfix = postfix + stack1.top() + " ";
                    stack1.pop();
                }else {
                    stack1.push(expression[i]);
                    break;
                }
            }
            if (stack1.empty())
            {
                stack1.push(expression[i]);
            }
        }
    }
    while (!stack1.empty())
    {
        postfix = postfix + " " + stack1.top();
        stack1.pop();
    }
    return postfix;
}

/**
 * @brief Conversion::replaceSymbol checks expression and replaces passed symbol (x,y or z) with passed number
 *
 * @param exp expression to check
 * @param sy symbol which will be replaced
 * @param num number which will replace the symbol
 * @return transformed expression string type
 */
string Conversion::replaceSymbol(string exp, string sy, double num)
{
    string newString = "";
    for (unsigned i = 0; i < exp.length(); i++){
        std::string s(1,exp[i]);
        if (s == sy) {
            if (num >= 0) {
                newString += to_string(num);
            }
            else {
                newString += "(" + to_string(num) + ")";
            }
        }
        else{
            newString += s;
        }
    }
    return newString;
}

/**
 * @brief Conversion::PerformBinary performs binary operation based on sent char operator
 * @param a first operand
 * @param b second operand
 * @param op operator to perform
 * @return result of performed operation type double
 */
double Conversion::PerformBinary(double a, double b, char op)
{
    if (op == '+'){
        return (double)(a + b);
    }else if (op == '-'){
        return (double)(a - b);
    }else if (op == '*'){
        return (double)(a * b);
    }else if (op == '/'){
        return (double)(a / b);
    }else if (op == '^'){
        return (double)(pow(a, b));
    }
}

/**
 * @brief Conversion::PerformUnary performs unary operation based on sent operator
 * @param a operand
 * @param op operator to perform
 * @return result of performed operation type double
 */
double Conversion::PerformUnary(double a, char op)
{
    if (op == 'S') {
        return sin(a);
    }else if (op == 'C'){
        return cos(a);
    }else if (op == 'T'){
        return (double)(sin(a) / cos(a));
    }else if (op == 'K'){
        return (double)(sin(a) / cos(a));
    }else if (op == 'L'){
        return (double)log10(a);
    }else if (op == 'Q'){
        return sqrtl((double)a);
    }else if (op == 's'){
        return asin(a);
    }else if (op == 'c'){
        return acos(a);
    }
}

/**
 * @brief Conversion::SolveEquation takes postfix string as argument, calculates result of whole equation by calling side methods
 * @param eq sent postfix equation
 * @return result of equation type string
 */
double Conversion::SolveEquation(string eq)
{
    stack<double> stack1;
    string buff = "";
    double x1;
    double x2;
    double result = 0;
    double tmp;
    bool operator_found = false;
    map<char, int> operator_type = { {'S',1}, {'C',1}, {'T',1}, {'K',1}, {'L',1}, {'+',2}, {'-',2}, {'*',2}, {'/',2}, {'^',2} };
    for (unsigned i = 0; i < eq.length(); i++) {
        if (isdigit(eq[i]) || eq[i] == '.' || (eq[i] == '-' && isdigit(eq[i+1]))) {
            buff += eq[i];
        }else {
            if (!buff.empty()){
                stack1.push(stod(buff));
                buff = "";
            }
            if (eq[i] != ' ' && (!(operator_type.find(eq[i]) == operator_type.end()))) {
                operator_found = true;
                if (operator_type.at(eq[i]) == 1) {
                    x1 = stack1.top();
                    stack1.pop();
                    tmp = PerformUnary(x1, eq[i]);
                    stack1.push(tmp);
                }else {
                    x1 = stack1.top();
                    stack1.pop();
                    x2 = stack1.top();
                    stack1.pop();
                    tmp = PerformBinary(x2, x1, eq[i]);
                    stack1.push(tmp);
                }
            }
        }
    }
    if (operator_found == true)
    {
        result = stack1.top();
    }else {
        result = stod(eq);
    }
    return result;
}

/**
 * @brief Conversion::isWholeInputNumber checks if inputs like range and rate are valid numbers,
 * if there are only digits or x,y,z variables and if it contains a dot, checks if only one.
 * @param input entered expression, may be a number or equation
 * @return true if input number, false if equation
 */
bool Conversion::isWholeInputNumber(string input) {
    int point_counter = 0;
    for(unsigned i = 0; i < input.size(); i++) {
        if(input[i] == '.') {
            point_counter++;
        }
		if(isalpha(input[i]) && input[i] != 'x' && input[i] != 'y' && input[i] != 'z')
		{
			return false;
		}
        if( !(isdigit(input[i]) || (input[i] == '-' && i == 0) || (input[i] == '.' && i != input.size()-1 && point_counter < 2))) {
            return false;
        }
    }
    return true;
}

