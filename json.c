#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>





typedef struct _json_node_t
{
    struct _json_node_t* next;
    struct _json_node_t* prev;
    struct _json_node_t* sub;

    enum json_type_t
    {
        JSON_TYPE_NULL   = 0,
        JSON_TYPE_TRUE   = 1,
        JSON_TYPE_FALSE  = 2,
        JSON_TYPE_OBJECT = 3,
        JSON_TYPE_ARRAY  = 4,
        JSON_TYPE_NUMBER = 5,
        JSON_TYPE_STRING = 6,
    } type;

    union
    {
        char* string;
        int number;
        struct _json_node_t* name;
    } content;

} *json_node_t;
#define json_node_size sizeof(json_node_t)


json_node_t createJsonNode(enum json_type_t type)
{
    json_node_t node = malloc(json_node_size);
    memset(node, 0x0, json_node_size);
    node->type = type;
    return node;
}






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


json_node_t parseObject();
json_node_t parseArray();
json_node_t parseString();
json_node_t parse();


void linkNodes(json_node_t a, json_node_t b)
{
    if (a) a->next = b;
    if (b) b->prev = a;
}



json_node_t parseObject()
{
    json_node_t node;
    json_node_t prev = NULL;

    expect('{');
    while (peek() != '}')
    {
        node = createJsonNode(JSON_TYPE_OBJECT);
        linkNodes(node, prev);

        ws();
        node->content.name = parseString();
        expect(':');
        node->sub = parse();
        cons(',');

        prev = node;
    }
    cons('}');

    return node;
}

json_node_t parseArray()
{
    json_node_t node;
    json_node_t prev = NULL;

    expect('[');
    while (peek() != ']')
    {
        node = createJsonNode(JSON_TYPE_ARRAY);
        linkNodes(node, prev);

        ws();
        node->sub = parse();
        cons(',');

        prev = node;
    }
    cons(']');

    return node;
}

json_node_t parseString()
{
    json_node_t node = createJsonNode(JSON_TYPE_STRING);

    expect('"');

    char* strBase = srcPtr;

    while (peek() != '"')
        pop();

    int strLen = srcPtr - strBase;
    char* str = malloc(strLen + 1);
    memcpy(str, strBase, strLen);
    str[strLen] = '\0';
    node->content.string = str;

    expect('"');

    return node;
}


json_node_t parseInt()
{
    json_node_t node = createJsonNode(JSON_TYPE_NUMBER);

    int num = 0;
    while (isdigit(peek()))
        num = num * 10 + (pop() - '0');

    while (
      peek() != ',' &&
      peek() != '}' &&
      peek() != ']'
    ) srcPtr++;

    node->content.number = num;
}

json_node_t parseNeg()
{
    expect('-');

    json_node_t node = parseInt();
    node->content.number *= -1;

    return node;
}

json_node_t parseTrue()
{
    expectSeq("true");
    return createJsonNode(JSON_TYPE_TRUE);
}

json_node_t parseFalse()
{
    expectSeq("false");
    return createJsonNode(JSON_TYPE_FALSE);
}
json_node_t parseNull()
{
    expectSeq("null");
    return createJsonNode(JSON_TYPE_NULL);
}




json_node_t parse()
{
    ws();

    printf("%c\n", peek());
    
    switch(peek())
    {
        case '{': return parseObject();    break;
        case '[': return parseArray();     break;
        case '"': return parseString();    break;
        case '0' ... '9': 
        case '+': return parseInt();       break;
        case '-': return parseNeg();       break;
        case 't': return parseTrue();      break;
        case 'f': return parseFalse();     break;
        case 'n': return parseNull();      break;
        default:
            printf("parse(): invaild  char: '%c'\n", peek()); 
            error();
    }
}


