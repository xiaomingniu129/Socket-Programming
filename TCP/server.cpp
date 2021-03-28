#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h> 
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {
  /* step 0: check arguments */
  if (argc != 2) {
    printf("Using: ./server port\nExample: ./server 5005\n");
    return -1;
  }

  /* step 1: create socket at server-side */
  int listenfd; // server-side socket file descriptor, unix下一切都是文件, e.g. 0, 1, 2, ...
  if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) { // 用于监听的socket
    perror("socket");
    return -1;
  }

  /* step 2: bind socket with IP & port number */
  sockaddr_in servaddr; // Structure describing an Internet socket address: 用来表示ip和port number
  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET; // set protocol family
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY); // any IP address: 
  // servaddr.sin_addr.s_addr = inet_addr("192.168.190.134"); // set IP address
  servaddr.sin_port = htons(atoi(argv[1])); // set port number, htons(): 使用网络字节顺序

  /* bind */
  if (bind(listenfd, (sockaddr *)&servaddr, sizeof(servaddr)) != 0) {
    perror("bind");
    close(listenfd);
    return -1;
  }

  /* step3: set socket into listen mode */
  /*
    int listen(int sockfd, int backlog);
    DESCRIPTION:
      listen() marks the socket referred to by sockfd as a passive socket, that is, 
      as a socket that will be used to accept in‐coming connection requests using accept(2).
    ARGUMENTS:
      1. The sockfd argument is a file descriptor that refers to a socket of type SOCK_STREAM or SOCK_SEQPACKET.
      2. The backlog argument defines the max length to which the queue of the pending connections for sockfd may grow.
         If a con‐nection request arrives when the queue is full, the client may
         receive an error with an indication of ECONNREFUSED or, if the
         underlying protocol supports retransmission, the  request  may
         be ignored so that a later reattempt at connection succeeds.
  */
  if (listen(listenfd, 5) != 0) {
    perror("listen");
    close(listenfd);
    return -1;
  }

  /* step4: accept client */
  int clientfd; // client-side socket: 用于通信的scoket
  int socklen = sizeof(sockaddr_in);
  sockaddr_in clientaddr; // 用于存放客户端的ip

  clientfd = accept(listenfd, (sockaddr *)&clientaddr, (socklen_t*)&socklen); // child socket与服务端通信
  // 从已准备好的连接队列中获取一个请求，如果队列为空，accept函数将阻塞等待
  // It extracts the first connection request on the queue of pending connections for the
  // listening socket(sockfd) creates a new connected socket, and returns a new file descriptor referring to that socket.

  printf("client (%s) is connected\n", inet_ntoa(clientaddr.sin_addr));

  /* step5: communicate with client: after receive the request from client, reply "ok" */
  char buffer[1024];
  while (1) {
    int iret;
    memset(buffer, 0, sizeof(buffer));
    if ((iret = recv(clientfd, buffer, sizeof(buffer), 0)) <= 0) { // receive the request from client
      printf("iret = %d\n", iret);
      break;
    }
    printf("receive: %s\n", buffer);

    strcpy(buffer, "ok");
    if ((iret = send(clientfd, buffer, strlen(buffer), 0)) <= 0) { // send response to client
      perror("send");
      break;
    }
    printf("reply: %s\n", buffer);
  }

  /* step 6: close socket */
  close(listenfd);
  close(clientfd);

  return 0;
}