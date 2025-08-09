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
  int length = LENGTH_HEADING+strlen(data)+1;
  uint16_t portSrcBigIndian = htons(portSrc);
  uint16_t portDestBigIndian = htons(portDest);
  uint16_t lengthBigIndian = htons(length);
  uint16_t checksum = 0;

  memcpy(&sndPacket[0], &portSrcBigIndian, sizeof(portSrcBigIndian));
  memcpy(&sndPacket[2], &portDestBigIndian, sizeof(portDestBigIndian));
  memcpy(&sndPacket[4], &lengthBigIndian, sizeof(lengthBigIndian));
  memcpy(&sndPacket[6], &checksum, sizeof(checksum));
  memcpy(&sndPacket[8], data, strlen(data)+1);

  return length;
}

