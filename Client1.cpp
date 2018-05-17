#include "Client.h"

#include <Winsock2.h>
#include <stdio.h>
#pragma comment(lib, "ws2_32.lib")
 
#define MSG_LEN 1024
PROCESS_INFORMATION process_info;
 
BOOL strstart(char *text, char *begin);
BOOL CreateNotepad();
void WriteNotepadText(char *str);
void CloseNotepad();
 
int run(char *recvCmd, char *message)
{
    if (strcmp(recvCmd, "test") == 0)
    {
        strcpy(message, "服务端你好，有什么事吗~");
    }
    else if (strcmp(recvCmd, "open") == 0)
    {
        if(CreateNotepad())
        {
            strcpy(message, "记事本创建成功");
        } else {
            strcpy(message, "记事本创建失败!");
        }
    }
    else if (strcmp(recvCmd, "close") == 0)
    {
        CloseNotepad();
        strcpy(message, "Closed.");
    }
    else if (strstart(recvCmd, "write"))
    {
        WriteNotepadText(recvCmd + sizeof("write"));
        strcpy(message, "send");
    }
    else if (strcmp(recvCmd, "exit") == 0)
    {
        return 1;
    }
     
    return 0;
}
 
void main()
{
    int err = 0;
    char message[MSG_LEN] = {0};
    char recvCmd[100] = {0};
 
    SOCKET sockClient; // 客户端 Scoket
    SOCKADDR_IN addrServer; // 服务端地址
 
    WSADATA wsaData;
    WORD wVersionRequested;
 
    wVersionRequested = MAKEWORD( 2, 2 );
 
    err = WSAStartup( wVersionRequested, &wsaData );
 
    if ( err != 0 )
    {
        return;
    }
 
    if ( LOBYTE( wsaData.wVersion ) != 2 || HIBYTE( wsaData.wVersion ) != 2 )
    {
        // 启动错误，程序结束
        WSACleanup( );
        return;
    }
 
    
    sockClient = socket(AF_INET, SOCK_STREAM, 0);
 
   
    addrServer.sin_addr.S_un.S_addr = inet_addr("192.168.21.144");  // 目标IP 
    addrServer.sin_family = AF_INET;                           
    addrServer.sin_port = htons(6000);                         // 连接端口1234
 
    // 让 sockClient 连接到 服务端
    connect(sockClient, (SOCKADDR *)&addrServer, sizeof(SOCKADDR));
 
    while(1)
    {
        // 清空字符串
        ZeroMemory(recvCmd, sizeof(recvCmd));
        ZeroMemory(message, sizeof(message));
        
        recv(sockClient, recvCmd, MSG_LEN, 0);
       
        printf("-- 收到命令: [%s]\n", recvCmd);
 
        run(recvCmd, message);
 
        if (strlen(recvCmd) > 0)
        {
            
            send(sockClient, message, strlen(message) + 1, 0);
        }
    }
 
   
    closesocket(sockClient);
    WSACleanup();
}
 
/*
 * 创建一个记事本
 */
BOOL CreateNotepad()
{
    BOOL flag;
    STARTUPINFO start_info;
 
    ZeroMemory( &process_info, sizeof(process_info) );
    ZeroMemory( &start_info, sizeof(start_info) );
 
    start_info.cb = sizeof(start_info);
 
    flag = CreateProcess( 
        NULL,           
        "notepad.exe",  
        NULL,          
        NULL,           
        FALSE,          
        0,          
        NULL,         
        NULL,           
        &start_info,   
        &process_info );
 
    if ( !flag )
    {
        printf( "创建失败 Error: (%d).\n", GetLastError() );
        return 0;
    }
    return 1;
}
 
/*
 * 将字符串写到记事本的 RichEdit 控件中
 */
void WriteNotepadText(char *str)
{
    char text[250];
    HWND hNotepad = FindWindow(TEXT("Notepad"), NULL);
    HWND hEdit = FindWindowEx(hNotepad, NULL, TEXT("Edit"), NULL);
    wsprintf(text, "%srn", str);
    SendMessage(hEdit, EM_REPLACESEL, NULL, (LPARAM)text);
}
 
/*
 * 关闭记事本
 */
void CloseNotepad() 
{
   
    DWORD dwDesiredAccess = PROCESS_TERMINATE;
    BOOL  bInheritHandle  = FALSE;
    HANDLE hProcess = OpenProcess(dwDesiredAccess, bInheritHandle, process_info.dwProcessId);
    if (hProcess == NULL)
        return;
    TerminateProcess(hProcess, 0);
    CloseHandle(hProcess);
}
 
/*
 * 判断 text 字符串是否以 begin 字符串开头
 */
BOOL strstart(char *text, char *begin)
{
    while(*begin)
        if (*begin++ != *text++)
            return FALSE;
    return TRUE;
}
Client::Client(void)
{
}


Client::~Client(void)
{
}
