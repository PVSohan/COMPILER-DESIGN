#include <stdio.h>
#include <string.h>

#define MAX 30
//predictive parser
// Function declarations
int scant(char, char t1[], int t);
int scannt(char, char n1[], int nt);

/* Helpers for FIRST/FOLLOW sets */
int add_to_set(char set[][MAX], int idx, char ch) {
    // add ch to set[idx] if not present; return 1 if changed
    if (ch == '\0') return 0;
    char *s = set[idx];
    for (int i = 0; s[i]; i++) if (s[i] == ch) return 0;
    int len = strlen(s);
    s[len] = ch;
    s[len+1] = '\0';
    return 1;
}

int set_contains(char set[][MAX], int idx, char ch) {
    char *s = set[idx];
    for (int i = 0; s[i]; i++) if (s[i] == ch) return 1;
    return 0;
}

// Compute FIRST of a string alpha (given as chars array), put results into tmpset (as string), return 1 if epsilon present
int first_of_string(char alpha[], int start, int n, char n1[], int nt, char first[][MAX], char tmpset[]) {
    tmpset[0] = '\0';
    int hasEpsilonAll = 1;
    for (int k = start; k < n; k++) {
        char sym = alpha[k];
        if (sym == '\0') break;
        if (scannt(sym, n1, nt) == -1) {
            // terminal
            // add sym to tmpset
            if (!strchr(tmpset, sym)) {
                int l = strlen(tmpset); tmpset[l] = sym; tmpset[l+1] = '\0';
            }
            hasEpsilonAll = 0;
            break;
        } else {
            int idx = scannt(sym, n1, nt);
            // add FIRST(idx) excluding 'e'
            for (int p = 0; first[idx][p]; p++) {
                char ch = first[idx][p];
                if (ch != 'e' && !strchr(tmpset, ch)) {
                    int l = strlen(tmpset); tmpset[l] = ch; tmpset[l+1] = '\0';
                }
            }
            if (!set_contains(first, idx, 'e')) { hasEpsilonAll = 0; break; }
        }
    }
    return hasEpsilonAll;
}

