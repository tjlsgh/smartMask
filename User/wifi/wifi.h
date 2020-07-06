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

/********************************** �û���Ҫ���õĲ���**********************************/
//Ҫ���ӵ��ȵ�����ƣ���WIFI����
#define      macUser_ESP8266_ApSsid           "MERCURY_D449" 

//Ҫ���ӵ��ȵ����Կ
#define      macUser_ESP8266_ApPwd            "13725702089" 

//Ҫ���ӵķ������� IP�������Ե�IP
#define      macUser_ESP8266_TcpServer_IP     "a104fcohulX.iot-as-mqtt.cn-shanghai.aliyuncs.com" 

//Ҫ���ӵķ������Ķ˿�
#define      macUser_ESP8266_TcpServer_Port    "1883"         


/********************************** �û���Ҫ���õĲ���**********************************/
#define   macUser_ESP8266_BulitApSsid         "YehuoLink"      //Ҫ�������ȵ������
#define   macUser_ESP8266_BulitApEcn           OPEN               //Ҫ�������ȵ�ļ��ܷ�ʽ
#define   macUser_ESP8266_BulitApPwd           "wildfire"         //Ҫ�������ȵ����Կ

#define   macUser_ESP8266_AP_IP                "192.168.4.1"      //������������IP��ַ
#define   macUser_ESP8266_AP_Port              "8086"             //�����������Ķ˿�   

#define   macUser_ESP8266_TcpServer_OverTime   "1800"             //��������ʱʱ�䣨��λ���룩


/********************************** �ⲿȫ�ֱ��� ***************************************/
extern volatile uint8_t ucTcpClosedFlag;



/********************************** ���Ժ������� ***************************************/
void                     ESP8266_StaTcpClient_UnvarnishTest  ( void );
int 										 wifiConfig 												 ( void );



#endif

