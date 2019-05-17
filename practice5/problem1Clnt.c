#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>

#define BUF_SIZE 1024
void error_handling(char *message);

int main(int argc, char *argv[])
{
	fd_set reads, temps;
	int sock;
	char buf[BUF_SIZE];
	int result,str_len;
	struct sockaddr_in serv_adr;
	struct timeval timeout;

	FD_ZERO(&reads);
	FD_SET(0,&reads);


	if(argc!=3) {
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}
	
	sock=socket(PF_INET, SOCK_STREAM, 0);   
	if(sock==-1)
		error_handling("socket() error");
	
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_adr.sin_port=htons(atoi(argv[2]));

	
	if(connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
		error_handling("connect() error!");
	else
		puts("Connected...........");
	FD_SET(sock,&reads);
	while(1) 
	{	
		temps=reads;
		timeout.tv_sec =5;
		timeout.tv_usec= 0;
		;
		if ((result = select(sock+1,&temps,0,0,&timeout)) == -1)
            break;
		else if(result ==0)
			continue;
		else
		{
			for (int i = 0; i < sock + 1; i++)
			{
				if (FD_ISSET(i, &temps))
				{
					if(i == 0)
					{
						str_len = read(0,buf,BUF_SIZE);
						buf[str_len] =0;
						write(sock, buf, strlen(buf));
						if(!strcmp(buf,"q\n") || !strcmp(buf,"Q\n"))
							exit(1);
					}
					else
					{
						str_len=read(sock, buf, BUF_SIZE);
						buf[str_len]=0;
						printf("%s", buf);
					}
					
				}
			}
		
		}
		
	}
	
	close(sock);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}