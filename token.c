#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Returns 'true' if the character is a delimiter.
bool isDelimiter(char ch)
{
    return (ch == ' ' || ch == '+' || ch == '-' || ch == '*' || ch == '/' ||
            ch == ',' || ch == ';' || ch == '>' || ch == '<' || ch == '=' ||
            ch == '(' || ch == ')' || ch == '[' || ch == ']' ||
            ch == '{' || ch == '}');
}

// Returns 'true' if the character is an operator.
bool isOperator(char ch)
{
    return (ch == '+' || ch == '-' || ch == '*' ||
            ch == '/' || ch == '>' || ch == '<' || ch == '=');
}

// Returns 'true' if the string is a valid identifier.
bool validIdentifier(char *str)
{
    if (!str || strlen(str) == 0)
        return false;

    if ((str[0] >= '0' && str[0] <= '9') || isDelimiter(str[0]))
        return false;

    return true;
}

// Returns 'true' if the string is a keyword.
bool isKeyword(char *str)
{
    const char *keywords[] = {
        "if", "else", "while", "do", "break", "continue", "int", "double", "float",
        "return", "char", "case", "sizeof", "long", "short", "typedef", "switch",
        "unsigned", "void", "static", "struct", "goto"};
    int count = sizeof(keywords) / sizeof(keywords[0]);

    for (int i = 0; i < count; i++)
    {
        if (strcmp(str, keywords[i]) == 0)
            return true;
    }
    return false;
}

// Returns 'true' if the string is an integer.
bool isInteger(char *str)
{
    if (!str || strlen(str) == 0)
        return false;

    int i = 0;
    if (str[0] == '-')
        i = 1; // allow negative sign at beginning

    for (; i < strlen(str); i++)
    {
        if (!(str[i] >= '0' && str[i] <= '9'))
            return false;
    }
    return true;
}

// Returns 'true' if the string is a real number.
bool isRealNumber(char *str)
{
    if (!str || strlen(str) == 0)
        return false;

    bool hasDecimal = false;
    int i = 0;
    if (str[0] == '-')
        i = 1;

    for (; i < strlen(str); i++)
    {
        if (str[i] == '.')
        {
            if (hasDecimal)
                return false; // more than one decimal
            hasDecimal = true;
        }
        else if (!(str[i] >= '0' && str[i] <= '9'))
        {
            return false;
        }
    }
    return hasDecimal;
}

// Extracts a substring from `left` to `right` (inclusive).
char *subString(char *str, int left, int right)
{
    int len = right - left + 1;
    char *subStr = (char *)malloc(len + 1);

    if (!subStr)
        return NULL;

    for (int i = 0; i < len; i++)
        subStr[i] = str[left + i];

    subStr[len] = '\0';
    return subStr;
}

// Parses the input string and classifies tokens.
void parse(char *str)
{
    int left = 0, right = 0;
    int len = strlen(str);

    while (right <= len && left <= right)
    {
        if (!isDelimiter(str[right]))
            right++;

        if (isDelimiter(str[right]) && left == right)
        {
            if (isOperator(str[right]))
                printf("'%c' IS AN OPERATOR\n", str[right]);
            right++;
            left = right;
        }
        else if ((isDelimiter(str[right]) && left != right) ||
                 (right == len && left != right))
        {
            char *subStr = subString(str, left, right - 1);

            if (isKeyword(subStr))
                printf("'%s' IS A KEYWORD\n", subStr);
            else if (isInteger(subStr))
                printf("'%s' IS AN INTEGER\n", subStr);
            else if (isRealNumber(subStr))
                printf("'%s' IS A REAL NUMBER\n", subStr);
            else if (validIdentifier(subStr))
                printf("'%s' IS A VALID IDENTIFIER\n", subStr);
            else
                printf("'%s' IS NOT A VALID IDENTIFIER\n", subStr);

            free(subStr);
            left = right;
        }
    }
}

// Driver function
int main()
{
    char str[100] = "int a = b + c; ";
    parse(str);
    return 0;
}
