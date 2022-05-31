#ifndef EPOLLWORKER_H
#define EPOLLWORKER_H

#include "values.h"
#include <QObject>
#include <QMutex>
#include <QThread>
#include <QCoreApplication>

class EpollWorker : public QObject
{
    Q_OBJECT
public:
    EpollWorker();
    ~EpollWorker();

signals:
    void sendString(QString msg);

public slots:
    void EpollRun();
    void recvSock(int &sock);
    void close();

private:
    volatile bool thisAlive;
    int epfd;
    struct epoll_event ev, events[20];
    char sendMsg[BUFF_SIZE];
    char recvMsg[BUFF_SIZE*2];
    int server_sock;
    QMutex mutex;
};

#endif // EPOLLWORKER_H
