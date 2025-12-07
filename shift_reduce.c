/*
 * shift_reduce.c
 * Simple shift-reduce parser simulator using a stack data structure.
 *
 * Usage:
 *  - Enter number of productions, then each production in the form A->rhs
 *    (no spaces). Use a single uppercase letter for nonterminals.
 *    Use 'e' as the RHS to denote epsilon (empty string).
 *  - Enter the input string (terminals only, no spaces).
 *GRAMMAR is -
       E->2E2
       E->3E3
       E->4      string 32423
 * The simulator shifts characters from the input to the stack and repeatedly
 * attempts reductions (preferring longer RHS matches). When input is
 * exhausted and no more reductions are possible, the string is accepted if
 * the stack contains only the start symbol.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_PRODS 64
#define MAX_RHS 128
#define MAX_INPUT 1024
#define STACK_SIZE 2048

typedef struct { char lhs; char rhs[MAX_RHS]; int rhs_len; } Prod;

int find_longest_match(Prod prods[], int pcount, const char stack[], int top) {
    /* find production whose RHS matches the stack suffix; prefer longer rhs */
    int best = -1;
    int bestlen = -1;
    for (int i = 0; i < pcount; ++i) {
        int rlen = prods[i].rhs_len;
        if (rlen == 1 && prods[i].rhs[0] == 'e') {
            /* epsilon can always match (length 0) */
            if (0 > bestlen) { best = i; bestlen = 0; }
            continue;
        }
        if (rlen <= top + 1) {
            int match = 1;
            for (int k = 0; k < rlen; ++k) {
                if (stack[top - rlen + 1 + k] != prods[i].rhs[k]) { match = 0; break; }
            }
            if (match && rlen > bestlen) { best = i; bestlen = rlen; }
        }
    }
    return best; /* -1 if none */
}

int main(void) {
    int n;
    printf("Shift-Reduce Parser Simulator\n");
    printf("Enter number of productions: ");
    if (scanf("%d%*c", &n) != 1 || n <= 0 || n > MAX_PRODS) {
        fprintf(stderr, "invalid number of productions\n");
        return 1;
    }

    Prod prods[MAX_PRODS];
    int pcount = 0;
    char line[512];
    char start = 0;

    for (int i = 0; i < n; ++i) {
        if (!fgets(line, sizeof line, stdin)) { fprintf(stderr, "EOF reading productions\n"); return 1; }
        /* trim newline and spaces */
        char *nl = strchr(line, '\n'); if (nl) *nl = '\0';
        char *p = line; while (*p && isspace((unsigned char)*p)) p++;
        if (*p == '\0') { i--; continue; }
        /* expect format A->rhs */
        if (!isupper((unsigned char)p[0]) || p[1] != '-' || p[2] != '>') {
            fprintf(stderr, "invalid production format: %s\n", p);
            return 1;
        }
        if (!start) start = p[0];
        prods[pcount].lhs = p[0];
        strncpy(prods[pcount].rhs, p + 3, MAX_RHS - 1);
        prods[pcount].rhs[MAX_RHS - 1] = '\0';
        prods[pcount].rhs_len = (int)strlen(prods[pcount].rhs);
        pcount++;
    }

    printf("Enter input string: ");
    if (!fgets(line, sizeof line, stdin)) return 0;
    char *nl = strchr(line, '\n'); if (nl) *nl = '\0';
    char input[MAX_INPUT]; strncpy(input, line, MAX_INPUT-1); input[MAX_INPUT-1] = '\0';
    int ip = 0, ilen = (int)strlen(input);

    char stack[STACK_SIZE]; int top = -1; /* stack[top] is top char */

    printf("\nInitial: stack=$\tinput=%s$\n", input);

    while (ip < ilen) {
        /* SHIFT */
        char ch = input[ip++];
        stack[++top] = ch; stack[top+1] = '\0';
        printf("ACTION: SHIFT '%c'\n", ch);
        printf("STACK: $%s$  INPUT: %s\n", stack, input + ip);

        /* try to reduce as much as possible (prefer longest rhs) */
        int reduced_any;
        do {
            reduced_any = 0;
            int pi = find_longest_match(prods, pcount, stack, top);
            if (pi != -1) {
                Prod *pr = &prods[pi];
                if (pr->rhs_len == 1 && pr->rhs[0] == 'e') {
                    /* epsilon production: push lhs without consuming input */
                    printf("ACTION: REDUCE %c -> e (epsilon)\n", pr->lhs);
                    stack[++top] = pr->lhs; stack[top+1] = '\0';
                    reduced_any = 1;
                } else {
                    printf("ACTION: REDUCE %c -> %s\n", pr->lhs, pr->rhs);
                    /* pop rhs_len elements */
                    top -= pr->rhs_len;
                    if (top < -1) top = -1;
                    /* push LHS */
                    stack[++top] = pr->lhs; stack[top+1] = '\0';
                    reduced_any = 1;
                }
                printf("STACK after reduce: $%s$  INPUT: %s\n", stack, input + ip);
            }
        } while (reduced_any);
    }

    /* After input consumed, perform reductions if possible */
    int changed;
    do {
        changed = 0;
        int pi = find_longest_match(prods, pcount, stack, top);
        if (pi != -1) {
            Prod *pr = &prods[pi];
            if (pr->rhs_len == 1 && pr->rhs[0] == 'e') {
                printf("ACTION: REDUCE %c -> e (epsilon)\n", pr->lhs);
                stack[++top] = pr->lhs; stack[top+1] = '\0'; changed = 1;
            } else {
                printf("ACTION: REDUCE %c -> %s\n", pr->lhs, pr->rhs);
                top -= pr->rhs_len; if (top < -1) top = -1; stack[++top] = pr->lhs; stack[top+1] = '\0'; changed = 1;
            }
            printf("STACK after reduce: $%s$  INPUT: %s\n", stack, "$");
        }
    } while (changed);

    if (top == 0 && stack[0] == start) {
        printf("\nString accepted.\n");
    } else {
        printf("\nString rejected.\n");
    }

    return 0;
}
