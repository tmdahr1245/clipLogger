#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define LISTENQ 10
#define THREAD_NUM 10

void *func(void *arg);

int cntNum = 0;
struct sockaddr_in cliaddr;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char *argv[]) {
	struct sockaddr_in servaddr;
	int listen_sock, accp_sock[THREAD_NUM];
	int addrlen = sizeof(servaddr);
	int i;
	pthread_t tid[THREAD_NUM];
	pid_t pid;

	listen_sock = socket(PF_INET, SOCK_STREAM, 0);

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(atoi(argv[1]));

	bind(listen_sock, (struct sockaddr *)&servaddr, sizeof(servaddr));

	while(1) {
		listen(listen_sock, LISTENQ);
		printf("client wait....");
		accp_sock[cntNum] = accept(listen_sock, (struct sockaddr *)&cliaddr, &addrlen);

		pthread_create(&tid[cntNum], NULL, &func, (void *) &accp_sock[cntNum]);
		pthread_join(tid[cntNum], NULL);
		
		cntNum++;
		if(cntNum == 10)cntNum = 0;
	}
	return 0;
}
void *func(void *arg) {
	int accp_sock = (int) *((int*) arg);
	int msg_size,i;
	char buffer[1000024];
	
	char ip[20];

	pthread_mutex_lock(&lock);
	inet_ntop(AF_INET, &cliaddr.sin_addr.s_addr,ip,sizeof(ip));
	pthread_mutex_unlock(&lock);

	printf("%s client connected\n",ip);
		
	memset(buffer, 0x00, 1000024);

	strcpy(buffer,"");
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	char path[1024];
	sprintf(path,".//log//%d-%d-%d_%d:%d:%d_%s.log",tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday,tm.tm_hour,tm.tm_min,tm.tm_sec,ip);
	FILE* fp =fopen(path, "a+");

	while((msg_size=read(accp_sock, &buffer, 1000024))!=0){
		int len=strlen(buffer);
		for(i=0;i<len;i++)buffer[i] = buffer[i]^0x0F;
		fwrite(buffer,sizeof(char),msg_size,fp);
	}
	fclose(fp);
	printf("%s client closed\n",ip);
}