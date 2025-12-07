#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAXTOK 100
#define TOKLEN 64

int addr = 100; /* starting address for relational TAC */

/* Utility: check if token is operator (arithmetic) */
int is_arith_op(const char *t) {
    return (strcmp(t, "+") == 0 || strcmp(t, "-") == 0 ||
            strcmp(t, "*") == 0 || strcmp(t, "/") == 0);
}

/* Precedence for arithmetic operators */
int prec(const char *op) {
    if (strcmp(op, "*") == 0 || strcmp(op, "/") == 0) return 2;
    if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0) return 1;
    return 0;
}

/* Tokenize input expression into tokens[]; returns count */
int tokenize(const char *expr, char tokens[][TOKLEN]) {
    int n = 0;
    int i = 0, L = strlen(expr);
    while (i < L) {
        if (isspace((unsigned char)expr[i])) { i++; continue; }
        if (isalpha((unsigned char)expr[i]) || isdigit((unsigned char)expr[i]) || expr[i]=='_') {
            int j = 0;
            while (i < L && (isalnum((unsigned char)expr[i]) || expr[i]=='_')) {
                tokens[n][j++] = expr[i++];
            }
            tokens[n][j] = '\0';
            n++;
            continue;
        }
        /* multi-char relational operators */
        if (i+1 < L) {
            if ((expr[i] == '=' && expr[i+1] == '=') ||
                (expr[i] == '!' && expr[i+1] == '=') ||
                (expr[i] == '<' && expr[i+1] == '=') ||
                (expr[i] == '>' && expr[i+1] == '=')) {
                tokens[n][0] = expr[i];
                tokens[n][1] = expr[i+1];
                tokens[n][2] = '\0';
                n++; i += 2; continue;
            }
        }
        /* single char token */
        tokens[n][0] = expr[i];
        tokens[n][1] = '\0';
        n++; i++;
    }
    return n;
}

/* Convert infix tokens (only arithmetic + - * / and parentheses) to postfix tokens */
int infix_to_postfix(char tokens[][TOKLEN], int nt, char out[][TOKLEN]) {
    char stack[MAXTOK][TOKLEN];
    int top = -1;
    int outn = 0;
    for (int i = 0; i < nt; i++) {
        char *tk = tokens[i];
        if (is_arith_op(tk) == 0 && strcmp(tk,"(") != 0 && strcmp(tk,")") != 0) {
            /* operand */
            strcpy(out[outn++], tk);
        } else if (is_arith_op(tk)) {
            while (top >= 0 && is_arith_op(stack[top]) && prec(stack[top]) >= prec(tk)) {
                strcpy(out[outn++], stack[top--]);
            }
            strcpy(stack[++top], tk);
        } else if (strcmp(tk, "(") == 0) {
            strcpy(stack[++top], tk);
        } else if (strcmp(tk, ")") == 0) {
            while (top >= 0 && strcmp(stack[top], "(") != 0) {
                strcpy(out[outn++], stack[top--]);
            }
            if (top >= 0 && strcmp(stack[top], "(") == 0) top--; /* pop '(' */
        }
    }
    while (top >= 0) {
        strcpy(out[outn++], stack[top--]);
    }
    return outn;
}

/* Generate TAC from postfix tokens, printing instructions. Returns name of temp holding result in outtemp (caller must free or use static) */
char *gen_tac_from_postfix(char out[][TOKLEN], int outn, int *temp_counter) {
    char stk[MAXTOK][TOKLEN];
    int top = -1;
    static char tempname[TOKLEN];
    for (int i = 0; i < outn; i++) {
        char *tk = out[i];
        if (!is_arith_op(tk)) {
            /* operand */
            strcpy(stk[++top], tk);
        } else {
            /* operator */
            if (top < 1) {
                printf("Error: invalid expression for TAC generation\n");
                return NULL;
            }
            char rhs[TOKLEN], lhs[TOKLEN];
            strcpy(rhs, stk[top--]);
            strcpy(lhs, stk[top--]);
            int id = (*temp_counter)++;
            char tname[TOKLEN];
            snprintf(tname, sizeof tname, "t%d", id);
            printf("%s = %s %s %s\n", tname, lhs, tk, rhs);
            strcpy(stk[++top], tname);
        }
    }
    if (top != 0) {
        printf("Error: postfix did not reduce to single result\n");
        return NULL;
    }
    strcpy(tempname, stk[top]);
    return tempname;
}