int main() {
    int nt = 0, t = 0, m[20][20], i, s, n, p1, q, k, j;
    char p[MAX][MAX], n1[20], t1[20], ch, b, c, f[MAX][MAX], fl[MAX][MAX];

    // Input productions
    printf("Enter the number of productions: ");
    scanf("%d", &n);
    printf("Enter the productions one by one (e.g., E->E+T):\n");
    for (i = 0; i < n; i++) {
        scanf("%s", p[i]);
    }

    // Find non-terminals
    nt = 0;
    for (i = 0; i < n; i++) {
        if (scannt(p[i][0], n1, nt) == -1) {
            n1[nt++] = p[i][0];
        }
    }

    // Find terminals
    t = 0;
    for (i = 0; i < n; i++) {
        for (j = 3; j < strlen(p[i]); j++) {
            if (p[i][j] != 'e') {
                if (scannt(p[i][j], n1, nt) == -1) {
                    if (scant(p[i][j], t1, t) == -1) {
                        t1[t++] = p[i][j];
                    }
                }
            }
        }
    }
    t1[t++] = '$';

    // Initialize parsing table
    for (i = 0; i < nt; i++) {
        for (j = 0; j < t; j++) {
            m[i][j] = -1;
        }
    }
    // Compute FIRST sets automatically
    char first[MAX][MAX];
    for (i = 0; i < nt; i++) first[i][0] = '\0';

    int changed = 1;
    while (changed) {
        changed = 0;
        for (i = 0; i < n; i++) {
            char A = p[i][0];
            int Ai = scannt(A, n1, nt);
            // RHS starts at index 3
            // if first symbol terminal -> add it
            char sym = p[i][3];
            if (scannt(sym, n1, nt) == -1) {
                // terminal
                if (!set_contains(first, Ai, sym)) {
                    changed |= add_to_set(first, Ai, sym);
                }
            } else {
                // non-terminal: add its FIRST excluding 'e'
                int Bi = scannt(sym, n1, nt);
                for (int x=0; first[Bi][x]; x++) if (first[Bi][x] != 'e') changed |= add_to_set(first, Ai, first[Bi][x]);
                // if FIRST(Bi) contains epsilon, check next symbol(s)
                int kpos = 3;
                int allEpsilon = 1;
                while (p[i][kpos]) {
                    char s = p[i][kpos];
                    if (scannt(s, n1, nt) == -1) { allEpsilon = 0; break; }
                    int sidx = scannt(s, n1, nt);
                    if (!set_contains(first, sidx, 'e')) { allEpsilon = 0; break; }
                    kpos++;
                }
                if (allEpsilon) changed |= add_to_set(first, Ai, 'e');
            }
        }
    }

    // Compute FOLLOW sets
    char follow[MAX][MAX];
    for (i = 0; i < nt; i++) follow[i][0] = '\0';
    // add $ to start symbol
    add_to_set(follow, 0, '$');
    changed = 1;
    while (changed) {
        changed = 0;
        for (i = 0; i < n; i++) {
            char A = p[i][0];
            int Ai = scannt(A, n1, nt);
            // for each symbol B in RHS
            int len = strlen(p[i]);
            for (int pos = 3; pos < len; pos++) {
                char B = p[i][pos];
                int Bi = scannt(B, n1, nt);
                if (Bi == -1) continue;
                // compute FIRST of beta = rest after B
                char tmpset[MAX]; tmpset[0] = '\0';
                int hasEpsilon = first_of_string(p[i], pos+1, len, n1, nt, first, tmpset);
                // add tmpset to FOLLOW(B)
                for (int z2 = 0; tmpset[z2]; z2++) {
                    changed |= add_to_set(follow, Bi, tmpset[z2]);
                }
                if (hasEpsilon) {
                    // add FOLLOW(A) to FOLLOW(B)
                    for (int z3 = 0; follow[Ai][z3]; z3++) changed |= add_to_set(follow, Bi, follow[Ai][z3]);
                }
            }
        }
    }

    // Fill predictive parsing table M[A,a]
    for (i = 0; i < n; i++) {
        p1 = scannt(p[i][0], n1, nt);
        // compute FIRST of RHS
        char rhs_first[MAX]; rhs_first[0] = '\0';
        int eps = first_of_string(p[i], 3, strlen(p[i]), n1, nt, first, rhs_first);
        // for each terminal a in rhs_first, add production
        for (int z2 = 0; rhs_first[z2]; z2++) {
            if (rhs_first[z2] == 'e') continue;
            int col = scant(rhs_first[z2], t1, t);
            if (col != -1) m[p1][col] = i;
        }
        if (eps) {
            // for each b in FOLLOW(A) add production
            for (int z3 = 0; follow[p1][z3]; z3++) {
                int col = scant(follow[p1][z3], t1, t);
                if (col != -1) m[p1][col] = i;
            }
        }
    }

    // Print parsing table
    printf("\nParsing Table:\n\t");
    for (i = 0; i < t; i++) {
        printf("%c\t", t1[i]);
    }
    printf("\n");
    for (j = 0; j < nt; j++) {
        printf("%c\t", n1[j]);
        for (i = 0; i < t; i++) {
            if (m[j][i] != -1) {
                printf("%s\t", p[m[j][i]]);
            } else {
                printf(" \t");
            }
        }
        printf("\n");
    }

    return 0;
}

// Returns index of terminal in t1, or -1 if not found
int scant(char b, char t1[], int t) {
    int j;
    for (j = 0; j < t; j++) {
        if (t1[j] == b) {
            return j;
        }
    }
    return -1;
}

// Returns index of non-terminal in n1, or -1 if not found
int scannt(char a, char n1[], int nt) {
    int i;
    for (i = 0; i < nt; i++) {
        if (n1[i] == a) {
            return i;
        }
    }
    return -1;
}