#include "stm32f10x.h"
#include "bsp_usart1.h"
#include "bsp_SysTick.h"
#include "bsp_esp8266.h"
#include "wifi.h"
#include "bsp_dht11.h"
#include "bsp_ili9341_lcd.h"
#include "mqtt.h"
#include "bsp_led.h"
#include "bsp_key.h"
#include "bsp_TIMbase.h"
#include "bsp_beep.h"
#include "bsp_key_exti.h"

volatile uint32_t time = 0; // ms 计时变量 
u8 sta;
char dispBuff[100];
u8 txbuf[256];
u8 rxbuf[256];
char cStr [ 100 ] = { 0 };
DHT11_Data_TypeDef DHT11_Data;
char mqtt_message[200];
static u8 line=0;
u8 stopFan[7] = "\"Fan\":0";
//char *stopfan = "{\"Fan\":0}";
u8 stopMotor[9] = "\"Motor\":0";

void mqttMessage_Handler(void);

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main ( void )
{
	/* 初始化 */
	ILI9341_Init(); 	 																														 //LCD 初始化
	ILI9341_GramScan( 6 );
	LCD_SetColors( WHITE,BLACK );
  ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);																	 /* 清屏，显示全黑 */
  USART1_Config();                                                              //初始化串口1
	SysTick_Init();                                                               //配置 SysTick 为 1ms 中断一次 
	ESP8266_Init();                                                               //初始化WiFi模块使用的接口和外设
	DHT11_Init();      
  LED_Init();
	KEY_Init();
  BASIC_TIM_Init();
	Beep_Init();
	printf ( "\r\n-----智能口罩设计测试ing-----\r\n" );                            //打印提示信息
	
	printf ( "\r\nWiFi连接测试\r\n" ); 
	BEEP_ON();
	Delay_ms(200);
	BEEP_OFF();

	sta=0;
	//连接WIFI
	if(sta == 0)
	{
		if( !wifiConfig() )
		{
			sprintf(dispBuff,"WIFI OK");
			LCD_ClearLine(LINE(1));	/* 清除单行文字 */
			ILI9341_DispStringLine_EN(LINE(1),dispBuff);
			ESP8266_Inquire_ApIp ( cStr, 20 );
			sta++;
		}else
		{
			sprintf(dispBuff,"WIFI FAIL");
			LCD_ClearLine(LINE(1));	/* 清除单行文字 */
			ILI9341_DispStringLine_EN(LINE(1),dispBuff);
			sta = 0;
		}
	}

	
	Delay_ms(200);
	
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

	Delay_ms(200);
	
	//主题订阅
	if(sta == 3)
	{
		if(_mqtt.SubscribeTopic(mqttTopicStopFan,0,1) &&_mqtt.SubscribeTopic(mqttTopicStopMotor,0,1) != 0)
		{
			printf("SubscribeTopic OK!\r\n");
			sprintf(dispBuff,"SubscribeTopic OK!");
			LCD_ClearLine(LINE(4));	/* 清除单行文字 */
			ILI9341_DispStringLine_EN(LINE(4),dispBuff);
			sta++;
		}
		else
		{			
			printf("SubscribeTopic FAIL!\r\n");
			sprintf(dispBuff,"SubscribeTopic FAIL!");
			LCD_ClearLine(LINE(4));	/* 清除单行文字 */
			ILI9341_DispStringLine_EN(LINE(4),dispBuff);
		}	
	}
	
	Delay_ms(2000);
	
	//提示
	if(sta == 4)
	{
		printf( "Please press the k1 \r\n" );  
		sprintf( dispBuff, "Please press the k1 " );
		LCD_ClearLine(LINE(5));	/* 清除单行文字 */
		ILI9341_DispStringLine_EN(LINE(5),dispBuff);
		Delay_ms(500);
		printf( "Press the k2 to stop beep\r\n" );  
		sprintf( dispBuff, "Press the k2 to stop beep" );
		LCD_ClearLine(LINE(6));	/* 清除单行文字 */
		ILI9341_DispStringLine_EN(LINE(6),dispBuff);
		
	}
	Delay_ms(500);
	
	
	#if 0  //第一版
  while ( 1 )
	{
		printf("读取中……\n");
		Delay_ms(5000);
		//_mqtt.SendHeart();
		Delay_ms(200);
		if ( DHT11_Read_TempAndHumidity ( & DHT11_Data ) != SUCCESS )       //读取 DHT11 温湿度信息
		{
			sprintf ( cStr, "Read DHT11 ERROR!\r\n" );
			printf ( "%s", cStr );     //打印读取 DHT11 温湿度信息	
			LCD_ClearLine(LINE(line));	/* 清除单行文字 */
			ILI9341_DispStringLine_EN(LINE(line++),cStr);
		}
		else
		{
			sprintf ( cStr, "\r\n读取DHT11成功!\r\n\r\n湿度为%d.%d ％RH ，温度为 %d.%d℃ \r\n\r\n", 
			DHT11_Data .humi_int, DHT11_Data .humi_deci, DHT11_Data .temp_int, DHT11_Data.temp_deci );
			printf ( "%s", cStr );     //打印读取 DHT11 温湿度信息
			Delay_ms(100);
			
			sprintf(dispBuff,"AQI: %d.%d ，Temp: %d.%d",
							DHT11_Data .humi_int, DHT11_Data .humi_deci, DHT11_Data .temp_int, DHT11_Data.temp_deci);
			
			LCD_ClearLine(LINE(line));	/* 清除单行文字 */
			ILI9341_DispStringLine_EN(LINE(line++),dispBuff);
			//thing.event.property.post
			sprintf(mqtt_message,
			"{\"method\":\"thing.event.property.post\",\"id\":\"630262306\",\"params\":{\
				\"AQI\":%d,\
				\"Temperature\":%d.%d\
			},\"version\":\"1.0.0\"}",
			DHT11_Data.humi_int,
			DHT11_Data .temp_int,
			DHT11_Data.temp_deci
			);
			//发布消息
			_mqtt.PublishData(mqttTopicProPost,mqtt_message,0);
		
		}
		//大于7行 清屏
		if(line>=7)
		{
			line = 0;
			ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	//清屏
		}

		//8266处理接收接受aliyun消息
	 mqttMessage_Handler();
	 Delay_ms(100);
	}
	#else
	#if 1  //修改版
	while(1)                            
	{	   
		
		//如果按下 说明佩戴口罩 开始记录时间
		if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON  )
		{
			// 使能计数器
			TIM_Cmd(BASIC_TIM, ENABLE);	
			printf( "口罩已戴上\n" );
			sprintf ( dispBuff, "MASK ON" );
			LCD_ClearLine(LINE(7));	/* 清除单行文字 */
			ILI9341_DispStringLine_EN(LINE(7),dispBuff);
			Delay_ms(2000);
			ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	//清屏

			while ( 1 )
			{
				printf("读取中……\n");
				//_mqtt.SendHeart();
				if ( DHT11_Read_TempAndHumidity ( & DHT11_Data ) != SUCCESS )       //读取 DHT11 温湿度信息
				{
					printf ( "Read DHT11 ERROR!\r\n" );     //打印读取 DHT11 温湿度信息	
					sprintf ( cStr, "Read DHT11 ERROR!" );
					LCD_ClearLine(LINE(line));	/* 清除单行文字 */
					ILI9341_DispStringLine_EN(LINE(line++),cStr);
				}
				else
				{
					sprintf ( cStr, "\r\n读取DHT11成功!\r\n\r\n湿度为%d.%d ％RH ，温度为 %d.%d℃ \r\n\r\n", 
					DHT11_Data .humi_int, DHT11_Data .humi_deci, DHT11_Data .temp_int, DHT11_Data.temp_deci );
					printf ( "%s", cStr );     //打印读取 DHT11 温湿度信息
					Delay_ms(100);
					
					sprintf(dispBuff,"AQI: %d.%d ，Temp: %d.%d",
									DHT11_Data .humi_int, DHT11_Data .humi_deci, DHT11_Data .temp_int, DHT11_Data.temp_deci);
					
					LCD_ClearLine(LINE(line));	/* 清除单行文字 */
					ILI9341_DispStringLine_EN(LINE(line++),dispBuff);
					//thing.event.property.post
					sprintf(mqtt_message,
					"{\"method\":\"thing.event.property.post\",\"id\":\"630262306\",\"params\":{\
					  \"UsingTime\":%d.%.2d,\
						\"AQI\":%d,\
						\"Temperature\":%d.%d\
					},\"version\":\"1.0.0\"}",
					time/1000/60,
					( time/1000 )%60,
					DHT11_Data.humi_int,
					DHT11_Data .temp_int,
					DHT11_Data.temp_deci
					);
					//属性上报
					_mqtt.PublishData(mqttTopicProPost,mqtt_message,0);
					//发布消息
					printf("发布消息成功\n");

				}
				//大于7行 清屏
				if(line>=7)
				{
					line = 0;
					ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	//清屏
				}

			  //8266处理接收接受aliyun消息
			  mqttMessage_Handler();
			  Delay_ms(100);
				
				if ( time >= 20000 ) /* 60000*60 * 1 ms = 1h 时间到 口罩过期 */
				{
					// 失能计数器
					TIM_Cmd(BASIC_TIM, DISABLE);	
					BEEP_ON();
					time = 0;
					sprintf ( dispBuff, "press k2 and change mask" );
					LCD_ClearLine(LINE(8));	/* 清除单行文字 */
					ILI9341_DispStringLine_EN(LINE(8),dispBuff);
					
					//蜂鸣器状态上报
					sprintf(mqtt_message,
					"{\"method\":\"thing.event.property.post\",\"id\":\"630262306\",\"params\":{\
					  \"Motor\":%d,\
					},\"version\":\"1.0.0\"}", 1 );
					//属性上报
					_mqtt.PublishData(mqttTopicProPost,mqtt_message,0);
					
					Delay_ms(100);
					//事件上报
					sprintf(mqtt_message,
					"{\"method\":\"thing.event.Shake.post\",\"id\":\"630262306\",\"params\":{\
					  \"motor\":%d,\
						},\"version\":\"1.0.0\"}",
						1
					);
					//事件上报
					_mqtt.PublishData(mqttTopicEventPost,mqtt_message,0);
					printf("事件上报\r\n");
				}
				//按下k2停止蜂鸣器
				if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON  )
				{
					// 失能计数器
					TIM_Cmd(BASIC_TIM, DISABLE);
					BEEP_OFF();
					time = 0;
					break;
				}
				//如果按下k1 说明口罩摘下
				if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON  )
				{
					// 失能计数器
					TIM_Cmd(BASIC_TIM, DISABLE);	
					BEEP_OFF();
					printf("口罩已摘下\n");
					sprintf ( cStr, "MASK OFF" );
					printf ( "%s", cStr );    
					LCD_ClearLine(LINE(line));	/* 清除单行文字 */
					ILI9341_DispStringLine_EN(LINE(line++),cStr);
					time = 0;

					break;
				}
				
			}
		} 

	}
	
	#else  //使用外部中断但还没有着手

	while(1)                            
	{	   
		//如果按下 说明佩戴口罩 开始记录时间
		if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON  )
		{
			// 使能计数器
			TIM_Cmd(BASIC_TIM, ENABLE);	
			printf("口罩已戴上\n");
			sprintf ( dispBuff, "MASK ON" );
			LCD_ClearLine(LINE(7));	/* 清除单行文字 */
			ILI9341_DispStringLine_EN(LINE(7),dispBuff);
			ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	//清屏
			
			while ( 1 )
			{
				printf("读取中……\n");
				//_mqtt.SendHeart();
				Delay_ms(5000);
				if ( DHT11_Read_TempAndHumidity ( & DHT11_Data ) != SUCCESS )       //读取 DHT11 温湿度信息
				{
					sprintf ( cStr, "Read DHT11 ERROR!\r\n" );
					printf ( "%s", cStr );     //打印读取 DHT11 温湿度信息	
					LCD_ClearLine(LINE(line));	/* 清除单行文字 */
					ILI9341_DispStringLine_EN(LINE(line++),cStr);
				}
				else
				{
					sprintf ( cStr, "\r\n读取DHT11成功!\r\n\r\n湿度为%d.%d ％RH ，温度为 %d.%d℃ \r\n\r\n", 
					DHT11_Data .humi_int, DHT11_Data .humi_deci, DHT11_Data .temp_int, DHT11_Data.temp_deci );
					printf ( "%s", cStr );     //打印读取 DHT11 温湿度信息
					Delay_ms(100);
					
					sprintf(dispBuff,"AQI: %d.%d ，Temp: %d.%d",
									DHT11_Data .humi_int, DHT11_Data .humi_deci, DHT11_Data .temp_int, DHT11_Data.temp_deci);
					
					LCD_ClearLine(LINE(line));	/* 清除单行文字 */
					ILI9341_DispStringLine_EN(LINE(line++),dispBuff);
					//thing.event.property.post
					sprintf(mqtt_message,
					"{\"method\":\"thing.event.property.post\",\"id\":\"630262306\",\"params\":{\
						\"AQI\":%d,\
						\"Temperature\":%d.%d\
					},\"version\":\"1.0.0\"}",
					DHT11_Data.humi_int,
					DHT11_Data .temp_int,
					DHT11_Data.temp_deci
					);
					//发布消息
					_mqtt.PublishData(mqttTopicProPost,mqtt_message,0);
					printf("发布消息成功\n");

				}
				//大于7行 清屏
				if(line>=7)
				{
					line = 0;
					ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	//清屏
				}

			  //8266处理接收接受aliyun消息
			  mqttMessage_Handler();
			  Delay_ms(100);
				
				if ( time >= 30000 ) /* 60000 * 1 ms = 1min 时间到 口罩过期 */
				{
					// 失能计数器
					TIM_Cmd(BASIC_TIM, DISABLE);	
					BEEP_ON();
					time = 0;
					sprintf ( dispBuff, "press k2 and change mask" );
					LCD_ClearLine(LINE(8));	/* 清除单行文字 */
					ILI9341_DispStringLine_EN(LINE(8),dispBuff);
				}
				//按下k2停止蜂鸣器
				if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON  )
				{
					// 失能计数器
					TIM_Cmd(BASIC_TIM, DISABLE);
					BEEP_OFF();
					time = 0;
					break;
				}
				//如果按下k1 说明口罩摘下
				if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON  )
				{
					// 失能计数器
					TIM_Cmd(BASIC_TIM, DISABLE);	
					BEEP_OFF();
					printf("口罩已摘下\n");
					sprintf ( cStr, "MASK OFF" );
					printf ( "%s", cStr );    
					LCD_ClearLine(LINE(line));	/* 清除单行文字 */
					ILI9341_DispStringLine_EN(LINE(line++),cStr);
					time = 0;
					break;
				}
				
			}
		} 

	}  
	#endif
	#endif

}
//这两个中断未用到
void KEY1_IRQHandler(void)
{
  //确保是否产生了EXTI Line中断
	if(EXTI_GetITStatus(KEY1_INT_EXTI_LINE) != RESET) 
	{

    //清除中断标志位
		EXTI_ClearITPendingBit(KEY1_INT_EXTI_LINE);     
	}  
}

