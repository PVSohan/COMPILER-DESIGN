#include <stdio.h>
#include <string.h>

/*
 * Simple shift-reduce (LR-like) parser simulator for the grammar:
 *   E -> 2 E 2
 *   E -> 3 E 3
 *   E -> 4
 *
 * This program reads an input string (e.g. "32423") and performs
 * shift and reduce actions, printing each step. It's a demonstrative
 * simulator — not a full general LR(1) parser generator.
 */

int main(void) {
    const char *grammar = "E->2E2 | E->3E3 | E->4";
    char input[128];
    char stack[128];
    int top = -1; /* index of top element in stack */

    printf("Grammar: %s\n", grammar);
    printf("Enter input string (e.g. 32423): ");
    if (!fgets(input, sizeof input, stdin)) return 0;
    /* trim newline */
    char *nl = strchr(input, '\n'); if (nl) *nl = '\0';
    int ip = 0; /* input pointer */
    int len = (int)strlen(input);

    printf("\nstack\tinput\taction\n");
    /* initial state */
    printf("$\t%s$\t\n", input);

    while (ip < len) {
        /* SHIFT */
        char ch = input[ip++];
        stack[++top] = ch;
        stack[top+1] = '\0';
        printf("$%s\t%s$\tSHIFT %c\n", stack, input + ip, ch);

        /* after a shift, try to reduce as much as possible */
        int reduced;
        do {
            reduced = 0;
            /* Rule E -> 4 */
            if (top >= 0 && stack[top] == '4') {
                printf("REDUCE: E -> 4\n");
                stack[top] = 'E'; /* replace '4' with 'E' */
                reduced = 1;
                continue;
            }
            /* Rule E -> 2 E 2 */
            if (top >= 2 && stack[top-2] == '2' && stack[top-1] == 'E' && stack[top] == '2') {
                printf("REDUCE: E -> 2E2\n");
                /* pop 3 and push E */
                top -= 2; /* remove two right elements; overwrite left with 'E' */
                stack[top] = 'E';
                stack[top+1] = '\0';
                reduced = 1;
                continue;
            }
            /* Rule E -> 3 E 3 */
            if (top >= 2 && stack[top-2] == '3' && stack[top-1] == 'E' && stack[top] == '3') {
                printf("REDUCE: E -> 3E3\n");
                top -= 2;
                stack[top] = 'E';
                stack[top+1] = '\0';
                reduced = 1;
                continue;
            }
        } while (reduced);
    }

    /* After input consumed, try reducing remaining stack */
    int changed;
    do {
        changed = 0;
        if (top >= 0 && stack[top] == '4') {
            printf("REDUCE: E -> 4\n");
            stack[top] = 'E'; changed = 1; continue;
        }
        if (top >= 2 && stack[top-2] == '2' && stack[top-1] == 'E' && stack[top] == '2') {
            printf("REDUCE: E -> 2E2\n"); top -= 2; stack[top] = 'E'; stack[top+1] = '\0'; changed = 1; continue;
        }
        if (top >= 2 && stack[top-2] == '3' && stack[top-1] == 'E' && stack[top] == '3') {
            printf("REDUCE: E -> 3E3\n"); top -= 2; stack[top] = 'E'; stack[top+1] = '\0'; changed = 1; continue;
        }
    } while (changed);

    if (top == 0 && stack[0] == 'E') {
        printf("\nAccept\n");
    } else {
        printf("\nReject\n");
    }

    return 0;
}
