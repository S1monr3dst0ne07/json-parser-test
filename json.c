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
#define json_node_size sizeof(struct _json_node_t)


json_node_t createJsonNode(enum json_type_t type)
{
    json_node_t node = malloc(json_node_size);
    memset(node, 0x0, json_node_size);
    node->type = type;
    return node;
}


char* srcPtr = NULL;


char parsePeek()
{
    return *srcPtr;
}

char parsePop()
{
    return *srcPtr++;
}

#define isSpace(c) (c == ' ' || c == '\t' || c == '\n' || c == '\r')

void parseWhiteSpace()
{
    while (isSpace(*srcPtr)) 
        srcPtr++;
}

#define error() _error(__LINE__) 
void _error(int line)
{
    printf("error at line: %d\n", line);
    exit(0);
}


void parseExpect(char* c)
{
    parseWhiteSpace();
    for (; *c; c++)
        if (parsePop() != *c)
            error();
    parseWhiteSpace();
}



void parseCons(char c)
{
    parseWhiteSpace();
    while (*srcPtr == c)
        srcPtr++; 
    parseWhiteSpace();
}


json_node_t parseObject();
json_node_t parseArray();
json_node_t parseString();
json_node_t parseValue();


void linkNodes(json_node_t a, json_node_t b)
{
    if (a) a->next = b;
    if (b) b->prev = a;
}



json_node_t parseObject()
{
    json_node_t node;
    json_node_t prev = NULL;
    json_node_t base = NULL;

    parseExpect("{");
    while (parsePeek() != '}')
    {
        node = createJsonNode(JSON_TYPE_OBJECT);
        linkNodes(prev, node);
        if (!base) base = node;

        parseWhiteSpace();
        node->content.name = parseString();
        parseExpect(":");
        node->sub = parseValue();
        parseCons(',');

        prev = node;
    }
    parseCons('}');

    return base;
}

json_node_t parseArray()
{
    json_node_t node;
    json_node_t prev = NULL;
    json_node_t base = NULL;

    parseExpect("[");
    while (parsePeek() != ']')
    {
        node = createJsonNode(JSON_TYPE_ARRAY);
        linkNodes(prev, node);
        if (!base) base = node;

        parseWhiteSpace();
        node->sub = parseValue();
        parseCons(',');

        prev = node;
    }
    parseCons(']');

    return base;
}

json_node_t parseString()
{
    json_node_t node = createJsonNode(JSON_TYPE_STRING);

    parseExpect("\"");

    char* strBase = srcPtr;

    while (parsePeek() != '"')
        parsePop();

    int strLen = srcPtr - strBase;
    char* str = malloc(strLen + 1);
    memcpy(str, strBase, strLen);
    str[strLen] = '\0';
    node->content.string = str;

    parseExpect("\"");

    return node;
}


json_node_t parseInt()
{
    json_node_t node = createJsonNode(JSON_TYPE_NUMBER);

    int num = 0;
    while (isdigit(parsePeek()))
        num = num * 10 + (parsePop() - '0');

    node->content.number = num;
}

json_node_t parseNeg()
{
    parseExpect("-");

    json_node_t node = parseInt();
    node->content.number *= -1;

    return node;
}

json_node_t parseTrue()
{
    parseExpect("true");
    return createJsonNode(JSON_TYPE_TRUE);
}

json_node_t parseFalse()
{
    parseExpect("false");
    return createJsonNode(JSON_TYPE_FALSE);
}
json_node_t parseNull()
{
    parseExpect("null");
    return createJsonNode(JSON_TYPE_NULL);
}

json_node_t parseValue()
{
    parseWhiteSpace();

    
    switch(parsePeek())
    {
        case '{': return parseObject();
        case '[': return parseArray();
        case '"': return parseString();
        case '0' ... '9': 
        case '+': return parseInt();
        case '-': return parseNeg();
        case 't': return parseTrue();
        case 'f': return parseFalse();
        case 'n': return parseNull();
        default:
            printf("parseValue(): invaild  char: '%c'\n", parsePeek()); 
            error();
    }
}

json_node_t parse(char* src)
{
    srcPtr = src;
    return parseValue();
}


char* dstPtr = NULL;

void generEmit(char* c)
{
    for (; *c; c++)
        *dstPtr++ = *c;
}

#define generNull  generEmit("null")
#define generTrue  generEmit("true")
#define generFalse generEmit("false")


void generValue(json_node_t node) 
{
    if (!node)
        generNull;

    switch(node->type)
    {
        case JSON_TYPE_NULL:  generNull; break;
        case JSON_TYPE_TRUE:  generTrue; break;
        case JSON_TYPE_FALSE: generFalse; break;
        case JSON_TYPE_OBJECT:
            if (!node->prev)
                generEmit("{ ");

            generValue(node->content.name);
            generEmit(" : ");
            generValue(node->sub);

            if (!node->next) generEmit(" }");
            else
            {
                generEmit(", ");
                generValue(node->next);
            }
            break;

        case JSON_TYPE_ARRAY:
            if (!node->prev)
                generEmit("[ ");

            generValue(node->sub);

            if (!node->next) generEmit(" ]");
            else
            {
                generEmit(", ");
                generValue(node->next);
            }
            break;

        case JSON_TYPE_NUMBER:
            static char buf[32] = { '\0' };
            char* p = &buf[sizeof(buf)];
            int num = node->content.number;
            if (!num) *(--p) = '0';
            if (num < 0)
            {
                generEmit("-");
                num = -num;
            }
            for (; num; num /= 10)
                *(--p) = (num % 10) + '0';

            generEmit(p); 

            break;

        case JSON_TYPE_STRING:
            generEmit("\"");
            generEmit(node->content.string);
            generEmit("\"");
            break;
            
    }
}


void gener(json_node_t node, char* dst)
{
    dstPtr = dst;
    generValue(node);
    *dstPtr = '\0';
}



void delete(json_node_t node)
{
    switch(node->type)
    {
        case JSON_TYPE_NULL:
        case JSON_TYPE_TRUE:
        case JSON_TYPE_FALSE:
        case JSON_TYPE_NUMBER:
            break;
        case JSON_TYPE_OBJECT:
            free(node->content.name);
        case JSON_TYPE_ARRAY:
            if (node->next) delete(node->next);
            delete(node->sub);
            break;
        case JSON_TYPE_STRING:
            free(node->content.string);
            break;
    }

    free(node);
}








