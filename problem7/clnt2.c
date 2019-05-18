#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>

#define BUF_SIZE 1024
#define EPOLL_SIZE 50
void error_handling(char *message);

int main(int argc, char *argv[])
{
	int sock, i;
	char message[BUF_SIZE];
	int str_len;
	struct sockaddr_in serv_adr;

	struct epoll_event *ep_events;
	struct epoll_event event;
	int epfd, event_cnt;

	if (argc != 3)
	{
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}

	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock == -1)
		error_handling("socket() error");

	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_adr.sin_port = htons(atoi(argv[2]));

	if (connect(sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1)
		error_handling("connect() error!");
	else
		puts("Connected...........");

	epfd = epoll_create(EPOLL_SIZE);
	ep_events = malloc(sizeof(struct epoll_event) * EPOLL_SIZE);

	event.events = EPOLLIN;
	event.data.fd = 0;
	epoll_ctl(epfd, EPOLL_CTL_ADD, 0, &event);
	//  stdin

	event.events = EPOLLIN;
	event.data.fd = sock;
	epoll_ctl(epfd, EPOLL_CTL_ADD, sock, &event);


	printf("select number(1:rock 2:paper 3:scissors  Q:quit)\n");
	while (1)
	{
		event_cnt = epoll_wait(epfd, ep_events, EPOLL_SIZE, -1);
		if (event_cnt == -1)
		{
			puts("epoll_wait() error");
			break;
		}
		for (i = 0; i < event_cnt; i++)
		{
			if (ep_events[i].data.fd == sock)
			{
				str_len = read(sock, message, BUF_SIZE - 1);
				if (str_len == 0)
					break;
				message[str_len] = 0;
				printf("Message from server: %s", message);
			}
			else if (ep_events[i].data.fd == 0)
			{
				
				//fgets(message, BUF_SIZE, stdin);

				str_len = read(0, message, BUF_SIZE);
				message[str_len] = 0;
				if (!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
					exit(1);
				else if (!strcmp(message, "1\n") || !strcmp(message, "2\n") || !strcmp(message, "3\n"))
					write(sock, message, strlen(message));
				else{
					printf("input again!\n");
					continue;
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