/* Trim leading/trailing whitespace copy into dst */
void trimcpy(char *dst, const char *src) {
    const char *s = src;
    while (isspace((unsigned char)*s)) s++;
    int len = strlen(s);
    while (len>0 && isspace((unsigned char)s[len-1])) len--;
    strncpy(dst, s, len);
    dst[len] = '\0';
}

int main(void) {
    int choice;
    int temp_counter = 1;

    while (1) {
        printf("\nMenu:\n1. assignment\n2. arithmetic expression (TAC only)\n3. relational expression\n4. Exit\nEnter the choice: ");
        if (scanf("%d", &choice) != 1) break;
        while (getchar() != '\n'); /* flush newline */

        if (choice == 1) {
            char line[256];
            printf("Enter the expression with assignment operator (e.g., a=b+c*d): ");
            if (!fgets(line, sizeof line, stdin)) break;
            /* remove newline */
            line[strcspn(line, "\n")] = '\0';
            char *eq = strchr(line, '=');
            if (!eq) {
                printf("No assignment operator found.\n");
                continue;
            }
            char lhs[128], rhs[128];
            strncpy(lhs, line, eq - line);
            lhs[eq - line] = '\0';
            strcpy(rhs, eq + 1);
            trimcpy(lhs, lhs);
            trimcpy(rhs, rhs);

            /* tokenize RHS and generate TAC */
            char tokens[MAXTOK][TOKLEN];
            int nt = tokenize(rhs, tokens);
            char postfix[MAXTOK][TOKLEN];
            int pn = infix_to_postfix(tokens, nt, postfix);
            printf("Three address code:\n");
            char *res = gen_tac_from_postfix(postfix, pn, &temp_counter);
            if (res) {
                printf("%s = %s\n", lhs, res); /* assign result temp to lhs */
            }
        }
        else if (choice == 2) {
            char line[256];
            printf("Enter the arithmetic expression (e.g., a+b*c): ");
            if (!fgets(line, sizeof line, stdin)) break;
            line[strcspn(line, "\n")] = '\0';
            trimcpy(line, line);

            char tokens[MAXTOK][TOKLEN];
            int nt = tokenize(line, tokens);
            char postfix[MAXTOK][TOKLEN];
            int pn = infix_to_postfix(tokens, nt, postfix);
            printf("Three address code:\n");
            char *res = gen_tac_from_postfix(postfix, pn, &temp_counter);
            if (res) {
                printf("Result in %s\n", res);
            }
        }
        else if (choice == 3) {
            char line[256];
            printf("Enter the relational expression (e.g., a < b or x<=y): ");
            if (!fgets(line, sizeof line, stdin)) break;
            line[strcspn(line, "\n")] = '\0';
            trimcpy(line, line);

            /* find relational operator */
            const char *ops[] = {"<=", ">=", "==", "!=", "<", ">"};
            char op[4] = {0};
            int found = 0;
            int pos = -1;
            for (int k = 0; k < (int)(sizeof ops / sizeof ops[0]); k++) {
                char *p = strstr(line, ops[k]);
                if (p) { strcpy(op, ops[k]); pos = p - line; found = 1; break; }
            }
            if (!found) {
                printf("No relational operator found or unsupported format.\n");
                continue;
            }
            char left[128], right[128];
            strncpy(left, line, pos); left[pos] = '\0';
            strcpy(right, line + pos + strlen(op));
            trimcpy(left, left);
            trimcpy(right, right);

            /* print conditional TAC using numeric addresses like original */
            printf("\n%d\tif %s %s %s goto %d\n", addr, left, op, right, addr+3); addr++;
            printf("%d\tT:=0\n", addr); addr++;
            printf("%d\tgoto %d\n", addr, addr+2); addr++;
            printf("%d\tT:=1\n", addr);
            addr++; /* prepare next addresses */
        }
        else if (choice == 4) {
            printf("Exiting.\n");
            break;
        } else {
            printf("Invalid choice.\n");
        }
    }

    return 0;
}
