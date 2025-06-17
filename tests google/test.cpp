#include "pch.h"
#include <iostream>
#include <cmath>
#include <string>
#include <vector>
#include <ctime>
#include <gtest/gtest.h>

using namespace std;

bool exc = false;

// Объявления функций
double operations(char op, double a, double b, bool& error);
double readNumber(const string& expression, int& i, bool& error);
bool isOperator(char c);
double firstAnalis(const string& expression, int& i, bool& error);
double stepen(const string& expression, int& i, bool& error);
double mulDiv(const string& expression, int& i, bool& error);
double resultAnalis(const string& expression, int& i, bool& error);

double operations(char op, double a, double b, bool& error) {
    switch (op) {
    case '+': return a + b;
    case '-': return a - b;
    case '*': return a * b;
    case '/':
        if (b == 0) {
            cout << "Ошибка: деление на ноль\n";
            error = true;
            return 0;
        }
        return a / b;
    case '^': return pow(a, b);
    case '#':
        if (b < 0) {
            cout << "Ошибка: корень из отрицательного числа\n";
            error = true;
            return 0;
        }
        return sqrt(b);
    default:
        cout << "Ошибка: неизвестный оператор\n";
        error = true;
        return 0;
    }
}

void testApply() {
    struct test {
        char op;
        double a, b;
        double expectedResult;
        bool expectError;
    };

    vector<test> tests = {
        {'+', 3, 5, 8, false},
        {'-', 10, 4, 6, false},
        {'/', 9, 3, 3, false},
        {'/', 5, 0, 0, true},
        {'^', 2, 3, 8, false},
        {'#', 0, 16, 4, false},
        {'#', 0, -9, 0, true},
        {'$', 4, 2, 0, true},
    };

    int passed = 0;
    for (int i = 0; i < tests.size(); i++) {
        cout << "Тест #" << i + 1 << endl;
        cout << "Выражение:" << tests[i].a << tests[i].op << tests[i].b << endl;

        bool error = false;
        double result = operations(tests[i].op, tests[i].a, tests[i].b, error);
        cout << "Ожидаемый результат: " << tests[i].expectedResult << ", Ожидаемая ошибка: " << boolalpha << tests[i].expectError << endl;
        cout << "Фактический результат: " << result << ", Ошибка: " << boolalpha << error << endl;

        if (error == tests[i].expectError) {
            if (result == tests[i].expectedResult) {
                cout << "Тест пройден!" << endl;
                ++passed;
            }
        }
        else {
            cout << "Тест НЕ пройден!" << endl;
        }

        cout << "---------------------------------------------------------------------" << endl;
    }

    cout << "Успешных тестов: " << passed << " / " << tests.size() << endl;
}

double readNumber(const string& expression, int& i, bool& error) {
    string value;
    bool hasPoint = false;
    int start = i;

    if (expression[i] == '.' && (i + 1 >= expression.size() || !isdigit(expression[i + 1]))) {
        error = true;
        return 0;
    }

    while (i < expression.size() && (isdigit(expression[i]) || expression[i] == '.')) {
        if (expression[i] == '.') {
            if (hasPoint) {
                error = true;
                return 0;
            }
            hasPoint = true;
        }
        i++;
    }

    if (i > start && expression[i - 1] == '.') {
        error = true;
        return 0;
    }

    try {
        return stod(expression.substr(start, i - start));
    }
    catch (...) {
        error = true;
        return 0;
    }
}

bool isOperator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '^';
}

double firstAnalis(const string& expression, int& i, bool& error) {
    while (i < expression.length() && isspace(expression[i]))
        i++;

    bool negative = false;

    if (i < expression.size() && expression[i] == '-' &&
        (i == 0 || expression[i - 1] == '(' || isOperator(expression[i - 1]))) {
        negative = true;
        i++;
        while (i < expression.length() && isspace(expression[i]))
            i++;
    }

    if (i < expression.size() && expression[i] == '(') {
        i++;
        double value = resultAnalis(expression, i, error);
        if (i >= expression.size() || expression[i] != ')') {
            error = true;
            return 0;
        }
        i++;
        return negative ? -value : value;
    }
    else if (i < expression.size() && expression[i] == '#') {
        i++;
        double value = firstAnalis(expression, i, error);
        double result = operations('#', 0, value, error);
        return negative ? -result : result;
    }
    else if (i < expression.size() && (isdigit(expression[i]) || expression[i] == '.')) {
        return readNumber(expression, i, error);
    }
    else {
        error = true;
        return 0;
    }
}

double stepen(const string& expression, int& i, bool& error) {
    double left = firstAnalis(expression, i, error);

    if (error)
        return 0;

    while (i < expression.length()) {
        while (i < expression.length() && isspace(expression[i]))
            i++;

        if (i < expression.length() && expression[i] == '^') {
            i++;
            double right = stepen(expression, i, error);
            if (error)
                return 0;
            left = operations('^', left, right, error);
            if (error)
                return 0;
        }
        else break;
    }
    return left;
}

double mulDiv(const string& expression, int& i, bool& error) {
    double left = stepen(expression, i, error);

    if (error)
        return 0;

    while (i < expression.length()) {
        while (i < expression.length() && isspace(expression[i]))
            i++;

        if (i < expression.length() && (expression[i] == '*' || expression[i] == '/')) {
            char op = expression[i++];
            double right = stepen(expression, i, error);
            if (error)
                return 0;
            left = operations(op, left, right, error);
            if (error)
                return 0;
        }
        else break;
    }
    return left;
}

double resultAnalis(const string& expression, int& i, bool& error) {
    double left = mulDiv(expression, i, error);

    if (error)
        return 0;

    while (i < expression.length()) {
        while (i < expression.length() && isspace(expression[i]))
            i++;

        if (i < expression.length() && (expression[i] == '+' || expression[i] == '-')) {
            char op = expression[i++];
            double right = mulDiv(expression, i, error);
            if (error)
                return 0;
            left = operations(op, left, right, error);
            if (error)
                return 0;
        }
        else break;
    }
    return left;
}

void runAnalis() {
    while (true) {
        string input;
        cout << "Введите выражение" << endl;
        getline(cin, input);
        if (input == "exit")
            break;
        bool error = false;
        int i = 0;
        double result = resultAnalis(input, i, error);
        if (!error)
            cout << "Результат: " << result << endl;
        if (error) {
            if (!exc)
                cout << "Ошибка в выражении" << endl;
        }
    }
}

TEST(ТестНaCтепень, базовый)
{
    bool error = false;
    int position = 0;
    string expression = "2^3";

    double result = stepen(expression, position, error);

    EXPECT_FALSE(error);
    EXPECT_DOUBLE_EQ(result, 8);
}

TEST(ТестНaCтепень, ассоциативность)
{
    bool error = false;
    int position = 0;
    string expression = "2^3^2";

    double result = stepen(expression, position, error);

    EXPECT_FALSE(error);
    EXPECT_DOUBLE_EQ(result, 512);
}

int main(int argc, char** argv) {
    setlocale(LC_ALL, "ru");

    // Инициализация Google Test
    ::testing::InitGoogleTest(&argc, argv);

    // Запуск тестов
    int test_result = RUN_ALL_TESTS();

    // Если нужно, оставляем вашу оригинальную логику
    if (test_result == 0) {
        cout << "Калькулятор" << endl;
        testApply();
        runAnalis();
    }

    return test_result;
}