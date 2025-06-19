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
string replaceMulti(const string& expression);

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
            cout << "ќшибка: деление на ноль\n";
            error = true;
            return 0;
        }
        return a / b;
    case '^': return pow(a, b);
    case '#':
        if (b < 0) {
            cout << "ќшибка: корень из отрицательного числа\n";
            error = true;
            return 0;
        }
        return sqrt(b);
    default:
        cout << "ќшибка: неизвестный оператор\n";
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

    };

    int passed = 0;
    for (int i = 0; i < tests.size(); i++) {
        cout << "“ест #" << i + 1 << endl;
        cout << "¬ыражение:" << tests[i].a << tests[i].op << tests[i].b << endl;

        bool error = false;
        double result = operations(tests[i].op, tests[i].a, tests[i].b, error);
        cout << "ќжидаемый результат: " << tests[i].expectedResult << ", ќжидаема€ ошибка: " << boolalpha << tests[i].expectError << endl;
        cout << "‘актический результат: " << result << ", ќшибка: " << boolalpha << error << endl;

        if (error == tests[i].expectError) {
            if (result == tests[i].expectedResult) {
                cout << "“ест пройден!" << endl;
                ++passed;
            }
        }
        else {
            cout << "“ест Ќ≈ пройден!" << endl;
        }

        cout << "---------------------------------------------------------------------" << endl;
    }

    cout << "”спешных тестов: " << passed << " / " << tests.size() << endl;
}

void skipSpaces(const string& expression, int& i) {
    while (i < expression.length() && isspace(expression[i]))
        i++;
}

