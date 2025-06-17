#include "pch.h"
#include <iostream>
#include <cmath>
#include <string>
#include <vector>
#include <ctime>
#include <gtest/gtest.h>

using namespace std;

bool exc = false;

double resultAnalis(const string& expression, int& i, bool& error);

double readNumber(const string& expression, int& i) {
    string value;
    while (i < expression.length() && (isdigit(expression[i]) || expression[i] == '.')) {
        value += expression[i++];
    }
    return stod(value);
}

bool isOperator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '^' || c == '#';
}

bool isValidSimbol(char c) {
    return isdigit(c) || c == '.' || c == '#' || isOperator(c) || c == '(' || c == ')' || isspace(c);
}

double mulDiv(const string& expression, int& i, bool& error);

double stepen(const string& expression, int& i, bool& error);


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

double firstAnalis(const string& expression, int& i, bool& error)
{
    while (i < expression.length() && isspace(expression[i]))
        i++;

    bool negative = false;

    if (i < expression.size() && expression[i] == '-' &&
        (i == 0 || expression[i - 1] == '(' || isOperator(expression[i - 1])))
    {
        negative = true;
        i++;
        while (i < expression.length() && isspace(expression[i]))
            i++;
    }
    if (i < expression.size() && expression[i] == '(')
    {
        if (i > 0)
        {
            char prevChar = expression[i - 1];
            if (isdigit(prevChar))
            {
                error = true;
                return 0;
            }
        }
        i++;

        double value = resultAnalis(expression, i, error);
        if (error) return 0;

        if (i >= expression.size() || expression[i] != ')') {
            error = true;
            return 0;
        }
        i++;
        return negative ? -value : value;
    }
    else if (i < expression.size() && expression[i] == '#')
    {
        i++;
        double value = firstAnalis(expression, i, error);
        double result = operations('#', 0, value, error);
        return negative ? -result : result;
    }
    else if (i < expression.size() && (isdigit(expression[i]) || expression[i] == '.'))
    {
        bool hasPoint = false;
        int start = i;

        if (expression[i] == '.' && (i + 1 >= expression.size() || !isdigit(expression[i + 1])))
        {
            error = true;
            return 0;
        }

        while (i < expression.size() && (isdigit(expression[i]) || expression[i] == '.'))
        {
            if (expression[i] == '.')
            {
                if (hasPoint)
                {
                    error = true;
                    return 0;
                }
                hasPoint = true;
            }
            i++;
        }

        if (i > start && expression[i - 1] == '.')
        {
            error = true;
            return 0;
        }

        i = start;
        double number = readNumber(expression, i);
        return negative ? -number : number;
    }
    else
    {
        error = true;
        return 0;
    }
}

//ТЕСТ
TEST(firstAnalis, отрицательноеЧисло) {
    bool error = false;
    int pos = 0;
    string expr = "-3.14";

    double result = firstAnalis(expr, pos, error);

    EXPECT_FALSE(error);
    EXPECT_DOUBLE_EQ(result, -3.14);
}


TEST(firstAnalis, ЧислоСТочки) {
    bool error = false;
    int pos = 0;
    string expression = ".5";

    double result = firstAnalis(expression, pos, error);
    EXPECT_FALSE(error);
    EXPECT_DOUBLE_EQ(result, 0.5);
}


TEST(firstAnalis, выражениеВСкобках) {
    bool error = true;
    int pos = 0;
    string expr = "(45-3";
    double result = firstAnalis(expr, pos, error);
    EXPECT_TRUE(error);
}


TEST(firstAnalis, неЧисло) {
    bool error = false;
    int pos = 0;
    string expr = "abc";

    double result = firstAnalis(expr, pos, error);

    EXPECT_TRUE(error);
}

TEST(firstAnalis, неправильноеДесятичное) {
    bool error = false;
    int pos = 0;
    string expr = "12..54";

    double result = firstAnalis(expr, pos, error);

    EXPECT_TRUE(error);
}

TEST(firstAnalis, отрицательныеСобки) {
    bool error = false;
    int pos = 0;
    string expr = "-(-5)";

    double result = firstAnalis(expr, pos, error);

    EXPECT_FALSE(error);
    EXPECT_DOUBLE_EQ(result, 5);
}

TEST(firstAnalis, послеЧислаОшибка) {
    bool error = true;
    int pos = 0;
    string expr = "1000%%";

    double result = firstAnalis(expr, pos, error);

    EXPECT_TRUE(error);
}

