#include "rawUdp.h"
void printData(uint8_t *rcvPacket)
{
  char str[MAX_LENGTH_MSG];
  extractData(rcvPacket, str);
  printf("%s\n", str);
}
void extractData(uint8_t *rcvPacket, char *dataStr)
{
  uint16_t lenRecv;
  memcpy(&lenRecv, &rcvPacket[IP_HEADER_OFFSET+4], sizeof(lenRecv));
  memcpy(dataStr, &rcvPacket[IP_HEADER_OFFSET+8], ntohs(lenRecv)-8);
}
int createPacket(uint8_t *sndPacket, char *data, int portSrc, int portDest)
{
  struct udpHeader udpHeader;
  int length = LENGTH_HEADING+strlen(data)+1;

  udpHeader.portSrc = htons(portSrc);
  udpHeader.portDest = htons(portDest);
  udpHeader.length = htons(length);
  udpHeader.checksum = 0;

  memcpy(sndPacket, &udpHeader, sizeof(udpHeader));
  memcpy(sndPacket+sizeof(data), data, strlen(data)+1);

  return length;
}

