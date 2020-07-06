#ifndef _MQTTTEST_H
#define _MQTTTEST_H


#include "stm32f10x.h"
#include "bsp_esp8266.h"
#include <string.h>
#include "MQTTConnect.h"
#include "MQTTPacket.h"
int transport_sendPacketBuffer(int sock, unsigned char* buf, int buflen);
int transport_getdata(unsigned char* buf, int count);
int transport_getdatanb(void *sck, unsigned char* buf, int count);
int transport_open(char* host, int port);
int transport_close(int sock);


#endif