TEST(firstAnalis, отрицательныйКорень) {
    bool error = true;
    int pos = 0;
    string expr = "#-100";

    double result = firstAnalis(expr, pos, error);

    EXPECT_TRUE(error);
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

TEST(ТестНaCтепень, нульСепень)
{
    bool error = false;
    int position = 0;
    string expression = "5^0";

    double result = stepen(expression, position, error);

    EXPECT_FALSE(error);
    EXPECT_DOUBLE_EQ(result, 1);
}

TEST(ТестНaCтепень, отрицательноеЧисло)
{
    bool error = false;
    int position = 0;
    string expression = "-2^3";

    double result = stepen(expression, position, error);

    EXPECT_FALSE(error);
    EXPECT_DOUBLE_EQ(result, -8);
}

double mulDiv(const string& expression, int& i, bool& error) {
    double left = stepen(expression, i, error);

    if (error)
        return 0;

    while (i < expression.length())
    {
        while (i < expression.length() && isspace(expression[i]))
            i++;
        if (i < expression.length() && (expression[i] == '*' || expression[i] == '/'))
        {
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

string replaceMulti(const std::string& expression) {
    std::string result;
    for (size_t i = 0; i < expression.length(); ++i) {
        if (expression[i] == '(')
        {
            if (i > 0 && (isdigit(expression[i - 1]) || expression[i - 1] == '.')) {
                result += '*';
            }
        }
        result += expression[i];
    }
    return result;
}

bool validateExpression(const string& expression) {
    for (char c : expression) {
        if (!isValidSimbol(c)) {
            return false;
        }
    }
    return true;
}

double resultAnalis(const string& expression, int& i, bool& error) {
    double left = mulDiv(expression, i, error);

    if (error)
        return 0;

    while (i < expression.length())
    {
        while (i < expression.length() && isspace(expression[i]))
            i++;

        if (i < expression.length() && (expression[i] == '+' || expression[i] == '-'))
        {

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

TEST(resultAnalis, сложение) {
    bool error = false;
    int pos = 0;
    string expr = "2+3-1";

    double result = resultAnalis(expr, pos, error);

    EXPECT_FALSE(error);
    EXPECT_DOUBLE_EQ(result, 4);
}

TEST(resultAnalis, сложениеСПробелами) {
    bool error = false;
    int pos = 0;
    string expr = "2    +     3    -   1";

    double result = resultAnalis(expr, pos, error);

    EXPECT_FALSE(error);
    EXPECT_DOUBLE_EQ(result, 4);
}

TEST(resultAnalis, приоритет) {
    bool error = false;
    int pos = 0;
    string expr = "2+3*5";

    double result = resultAnalis(expr, pos, error);

    EXPECT_FALSE(error);
    EXPECT_DOUBLE_EQ(result, 17);
}

TEST(resultAnalis, приоритетСкобки) {
    bool error = false;
    int pos = 0;
    string expr = "(2+3)*5";

    double result = resultAnalis(expr, pos, error);

    EXPECT_FALSE(error);
    EXPECT_DOUBLE_EQ(result, 25);
}

TEST(resultAnalis, сложноеВыражение1) {
    bool error = false;
    int pos = 0;
    string expr = "2+3*4-5.3/2";

    double result = resultAnalis(expr, pos, error);

    EXPECT_FALSE(error);
    EXPECT_DOUBLE_EQ(result, 2 + 3 * 4 - 5.3 / 2);
}

TEST(resultAnalis, сложноеВыражение2) {
    bool error = false;
    int pos = 0;
    string expr = "((2+3)4)-5";

    double result = resultAnalis(expr, pos, error);

    EXPECT_FALSE(error);
    EXPECT_DOUBLE_EQ(result, ((2 + 3) * 4) - 5);
}

TEST(resultAnalis, сложноеВыражение3) {
    bool error = false;
    int pos = 0;
    string expr = "(2+3)(4-1)";

    double result = resultAnalis(expr, pos, error);

    EXPECT_FALSE(error);
    EXPECT_DOUBLE_EQ(result, ((2 + 3) * (4 - 1)));
}


void runAnalis() {
    while (true) {
        string input;
        cout << "Введите выражение" << endl;
        getline(cin, input);
        if (input == "exit")
            break;

        if (!validateExpression(input)) {
            continue;
        }
        string processedInput = replaceMulti(input);

        bool error = false;
        int i = 0;
        double result = resultAnalis(processedInput, i, error);
        if (!error)
            cout << "Результат: " << result << endl;
        if (error) {
            if (!exc)
                cout << "Ошибка в выражении" << endl;
        }
    }
}


int main(int argc, char** argv) {
    setlocale(LC_ALL, "ru");
    setlocale(LC_NUMERIC, "C");

    ::testing::InitGoogleTest(&argc, argv);

    int test_result = RUN_ALL_TESTS();

    if (test_result == 0) {
        cout << "Калькулятор" << endl;
        testApply();
        runAnalis();
    }

    return test_result;
}