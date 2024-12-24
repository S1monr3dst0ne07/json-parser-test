
#include <stdio.h>
#include <unistd.h>
#include "json.c"


#define BUF_COUNT (1 << 16)
#define BUF_TYPE char
BUF_TYPE buf[BUF_COUNT];


int main()
{
    FILE* fp;
    fp = fopen("test.json", "r");
    fread(buf, BUF_COUNT, sizeof(char), fp);
    fclose(fp);

    json_node_t node = parse(buf);
    gener(node, buf);

    fp = fopen("out.json", "w");
    fwrite(buf, strlen(buf), sizeof(char), fp);
    fclose(fp);

    //delete(node);

    printf("%s\n", buf);

}
