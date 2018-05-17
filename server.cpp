#include "server.h"
#include <stdio.h>
#include <string.h>
#include <Winsock2.h>
#pragma comment(lib, "ws2_32.lib")
 
SOCKET sockClient;     // �ͻ��� Scoket

void FileRecv(SOCKET s, char *filename) 
{
    char recvBuf[1024] = {0};   
    HANDLE hFile;               
    DWORD count;                
 
    hFile = CreateFile(
        filename,               
        GENERIC_WRITE,          
        0,                      
        NULL,                   
        CREATE_NEW,             
        FILE_ATTRIBUTE_NORMAL,  
        NULL
    ); 
 
    while (1)
    {
        
        recv(s, recvBuf, 1024, 0);
        if (strlen(recvBuf) > 0)
        {
            // ����ǽ�����־��ֹͣд��
            if (strcmp(recvBuf, "%%over%%") == 0)
            {
                CloseHandle(hFile);
                break;
            }
            // ������д�뵽���ص��ļ�
            WriteFile(hFile,recvBuf,strlen(recvBuf),&count,0);
        }
    }
}
 
DWORD WINAPI DataThreadProc(LPVOID lpThreadParameter)
{
    int num = 0, len = sizeof(SOCKADDR);
    SOCKET sockClient;          
    SOCKADDR_IN addrClient;     // �ͻ��˵�ַ
    SOCKET sockServerData;     
    SOCKADDR_IN addrServerData; 
 
    sockServerData = socket(AF_INET, SOCK_STREAM, 0);              
    addrServerData.sin_addr.S_un.S_addr = inet_addr("192.168.21.144");// ���ñ���IP
    addrServerData.sin_family = AF_INET;                           
    addrServerData.sin_port = htons(6008);                          // �󶨶˿�6000
 
    bind(sockServerData, (SOCKADDR *)&addrServerData, sizeof(SOCKADDR)); 
    listen(sockServerData, 5); 
 
    while(1)
    {
        // �ȴ��ͻ�������
        sockClient = accept(sockServerData, (SOCKADDR *)&addrClient, &len);
 
        printf("n׼�����մ����ļ�\n");
 
        FileRecv(sockClient, "C:\1.txt");
 
        printf("�ļ��������\n>>");
    }
    return 0;
}
 
DWORD WINAPI ThreadProc (LPVOID lpThreadParameter)
{
    char recvBuf[1024] = {0};    //������
    int num = 0;
    while (1)
    {
        num = recv(sockClient, recvBuf, 1024, 0);
        if (num > 0)
        {
            printf("Receive:n%s\n>>", recvBuf);
            num = 0;
        }
        Sleep(50);
    }
    return 0;
}

void main()
{
    int err; 
    int len = sizeof(SOCKADDR);
    DWORD Tid; // �߳� ID
 
    char  cmdStr[100] = {0};
    char sendBuf[100] = {0}; 
    char recvBuf[1024] = {0}; 
    char * ip;
 
    SOCKET sockServer;     
    SOCKADDR_IN addrServer;// ����˵�ַ    
    SOCKADDR_IN addrClient;// �ͻ��˵�ַ
 
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
        
        WSACleanup();
        return;
    }
 
    
    sockServer = socket(AF_INET, SOCK_STREAM, 0);
    //  ���÷���� socket
    addrServer.sin_addr.S_un.S_addr = inet_addr("192.168.21.144"); // ����IP
    addrServer.sin_family = AF_INET;                   
    addrServer.sin_port = htons(6000);                 // �󶨶˿�6000
   
    bind(sockServer, (SOCKADDR *)&addrServer, sizeof(SOCKADDR));
    // Listen �����˿�
    listen(sockServer, 5); 
 
    printf("������������:n������...n");
 
    // �ȴ��ͻ�������
    sockClient = accept(sockServer, (SOCKADDR *)&addrClient, &len);
    
    ip = inet_ntoa(addrClient.sin_addr);
    
    printf("-- IP %s ���ӵ������n", ip);
 
   
    CreateThread(
        NULL,       
        0,         
        ThreadProc, 
        NULL,       
        0,          
        &Tid       
    );
  
    while (1)
    {
        printf(">>");
        gets(cmdStr);
             
       
        if (strcmp(cmdStr, "exit") == 0)
        {
            send(sockClient, cmdStr, strlen(cmdStr) + 1, 0);
            break; // ����û����� exit �����ѭ��
        }
        
        send(sockClient, cmdStr, strlen(cmdStr) + 1, 0);
    }
    closesocket(sockClient);
    WSACleanup();
}

server::server(void)
{
}


server::~server(void)
{
}
