#ifndef __MQTT_H_
#define __MQTT_H_

#if 1
#include "stm32f10x.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdarg.h"
#include "bsp_Systick.h"
#include "bsp_usart1.h"
#include "bsp_esp8266.h"

#define BYTE0(dwTemp)       (*( char *)(&dwTemp))
#define BYTE1(dwTemp)       (*((char *)(&dwTemp) + 1))
#define BYTE2(dwTemp)       (*((char *)(&dwTemp) + 2))
#define BYTE3(dwTemp)       (*((char *)(&dwTemp) + 3))
	
			/*mqttClientId: clientId+"|securemode=3,signmethod=hmacsha1,timestamp=132323232|"
			mqttUsername: deviceName+"&"+productKey
			mqttPassword: sign_hmac(deviceSecret,content)*/
#define mqttClientId        "111|securemode=3,signmethod=hmacsha1,timestamp=999|"
#define mqttUsername				"Smart_Mask&a104fcohulX"
#define	mqttPassword				"BF082E904FE31FF8B76D393209E545B2EBB8EEB2"
// /sys/a104fcohulX/smartmask/thing/event/property/post_reply
#define mqttTopicProPost_reply			"/sys/a104fcohulX/Smart_Mask/thing/event/property/post_reply"
#define mqttTopicProPost						"/sys/a104fcohulX/Smart_Mask/thing/event/property/post"
#define mqttTopicStopFan    				"/sys/a104fcohulX/Smart_Mask/thing/service/StopFan"
#define mqttTopicStopMotor    			"/sys/a104fcohulX/Smart_Mask/thing/service/StopShake"
#define mqttPayload                 "{\"method\":\"thing.event.property.post\",\"id\":\"630262306\",\"params\":{\
				                            \"AQI\":%d,\
				                            \"Temperature\":%d.%d\
			                              },\"version\":\"1.0.0\"}"


// /sys/a104fcohulX/Smart_Mask/thing/event/${tsl.event.identifer}/post
#define mqttTopicEventPost         "/sys/a104fcohulX/Smart_Mask/thing/event/Shake/post"



typedef struct
{
	u8 *rxbuf;u16 rxlen;
	u8 *txbuf;u16 txlen;
	void (*Init)(u8 *prx,u16 rxlen,u8 *ptx,u16 txlen);
	u8 (*Connect)(char *ClientID,char *Username,char *Password);
	u8 (*SubscribeTopic)(char *topic,u8 qos,u8 whether);
	u8 (*PublishData)(char *topic, char *message, u8 qos);
	void (*SendHeart)(void);
	void (*Disconnect)(void);
}_typdef_mqtt;

extern _typdef_mqtt _mqtt;
#else

#include "stm32f10x.h"
#include <string.h>
#define		MQTT_TypeCONNECT							1//��������  
#define		MQTT_TypeCONNACK							2//����Ӧ��  
#define		MQTT_TypePUBLISH							3//������Ϣ  
#define		MQTT_TypePUBACK								4//����Ӧ��  
#define		MQTT_TypePUBREC								5//�����ѽ��գ���֤����1  
#define		MQTT_TypePUBREL								6//�����ͷţ���֤����2  
#define		MQTT_TypePUBCOMP							7//������ɣ���֤����3  
#define		MQTT_TypeSUBSCRIBE						8//��������  
#define		MQTT_TypeSUBACK								9//����Ӧ��  
#define		MQTT_TypeUNSUBSCRIBE					10//ȡ������  
#define		MQTT_TypeUNSUBACK							11//ȡ������Ӧ��  
#define		MQTT_TypePINGREQ							12//ping����  
#define		MQTT_TypePINGRESP							13//ping��Ӧ  
#define		MQTT_TypeDISCONNECT 					14//�Ͽ�����  
 
#define		MQTT_StaCleanSession					1	//����Ự 
#define		MQTT_StaWillFlag							0	//������־
#define		MQTT_StaWillQoS								0	//����QoS���ӱ�־�ĵ�4�͵�3λ��
#define		MQTT_StaWillRetain						0	//��������
#define		MQTT_StaUserNameFlag					1	//�û�����־ User Name Flag
#define		MQTT_StaPasswordFlag					1	//�����־ Password Flag
#define		MQTT_KeepAlive								60
#define		MQTT_ClientIdentifier  "Tan1"	//�ͻ��˱�ʶ�� Client Identifier
#define		MQTT_WillTopic			""				//�������� Will Topic
#define		MQTT_WillMessage		""				//������Ϣ Will Message
#define		MQTT_UserName			"admin"			//�û��� User Name
#define		MQTT_Password			"password"	//���� Password
 
unsigned char GetDataFixedHead(unsigned char MesType,unsigned char DupFlag,unsigned char QosLevel,unsigned char Retain);
void GetDataPUBLISH(unsigned char *buff,unsigned char dup, unsigned char qos,unsigned char retain,const char *topic ,const char *msg);//��ȡ������Ϣ�����ݰ�		 	
void GetDataSUBSCRIBE(unsigned char *buff,const char *dat,unsigned int Num,unsigned char RequestedQoS);//������������ݰ� Num:������� RequestedQoS:��������Ҫ��0,1��2
void GetDataDisConnet(unsigned char *buff);//��ȡ�Ͽ����ӵ����ݰ�
void GetDataConnet(unsigned char *buff);//��ȡ���ӵ����ݰ���ȷ���ӷ���20 02 00 00
void GetDataPINGREQ(unsigned char *buff);//������������ݰ��ɹ�����d0 00


#endif


#endif