double firstAnalis(const string& expression, int& i, bool& error)
{
    skipSpaces(expression, i);

    bool negative = false;

    if (i < expression.size() && expression[i] == '-' &&
        (i == 0 || expression[i - 1] == '(' || isOperator(expression[i - 1]))) //con1
    {
        negative = true;
        i++;
        skipSpaces(expression, i);
    }
    if (i < expression.size() && expression[i] == '(') //con2
    {
        i++;
        double value = resultAnalis(expression, i, error);
        if (i >= expression.size() || expression[i] != ')') //con3
        {
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
            double resultAnalis(const string & expression, int& i, bool& error);
            {
                string newExpression = replaceMulti(expression);

                double left = mulDiv(newExpression, i, error);

                if (error)
                    return 0;

                while (i < newExpression.length())
                {
                    while (i < newExpression.length() && isspace(newExpression[i]))
                        i++;

                    if (i < newExpression.length() && (newExpression[i] == '+' || newExpression[i] == '-'))
                    {

                        char op = newExpression[i++];
                        double right = mulDiv(newExpression, i, error);
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


TEST(firstAnalis, отрицательное„исло1) {
    bool error = false;
    int position = 0;
    string expression = "-4";

    double result = firstAnalis(expression, position, error);

    EXPECT_FALSE(error);
    EXPECT_DOUBLE_EQ(result, -4);
}

TEST(firstAnalis, отрицательное„исло2) {
    bool error = true;
    int position = 0;
    string expression = "--4";

    double result = firstAnalis(expression, position, error);

    EXPECT_TRUE(error);
}


TEST(firstAnalis, „исло—“очки) {
    bool error = false;
    int position = 0;
    string expression = ".5";

    double result = firstAnalis(expression, position, error);
    EXPECT_FALSE(error);
    EXPECT_DOUBLE_EQ(result, 0.5);
}

TEST(firstAnalis, неправильноеƒес€тичное) {
    bool error = false;
    int position = 0;
    string expression = "5..5";

    double result = firstAnalis(expression, position, error);

    EXPECT_TRUE(error);
}

TEST(firstAnalis, выражение¬—кобках1) {
    bool error = true;
    int position = 0;
    string expression = "(45-3";
    double result = firstAnalis(expression, position, error);
    EXPECT_TRUE(error);
}

TEST(firstAnalis, выражение¬—кобках2) {
    bool error = false;
    int position = 0;
    string expression = "(45-3)";

    double result = firstAnalis(expression, position, error);

    EXPECT_FALSE(error);
    EXPECT_DOUBLE_EQ(result, 42);
}

TEST(firstAnalis, отрицательные—обки) {
    bool error = false;
    int position = 0;
    string expression = "-(-5)";

    double result = firstAnalis(expression, position, error);

    EXPECT_FALSE(error);
    EXPECT_DOUBLE_EQ(result, 5);
}

TEST(firstAnalis, корень»скобки) {
    bool error = false;
    int position = 0;
    string expression = "#(5+4)";

    double result = firstAnalis(expression, position, error);

    EXPECT_FALSE(error);
    EXPECT_DOUBLE_EQ(result, 3);
}

TEST(firstAnalis, отрицательный орень) {
    bool error = false;
    int position = 0;
    string expression = "#(-6+2)";

    double result = firstAnalis(expression, position, error);

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
            left = operations('^', left, right, error);
        }
        else break;
    }
    return left;
}

TEST(stepen, базовый)
{
    bool error = false;
    int position = 0;
    string expression = "2^3";

    double result = stepen(expression, position, error);

    EXPECT_FALSE(error);
    EXPECT_DOUBLE_EQ(result, 8);
}

TEST(stepen, ассоциативность)
{
    bool error = false;
    int position = 0;
    string expression = "2^3^2";

    double result = stepen(expression, position, error);

    EXPECT_FALSE(error);
    EXPECT_DOUBLE_EQ(result, 512);
}

TEST(stepen, нулева€—епень)
{
    bool error = false;
    int position = 0;
    string expression = "5^0";

    double result = stepen(expression, position, error);

    EXPECT_FALSE(error);
    EXPECT_DOUBLE_EQ(result, 1);
}

TEST(stepen, отрицательное„исло)
{
    bool error = false;
    int position = 0;
    string expression = "-2^3";

    double result = stepen(expression, position, error);

    EXPECT_FALSE(error);
    EXPECT_DOUBLE_EQ(result, -8);
}

TEST(stepen, пропущено„ислоѕеред—тепенью)
{
    bool error = true;
    int position = 0;
    string expression = "^3";

    double result = stepen(expression, position, error);

    EXPECT_TRUE(error);
}

TEST(stepen, пропущено„ислоѕосле—тепени)
{
    bool error = true;
    int position = 0;
    string expression = "3^";

    double result = stepen(expression, position, error);

    EXPECT_TRUE(error);
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
            left = operations(op, left, right, error);
        }
        else break;
    }
    return left;
}

TEST(mulDiv, последовательныеќперации) {
    bool error = false;
    int position = 0;
    string expression = "4 * 2 / 2";

    double result = mulDiv(expression, position, error);

    EXPECT_FALSE(error);
    EXPECT_DOUBLE_EQ(result, 4*2/2);
}


TEST(mulDiv, пропущено„ислоѕослеќперанда”множени€) {
    bool error = true;
    int position = 0;
    string expression = "4*";

    double result = mulDiv(expression, position, error);

    EXPECT_TRUE(error);
}

TEST(mulDiv, ассоциативность”множени€) {
    bool error = false;
    int position = 0;
    string expression = "4*5*6";

    double result = mulDiv(expression, position, error);

    EXPECT_FALSE(error);
    EXPECT_DOUBLE_EQ(result, 4 * 5 * 6);
}

TEST(mulDiv, ассоциативностьƒеление) {
    bool error = false;
    int position = 0;
    string expression = "10/5/2";

    double result = mulDiv(expression, position, error);

    EXPECT_FALSE(error);
    EXPECT_DOUBLE_EQ(result, 10/ 5 / 2);
}

TEST(mulDiv, пропущено„ислоѕередќперанда”множени€) {
    bool error = true;
    int position = 0;
    string expression = "*4";

    double result = mulDiv(expression, position, error);

    EXPECT_TRUE(error);
}

string replaceMulti(const string& expression) {
    string result;
    for (size_t i = 0; i < expression.length(); ++i) {
        if (i > 0 && expression[i] == '(' &&
            (isdigit(expression[i - 1]) || expression[i - 1] == ')')) {
            result += '*';
        }
        else if (i > 0 && isdigit(expression[i - 1]) && expression[i] == '(') {
            result += '*';
        }
        else if (i > 0 && expression[i - 1] == ')' &&
            (isdigit(expression[i]) || expression[i] == '.')) {
            result += '*';
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

TEST(validateExpression, неправильныйќператор) {
    string expression = "100%%";
    bool result = validateExpression(expression);

    EXPECT_FALSE(result);
}

TEST(validateExpression, корректное¬ыражение) {
    string expression = "3 + 5 * (2 - 8)";
    EXPECT_TRUE(validateExpression(expression));
}

TEST(validateExpression, содержитЅуквы) {
    string expression = "5 + x - 3";
    EXPECT_FALSE(validateExpression(expression));
}

TEST(validateExpression, толькоѕробелы) {
    string expression = "     ";
    EXPECT_TRUE(validateExpression(expression));
}

TEST(validateExpression, пуста€—трока) {
    string expression = "";
    EXPECT_TRUE(validateExpression(expression));
}


double resultAnalis(const string& expression, int& i, bool& error) {

    string newExpression = replaceMulti(expression);

    double left = mulDiv(newExpression, i, error);

    if (error)
        return 0;

    while (i < newExpression.length())
    {
        while (i < newExpression.length() && isspace(newExpression[i]))
            i++;

        if (i < newExpression.length() && (newExpression[i] == '+' || newExpression[i] == '-'))
        {
            char op = newExpression[i++];
            double right = mulDiv(newExpression, i, error);
            left = operations(op, left, right, error);
        }
        else break;
    }
    return left;
}

TEST(resultAnalis, сложение¬ычитание) {
    bool error = false;
    int position = 0;
    string expression = "2+3-1";

    double result = resultAnalis(expression, position, error);

    EXPECT_FALSE(error);
    EXPECT_DOUBLE_EQ(result, 4);
}

TEST(resultAnalis, сложение—ѕробелами) {
    bool error = false;
    int  position = 0;
    string expression = "2    +     3    -   1";

    double result = resultAnalis(expression, position, error);

    EXPECT_FALSE(error);
    EXPECT_DOUBLE_EQ(result, 4);
}

TEST(resultAnalis, приоритет) {
    bool error = false;
    int  position = 0;
    string expression  = "2+3*5";

    double result = resultAnalis(expression, position, error);

    EXPECT_FALSE(error);
    EXPECT_DOUBLE_EQ(result, 17);
}

TEST(resultAnalis, приоритет—кобки) {
    bool error = false;
    int  position = 0;
    string expression = "(2+3)*5";

    double result = resultAnalis(expression, position, error);

    EXPECT_FALSE(error);
    EXPECT_DOUBLE_EQ(result, 25);
}

TEST(resultAnalis, —ложное¬ыражение—о—кобками) {
    bool error = false;
    int position = 0;
    string expression = "((2+3)(4-1))-5";

    double result = resultAnalis(expression, position, error);

    EXPECT_FALSE(error);
    EXPECT_DOUBLE_EQ(result, ((2 + 3) * (4 - 1)) - 5);
}

TEST(resultAnalis, двойнойќператор) {
    bool error = true;
    int  position = 0;
    string expression = "2++5";

    double result = resultAnalis(expression, position, error);

    EXPECT_TRUE(error);
}

void runAnalis() {
    while (true) {
        string input;
        cout << "¬ведите выражение" << endl;
        getline(cin, input);
        if (input == "exit")
            break;

        if (!validateExpression(input)) {
            continue;
        }

        bool error = false;
        int i = 0;
        double result = resultAnalis(input, i, error);
        if (!error)
            cout << "–езультат: " << result << endl;
        if (error) {
            if (!exc)
                cout << "ќшибка в выражении" << endl;
        }
    }
}


int main(int argc, char** argv) {
    setlocale(LC_ALL, "ru");
    setlocale(LC_NUMERIC, "C");

    ::testing::InitGoogleTest(&argc, argv);

    int test_result = RUN_ALL_TESTS();

    if (test_result == 0) {
        cout << " алькул€тор" << endl;
        testApply();
        runAnalis();
    }

    return test_result;
}