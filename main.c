
#include <stdio.h>
#include <unistd.h>
#include "json.c"


#define BUF_COUNT 4096
char buf[BUF_COUNT];

int main()
{
    FILE* fp = fopen("test.json", "r");
    
    read(fp, buf, BUF_COUNT);
    init(buf);

}
