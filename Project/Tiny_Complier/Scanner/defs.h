#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// Tokens Sign
enum {
    T_PLUS, T_MINUS, T_STAR, T_SLASH, T_INTLIT
};

// Token
struct token
{
    int token;
    int intvalue;   //if token == T_INTLIT, intvalue will store it
};
