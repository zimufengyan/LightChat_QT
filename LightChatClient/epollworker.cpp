#include "epollworker.h"


EpollWorker::EpollWorker()
{
    mutex.lock();
    thisAlive = true;
    mutex.unlock();
}

void EpollWorker::close()
{
    mutex.lock();
    thisAlive = false;
    mutex.unlock();
}

void EpollWorker::recvSock(int &sock)
{
    this->server_sock = sock;
}

void EpollWorker::EpollRun()
{
    epfd = epoll_create(1);
    ev.data.fd = this->server_sock;
    ev.events = EPOLLIN;
    epoll_ctl(epfd, EPOLL_CTL_ADD,  server_sock, &ev);
    int i, ret;

    while(thisAlive) {
        QCoreApplication::processEvents();
        mutex.lock();
        if (!thisAlive) {
            emit sendString("[INFO] child thread had exited.");
            mutex.unlock();
            return;
        }
        mutex.unlock();
        int nfds = epoll_wait(epfd, events, 12, -1);//等待epoll事件的发生
        for(i=0; i<nfds; ++i) {
//            if(events[i].events&EPOLLOUT) {//有数据发送，写socket
//                bzero(sendMsg, BUFF_SIZE);
//                ret = read(events[i].data.fd, recvMsg, BUFF_SIZE);
//                if (ret == 0){
//                    emit sendString("Failed to read.");
//                }
//                else{
//                    send(sockfd, recvMsg, BUFF_SIZE, 0);
//                    ev.data.fd = sockfd;
//                    ev.events = EPOLLIN|EPOLLET;
//                    epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);
//                }
//            }

            if(events[i].data.fd == server_sock) {//有数据到来，读socket
                bzero(recvMsg, BUFF_SIZE*2);
                ret = recv(server_sock, recvMsg, BUFF_SIZE*2, 0);
                if(ret < 0) {
                    emit sendString("Failed to recv!");
                }
                else if (ret == 0){
                    ::close(server_sock);
                    emit this->sendString("Server had exited.");
                }
                else {
                    ev.data.fd =  server_sock;
                    ev.events = EPOLLIN;
                    emit sendString(recvMsg);
                }
            }

        }
    }

}


EpollWorker::~EpollWorker()
{

}
