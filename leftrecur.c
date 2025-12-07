#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX 10

void eliminateLeftRecursion(char nonTerminal, char prods[][MAX], int n) {
    int i, j, k = 0, m = 0;
    char alpha[MAX][MAX], beta[MAX][MAX];
    int hasLeftRec = 0;

    for (i = 0; i < n; i++) {
        if (prods[i][0] == nonTerminal) {
            hasLeftRec = 1;
            strcpy(alpha[k++], prods[i] + 1);
        } else {
            strcpy(beta[m++], prods[i]);
        }
    }

    if (hasLeftRec) {
        printf("%c -> ", nonTerminal);
        for (i = 0; i < m; i++) {
            printf("%s%c' ", beta[i], nonTerminal);
            if (i != m - 1) printf("| ");
        }
        printf("\n%c' -> ", nonTerminal);
        for (i = 0; i < k; i++) {
            printf("%s%c' ", alpha[i], nonTerminal);
            if (i != k - 1) printf("| ");
        }
        printf("| ε\n");
    } else {
        printf("%c -> ", nonTerminal);
        for (i = 0; i < n; i++) {
            printf("%s", prods[i]);
            if (i != n - 1) printf(" | ");
        }
        printf("\n");
    }
}

void leftFactoring(char nonTerminal, char prods[][MAX], int n) {
    int i, j, k, maxPrefix = 0;
    char prefix[MAX], newProds[MAX][MAX], factored[MAX][MAX];
    int factoredCount = 0, newCount = 0;

    // Find the longest common prefix
    for (i = 0; i < n - 1; i++) {
        int len = 0;
        while (prods[i][len] && prods[i][len] == prods[i + 1][len]) len++;
        if (len > maxPrefix) maxPrefix = len;
    }

    if (maxPrefix == 0) {
        // No factoring needed
        for (i = 0; i < n; i++)
            strcpy(factored[factoredCount++], prods[i]);
    } else {
        strncpy(prefix, prods[0], maxPrefix);
        prefix[maxPrefix] = '\0';
        for (i = 0; i < n; i++) {
            if (strncmp(prods[i], prefix, maxPrefix) == 0) {
                strcpy(newProds[newCount++], prods[i] + maxPrefix);
            } else {
                strcpy(factored[factoredCount++], prods[i]);
            }
        }
        printf("%c -> %s%c' ", nonTerminal, prefix, nonTerminal);
        for (i = 0; i < factoredCount; i++)
            printf("| %s ", factored[i]);
        printf("\n");
        printf("%c' -> ", nonTerminal);
        for (i = 0; i < newCount; i++)
            printf("%s ", newProds[i][0] ? newProds[i] : "ε");
        printf("\n");
        return;
    }

    // Print the productions
    printf("%c -> ", nonTerminal);
    for (i = 0; i < factoredCount; i++) {
        printf("%s", factored[i]);
        if (i != factoredCount - 1) printf(" | ");
    }
    printf("\n");
}

int main() {
    int n, i, j, choice;
    char nonTerminal, prods[MAX][MAX];

    printf("Enter the number of productions: ");
    scanf("%d", &n);

    for (i = 0; i < n; i++) {
        printf("Enter production %d (format A->alpha): ", i + 1);
        scanf(" %c->%s", &nonTerminal, prods[i]);
    }

    printf("\nChoose operation:\n1. Eliminate Left Recursion\n2. Left Factoring\nEnter choice: ");
    scanf("%d", &choice);

    if (choice == 1) {
        eliminateLeftRecursion(nonTerminal, prods, n);
    } else if (choice == 2) {
        leftFactoring(nonTerminal, prods, n);
    } else {
        printf("Invalid choice.\n");
    }

    return 0;
}