
#include <stdio.h>
#include <unistd.h>
#include "json.c"


#define BUF_COUNT 4096
#define BUF_TYPE char
BUF_TYPE buf[BUF_COUNT];

int main()
{
    FILE* fp = fopen("test.json", "r");
    
    fread(buf, BUF_COUNT, sizeof(char), fp);
    init(buf);

    parse();

}
