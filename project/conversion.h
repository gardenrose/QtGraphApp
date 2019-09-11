#ifndef CONVERSION_H
#define CONVERSION_H

#include <QObject>
#include <QGuiApplication>
#include <QDir>
#include <iostream>
#include "math.h"
#include <stack>
#include <map>
#include <string>

using namespace std;
class Conversion : public QObject
{
    Q_OBJECT
public:
    Conversion();
    static string infixToPostfix(string expression);
    static string replaceSymbol(string exp, string sy, double num);
    static double PerformBinary(double a, double b, char op);
    static double PerformUnary(double a, char op);
    static double SolveEquation(string eq);
    static bool isWholeInputNumber(string input);

signals:

public slots:
};

#endif // CONVERSION_H
