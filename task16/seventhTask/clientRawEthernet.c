#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <net/if.h>
#include <net/ethernet.h>

#define MAX_LENGTH_PACKET 65535
#define PORT_SRC 7777
#define PORT_DEST 7778
#define UDP_HEADER_SIZE 8
#define IP_HEADER_SIZE 20
#define ETHERNET_HEADER_SIZE 14

#define IP_SRC "192.168.56.1"
#define IP_DEST "192.168.56.2"
#define INTERFACE "vboxnet0"

void printData(uint8_t *rcvPacket)
{
  char dataRecv[MAX_LENGTH_PACKET];
  uint16_t lenRecv;
  memcpy(&lenRecv, &rcvPacket[ETHERNET_HEADER_SIZE+IP_HEADER_SIZE+4], sizeof(lenRecv));
  memcpy(dataRecv, &rcvPacket[ETHERNET_HEADER_SIZE+IP_HEADER_SIZE+8], lenRecv-8);

  printf("%s\n", dataRecv);
}
uint16_t calcChecksum(const uint8_t *header, int lengthHeader)
{
  uint32_t checksum = 0;
  uint16_t temp = 0;
  uint16_t buf = 0;
  for(int i =0; i < lengthHeader; i+=2)
  {
    memcpy(&buf, &header[i], sizeof(buf));
    checksum += buf;
  }
  temp = checksum >> 16;
  checksum = (checksum & 0xFFFF) + temp;

  return (uint16_t)~checksum;
}
void createEthernetHeader(uint8_t *sndPacket)
{
  uint16_t type = htons(0x0800); //ipv4

  memset(sndPacket, 0, ETHERNET_HEADER_SIZE);
  sndPacket[0] = 0x08;
  sndPacket[1] = 0x00;
  sndPacket[2] = 0x27;
  sndPacket[3] = 0x83;
  sndPacket[4] = 0x57;
  sndPacket[5] = 0xae;

  sndPacket[6] = 0x0a;
  sndPacket[7] = 0x00;
  sndPacket[8] = 0x27;
  sndPacket[9] = 0x00;
  sndPacket[10] = 0x00;
  sndPacket[11] = 0x00;
  memcpy(&sndPacket[12], &type, sizeof(type));
  
}
void createIpHeader(uint8_t *sndPacket, int lengthUdp)
{

  int offset = ETHERNET_HEADER_SIZE;
  uint8_t versionAndIhl = 0;
  versionAndIhl = (versionAndIhl | 4) << 4;
  versionAndIhl |= 5;
  uint16_t lengthBigIndian = htons(lengthUdp+IP_HEADER_SIZE);
  uint8_t ttl = 255;
  uint8_t protocol = 17; //udp
  uint32_t ipSrc;
  inet_pton(AF_INET, IP_SRC, &ipSrc);
  uint32_t ipDest;
  inet_pton(AF_INET, IP_DEST, &ipDest);

  memset(sndPacket, 0, IP_HEADER_SIZE);
  memcpy(&sndPacket[0+offset], &versionAndIhl, sizeof(versionAndIhl));
  memcpy(&sndPacket[2+offset], &lengthBigIndian, sizeof(lengthBigIndian));

  memcpy(&sndPacket[8+offset], &ttl, sizeof(ttl));
  memcpy(&sndPacket[9+offset], &protocol, sizeof(protocol));
  memcpy(&sndPacket[12+offset], &ipSrc, sizeof(ipSrc));
  memcpy(&sndPacket[16+offset], &ipDest, sizeof(ipDest));

  uint16_t checksum = calcChecksum(&sndPacket[0+offset], IP_HEADER_SIZE);
  memcpy(&sndPacket[10+offset], &checksum, sizeof(checksum));
}
int createUdpHeader(uint8_t *sndPacket, char *data)
{
  int offset = IP_HEADER_SIZE + ETHERNET_HEADER_SIZE;
  int length = UDP_HEADER_SIZE+strlen(data)+1;
  uint16_t portSrc = htons(PORT_SRC);
  uint16_t portDest = htons(PORT_DEST);
  uint16_t lengthBigIndian = htons(length);
  uint16_t checksum = 0;

  memcpy(&sndPacket[0+offset], &portSrc, sizeof(portSrc));
  memcpy(&sndPacket[2+offset], &portDest, sizeof(portDest));
  memcpy(&sndPacket[4+offset], &lengthBigIndian, sizeof(lengthBigIndian));
  memcpy(&sndPacket[6+offset], &checksum, sizeof(checksum));
  memcpy(&sndPacket[8+offset], data, strlen(data)+1);

  return length;
}
int main()
{
  uint8_t rcvPacket[MAX_LENGTH_PACKET];
  uint8_t sndPacket[MAX_LENGTH_PACKET];
  char *data = "Hi";
  uint16_t srcPortRcvPacket = 0;
  int fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
  if(fd == -1)
  {
    perror("Error client create fd");
    exit(EXIT_FAILURE);
  }

  int length = createUdpHeader(sndPacket, data);
  createIpHeader(sndPacket, length);
  createEthernetHeader(sndPacket);

  struct sockaddr_ll server;
  memset(&server, 0, sizeof(server));
  server.sll_family = AF_PACKET;
  server.sll_ifindex = if_nametoindex(INTERFACE);
  server.sll_halen = 6;
  server.sll_addr[0] = 0x08;
  server.sll_addr[1] = 0x00;
  server.sll_addr[2] = 0x27;
  server.sll_addr[3] = 0x83;
  server.sll_addr[4] = 0x57;
  server.sll_addr[5] = 0xae;
  server.sll_addr[6] = 0x00;
  server.sll_addr[7] = 0x00;

  struct sockaddr_in from;
  socklen_t socklen;
  if(sendto(fd, sndPacket, ETHERNET_HEADER_SIZE+IP_HEADER_SIZE+length, 0, (struct sockaddr *)&server, sizeof(server)) == -1)
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
    memcpy(&srcPortRcvPacket, &rcvPacket[IP_HEADER_SIZE+ETHERNET_HEADER_SIZE], sizeof(srcPortRcvPacket));
  } while(ntohs(srcPortRcvPacket) != PORT_DEST);

  printData(rcvPacket);

  close(fd);

  return 0;
}
