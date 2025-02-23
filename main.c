#include <stdio.h>
#include <stdlib.h>

char *expr;

int number();
int factor();
int term();
int expression();

int number() {
    int result = 0;
    while (*expr >= '0' && *expr <= '9') {
        result = result * 10 + (*expr - '0');
        expr++;
    }
    return result;
}

int factor() {
    if (*expr == '(') {
        expr++;
        int result = expression();
        expr++;
        return result;
    }
    return number();
}

int term() {
    int result = factor();
    while (*expr == '*' || *expr == '/') {
        char op = *expr;
        expr++;
        int next = factor();
        if (op == '*') result *= next;
        else result /= next;
    }
    return result;
}

int expression() {
    int result = term();
    while (*expr == '+' || *expr == '-') {
        char op = *expr;
        expr++;
        int next = term();
        if (op == '+') result += next;
        else result -= next;
    }
    return result;
}

int main() {
    char buffer[1024];
    fread(buffer, 1, sizeof(buffer) - 1, stdin);
    buffer[sizeof(buffer) - 1] = '\0';
    expr = buffer;
    
    printf("%d\n", expression());
    return 0;
}
