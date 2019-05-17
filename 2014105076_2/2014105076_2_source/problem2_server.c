#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
#define OPSZ 4

void error_handling(char *message);
int calculate(int opnum, int opnds[], char oprator);
void printData(int opnum, int opnds[], char oprator);
int main(int argc, char *argv[])
{
    int serv_sock, clnt_sock;
    char opinfo[BUF_SIZE];
    int result, opnd_cnt, i;
    int recv_len, recv_cnt;
    int read_cnt;
    char buf[BUF_SIZE];
    struct sockaddr_in serv_adr, clnt_adr;
    socklen_t clnt_adr_sz;

    FILE *srcfile;
    FILE *destfile;
    FILE *fp;

    char buffer[2] = {0};

    srcfile = fopen("data.txt", "rb");
    destfile = fopen("out.txt", "wb");

    while (1)
    {
        fread(buffer, 2, 1, srcfile);

        if (feof(srcfile) != 0)
        {
            break;
        }
        else
        {
            fwrite(buffer, 2, 1, destfile);
        }
    }

    fclose(destfile);
    fclose(srcfile);

    if (argc != 2)
    {
        printf("Usage : %s<port>\n", argv[0]);
        exit(1);
    }

    fp = freopen("out.txt", "a",stdout);
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1)
        error_handling("socket() erorr!");
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    if (bind(serv_sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("bind() erorr!");
    if (listen(serv_sock, 5) == -1)
        error_handling("listen() error!");
    clnt_adr_sz = sizeof(clnt_adr);
    printf("\n");
    clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_adr, &clnt_adr_sz);
    while (1)
    {
        opnd_cnt = 0;
        recv_len = 0;
    
        read(clnt_sock, &opnd_cnt, 1);
        printf("%d\n", opnd_cnt);
        if (opnd_cnt == 'q')
            break;
        

        while ((opnd_cnt * OPSZ + 1) > recv_len)
        {
            recv_cnt = read(clnt_sock, &opinfo[recv_len], BUF_SIZE - 1);
            recv_len += recv_cnt;
        }

        
        printData(opnd_cnt, (int *)opinfo, opinfo[recv_len - 1]);
        result = calculate(opnd_cnt, (int *)opinfo, opinfo[recv_len - 1]);
        write(clnt_sock, (char *)&result, sizeof(result));

    }
    close(clnt_sock);
    fclose(fp);
    close(serv_sock);
    return 0;
}
void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
int calculate(int opnum, int opnds[], char oprator)
{
    int result = opnds[0], i;
    switch (oprator)
    {
    case '+':
        for (i = 1; i < opnum; i++)
            result += opnds[i];
        break;
    case '-':
        for (i = 1; i < opnum; i++)
            result -= opnds[i];
        break;
    case '*':
        for (i = 1; i < opnum; i++)
            result *= opnds[i];
        break;
    }
    return result;
}
void printData(int opnum, int opnds[], char oprator)
{
    int result = opnds[0], i;
    for (i = 0; i < opnum; i++)
        printf("%d\n",opnds[i]);
    printf("%c\n",oprator);
}