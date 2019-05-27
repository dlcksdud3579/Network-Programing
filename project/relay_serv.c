#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define BUF_SIZE 100
#define MAX_CLNT 256

void * handle_clnt(void * arg);
void send_msg(char * msg, int len);
void write_novel(char * msg, int len, int source);
void error_handling(char * msg);
void send_novel(int dest);

int clnt_cnt=0;
int clnt_socks[MAX_CLNT];
int before = 0;
pthread_mutex_t mutx;

FILE * writeFP, *readFP;

int main(int argc, char *argv[])
{
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_adr, clnt_adr;
	int clnt_adr_sz;
	int before;

	pthread_t t_id;
	if(argc!=2) {
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}

	writeFP = fopen("novel.txt","a");
	readFP = fopen("novel.txt","r");


  
	pthread_mutex_init(&mutx, NULL);
	serv_sock=socket(PF_INET, SOCK_STREAM, 0);

	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET; 
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_adr.sin_port=htons(atoi(argv[1]));
	
	if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr))==-1)
		error_handling("bind() error");
	if(listen(serv_sock, 5)==-1)
		error_handling("listen() error");
	
	while(1)
	{
		clnt_adr_sz=sizeof(clnt_adr);
		clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr,&clnt_adr_sz);
		
		pthread_mutex_lock(&mutx);
		clnt_socks[clnt_cnt++]=clnt_sock;
		pthread_mutex_unlock(&mutx);
	
		pthread_create(&t_id, NULL, handle_clnt, (void*)&clnt_sock);
		pthread_detach(t_id);
		printf("Connected client sock:%d\n", clnt_sock);
	}

	fclose(writeFP);
	close(serv_sock);
	return 0;
}
	
void * handle_clnt(void * arg)
{
	int clnt_sock=*((int*)arg);
	int str_len=0, i;
	char msg[BUF_SIZE];

	send_msg("Come in new man\n",18);


	while((str_len=read(clnt_sock, msg, sizeof(msg)))!=0)
	{
		if(!strcmp(msg,"2"))
		{
			send_novel(clnt_sock);
		}
		else
		{
			write_novel(msg, str_len, clnt_sock);
		}
	}
	printf("Disconnected client: %d\n", clnt_sock);

	
	pthread_mutex_lock(&mutx);
	for(i=0; i<clnt_cnt; i++)   // remove disconnected client
	{
		if(clnt_sock==clnt_socks[i])
		{
			while(i++<clnt_cnt-1)
				clnt_socks[i]=clnt_socks[i+1];
			break;
		}
	}
	clnt_cnt--;
	pthread_mutex_unlock(&mutx);
	close(clnt_sock);
	return NULL;
}
void send_msg(char * msg, int len)   // send to all
{
	int i;
	pthread_mutex_lock(&mutx);
	for(i=0; i<clnt_cnt; i++)
		write(clnt_socks[i], msg, len);
	pthread_mutex_unlock(&mutx);
}
void write_novel(char * msg, int len, int source)   // send to all
{
	int i;
	if(source == before)
	{
		write(source, "no repeat\n", 10);
		return ;
	}

	pthread_mutex_lock(&mutx);
	for(i=0; i<clnt_cnt; i++)
		write(clnt_socks[i], msg, len);
	
	fprintf(writeFP,"%s",msg);
	fflush(writeFP);
	before = source;

	pthread_mutex_unlock(&mutx);
}
void error_handling(char * msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
void send_novel(int dest)   // send to all
{
	int i;
	char msg[100];
	
	rewind(readFP);
	while(!feof(readFP))
	{
		fgets(msg,100,readFP);
		write(dest, msg, sizeof(msg));
		printf("%s",msg);
		strcpy(msg,"");
		usleep(1000);
		fflush(NULL);
	}

}