void KEY2_IRQHandler(void)
{
  //确保是否产生了EXTI Line中断
	if(EXTI_GetITStatus(KEY2_INT_EXTI_LINE) != RESET) 
	{

    //清除中断标志位
		EXTI_ClearITPendingBit(KEY2_INT_EXTI_LINE);     
	}  
}
//8266中断
void macESP8266_USART_INT_FUN ( void )
{		
	u8 temp;
	uint8_t ucCh;//用于处理mqtt

	static u8 rxlen = 0;
	
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) //判断接收数据寄存器是否有数据
	{
		ucCh  = USART_ReceiveData( macESP8266_USARTx );
		temp = ucCh;
		//		USART1->DR = temp;//这里用作串口1打印WIFI模块发送的数据
		if(rxlen>=255) rxlen=0;
		rxbuf[rxlen++] = temp;
		rxlen%=sizeof(rxbuf);//rxbuf.length()==256
		if ( strEsp8266_Fram_Record .InfBit .FramLength < ( RX_BUF_MAX_LEN - 1 ) )                       //预留1个字节写结束符
			strEsp8266_Fram_Record .Data_RX_BUF [ strEsp8266_Fram_Record .InfBit .FramLength ++ ]  = ucCh;
	}

	if(USART_GetITStatus(USART3, USART_IT_IDLE))
	{
		strEsp8266_Fram_Record .InfBit .FramFinishFlag = 1;
		
		ucCh = USART_ReceiveData( macESP8266_USARTx );                                                              //由软件序列清除中断标志位(先读USART_SR，然后读USART_DR)
		temp = ucCh;
		ucTcpClosedFlag = strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "CLOSED\r\n" ) ? 1 : 0;
		
		temp = USART3->SR;
		_mqtt.rxlen = rxlen;
		rxlen = 0;
	}
}
//定时器中断
void  BASIC_TIM_IRQHandler (void)
{
	if ( TIM_GetITStatus( BASIC_TIM, TIM_IT_Update) != RESET ) 
	{	
		time++;
		TIM_ClearITPendingBit(BASIC_TIM , TIM_FLAG_Update);  		 
	}		 	
}


