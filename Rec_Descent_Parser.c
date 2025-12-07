#include <stdio.h>
#include <string.h>
#include <ctype.h>  // for isalpha and isalnum

int E(), Edash(), T(), Tdash(), F();
char *ip;
char string[50];

void skip_whitespace() {
    while (*ip == ' ' || *ip == '\t') {
        ip++;
    }
}

int main() {
    printf("Enter the string: ");
    scanf("%49s", string);  // safer scanf
    ip = string;

    printf("\n\nInput\tAction\n--------------------------------\n");

    skip_whitespace();
    if (E() && *ip == '\0') {
        printf("\n--------------------------------\n");
        printf("String is successfully parsed\n");
    } else {
        printf("\n--------------------------------\n");
        printf("Error in parsing the string\n");
    }
    return 0;
}

int E() {
    printf("%s\tE -> T E'\n", ip);
    if (T()) {
        if (Edash()) {
            return 1;
        }
    }
    return 0;
}

int Edash() {
    skip_whitespace();
    if (*ip == '+') {
        printf("%s\tE' -> + T E'\n", ip);
        ip++;
        skip_whitespace();
        if (T()) {
            if (Edash()) {
                return 1;
            }
        }
        return 0;
    } else {
        printf("%s\tE' -> ε\n", ip);
        return 1; // epsilon transition
    }
}

int T() {
    printf("%s\tT -> F T'\n", ip);
    if (F()) {
        if (Tdash()) {
            return 1;
        }
    }
    return 0;
}

int Tdash() {
    skip_whitespace();
    if (*ip == '*') {
        printf("%s\tT' -> * F T'\n", ip);
        ip++;
        skip_whitespace();
        if (F()) {
            if (Tdash()) {
                return 1;
            }
        }
        return 0;
    } else {
        printf("%s\tT' -> ε\n", ip);
        return 1; // epsilon transition
    }
}

int F() {
    skip_whitespace();
    if (*ip == '(') {
        printf("%s\tF -> ( E )\n", ip);
        ip++;
        skip_whitespace();
        if (E()) {
            skip_whitespace();
            if (*ip == ')') {
                ip++;
                return 1;
            }
        }
        return 0;
    } else if (isalpha(*ip)) {
        printf("%s\tF -> id\n", ip);
        while (isalnum(*ip)) {
            ip++;  // consume full identifier
        }
        return 1;
    }
    return 0;
}
