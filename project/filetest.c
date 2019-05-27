#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    FILE *fp;

    fp= fopen("novel.txt","w");
    //fsets(fp,"hello");
    fputs("asd",fp);
    fflush(fp);
    int c = getchar();
    fclose(fp);
    return 0;
}