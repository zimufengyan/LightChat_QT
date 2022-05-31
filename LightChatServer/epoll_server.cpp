#include "values.h"

int init_server(char* ip, char* port)
{
    int sock = socket(AF_INET,SOCK_STREAM,0);
    if(sock<0){
            perror("socket");
            exit(2);
    }
    int opt = 1;
    //设置socket 先断开时 避免进入time_wait状态，属性SO_REUSEADDR,是使其地址能够重用
    if(setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt))<0)
    {
            perror("setsockopt");
            exit(3);
    }
    struct sockaddr_in local;
    local.sin_family = AF_INET;
    local.sin_port = htons(atoi(port));
    local.sin_addr.s_addr = inet_addr(ip);
    if(bind(sock,(struct sockaddr*)&local,sizeof(local))<0)
    {
            perror("bind");
            exit(4);
    }
    if(listen(sock,5)<0){
            perror("listen");
            exit(5);
    }
    printf("listen and bind succeed\n");
    return sock;
}
