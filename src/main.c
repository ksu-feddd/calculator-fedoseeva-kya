#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "calculator.h"

#define MAX_INPUT 1024       // 1 КиБ
#define RESULT_RANGE 2e9     // Диапазон результата: [-2 × 10^9, +2 × 10^9]

// Проверка разрешённых символов [0-9()*+\/\s-]
static int is_allowed_char(char c, int is_float) {
    int base_allowed = (c >= '0' && c <= '9') || c == '(' || c == ')' || c == '*' ||
                       c == '+' || c == '/' || c == '-' || isspace(c);
    if (is_float) {
        return base_allowed || c == '.'; // Только точка для float
    }
    return base_allowed;
}

// Проверка на унарный минус (запрет чисел вида -3)
static int has_negative_numbers(const char *input, int is_float) {
    int i = 0;
    int after_number = 0;

    while (input[i]) {
        if (isspace(input[i])) {
            i++;
            continue;
        }
        if (input[i] == '-') {
            if (i == 0 || (isspace(input[i - 1]) && !after_number)) {
                return 1; // Унарный минус перед числом
            }
            after_number = 0;
        } else if (isdigit(input[i])) {
            after_number = 1;
        } else if (input[i] == '.' && is_float) {
            after_number = 1;
        } else {
            after_number = 0;
        }
        i++;
    }
    return 0;
}

// Проверка неотрицательности входных чисел
static int check_input_non_negative(double value) {
    if (value < 0) {
        fprintf(stderr, "Число вне диапазона [0, ∞]\n");
        return 1;
    }
    return 0;
}

// Проверка диапазона результата [-2 × 10^9, +2 × 10^9]
static int check_result_range(double value) {
    if (value < -RESULT_RANGE || value > RESULT_RANGE) {
        fprintf(stderr, "Результат вне диапазона [-2e9, +2e9]\n");
        return 1;
    }
    return 0;
}

// Простая проверка корректности и вычисление с поддержкой одной пары скобок
static int parse_and_calculate(const char *input, int is_float, char *result, size_t result_size) {
    unsigned int a_int, b_int, c_int; // Для int-режима
    double a_float, b_float, c_float; // Для float-режима
    char op1, op2;                    // Первый оператор (в скобках) и второй (снаружи)
    double intermediate, final_result;

    // Проверка символов
    for (const char *p = input; *p; p++) {
        if (!is_allowed_char(*p, is_float)) {
            fprintf(stderr, "Недопустимый символ\n");
            return 1;
        }
    }

    // Проверка унарного минуса
    if (has_negative_numbers(input, is_float)) {
        fprintf(stderr, "Унарный минус не поддерживается\n");
        return 1;
    }

    if (is_float) {
        // Проверяем формат: (число оператор число) оператор число
        if (input[0] == '(') {
            if (sscanf(input, "(%lf %c %lf) %c %lf", &a_float, &op1, &b_float, &op2, &c_float) != 5) {
                // Пробуем простой формат: число оператор число
                if (sscanf(input, "%lf %c %lf", &a_float, &op1, &b_float) != 3) {
                    fprintf(stderr, "Некорректное выражение\n");
                    return 1;
                }
                c_float = 0; // Нет внешнего числа
                op2 = '\0';  // Нет внешней операции
            }
        } else {
            if (sscanf(input, "%lf %c %lf", &a_float, &op1, &b_float) != 3) {
                fprintf(stderr, "Некорректное выражение\n");
                return 1;
            }
            c_float = 0;
            op2 = '\0';
        }

        // Проверка неотрицательности входных чисел
        if (check_input_non_negative(a_float) || check_input_non_negative(b_float) || 
            (op2 != '\0' && check_input_non_negative(c_float))) {
            return 1;
        }

        // Вычисление выражения в скобках
        switch (op1) {
            case '+':
                intermediate = a_float + b_float;
                break;
            case '-':
                intermediate = a_float - b_float;
                break;
            case '*':
                intermediate = a_float * b_float;
                break;
            case '/':
                if (b_float < 1e-10) {
                    fprintf(stderr, "Деление на слишком малое число\n");
                    return 1;
                }
                intermediate = a_float / b_float;
                break;
            default:
                fprintf(stderr, "Некорректная операция\n");
                return 1;
        }
        if (check_result_range(intermediate)) return 1;

        // Вычисление внешней операции, если есть
        if (op2 != '\0') {
            switch (op2) {
                case '+':
                    final_result = intermediate + c_float;
                    break;
                case '-':
                    final_result = intermediate - c_float;
                    break;
                case '*':
                    final_result = intermediate * c_float;
                    break;
                case '/':
                    if (c_float < 1e-10) {
                        fprintf(stderr, "Деление на слишком малое число\n");
                        return 1;
                    }
                    final_result = intermediate / c_float;
                    break;
                default:
                    fprintf(stderr, "Некорректная операция\n");
                    return 1;
            }
            if (check_result_range(final_result)) return 1;
        } else {
            final_result = intermediate;
        }

        snprintf(result, result_size, "%.4f", final_result);
    } else {
        // Проверяем формат: (число оператор число) оператор число
        if (input[0] == '(') {
            if (sscanf(input, "(%u %c %u) %c %u", &a_int, &op1, &b_int, &op2, &c_int) != 5) {
                // Пробуем простой формат: число оператор число
                if (sscanf(input, "%u %c %u", &a_int, &op1, &b_int) != 3) {
                    fprintf(stderr, "Некорректное выражение\n");
                    return 1;
                }
                c_int = 0;
                op2 = '\0';
            }
        } else {
            if (sscanf(input, "%u %c %u", &a_int, &op1, &b_int) != 3) {
                fprintf(stderr, "Некорректное выражение\n");
                return 1;
            }
            c_int = 0;
            op2 = '\0';
        }

        // Проверка неотрицательности (уже обеспечена %u, но для единообразия)
        if (check_input_non_negative(a_int) || check_input_non_negative(b_int) || 
            (op2 != '\0' && check_input_non_negative(c_int))) {
            return 1;
        }

        // Вычисление выражения в скобках
        switch (op1) {
            case '+':
                intermediate = (double)a_int + (double)b_int;
                break;
            case '-':
                intermediate = (double)a_int - (double)b_int;
                break;
            case '*':
                intermediate = (double)a_int * (double)b_int;
                break;
            case '/':
                if (b_int == 0) {
                    fprintf(stderr, "Деление на ноль\n");
                    return 1;
                }
                intermediate = (double)a_int / (double)b_int;
                break;
            default:
                fprintf(stderr, "Некорректная операция\n");
                return 1;
        }
        if (check_result_range(intermediate)) return 1;

        // Вычисление внешней операции, если есть
        if (op2 != '\0') {
            switch (op2) {
                case '+':
                    final_result = intermediate + (double)c_int;
                    break;
                case '-':
                    final_result = intermediate - (double)c_int;
                    break;
                case '*':
                    final_result = intermediate * (double)c_int;
                    break;
                case '/':
                    if (c_int == 0) {
                        fprintf(stderr, "Деление на ноль\n");
                        return 1;
                    }
                    final_result = intermediate / (double)c_int;
                    break;
                default:
                    fprintf(stderr, "Некорректная операция\n");
                    return 1;
            }
            if (check_result_range(final_result)) return 1;
        } else {
            final_result = intermediate;
        }

        snprintf(result, result_size, "%d", (int)final_result);
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
