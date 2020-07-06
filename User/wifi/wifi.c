#include "wifi.h"

volatile uint8_t ucTcpClosedFlag = 0;


int wifiConfig ( void )
{
//	char cStr [ 100 ] = { 0 };
  printf ( "\r\n正在配置 ESP8266 ......\r\n" );
	//使能8266
	macESP8266_CH_ENABLE();
	//AT指令测试
	ESP8266_AT_Test ();
	#if 1
	ESP8266_Net_Mode_Choose ( STA  );

	#else
	//模式选择
	ESP8266_Net_Mode_Choose ( STA_AP  );
		//配置8266的ip地址
  while ( ! ESP8266_CIPAP ( macUser_ESP8266_AP_IP ) );
	//配置8266的wifi名称 密码 加密方式
  while ( ! ESP8266_BuildAP ( macUser_ESP8266_BulitApSsid, macUser_ESP8266_BulitApPwd, macUser_ESP8266_BulitApEcn ) );	
	//是否启用多连接
	ESP8266_Enable_MultipleId ( DISABLE );
	//8266是否开启服务器模式
	while ( !	ESP8266_StartOrShutServer ( ENABLE, macUser_ESP8266_AP_Port, macUser_ESP8266_TcpServer_OverTime ) );
	//获取 F-ESP8266 的 AP IP
	ESP8266_Inquire_ApIp ( cStr, 20 );
	printf ( "\r\n本模块WIFI为%s，密码开放\r\nAP IP 为：%s，开启的端口为：%s\r\n手机网络助手连接该 IP 和端口，最多可连接5个客户端\r\n",
           macUser_ESP8266_BulitApSsid, cStr, macUser_ESP8266_AP_Port );
	
	
	
	#endif
	//连接路由器wifi           名称                    密码
  if(  ESP8266_JoinAP ( macUser_ESP8266_ApSsid, macUser_ESP8266_ApPwd ) )
	{
		//是否允许多连接
		ESP8266_Enable_MultipleId ( DISABLE );
		
		printf ( "\r\n配置 wifi 完毕\r\n" );
	}else{
		printf ( "\r\n配置 wifi 失败\r\n" );
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
	//连接WIFI
	if( !wifiConfig() )
	{
		sprintf(dispBuff,"WIFI OK");
		LCD_ClearLine(LINE(1));	/* 清除单行文字 */
		ILI9341_DispStringLine_EN(LINE(1),dispBuff);
		ESP8266_Inquire_ApIp ( cStr, 20 );
		//printf ( "IP 为：%s",cStr);
		sta++;
	}else
	{
		sprintf(dispBuff,"WIFI FAIL");
		LCD_ClearLine(LINE(1));	/* 清除单行文字 */
		ILI9341_DispStringLine_EN(LINE(1),dispBuff);
		sta = 0;
	}
	Delay_ms(200);
}

void linkServerTest()
{
	//连接服务器
	if(sta == 1)
	{
		if( ESP8266_Link_Server ( enumTCP, macUser_ESP8266_TcpServer_IP, macUser_ESP8266_TcpServer_Port, Single_ID_0 ) )
		{
				//进入透传模式
			if( ESP8266_UnvarnishSend () )
			{
				printf ( "\r\n进入透传模式成功\r\n" );
			}else
			{
				printf ( "\r\n进入透传模式失败\r\n" );
			}
			printf ( "\r\n连接服务器成功\r\n" ); 
			sprintf(dispBuff,"SERVER OK");
			LCD_ClearLine(LINE(2));	/* 清除单行文字 */
			ILI9341_DispStringLine_EN(LINE(2),dispBuff);
			sta++;
		}else
		{
			printf ( "\r\n连接服务器失败\r\n" ); 
			sprintf(dispBuff,"SERVER FAIL");
			LCD_ClearLine(LINE(2));	/* 清除单行文字 */
			ILI9341_DispStringLine_EN(LINE(2),dispBuff);
			sta = 0;
		}
	}
	
	Delay_ms(200);
}

void mqttConnectTest()
{
	//mqtt连接
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
			LCD_ClearLine(LINE(3));	/* 清除单行文字 */
			ILI9341_DispStringLine_EN(LINE(3),dispBuff);
			sta++;
		}
		else
		{			
			printf("\r\nMQTT FAIL!\r\n");
			sprintf(dispBuff,"MQTT FAIl");
			LCD_ClearLine(LINE(3));	/* 清除单行文字 */
			ILI9341_DispStringLine_EN(LINE(3),dispBuff);
			sta=0;
		}	
	}
	Delay_ms(5000);
}
#endif

