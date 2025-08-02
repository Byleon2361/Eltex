#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_LENGTH_PACKET 65535
#define PORT_SRC 7777
#define PORT_DEST 7778
#define LENGTH_HEADING 8
#define IP_HEADER_OFFSET 20

void printData(uint8_t *rcvPacket)
{
  char dataRecv[MAX_LENGTH_PACKET];
  uint16_t lenRecv;
  memcpy(&lenRecv, &rcvPacket[IP_HEADER_OFFSET+4], sizeof(lenRecv));
  memcpy(dataRecv, &rcvPacket[IP_HEADER_OFFSET+8], lenRecv-8);

  printf("%s\n", dataRecv);
}
int createPacket(uint8_t *sndPacket, char *data)
{
  int length = LENGTH_HEADING+strlen(data)+1;
  uint16_t portSrc = htons(PORT_SRC);
  uint16_t portDest = htons(PORT_DEST);
  uint16_t lengthBigIndian = htons(length);
  uint16_t checksum = 0;

  memcpy(&sndPacket[0], &portSrc, sizeof(portSrc));
  memcpy(&sndPacket[2], &portDest, sizeof(portDest));
  memcpy(&sndPacket[4], &lengthBigIndian, sizeof(lengthBigIndian));
  memcpy(&sndPacket[6], &checksum, sizeof(checksum));
  memcpy(&sndPacket[8], data, strlen(data)+1);

  return length;
}
int main()
{
  struct sockaddr_in server;
  uint8_t rcvPacket[MAX_LENGTH_PACKET];
  uint8_t sndPacket[MAX_LENGTH_PACKET];
  char *data = "Hi";
  uint16_t srcPortRcvPacket = 0;
  int fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
  if(fd == -1)
  {
    perror("Error client create fd");
    exit(EXIT_FAILURE);
  }

  int length = createPacket(sndPacket, data);

  struct in_addr ip; 
  inet_pton(AF_INET, "127.0.0.1", &ip);
  memset(&server, 0, sizeof(server));
  server.sin_family = AF_INET;
  server.sin_addr = ip;

  struct sockaddr_in from;
  socklen_t socklen;
  if(sendto(fd, sndPacket, length, 0, (struct sockaddr *)&server, sizeof(server)) == -1)
  {
    perror("Error send");
    close(fd);
    exit(EXIT_FAILURE);
  }

  do
  {
    int bytes = recvfrom(fd, rcvPacket, MAX_LENGTH_PACKET, 0, (struct sockaddr *)&from, &socklen);
    if(bytes <= 0)
    {
      perror("Error recv");
      close(fd);
      exit(EXIT_FAILURE);
    }
    memcpy(&srcPortRcvPacket, &rcvPacket[IP_HEADER_OFFSET], sizeof(srcPortRcvPacket));
  } while(ntohs(srcPortRcvPacket) != PORT_DEST);

  printData(rcvPacket);

  close(fd);

  return 0;
}
