#ifndef RAW_UDP_H
#define RAW_UDP_H
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>

#define MAX_LENGTH_PACKET 65535
#define LENGTH_HEADING 8
#define IP_HEADER_OFFSET 20
#define MAX_LENGTH_MSG 64
struct udpHeader
{
  uint16_t portSrc;
  uint16_t portDest;
  uint16_t length;
  uint16_t checksum;
};

void printData(uint8_t *rcvPacket);
void extractData(uint8_t *rcvPacket, char *dataStr);
int createPacket(uint8_t *sndPacket, char *data, int portSrc, int portDest);
#endif
