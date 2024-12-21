#include <stdlib.h>
#include <stdio.h>


char* srcPtr = NULL;

void init(char* src)
{
    srcPtr = src;
}

char peek()
{
    return *srcPtr;
}

char pop()
{
    return *srcPtr++;
}

#define isSpace(c) (c == ' ' || c == '\t' || c == '\n' || c == '\r')

#define start
#define end

void ws()
{
    while (isSpace(*srcPtr)) 
        srcPtr++;
}

#define error() _error(__LINE__) 
void _error(int line)
{
    printf("error at line: %d\n", line);
    printf("dump:\n'%s'\n", srcPtr);
    exit(0);
}


void expect(char c)
{
    ws();
    if (pop() != c)
        error();
    ws();
}

void expectSeq(const char* c)
{
    for (; *c; c++)
        expect(*c);

}


void cons(char c)
{
    ws();
    while (*srcPtr == c)
        srcPtr++; 
    ws();
}


void parse();
void parseObject();
void parseArray();
void parseString();


void parseObject()
{
    start
    expect('{');
    while (peek() != '}')
    {
        ws();

        //should be string
        parseString();

        expect(':');

        parse();

        cons(',');
    }
    cons('}');

    end
}

void parseArray()
{
    start
    expect('[');
    while (peek() != ']')
    {
        ws();

        parse();

        cons(',');
    }
    cons(']');
    end
}

void parseString()
{
    start
    expect('"');

    while (peek() != '"')
        pop();

    expect('"');

    end
}

void parseInt()
{
    start

    int num = 0;
    while (peek() >= '0' && peek() <= '9')
        num = num * 10 + (pop() - '0');

    while (
      peek() != ',' &&
      peek() != '}' &&
      peek() != ']'
    ) srcPtr++;

    //printf("num: %d\n", num);

    end
}

void parseNeg()
{
    start

    expect('-');

    parseInt();


    end
}

void parseTrue()
{
    start
    expectSeq("true");
    end
}

void parseFalse()
{
    start
    expectSeq("false");
    end
}
void parseNull()
{
    start
    expectSeq("null");
    end
}




void parse()
{
    start
    
    ws();
    
    switch(peek())
    {
        case '{': parseObject();    break;
        case '[': parseArray();     break;
        case '"': parseString();    break;
        case '0' ... '9': 
        case '+': parseInt();       break;
        case '-': parseNeg();       break;
        case 't': parseTrue();      break;
        case 'f': parseFalse();     break;
        case 'n': parseNull();      break;
        default:
            printf("parse(): invaild  char: '%c'\n", peek()); 
            error();
    }

    end
}


