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
  int listenfd; // server-side socket
  if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("socket");
    return -1;
  }

  /* step 2: bind socket with IP & Port Number */
  struct sockaddr_in servaddr; // data structure of server-side address
  memset(&servaddr, 0, sizeof(servaddr)); 
  servaddr.sin_family = AF_INET; // set protocol family
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY); // any IP address
  // servaddr.sin_addr.s_addr = inet_addr("192.168.190.134"); // specific IP address
  servaddr.sin_port = htons(atoi(argv[1])); // set port number
  if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0) {
    perror("bind");
    close(listenfd);
    return -1;
  }

  /* step3: set socket to listen mode */
  if (listen(listenfd, 5) != 0) {
    perror("listen");
    close(listenfd);
    return -1;
  }

  /* step4: accept client */
  int clientfd; // client-side socket
  int socklen = sizeof(struct sockaddr_in);
  struct sockaddr_in clientaddr;
  clientfd = accept(listenfd, (struct sockaddr *)&clientaddr, (socklen_t*)&socklen);
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
    if ((iret = send(clientfd, buffer, strlen(buffer), 0)) <= 0) {
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