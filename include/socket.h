#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
class Socket {
public:
  Socket();
  ~Socket();
  int createSocket();
  void setServerAddr(sockaddr_in temp);
  int bindSocket();
  int listenSocket();
  int acceptSocket();
  int  readSocket(char *buf);
  void writeSocket(char *buf);

private:
  int socketfd_;
  int connectfd_;
  int len_;
  sockaddr_in serverAddr_;
};