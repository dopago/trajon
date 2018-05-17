#include "server.h"
#include <stdio.h>
#include <string.h>
#include <Winsock2.h>
#pragma comment(lib, "ws2_32.lib")
 
SOCKET sockClient;     // 客户端 Scoket

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
            // 如果是结束标志则停止写入
            if (strcmp(recvBuf, "%%over%%") == 0)
            {
                CloseHandle(hFile);
                break;
            }
            // 将数据写入到本地的文件
            WriteFile(hFile,recvBuf,strlen(recvBuf),&count,0);
        }
    }
}
 
DWORD WINAPI DataThreadProc(LPVOID lpThreadParameter)
{
    int num = 0, len = sizeof(SOCKADDR);
    SOCKET sockClient;          
    SOCKADDR_IN addrClient;     // 客户端地址
    SOCKET sockServerData;     
    SOCKADDR_IN addrServerData; 
 
    sockServerData = socket(AF_INET, SOCK_STREAM, 0);              
    addrServerData.sin_addr.S_un.S_addr = inet_addr("192.168.21.144");// 设置本机IP
    addrServerData.sin_family = AF_INET;                           
    addrServerData.sin_port = htons(6008);                          // 绑定端口6000
 
    bind(sockServerData, (SOCKADDR *)&addrServerData, sizeof(SOCKADDR)); 
    listen(sockServerData, 5); 
 
    while(1)
    {
        // 等待客户端连接
        sockClient = accept(sockServerData, (SOCKADDR *)&addrClient, &len);
 
        printf("n准备接收传输文件\n");
 
        FileRecv(sockClient, "C:\1.txt");
 
        printf("文件接受完毕\n>>");
    }
    return 0;
}
 
DWORD WINAPI ThreadProc (LPVOID lpThreadParameter)
{
    char recvBuf[1024] = {0};    //缓冲区
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
    DWORD Tid; // 线程 ID
 
    char  cmdStr[100] = {0};
    char sendBuf[100] = {0}; 
    char recvBuf[1024] = {0}; 
    char * ip;
 
    SOCKET sockServer;     
    SOCKADDR_IN addrServer;// 服务端地址    
    SOCKADDR_IN addrClient;// 客户端地址
 
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
    //  设置服务端 socket
    addrServer.sin_addr.S_un.S_addr = inet_addr("192.168.21.144"); // 本机IP
    addrServer.sin_family = AF_INET;                   
    addrServer.sin_port = htons(6000);                 // 绑定端口6000
   
    bind(sockServer, (SOCKADDR *)&addrServer, sizeof(SOCKADDR));
    // Listen 监听端口
    listen(sockServer, 5); 
 
    printf("服务器已启动:n监听中...n");
 
    // 等待客户端连接
    sockClient = accept(sockServer, (SOCKADDR *)&addrClient, &len);
    
    ip = inet_ntoa(addrClient.sin_addr);
    
    printf("-- IP %s 连接到服务端n", ip);
 
   
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
            break; // 如果用户输入 exit 则结束循环
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
