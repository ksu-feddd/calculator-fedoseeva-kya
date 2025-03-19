#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "calculator.h"

#define MAX_INPUT 1024       // 1 КиБ
#define RESULT_RANGE 2e9     // Диапазон результата: [-2 × 10^9, +2 × 10^9] для всех режимов

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

// Проверка соответствия скобок
static int check_parentheses(const char *input) {
    int open = 0;
    for (const char *p = input; *p; p++) {
        if (*p == '(') open++;
        else if (*p == ')') open--;
        if (open < 0) return 1; // Закрывающая скобка без открывающей
    }
    return open != 0; // True, если скобки не сбалансированы
}

// Проверка входных чисел в диапазоне [0, 2e9]
static int check_input_range(double value) {
    if (value < 0 || value > RESULT_RANGE) {
        fprintf(stderr, "Число вне диапазона [0, 2e9]\n");
        return 1;
    }
    return 0;
}

// Проверка диапазона результата: [-2e9, +2e9] для всех режимов
static int check_result_range(double value) {
    if (value < -RESULT_RANGE || value > RESULT_RANGE) {
        fprintf(stderr, "Результат вне диапазона [-2e9, +2e9]\n");
        return 1;
    }
    return 0;
}

// Простая проверка корректности и вычисление с поддержкой цепочки операций
static int parse_and_calculate(const char *input, int is_float, char *result, size_t result_size) {
    double a, b, c, d;                // Используем double для всех чисел
    char op1, op2, op3;               // Операторы
    double intermediate1, intermediate2, final_result;

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

    // Проверка баланса скобок
    if (check_parentheses(input)) {
        fprintf(stderr, "Некорректное выражение\n");
        return 1;
    }

    // Парсинг ввода
    if (sscanf(input, "(%lf %c %lf) %c (%lf %c %lf)", &a, &op1, &b, &op2, &c, &op3, &d) == 7) {
        // Формат: (a op1 b) op2 (c op3 d), например, "(1000 + 2000) * (10 + 5)"
        if (check_input_range(a) || check_input_range(b) || check_input_range(c) || check_input_range(d)) {
            return 1;
        }

        // Вычисление первого выражения в скобках
        switch (op1) {
            case '+':
                intermediate1 = a + b;
                break;
            case '-':
                intermediate1 = a - b;
                break;
            case '*':
                intermediate1 = a * b;
                break;
            case '/':
                if (is_float && b < 1e-10) {
                    fprintf(stderr, "Деление на слишком малое число\n");
                    return 1;
                } else if (!is_float && b < 1) {
                    fprintf(stderr, "Деление на ноль\n");
                    return 1;
                }
                intermediate1 = a / b;
                break;
            default:
                fprintf(stderr, "Некорректная операция\n");
                return 1;
        }
        if (check_result_range(intermediate1)) return 1;

        // Вычисление второго выражения в скобках
        switch (op3) {
            case '+':
                intermediate2 = c + d;
                break;
            case '-':
                intermediate2 = c - d;
                break;
            case '*':
                intermediate2 = c * d;
                break;
            case '/':
                if (is_float && d < 1e-10) {
                    fprintf(stderr, "Деление на слишком малое число\n");
                    return 1;
                } else if (!is_float && d < 1) {
                    fprintf(stderr, "Деление на ноль\n");
                    return 1;
                }
                intermediate2 = c / d;
                break;
            default:
                fprintf(stderr, "Некорректная операция\n");
                return 1;
        }
        if (check_result_range(intermediate2)) return 1;

        // Вычисление между выражениями
        switch (op2) {
            case '+':
                final_result = intermediate1 + intermediate2;
                break;
            case '-':
                final_result = intermediate1 - intermediate2;
                break;
            case '*':
                final_result = intermediate1 * intermediate2;
                break;
            case '/':
                if (is_float && intermediate2 < 1e-10) {
                    fprintf(stderr, "Деление на слишком малое число\n");
                    return 1;
                } else if (!is_float && intermediate2 < 1) {
                    fprintf(stderr, "Деление на ноль\n");
                    return 1;
                }
                final_result = intermediate1 / intermediate2;
                break;
            default:
                fprintf(stderr, "Некорректная операция\n");
                return 1;
        }
    } else if (sscanf(input, "%lf %c %lf %c (%lf %c %lf)", &a, &op1, &b, &op2, &c, &op3, &d) == 7) {
        // Формат: a op1 b op2 (c op3 d), например, "5 + 3 * (2 + 3)"
        if (check_input_range(a) || check_input_range(b) || check_input_range(c) || check_input_range(d)) {
            return 1;
        }

        // Вычисление выражения в скобках
        switch (op3) {
            case '+':
                intermediate1 = c + d;
                break;
            case '-':
                intermediate1 = c - d;
                break;
            case '*':
                intermediate1 = c * d;
                break;
            case '/':
                if (is_float && d < 1e-10) {
                    fprintf(stderr, "Деление на слишком малое число\n");
                    return 1;
                } else if (!is_float && d < 1) {
                    fprintf(stderr, "Деление на ноль\n");
                    return 1;
                }
                intermediate1 = c / d;
                break;
            default:
                fprintf(stderr, "Некорректная операция\n");
                return 1;
        }
        if (check_result_range(intermediate1)) return 1;

        // Вычисление с учётом приоритета
        if (op2 == '*' || op2 == '/') {
            if (op2 == '*') {
                final_result = b * intermediate1;
            } else {
                if (is_float && intermediate1 < 1e-10) {
                    fprintf(stderr, "Деление на слишком малое число\n");
                    return 1;
                } else if (!is_float && intermediate1 < 1) {
                    fprintf(stderr, "Деление на ноль\n");
                    return 1;
                }
                final_result = b / intermediate1;
            }
            if (check_result_range(final_result)) return 1;

            if (op1 == '+') final_result = a + final_result;
            else final_result = a - final_result;
        } else {
            if (op1 == '+') {
                final_result = a + b;
            } else {
                final_result = a - b;
            }
            if (check_result_range(final_result)) return 1;

            if (op2 == '+') final_result = final_result + intermediate1;
            else final_result = final_result - intermediate1;
        }
    } else if (input[0] == '(') {
        if (sscanf(input, "(%lf %c %lf) %c %lf %c %lf", &a, &op1, &b, &op2, &c, &op3, &d) == 7) {
            // Формат: (a op1 b) op2 c op3 d, например, "(5 + 5) * 2 / 5"
        } else if (sscanf(input, "(%lf %c %lf) %c %lf", &a, &op1, &b, &op2, &c) == 5) {
            // Формат: (a op1 b) op2 c
            d = 0;
            op3 = '\0';
        } else if (sscanf(input, "(%lf %c %lf)", &a, &op1, &b) == 3) {
            // Формат: (a op1 b)
            c = 0;
            op2 = '\0';
            d = 0;
            op3 = '\0';
        } else {
            fprintf(stderr, "Некорректное выражение\n");
            return 1;
        }

        if (check_input_range(a) || check_input_range(b) || 
            (op2 != '\0' && check_input_range(c)) || (op3 != '\0' && check_input_range(d))) {
            return 1;
        }

        // Вычисление первой операции
        switch (op1) {
            case '+':
                intermediate1 = a + b;
                break;
            case '-':
                intermediate1 = a - b;
                break;
            case '*':
                intermediate1 = a * b;
                break;
            case '/':
                if (is_float && b < 1e-10) {
                    fprintf(stderr, "Деление на слишком малое число\n");
                    return 1;
                } else if (!is_float && b < 1) {
                    fprintf(stderr, "Деление на ноль\n");
                    return 1;
                }
                intermediate1 = a / b;
                break;
            default:
                fprintf(stderr, "Некорректная операция\n");
                return 1;
        }
        if (check_result_range(intermediate1)) return 1;

        // Вычисление второй операции, если есть
        if (op2 != '\0') {
            switch (op2) {
                case '+':
                    intermediate1 = intermediate1 + c;
                    break;
                case '-':
                    intermediate1 = intermediate1 - c;
                    break;
                case '*':
                    intermediate1 = intermediate1 * c;
                    break;
                case '/':
                    if (is_float && c < 1e-10) {
                    fprintf(stderr, "Деление на слишком малое число\n");
                    return 1;
                } else if (!is_float && c < 1) {
                    fprintf(stderr, "Деление на ноль\n");
                    return 1;
                }
                intermediate1 = intermediate1 / c;
                break;
            default:
                fprintf(stderr, "Некорректная операция\n");
                return 1;
            }
            if (check_result_range(intermediate1)) return 1;
        }

        // Вычисление третьей операции, если есть
        if (op3 != '\0') {
            switch (op3) {
                case '+':
                    final_result = intermediate1 + d;
                    break;
                case '-':
                    final_result = intermediate1 - d;
                    break;
                case '*':
                    final_result = intermediate1 * d;
                    break;
                case '/':
                    if (is_float && d < 1e-10) {
                        fprintf(stderr, "Деление на слишком малое число\n");
                        return 1;
                    } else if (!is_float && d < 1) {
                        fprintf(stderr, "Деление на ноль\n");
                        return 1;
                    }
                    final_result = intermediate1 / d;
                    break;
                default:
                    fprintf(stderr, "Некорректная операция\n");
                    return 1;
            }
        } else {
            final_result = intermediate1;
        }
    } else {
        if (sscanf(input, "%lf %c %lf %c %lf", &a, &op1, &b, &op2, &c) == 5) {
            // Формат: a op1 b op2 c
            d = 0;
            op3 = '\0';
        } else if (sscanf(input, "%lf %c %lf", &a, &op1, &b) == 3) {
            // Формат: a op1 b
            c = 0;
            op2 = '\0';
            d = 0;
            op3 = '\0';
        } else {
            fprintf(stderr, "Некорректное выражение\n");
            return 1;
        }

        if (check_input_range(a) || check_input_range(b) || (op2 != '\0' && check_input_range(c))) {
            return 1;
        }

        // Вычисление первой операции
        switch (op1) {
            case '+':
                intermediate1 = a + b;
                break;
            case '-':
                intermediate1 = a - b;
                break;
            case '*':
                intermediate1 = a * b;
                break;
            case '/':
                if (is_float && b < 1e-10) {
                    fprintf(stderr, "Деление на слишком малое число\n");
                    return 1;
                } else if (!is_float && b < 1) {
                    fprintf(stderr, "Деление на ноль\n");
                    return 1;
                }
                intermediate1 = a / b;
                break;
            default:
                fprintf(stderr, "Некорректная операция\n");
                return 1;
        }
        if (check_result_range(intermediate1)) return 1;

        // Вычисление второй операции, если есть
        if (op2 != '\0') {
            switch (op2) {
                case '+':
                    final_result = intermediate1 + c;
                    break;
                case '-':
                    final_result = intermediate1 - c;
                    break;
                case '*':
                    final_result = intermediate1 * c;
                    break;
                case '/':
                    if (is_float && c < 1e-10) {
                        fprintf(stderr, "Деление на слишком малое число\n");
                        return 1;
                    } else if (!is_float && c < 1) {
                        fprintf(stderr, "Деление на ноль\n");
                        return 1;
                    }
                    final_result = intermediate1 / c;
                    break;
                default:
                    fprintf(stderr, "Некорректная операция\n");
                    return 1;
            }
        } else {
            final_result = intermediate1;
        }
    }

    if (check_result_range(final_result)) return 1;

    // Форматирование результата
    if (is_float) {
        snprintf(result, result_size, "%.4f", final_result);
    } else {
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
