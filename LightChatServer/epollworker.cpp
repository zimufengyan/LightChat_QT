#include "epollworker.h"
#include "values.h"
#include <QThread>

EpollWorker::EpollWorker()
{
    thisAlive = false;
}

void EpollWorker::closeThread()
{
    thisAlive = true;
}

void EpollWorker::recvSock(int &sock)
{
    this->server = sock;
}

void EpollWorker::EpollRun()
{
    struct sockaddr_in client;
    bzero(&client, sizeof(client));

    int epfd = epoll_create(1);
    ep_event.events = EPOLLIN;
    // ep_event.data.fd = 0;
    // int ret = epoll_ctl(epfd,EPOLL_CTL_ADD,0, &ep_event);
    // ep_event.events = EPOLLIN;
    ep_event.data.fd = server;
    epoll_ctl(epfd, EPOLL_CTL_ADD, server, &ep_event);
    int newfd, ret;

    while(1) {
        int ret1 = epoll_wait(epfd, evs, MaxNum, -1);

        for(int i=0; i<ret1; i++) {
            if(evs[i].data.fd == server) {// a new client.
                int len = sizeof(struct sockaddr);
                bzero(&client, sizeof(sockaddr_in));
                newfd = accept(server, (struct sockaddr*)&client, (socklen_t *)&len);
                clients_in[conn_num] = client;
                if(newfd == -1) {
                    emit sendString("A Client failed to connect!");
                }
                else{
                    this->clients[conn_num++] = newfd;
                    this->setnoblock(newfd);
                    ep_event.events = EPOLLIN;
                    ep_event.data.fd = newfd;
                    epoll_ctl(epfd, EPOLL_CTL_ADD, newfd, &ep_event);
                    char msg[100];
                    sprintf(msg, "A new Client %s:%d, number of connections: %d",
                            inet_ntoa(client.sin_addr), ntohs(client.sin_port), conn_num);
                    emit sendString(msg);
                }
            }
            else {
                int fd = evs[i].data.fd;
                bzero(recvMsg, sizeof(recvMsg));
                ret = recv(fd, recvMsg, sizeof(recvMsg)-1, 0);
                if(ret == 0) {
                    emit sendString("A Client exited.");
                    for (int j=0;j<conn_num;j++){
                        if (this->clients[j] == newfd){
                            this->clients[j] = -1;
                        }
                    }
                    conn_num--;
                    ep_event.events = EPOLLIN;
                    ep_event.data.fd = fd;
                    epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &ep_event);
                    close(fd);
                }
                else{
                    bzero(sendMsg, sizeof(sendMsg));
                    int cnt = this->find_client(fd);
                    if (cnt == -1)continue;
                    client = clients_in[cnt];
                    sprintf(sendMsg, "from %s:%d : %s",
                            inet_ntoa(client.sin_addr), ntohs(client.sin_port), recvMsg);
                    emit sendString(sendMsg);
                    for(int j=0; j<conn_num;j++){
                        bzero(sendMsg, sizeof(sendMsg));
                        if (this->clients[j] == -1)continue;
                        if (this->clients[j] == fd){
                            sprintf(sendMsg, "myself: %s", recvMsg);
                        }
                        else{
                            sprintf(sendMsg, "from %s:%d : %s",
                                    inet_ntoa(client.sin_addr), ntohs(client.sin_port), recvMsg);
                        }
                        if(send(this->clients[j], sendMsg, strlen(sendMsg), 0) < 0){
                            emit sendString("Failed to broadcast.");
                        }
                    }
                }
            }

//            if(evs[i].data.fd == 0) {
//                bzero(sendMsg, sizeof(sendMsg));
//                ret = read(STDIN_FILENO, sendMsg, sizeof(sendMsg)-1);
//                if(ret == 0) {
//                    printf("bye!\n");
//                    ep_event.events = EPOLLIN|EPOLLET;
//                    ep_event.data.fd = newfd;

//                    epoll_ctl(epfd,EPOLL_CTL_DEL,newfd, &ep_event);
//                    close(newfd);
//                }
//                send(newfd, sendMsg, strlen(sendMsg)-1, 0);
//            }
        }
    }
}

void EpollWorker::setnoblock(int fd)
{
    int status;
    status = fcntl(fd,F_GETFL);
    status = status|O_NONBLOCK;
    fcntl(fd, F_SETFL, status);
}

int EpollWorker::find_client(int &fd)
{
    for(int i=0; i<conn_num; i++){
        if (this->clients[i] == fd){
            return i;
        }
    }
    return -1;
}
