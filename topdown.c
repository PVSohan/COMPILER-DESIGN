/*
 * topdown.c
 * Brute-force top-down parser (leftmost derivation with backtracking).
 *
 * Usage: run the program and enter the grammar productions and the input string
 * as prompted. Productions format (one per line):
 *   S->aSb|ab
 * meaning nonterminal S has two productions: "aSb" and "ab".
 * Use '@' to represent epsilon (empty string) in RHS.
 *
 * The program attempts leftmost expansions (brute-force) up to a depth limit
 * and prints the successful leftmost derivation if the input is accepted.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_NT 256
#define MAX_PRODS_PER_NT 64
#define MAX_LEN 512

typedef struct {
    int count;
    char *rhs[MAX_PRODS_PER_NT];
} ProdList;

ProdList prods[MAX_NT];
char startSymbol = 0;
char target[MAX_LEN];
int maxDepth = 100;

void add_prod(char nt, const char *rhs) {
    unsigned char idx = (unsigned char)nt;
    if (prods[idx].count >= MAX_PRODS_PER_NT) return;
    /* strdup is not standard C; implement simple portable duplicate */
    size_t L = strlen(rhs) + 1;
    char *copy = (char*)malloc(L);
    if (!copy) return;
    memcpy(copy, rhs, L);
    prods[idx].rhs[prods[idx].count++] = copy;
}

int is_nonterminal(char c) {
    return (c >= 'A' && c <= 'Z');
}

/* Replace the first occurrence of a nonterminal at position pos in 'sent'
 * with the rhs string, writing result into out. Returns 1 on success. */
int replace_at(const char *sent, int pos, const char *rhs, char *out) {
    int len1 = (int)strlen(sent);
    int len2 = (int)strlen(rhs);
    /* resulting length = prefix (pos) + (rhs unless epsilon) + remainder (len1 - pos - 1) */
    int addlen = (len2 == 1 && rhs[0] == '@') ? 0 : len2;
    int reslen = pos + addlen + (len1 - pos - 1);
    if (reslen >= MAX_LEN) return 0; /* would overflow */

    /* copy prefix */
    if (pos > 0) memcpy(out, sent, pos);
    int cur = pos;
    /* append rhs unless it's epsilon '@' */
    if (!(len2 == 1 && rhs[0] == '@')) {
        memcpy(out + cur, rhs, len2);
        cur += len2;
    }
    /* append remainder after pos+1 */
    int rem = len1 - pos - 1;
    if (rem > 0) {
        memcpy(out + cur, sent + pos + 1, rem);
        cur += rem;
    }
    out[cur] = '\0';
    return 1;
}

/* Count terminals in sent (non-uppercase chars) */
int terminal_length(const char *sent) {
    int i, c = 0;
    for (i = 0; sent[i]; ++i) if (!is_nonterminal(sent[i])) c++;
    return c;
}

/* DFS leftmost expansion brute-force. path[] stores derivation steps; depth is current depth. */
int dfs(const char *sent, char path[][MAX_LEN], int pathLen, int depth) {
    /* prune by terminal length > target length */
    int tlen = terminal_length(sent);
    int tarlen = (int)strlen(target);
    if (tlen > tarlen) return 0;
    if (strcmp(sent, target) == 0) {
        /* success: print derivation */
        printf("Accepted. Leftmost derivation (%d steps):\n", pathLen);
        for (int i = 0; i < pathLen; ++i) printf("%s\n", path[i]);
        return 1;
    }
    if (depth >= maxDepth) return 0;

    /* find leftmost nonterminal */
    int i;
    for (i = 0; sent[i]; ++i) if (is_nonterminal(sent[i])) break;
    if (sent[i] == '\0') return 0; /* no nonterminals but not equal -> fail */

    unsigned char idx = (unsigned char)sent[i];
    ProdList *pl = &prods[idx];
    if (pl->count == 0) return 0; /* no productions for this nonterminal */

    char next[MAX_LEN];
    for (int p = 0; p < pl->count; ++p) {
        const char *rhs = pl->rhs[p];
        if (!replace_at(sent, i, rhs, next)) continue; /* overflow -> skip */
        /* skip if terminal count exceeds target */
        if (terminal_length(next) > tarlen) continue;
        /* record and recurse */
        strncpy(path[pathLen], next, MAX_LEN-1);
        path[pathLen][MAX_LEN-1] = '\0';
        if (dfs(next, path, pathLen+1, depth+1)) return 1;
    }
    return 0;
}

int main(void) {
    int n;
    printf("Brute-force Top-Down Parser\n");
    printf("Enter number of productions: ");
    if (scanf("%d%*c", &n) != 1) return 0;
    char line[1024];
    for (int i = 0; i < MAX_NT; ++i) prods[i].count = 0;
    for (int i = 0; i < n; ++i) {
        if (!fgets(line, sizeof line, stdin)) break;
        /* trim newline */
        char *nl = strchr(line, '\n'); if (nl) *nl = '\0';
        /* skip empty */
        char *p = line; while (*p && isspace((unsigned char)*p)) p++;
        if (*p == '\0') { i--; continue; }
        /* parse LHS -> RHS1 | RHS2 */
        char *arrow = strstr(p, "->");
        if (!arrow) { printf("Invalid production format: %s\n", p); return 1; }
        char lhs = p[0];
        if (!startSymbol) startSymbol = lhs;
        char *rhslist = arrow + 2;
        /* split at '|' */
        char *tok = strtok(rhslist, "|");
        while (tok) {
            /* trim spaces around tok */
            while (*tok && isspace((unsigned char)*tok)) tok++;
            char *end = tok + strlen(tok) - 1;
            while (end > tok && isspace((unsigned char)*end)) *end-- = '\0';
            add_prod(lhs, tok);
            tok = strtok(NULL, "|");
        }
    }

    printf("Start symbol: %c\n", startSymbol);
    printf("Enter input string: ");
    if (!fgets(line, sizeof line, stdin)) return 0;
    char *nl = strchr(line, '\n'); if (nl) *nl = '\0';
    strncpy(target, line, MAX_LEN-1);
    target[MAX_LEN-1] = '\0';

    /* prepare initial sentential form: start symbol */
    char startStr[2] = { startSymbol, '\0' };
    char path[1000][MAX_LEN];
    strncpy(path[0], startStr, MAX_LEN-1);

    /* set a reasonable depth: 2 * target length + 10 */
    maxDepth = 2 * (int)strlen(target) + 10;

    if (!dfs(startStr, path, 1, 0)) {
        printf("Rejected (no leftmost derivation found within depth %d).\n", maxDepth);
    }

    /* free allocated rhs strings */
    for (int i = 0; i < MAX_NT; ++i) {
        for (int j = 0; j < prods[i].count; ++j) free(prods[i].rhs[j]);
    }
    return 0;
}
