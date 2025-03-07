#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "calculator.h"

#define MAX_INPUT 1024  // 1 КиБ

// Проверка разрешённых символов
static int is_allowed_char(char c, int is_float) {
    int base_allowed = (c >= '0' && c <= '9') || c == '(' || c == ')' || c == '*' ||
                       c == '+' || c == '/' || c == '-' || isspace(c);
    if (is_float) {
        return base_allowed || c == '.';  // Разрешаем точку в float-режиме
    }
    return base_allowed;
}

// Проверка на наличие унарного минуса перед числами
static int has_negative_numbers(const char *input, int is_float) {
    int i = 0;
    int after_number = 0;  // Флаг: был ли предыдущий символ числом

    while (input[i]) {
        if (isspace(input[i])) {
            i++;
            continue;
        }

        if (input[i] == '-') {
            // Если '-' в начале строки или после пробела, но перед числом
            if (i == 0 || (isspace(input[i - 1]) && !after_number)) {
                return 1;  // Унарный минус в начале выражения
            }
            // Сбрасываем флаг после оператора
            after_number = 0;
        } else if (isdigit(input[i])) {
            after_number = 1;  // Отмечаем, что встретили число
        } else if (input[i] == '.' && is_float) {
            after_number = 1;  // Точка в float-режиме считается частью числа
        } else {
            after_number = 0;  // Любой другой символ (кроме цифр и точки) сбрасывает флаг
        }
        i++;
    }
    return 0;
}

// Простая проверка корректности выражения и вычисление
static int parse_and_calculate(const char *input, int is_float, char *result, size_t result_size) {
    unsigned int a_int, b_int;  // Входные числа неотрицательные
    double a_float, b_float;
    char op;

    // Проверка символов
    for (const char *p = input; *p; p++) {
        if (!is_allowed_char(*p, is_float)) {
            fprintf(stderr, "Недопустимый символ\n");
            return 1;
        }
    }

    // Проверка на унарный минус
    if (has_negative_numbers(input, is_float)) {
        fprintf(stderr, "Отрицательные числа во входных данных не поддерживаются\n");
        return 1;
    }

    if (is_float) {
        if (sscanf(input, "%lf %c %lf", &a_float, &op, &b_float) != 3) {
            fprintf(stderr, "Некорректное выражение\n");
            return 1;
        }
        switch (op) {
            case '+':
                snprintf(result, result_size, "%.4f", add_float(a_float, b_float));
                break;
            case '-':
                snprintf(result, result_size, "%.4f", a_float - b_float);  // Отрицательный результат допустим
                break;
            case '*':
                snprintf(result, result_size, "%.4f", a_float * b_float);
                break;
            case '/':
                if (b_float < 1e-10) {
                    fprintf(stderr, "Деление на слишком малое число\n");
                    return 1;
                }
                snprintf(result, result_size, "%.4f", a_float / b_float);
                break;
            default:
                fprintf(stderr, "Некорректная операция\n");
                return 1;
        }
    } else {
        if (sscanf(input, "%u %c %u", &a_int, &op, &b_int) != 3) {
            fprintf(stderr, "Некорректное выражение\n");
            return 1;
        }
        switch (op) {
            case '+':
                snprintf(result, result_size, "%u", add_int(a_int, b_int));
                break;
            case '-':
                snprintf(result, result_size, "%d", (int)a_int - (int)b_int);  // Отрицательный результат допустим
                break;
            case '*':
                snprintf(result, result_size, "%u", a_int * b_int);
                break;
            case '/':
                if (b_int == 0) {
                    fprintf(stderr, "Деление на ноль\n");
                    return 1;
                }
                snprintf(result, result_size, "%u", a_int / b_int);
                break;
            default:
                fprintf(stderr, "Некорректная операция\n");
                return 1;
        }
    }
    return 0;
}

int main(int argc, char *argv[]) {
    int is_float = 0;
    if (argc > 1 && strcmp(argv[1], "--float") == 0) {
        is_float = 1;
    }

    char input[MAX_INPUT];
    if (fgets(input, sizeof(input), stdin) == NULL) {
        fprintf(stderr, "Ошибка ввода или EOF\n");
        return 1;
    }

    input[strcspn(input, "\n")] = 0;
    if (strlen(input) >= MAX_INPUT) {
        return 1;  // UB: больше 1 КиБ
    }

    char result[32];
    int status = parse_and_calculate(input, is_float, result, sizeof(result));
    if (status == 0) {
        printf("%s\n", result);
    }
    return status;
}
