
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
/*
 * Simplified register-based code generator
 * Reads quadruples from input file (default: input.txt or argv[1]) and writes
 * register-style instructions to output.txt using a single register R0.
 */

int main(int argc, char **argv) {
    const char *inpath = (argc > 1) ? argv[1] : "input.txt";
    const char *outpath = "output.txt";

    FILE *in = fopen(inpath, "r");
    if (!in) {
        fprintf(stderr, "error: could not open '%s' for reading\n", inpath);
        return 1;
    }
    FILE *out = fopen(outpath, "w");
    if (!out) {
        fprintf(stderr, "error: could not open '%s' for writing\n", outpath);
        fclose(in);
        return 1;
    }

    char line[512];
    char op[8], arg1[64], arg2[64], result[64];
    while (fgets(line, sizeof line, in)) {
        /* trim leading whitespace */
        char *p = line;
        while (*p && isspace((unsigned char)*p)) p++;
        if (*p == '\0' || *p == '\n' || *p == '#') continue; /* blank or comment */

        /* parse tokens from the line */
        int items = sscanf(p, "%7s %63s %63s %63s", op, arg1, arg2, result);
        if (items <= 0) continue; /* nothing parsed */
        if (items < 2) continue;  /* malformed, skip */

        if (strcmp(op, "+") == 0) {
            fprintf(out, "MOV R0,%s\n", arg1);
            fprintf(out, "ADD R0,%s\n", arg2);
            fprintf(out, "MOV %s,R0\n", result);
            printf("MOV R0,%s\nADD R0,%s\nMOV %s,R0\n", arg1, arg2, result);
        } else if (strcmp(op, "*") == 0) {
            fprintf(out, "MOV R0,%s\n", arg1);
            fprintf(out, "MUL R0,%s\n", arg2);
            fprintf(out, "MOV %s,R0\n", result);
            printf("MOV R0,%s\nMUL R0,%s\nMOV %s,R0\n", arg1, arg2, result);
        } else if (strcmp(op, "-") == 0) {
            fprintf(out, "MOV R0,%s\n", arg1);
            fprintf(out, "SUB R0,%s\n", arg2);
            fprintf(out, "MOV %s,R0\n", result);
            printf("MOV R0,%s\nSUB R0,%s\nMOV %s,R0\n", arg1, arg2, result);
        } else if (strcmp(op, "/") == 0) {
            fprintf(out, "MOV R0,%s\n", arg1);
            fprintf(out, "DIV R0,%s\n", arg2);
            fprintf(out, "MOV %s,R0\n", result);
            printf("MOV R0,%s\nDIV R0,%s\nMOV %s,R0\n", arg1, arg2, result);
        } else if (strcmp(op, "=") == 0) {
            /* Prefer arg1 as source. If items==3 then arg2 holds result. */
            if (items == 3) {
                /* format: = src dest */
                fprintf(out, "MOV R0,%s\n", arg1);
                fprintf(out, "MOV %s,R0\n", arg2);
                printf("MOV R0,%s\nMOV %s,R0\n", arg1, arg2);
            } else {
                /* format: = arg1 arg2 result -> take arg1 -> result */
                fprintf(out, "MOV R0,%s\n", arg1);
                fprintf(out, "MOV %s,R0\n", result);
                printf("MOV R0,%s\nMOV %s,R0\n", arg1, result);
            }
        } else {
            /* unknown op: write comment */
            if (items == 4) {
                fprintf(out, "; UNKNOWN: %s %s %s %s\n", op, arg1, arg2, result);
                printf("; UNKNOWN: %s %s %s %s\n", op, arg1, arg2, result);
            } else {
                fprintf(out, "; UNKNOWN: %s (tokens=%d)\n", op, items);
                printf("; UNKNOWN: %s (tokens=%d)\n", op, items);
            }
        }
    }

    fclose(in);
    fclose(out);
    return 0;
}
