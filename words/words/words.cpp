#include <iostream>
#include <cctype>
#include <cstring>
using namespace std;;

const int maxSize = 16;  // Максимальная длина строки

// Функция для проверки, является ли строка числом
bool isNumber(const char* str) {
    bool hasDigit = false;
    int plusCount = 0;
    int minusCount = 0;
    bool hasDotOrComma = false;

    for (int i = 0; str[i] != '\0'; ++i) {
        if (isdigit(str[i])) {
            hasDigit = true;
        }
        if (str[i] == '+') {
            if (++plusCount > 1) return false;
        }
        if (str[i] == '-') {
            if (++minusCount > 1) return false;
        }
        if (str[i] == '.' || str[i] == ',') {
            if (hasDotOrComma) return false;
            hasDotOrComma = true;
        }
        if (!isdigit(str[i]) && str[i] != '+' && str[i] != '-' && str[i] != '.' && str[i] != ',') {
            return false;
        }
    }
    return hasDigit && plusCount <= 1 && minusCount <= 1 && (!hasDotOrComma || hasDotOrComma);
}

// Функция для проверки, является ли строка словом
bool isWord(const char* str) {
    bool hasAlpha = false;
    bool isNum = isNumber(str);

    for (int i = 0; str[i] != '\0'; ++i) {
        if (isalpha(str[i])) {
            hasAlpha = true;
            break;
        }
    }

    // Слово, если содержит буквы или если это не число
    return hasAlpha || !isNum;
}

int main() {
    char word[maxSize];
    int index = 0;
    int wordCount = 0;

    cout << "Enter string: ";

    while (true) {
        char ch;
        cin.get(ch);

        if (ch == ' ' || ch == '\t' || ch == '\n') {
            word[index] = '\0';

            if (index > 0) {
                if (isWord(word)) {
                    cout << word << " ";
                    wordCount++;
                }
            }

            index = 0;
            if (ch == '\n') break;
        }
        else if (isalpha(ch) || isdigit(ch) || ch == '+' || ch == '-' || ch == '.' || ch == ',') {
            if (index < maxSize - 1) word[index++] = ch;
        }
    }

    cout << "\nWord count: " << wordCount << endl;

    return 0;
}
//Даётся последовательнотсь символов(через cin), строка может содержать любые символы с клавиатуры(на английской раскладке). Нужно обработать её так, 
// чтобы вывести количество слов(Последовательность символов 
// будет являться словом только тогда и толька тогда когда она не является числом или хотя бы один из её смиволов буква, содержит цифры но не является числом). 
// Числом является последовательность когда хотя бы один её элемент цифра или +, - или ".", ",", при этомне более одного + и одного - и не более одной "." или ",". 
// Разделителями является пробел и табуляция. Все остальные символы с клавиатуры - это мусор. При реализации использовать c++, 
// исключительно си-строки не более 16 символ(вместе с термининрующем нулём). Вывести слова через пробел. Для считывания использовать get. 
// Для проверки использовать isalpha(char a), isdigit( char a)

//Словом является любая группа символов содержащая хотя бы одну букву или группа цифр не явлющаяся числом.