///**
//  * @brief  ESP8266 （Sta Tcp Client）透传
//  * @param  无
//  * @retval 无
//  */
//void ESP8266_StaTcpClient_UnvarnishTest ( void )
//{
//	uint8_t ucStatus;
//	
//	char cStr [ 100 ] = { 0 };
//	//DHT11数据
//	DHT11_Data_TypeDef DHT11_Data;
//	
//	
//  printf ( "\r\n正在配置 ESP8266 ......\r\n" );
//	//使能8266
//	macESP8266_CH_ENABLE();
//	//AT指令测试
//	ESP8266_AT_Test ();
//	//模式选择
//	ESP8266_Net_Mode_Choose ( STA );
//	//连接路由器wifi           名称                    密码
//  while ( ! ESP8266_JoinAP ( macUser_ESP8266_ApSsid, macUser_ESP8266_ApPwd ) );	
//	//是否允许多连接
//	ESP8266_Enable_MultipleId ( DISABLE );
//	//连接外部服务器
//	while ( !	ESP8266_Link_Server ( enumTCP, macUser_ESP8266_TcpServer_IP, macUser_ESP8266_TcpServer_Port, Single_ID_0 ) );
//	//进入透传模式
//	while ( ! ESP8266_UnvarnishSend () );
//	
//	printf ( "\r\n配置 ESP8266 完毕\r\n" );
//	
//	
//	while ( 1 )
//	{		
//		if ( DHT11_Read_TempAndHumidity ( & DHT11_Data ) == SUCCESS )       //读取 DHT11 温湿度信息
//			sprintf ( cStr, "\r\n读取DHT11成功!\r\n\r\n湿度为%d.%d ％RH ，温度为 %d.%d℃ \r\n", 
//								DHT11_Data .humi_int, DHT11_Data .humi_deci, DHT11_Data .temp_int, DHT11_Data.temp_deci );
//				
//		else
//			sprintf ( cStr, "Read DHT11 ERROR!\r\n" );

//		printf ( "%s", cStr );                                             //打印读取 DHT11 温湿度信息

//	
//		ESP8266_SendString ( ENABLE, cStr, 0, Single_ID_0 );               //发送 DHT11 温湿度信息到网络调试助手
//		
//		Delay_ms ( 1500 );
//		
//		if ( ucTcpClosedFlag )                                             //检测是否失去连接
//		{
//			ESP8266_ExitUnvarnishSend ();                                    //退出透传模式
//			
//			do ucStatus = ESP8266_Get_LinkStatus ();                         //获取连接状态
//			while ( ! ucStatus );
//			
//			if ( ucStatus == 4 )                                             //确认失去连接后重连
//			{
//				printf ( "\r\n正在重连热点和服务器 ......\r\n" );
//				
//				while ( ! ESP8266_JoinAP ( macUser_ESP8266_ApSsid, macUser_ESP8266_ApPwd ) );
//				
//				while ( !	ESP8266_Link_Server ( enumTCP, macUser_ESP8266_TcpServer_IP, macUser_ESP8266_TcpServer_Port, Single_ID_0 ) );
//				
//				printf ( "\r\n重连热点和服务器成功\r\n" );

//			}
//			
//			while ( ! ESP8266_UnvarnishSend () );		
//			
//		}

//	}
//	
//		
//}

