#include "data.h"
#include "defs.h"
#include "decl.h"

// get the position of c in string s
static int chrpos(char *s, int c) {
    char *p;

    p = strchr(s, c);
    return (p ? p - s : -1);
}

// get the next character
static int next(void) {
    int c;

    // if there is 1 character, Putback store it
    if(Putback) {
        c = Putback;
        Putback = 0;
        return c;
    }

    c = fgetc(Infile);
    if(c == '\n') {
        ++Line;
    }
    return c;
}

// put back an unwanted character
static void putback(int c) {
    Putback = c;
}

static int skip(void) {
    int c;

    c = next();
    while (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f')
    {
        c = next();
    }
    return c;
}

static int scanint(int c) {
    int k, val = 0;

    while ((k = chrpos("0123456789", c)) >= 0)
    {
        val = val * 10 + k;
        c = next();
    }
    
    putback(c);
    return val;
}

int scan(struct token *t) {
    int c;

    c = skip();

    switch(c) {
    case EOF:
        return 0;
    case '+':
        t->token = T_PLUS;
        break;
    case '-':
        t->token = T_MINUS;
        break;
    case '*':
        t->token = T_STAR;
        break;
    case '/':
        t->token = T_SLASH;
        break;
    default:
        if(isdigit(c)) {
            t->token = T_INTLIT;
            t->intvalue = c;
            break;
        }
        else {
            printf("Unrecognised character %c on line %d\n", c, Line);
            exit(1);
        }
    }
    return 1;
}
