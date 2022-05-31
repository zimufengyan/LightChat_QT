#ifndef EPOLLWORKER_H
#define EPOLLWORKER_H

#include "values.h"
#include <QObject>
#include <QThread>
#include <QString>

class EpollWorker : public QObject
{
    Q_OBJECT
public:
    EpollWorker();
    ~EpollWorker();
    void closeThread();
signals:
    void sendString(QString msg);

public slots:
    void EpollRun();
    void recvSock(int &sock);

private:
    void setnoblock(int fd);
    int find_client(int &fd);
    volatile bool thisAlive;
    int server;
    int conn_num = 0;
    int epfd;
    int clients[MaxNum] = {-1};
    int listen_sock, epoll_fd, control_sock=-1;
    char sendMsg[BUFF_SIZE];
    char recvMsg[BUFF_SIZE*2];
    struct sockaddr_in clients_in[MaxNum];
    struct epoll_event ep_event;
    struct epoll_event evs[128];
};

#endif // EPOLLWORKER_H
