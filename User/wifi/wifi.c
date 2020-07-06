#include "wifi.h"

volatile uint8_t ucTcpClosedFlag = 0;


int wifiConfig ( void )
{
//	char cStr [ 100 ] = { 0 };
  printf ( "\r\n�������� ESP8266 ......\r\n" );
	//ʹ��8266
	macESP8266_CH_ENABLE();
	//ATָ�����
	ESP8266_AT_Test ();
	#if 1
	ESP8266_Net_Mode_Choose ( STA  );

	#else
	//ģʽѡ��
	ESP8266_Net_Mode_Choose ( STA_AP  );
		//����8266��ip��ַ
  while ( ! ESP8266_CIPAP ( macUser_ESP8266_AP_IP ) );
	//����8266��wifi���� ���� ���ܷ�ʽ
  while ( ! ESP8266_BuildAP ( macUser_ESP8266_BulitApSsid, macUser_ESP8266_BulitApPwd, macUser_ESP8266_BulitApEcn ) );	
	//�Ƿ����ö�����
	ESP8266_Enable_MultipleId ( DISABLE );
	//8266�Ƿ���������ģʽ
	while ( !	ESP8266_StartOrShutServer ( ENABLE, macUser_ESP8266_AP_Port, macUser_ESP8266_TcpServer_OverTime ) );
	//��ȡ F-ESP8266 �� AP IP
	ESP8266_Inquire_ApIp ( cStr, 20 );
	printf ( "\r\n��ģ��WIFIΪ%s�����뿪��\r\nAP IP Ϊ��%s�������Ķ˿�Ϊ��%s\r\n�ֻ������������Ӹ� IP �Ͷ˿ڣ���������5���ͻ���\r\n",
           macUser_ESP8266_BulitApSsid, cStr, macUser_ESP8266_AP_Port );
	
	
	
	#endif
	//����·����wifi           ����                    ����
  if(  ESP8266_JoinAP ( macUser_ESP8266_ApSsid, macUser_ESP8266_ApPwd ) )
	{
		//�Ƿ����������
		ESP8266_Enable_MultipleId ( DISABLE );
		
		printf ( "\r\n���� wifi ���\r\n" );
	}else{
		printf ( "\r\n���� wifi ʧ��\r\n" );
		return 1;
	}

	return 0;
}
#if 0

char dispBuff[100];
char cStr [ 100 ] = { 0 };
u8 sta;
u8 txbuf[256];
u8 rxbuf[256];

void wifiTest()
{
	//����WIFI
	if( !wifiConfig() )
	{
		sprintf(dispBuff,"WIFI OK");
		LCD_ClearLine(LINE(1));	/* ����������� */
		ILI9341_DispStringLine_EN(LINE(1),dispBuff);
		ESP8266_Inquire_ApIp ( cStr, 20 );
		//printf ( "IP Ϊ��%s",cStr);
		sta++;
	}else
	{
		sprintf(dispBuff,"WIFI FAIL");
		LCD_ClearLine(LINE(1));	/* ����������� */
		ILI9341_DispStringLine_EN(LINE(1),dispBuff);
		sta = 0;
	}
	Delay_ms(200);
}

void linkServerTest()
{
	//���ӷ�����
	if(sta == 1)
	{
		if( ESP8266_Link_Server ( enumTCP, macUser_ESP8266_TcpServer_IP, macUser_ESP8266_TcpServer_Port, Single_ID_0 ) )
		{
				//����͸��ģʽ
			if( ESP8266_UnvarnishSend () )
			{
				printf ( "\r\n����͸��ģʽ�ɹ�\r\n" );
			}else
			{
				printf ( "\r\n����͸��ģʽʧ��\r\n" );
			}
			printf ( "\r\n���ӷ������ɹ�\r\n" ); 
			sprintf(dispBuff,"SERVER OK");
			LCD_ClearLine(LINE(2));	/* ����������� */
			ILI9341_DispStringLine_EN(LINE(2),dispBuff);
			sta++;
		}else
		{
			printf ( "\r\n���ӷ�����ʧ��\r\n" ); 
			sprintf(dispBuff,"SERVER FAIL");
			LCD_ClearLine(LINE(2));	/* ����������� */
			ILI9341_DispStringLine_EN(LINE(2),dispBuff);
			sta = 0;
		}
	}
	
	Delay_ms(200);
}

