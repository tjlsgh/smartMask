#ifndef  __TEST_H
#define	 __TEST_H



#include "stm32f10x.h"
#include "bsp_esp8266.h"
#include "bsp_SysTick.h"
#include <stdio.h>  
#include <string.h>  
#include <stdbool.h>
#include "bsp_dht11.h"
#include "bsp_ili9341_lcd.h"
#include "mqtt.h"

/********************************** 用户需要设置的参数**********************************/
//要连接的热点的名称，即WIFI名称
#define      macUser_ESP8266_ApSsid           "MERCURY_D449" 

//要连接的热点的密钥
#define      macUser_ESP8266_ApPwd            "13725702089" 

//要连接的服务器的 IP，即电脑的IP
#define      macUser_ESP8266_TcpServer_IP     "a104fcohulX.iot-as-mqtt.cn-shanghai.aliyuncs.com" 

//要连接的服务器的端口
#define      macUser_ESP8266_TcpServer_Port    "1883"         


/********************************** 用户需要设置的参数**********************************/
#define   macUser_ESP8266_BulitApSsid         "YehuoLink"      //要建立的热点的名称
#define   macUser_ESP8266_BulitApEcn           OPEN               //要建立的热点的加密方式
#define   macUser_ESP8266_BulitApPwd           "wildfire"         //要建立的热点的密钥

#define   macUser_ESP8266_AP_IP                "192.168.4.1"      //服务器开启的IP地址
#define   macUser_ESP8266_AP_Port              "8086"             //服务器开启的端口   

#define   macUser_ESP8266_TcpServer_OverTime   "1800"             //服务器超时时间（单位：秒）


/********************************** 外部全局变量 ***************************************/
extern volatile uint8_t ucTcpClosedFlag;



/********************************** 测试函数声明 ***************************************/
void                     ESP8266_StaTcpClient_UnvarnishTest  ( void );
int 										 wifiConfig 												 ( void );



#endif

