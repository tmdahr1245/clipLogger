#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "time.h"
#include "sys/types.h"
#include "sys/socket.h"
#include "netinet/in.h"
 
#define BUF_LEN 1000024
 
int main(int argc, char *argv[])
{
	char buffer[BUF_LEN];
    	struct sockaddr_in server_addr, client_addr;
    	char temp[20];
    	int server_fd, client_fd;
    	int len, msg_size;
	
   	if(argc != 2){
        	printf("usage : %s [port]\n", argv[0]);
        	exit(0);
    	}
 
	if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        	printf("Server : Can't open stream socket\n");
        	exit(0);
    	}
    	memset(&server_addr, 0x00, sizeof(server_addr));
 
    	server_addr.sin_family = AF_INET;
    	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    	server_addr.sin_port = htons(atoi(argv[1]));
 
    	if(bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) <0){
        	printf("Server : Can't bind local address.\n");
        	exit(0);
    	}
 
    	if(listen(server_fd, 5) < 0){
        	printf("Server : Can't listening connect.\n");
        	exit(0);
    	}
 
    	printf("Server : wating connection request.\n");
    	len = sizeof(client_addr);
    
	memset(buffer, 0x00, sizeof(buffer));
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &len);
        if(client_fd < 0){
            printf("Server: accept failed.\n");
            exit(0);
        }
        inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, temp, sizeof(temp));
        printf("Server : %s client connected.\n", temp);
    	
	strcpy(buffer,"");
	time_t t=time(NULL);
	struct tm tm = *localtime(&t);
	char path[1024];
	sprintf(path,".//log//%d-%d-%d_%d:%d:%d_%s.log",tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday,tm.tm_hour, tm.tm_min, tm.tm_sec, temp);
	FILE* fp=fopen(path,"a+");
	while((msg_size=read(client_fd,buffer,BUF_LEN))!=0)
		fwrite(buffer,sizeof(char),msg_size,fp);
	fclose(fp);
        close(client_fd);
        printf("Server : %s client closed.\n", temp);
	    
    	close(server_fd);
    	return 0;
}
