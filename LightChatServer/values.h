#ifndef VALUES_H
#define VALUES_H

#include <iostream>
#include <string>
#include <cstring>
#include <strings.h>      // for bzero
#include <netinet/in.h>   // for souockaddr_in
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/select.h>
#include "sys/epoll.h"
#include "fcntl.h"

#define BUFF_SIZE 1024
#define backlog 7
#define MaxNum 12       // max number of connections
#define DefaultPORT 13531
#define DefaultIP "127.0.0.1"

#endif // VALUES_H
