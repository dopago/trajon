#include "Server.h"
#include <stdio.h>
#include <string.h>
#include <Winsock2.h>
#pragma comment(lib, "ws2_32.lib")
 
void main()
{
    int err; 
    int len = sizeof(SOCKADDR);
 
    char  cmdStr[100] = {0};
    char sendBuf[100] = {0}; 
    char recvBuf[100] = {0}; 
    char * ip;
 
    SOCKET sockServer;     
    SOCKADDR_IN addrServer;// 服务端地址
    SOCKET sockClient;     
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
        // 启动错误，程序结束
        WSACleanup(); 
        return;
    }
 
  
    sockServer = socket(AF_INET, SOCK_STREAM, 0);
    
    addrServer.sin_addr.S_un.S_addr = htonl(INADDR_ANY); // 本机IP
    addrServer.sin_family = AF_INET;                   
    addrServer.sin_port = htons(6000);                 // 绑定端口6000
    
    bind(sockServer, (SOCKADDR *)&addrServer, sizeof(SOCKADDR));
    
    listen(sockServer, 5); 
 
    printf("服务器已启动:n监听中...\n");
 
    
    sockClient = accept(sockServer, (SOCKADDR *)&addrClient, &len);
    
    ip = inet_ntoa(addrClient.sin_addr);
   
    printf("-- IP %s 连接到服务端\n", ip);
 
    while (1)
    {
        if (strcmp(cmdStr, "exit") == 0)
        {
            break;
        }
 
        printf("-- %s: %s\n>>", ip, recvBuf);
        gets(cmdStr);
 
        send(sockClient, cmdStr, strlen(cmdStr) + 1, 0);
        recv(sockClient, recvBuf, 100, 0);
    }
 
    closesocket(sockClient);
    WSACleanup();
}

Server::Server(void)
{
}


Server::~Server(void)
{
}