void mqttConnectTest()
{
	//mqtt����
	if(sta == 2)
	{
		
		/*mqttClientId: clientId+"|securemode=3,signmethod=hmacsha1,timestamp=132323232|"
			mqttUsername: deviceName+"&"+productKey
			mqttPassword: sign_hmac(deviceSecret,content)*/
		_mqtt.Init(rxbuf,sizeof(rxbuf),txbuf,sizeof(txbuf));		
		if( _mqtt.Connect(mqttClientId,mqttUsername,mqttPassword) != 0 )
		{
			printf("\r\nMQTT OK!\r\n");
			sprintf(dispBuff,"MQTT OK");
			LCD_ClearLine(LINE(3));	/* ����������� */
			ILI9341_DispStringLine_EN(LINE(3),dispBuff);
			sta++;
		}
		else
		{			
			printf("\r\nMQTT FAIL!\r\n");
			sprintf(dispBuff,"MQTT FAIl");
			LCD_ClearLine(LINE(3));	/* ����������� */
			ILI9341_DispStringLine_EN(LINE(3),dispBuff);
			sta=0;
		}	
	}
	Delay_ms(5000);
}
#endif

///**
//  * @brief  ESP8266 ��Sta Tcp Client��͸��
//  * @param  ��
//  * @retval ��
//  */
//void ESP8266_StaTcpClient_UnvarnishTest ( void )
//{
//	uint8_t ucStatus;
//	
//	char cStr [ 100 ] = { 0 };
//	//DHT11����
//	DHT11_Data_TypeDef DHT11_Data;
//	
//	
//  printf ( "\r\n�������� ESP8266 ......\r\n" );
//	//ʹ��8266
//	macESP8266_CH_ENABLE();
//	//ATָ�����
//	ESP8266_AT_Test ();
//	//ģʽѡ��
//	ESP8266_Net_Mode_Choose ( STA );
//	//����·����wifi           ����                    ����
//  while ( ! ESP8266_JoinAP ( macUser_ESP8266_ApSsid, macUser_ESP8266_ApPwd ) );	
//	//�Ƿ����������
//	ESP8266_Enable_MultipleId ( DISABLE );
//	//�����ⲿ������
//	while ( !	ESP8266_Link_Server ( enumTCP, macUser_ESP8266_TcpServer_IP, macUser_ESP8266_TcpServer_Port, Single_ID_0 ) );
//	//����͸��ģʽ
//	while ( ! ESP8266_UnvarnishSend () );
//	
//	printf ( "\r\n���� ESP8266 ���\r\n" );
//	
//	
//	while ( 1 )
//	{		
//		if ( DHT11_Read_TempAndHumidity ( & DHT11_Data ) == SUCCESS )       //��ȡ DHT11 ��ʪ����Ϣ
//			sprintf ( cStr, "\r\n��ȡDHT11�ɹ�!\r\n\r\nʪ��Ϊ%d.%d ��RH ���¶�Ϊ %d.%d�� \r\n", 
//								DHT11_Data .humi_int, DHT11_Data .humi_deci, DHT11_Data .temp_int, DHT11_Data.temp_deci );
//				
//		else
//			sprintf ( cStr, "Read DHT11 ERROR!\r\n" );

//		printf ( "%s", cStr );                                             //��ӡ��ȡ DHT11 ��ʪ����Ϣ

//	
//		ESP8266_SendString ( ENABLE, cStr, 0, Single_ID_0 );               //���� DHT11 ��ʪ����Ϣ�������������
//		
//		Delay_ms ( 1500 );
//		
//		if ( ucTcpClosedFlag )                                             //����Ƿ�ʧȥ����
//		{
//			ESP8266_ExitUnvarnishSend ();                                    //�˳�͸��ģʽ
//			
//			do ucStatus = ESP8266_Get_LinkStatus ();                         //��ȡ����״̬
//			while ( ! ucStatus );
//			
//			if ( ucStatus == 4 )                                             //ȷ��ʧȥ���Ӻ�����
//			{
//				printf ( "\r\n���������ȵ�ͷ����� ......\r\n" );
//				
//				while ( ! ESP8266_JoinAP ( macUser_ESP8266_ApSsid, macUser_ESP8266_ApPwd ) );
//				
//				while ( !	ESP8266_Link_Server ( enumTCP, macUser_ESP8266_TcpServer_IP, macUser_ESP8266_TcpServer_Port, Single_ID_0 ) );
//				
//				printf ( "\r\n�����ȵ�ͷ������ɹ�\r\n" );

//			}
//			
//			while ( ! ESP8266_UnvarnishSend () );		
//			
//		}

//	}
//	
//		
//}

