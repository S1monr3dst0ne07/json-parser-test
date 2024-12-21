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

void ws()
{
    while (isSpace(*srcPtr)) 
        srcPtr++;
}

void error()
{
    printf("error\n");
    exit(0);
}

void expect(char c)
{
    ws();
    if (pop() != c)
        error();
    ws();
}

void cons(char c)
{
    ws();
    while (*srcPtr == c)
        srcPtr++; 
    ws();
}


void parse();


void parseObject()
{
    expect('{');
    while (peek() != '}')
    {
        ws();

        //should be string
        parse();

        expect(':');

        parse();

        cons(',');
    }

}

void parseArray()
{
    expect('[');
    while (peek() != ']')
    {
        ws();

        parse();

        cons(',');
    }
}

void parseString()
{
    expect('"');

    while (peek() != '"')
        pop();

    expect('"');

}


void parse()
{
    ws();
    
    switch(peek())
    {
        case '{': parseObject();    break;
        case '[': parseArray();     break;
        case '"': parseString();    break;
        default:
            error();
    }

}


