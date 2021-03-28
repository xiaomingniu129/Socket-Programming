#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

// 192.168.1.225
int main(int argc, char *argv[]) {
  /* step 0: check arguments */
  if (argc != 3) {
    printf("Using: ./client ip port\nExample: ./client 127.0.0.1 5005\n");
    return -1;
  }

  /* step 1: create client-side socket */
  int sockfd;
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("socket");
    return -1;
  }

  /* step 2: 向服务器发起连接请求 */
  hostent* h;
  if ((h = gethostbyname(argv[1])) == 0) { // 把ip地址或域名转换为 hostent 结构体表达的地址
    printf("gethostbyname failed\n");
    close(sockfd);
    return -1;
  }
  sockaddr_in servaddr; // Structure describing an Internet socket address
  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(atoi(argv[2]));
  memcpy(&servaddr.sin_addr, h->h_addr, h->h_length);

  if (connect(sockfd, (sockaddr *)&servaddr, sizeof(servaddr)) != 0) { // 向服务器发起连接请求
    perror("connect");
    close(sockfd);
    return -1;
  }

  /* step 3: communicate with server: send a request and wait for the reply */
  char buffer[1024];
  for (int i = 0; i < 3; i++) {
    int iret;
    memset(buffer, 0, sizeof(buffer));
    sprintf(buffer, "request %d", i + 1);
    if ((iret = send(sockfd, buffer, strlen(buffer), 0)) <= 0) { // send request to server
      perror("send\n");
      break;
    }
    printf("send: %s\n", buffer);

    memset(buffer, 0, sizeof(buffer));
    if ((iret = recv(sockfd, buffer, sizeof(buffer), 0)) <= 0) { // receive reply from server
      printf("iret = %d\n", iret);
      break;
    }
    printf("receive: %s\n", buffer);
  }

  /* step 4: close socket */
  close(sockfd);

  return 0;
}