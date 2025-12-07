#include <stdio.h>
#include <string.h>

int main()
{
    char arithmetic[5] = {'+', '-', '*', '/', '%'};
    char relational[4] = {'<', '>', '!', '='};
    char bitwise[4] = {'&', '^', '~', '|'};
    char str[3]; // Increased size to 3 to hold 2 characters and null terminator

    printf("Enter value to be identified: ");
    scanf("%2s", str); // Limit input to 2 characters for safety

    int i;

    // Logical operators: &&, ||, !
    if (((str[0] == '&' || str[0] == '|') && str[0] == str[1]) || (str[0] == '!' && str[1] == '\0'))
    {
        printf("\nIt is a Logical operator");
    }

    // Relational operators: <, >, <=, >=, !=, ==
    for (i = 0; i < 4; i++)
    {
        if (str[0] == relational[i] && (str[1] == '=' || str[1] == '\0'))
        {
            printf("\nIt is a Relational operator");
            break;
        }
    }

    // Bitwise operators: &, |, ^, ~, <<, >>
    for (i = 0; i < 4; i++)
    {
        if ((str[0] == bitwise[i] && str[1] == '\0') ||
            ((str[0] == '<' || str[0] == '>') && str[1] == str[0]))
        {
            printf("\nIt is a Bitwise operator");
            break;
        }
    }

    // Ternary operator: ?:
    if (str[0] == '?' && str[1] == ':')
    {
        printf("\nIt is a Ternary operator");
    }

    // Unary and Arithmetic and Assignment
    for (i = 0; i < 5; i++)
    {
        // Unary: ++, --
        if ((str[0] == '+' || str[0] == '-') && str[0] == str[1])
        {
            printf("\nIt is a Unary operator");
            break;
        }
        // Assignment: +=, -=, *=, /=, %=, =
        else if ((str[0] == arithmetic[i] && str[1] == '=') ||
                 (str[0] == '=' && str[1] == '\0'))
        {
            printf("\nIt is an Assignment operator");
            break;
        }
        // Arithmetic: +, -, *, /, %
        else if (str[0] == arithmetic[i] && str[1] == '\0')
        {
            printf("\nIt is an Arithmetic operator");
            break;
        }
    }

    return 0;
}