//处理mqtt消息
void mqttMessage_Handler(void)
{			
	Delay_ms(4000);
	if(_mqtt.rxlen>20)
	{
		int i,j=0;
		//printf("收到来自mqtt的消息，处理中……\r\n\r\n");
		//遍历接收区字符
		for(i=0;i<_mqtt.rxlen;i++)
		{
			for(j=0;j<9;j++)
			{
				
				if(_mqtt.rxbuf[i+j] != stopFan[j] && _mqtt.rxbuf[i+j] != stopMotor[j]) 
				{
					break;
				}
			}
			//调用服务
			if(j==7) 
			{
				printf("\r\n\r\n调用服务：停止吹风\r\n\r\n");
				sprintf(dispBuff,"invoke service:stopFan");
				LCD_ClearLine(LINE(line));	/* 清除单行文字 */
				ILI9341_DispStringLine_EN(LINE(line++),dispBuff);
				macLED1_ON ();
				Delay_ms(1000);
				macLED1_OFF();
				break;
			}
			if(j==9) 
			{
				BEEP_OFF();
				printf("\r\n\r\n调用服务：停止震动\r\n\r\n");
				sprintf(dispBuff,"invoke service:stopMotor");
				LCD_ClearLine(LINE(line));	/* 清除单行文字 */
				ILI9341_DispStringLine_EN(LINE(line++),dispBuff);
				macLED2_ON();
				Delay_ms(1000);
				macLED2_OFF();
				break;
			}
			
			
		}
		//清空接收区字符
		memset(_mqtt.rxbuf,0,_mqtt.rxlen);
		_mqtt.rxlen = 0;

	}
}



/*********************************************END OF FILE**********************/
