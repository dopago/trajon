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
        strcpy(message, "�������ã���ʲô����~");
    }
    else if (strcmp(recvCmd, "open") == 0)
    {
        if(CreateNotepad())
        {
            strcpy(message, "���±������ɹ�");
        } else {
            strcpy(message, "���±�����ʧ��!");
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
 
    SOCKET sockClient; // �ͻ��� Scoket
    SOCKADDR_IN addrServer; // ����˵�ַ
 
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
        // �������󣬳������
        WSACleanup( );
        return;
    }
 
    
    sockClient = socket(AF_INET, SOCK_STREAM, 0);
 
   
    addrServer.sin_addr.S_un.S_addr = inet_addr("192.168.21.144");  // Ŀ��IP 
    addrServer.sin_family = AF_INET;                           
    addrServer.sin_port = htons(6000);                         // ���Ӷ˿�1234
 
    // �� sockClient ���ӵ� �����
    connect(sockClient, (SOCKADDR *)&addrServer, sizeof(SOCKADDR));
 
    while(1)
    {
        // ����ַ���
        ZeroMemory(recvCmd, sizeof(recvCmd));
        ZeroMemory(message, sizeof(message));
        
        recv(sockClient, recvCmd, MSG_LEN, 0);
       
        printf("-- �յ�����: [%s]\n", recvCmd);
 
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
 * ����һ�����±�
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
        printf( "����ʧ�� Error: (%d).\n", GetLastError() );
        return 0;
    }
    return 1;
}
 
/*
 * ���ַ���д�����±��� RichEdit �ؼ���
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
 * �رռ��±�
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
 * �ж� text �ַ����Ƿ��� begin �ַ�����ͷ
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
