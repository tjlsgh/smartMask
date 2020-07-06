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

volatile uint32_t time = 0; // ms ��ʱ���� 
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
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main ( void )
{
	/* ��ʼ�� */
	ILI9341_Init(); 	 																														 //LCD ��ʼ��
	ILI9341_GramScan( 6 );
	LCD_SetColors( WHITE,BLACK );
  ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);																	 /* ��������ʾȫ�� */
  USART1_Config();                                                              //��ʼ������1
	SysTick_Init();                                                               //���� SysTick Ϊ 1ms �ж�һ�� 
	ESP8266_Init();                                                               //��ʼ��WiFiģ��ʹ�õĽӿں�����
	DHT11_Init();      
  LED_Init();
	KEY_Init();
  BASIC_TIM_Init();
	Beep_Init();
	printf ( "\r\n-----���ܿ�����Ʋ���ing-----\r\n" );                            //��ӡ��ʾ��Ϣ
	
	printf ( "\r\nWiFi���Ӳ���\r\n" ); 
	BEEP_ON();
	Delay_ms(200);
	BEEP_OFF();

	sta=0;
	//����WIFI
	if(sta == 0)
	{
		if( !wifiConfig() )
		{
			sprintf(dispBuff,"WIFI OK");
			LCD_ClearLine(LINE(1));	/* ����������� */
			ILI9341_DispStringLine_EN(LINE(1),dispBuff);
			ESP8266_Inquire_ApIp ( cStr, 20 );
			sta++;
		}else
		{
			sprintf(dispBuff,"WIFI FAIL");
			LCD_ClearLine(LINE(1));	/* ����������� */
			ILI9341_DispStringLine_EN(LINE(1),dispBuff);
			sta = 0;
		}
	}

	
	Delay_ms(200);
	
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

	Delay_ms(200);
	
	//���ⶩ��
	if(sta == 3)
	{
		if(_mqtt.SubscribeTopic(mqttTopicStopFan,0,1) &&_mqtt.SubscribeTopic(mqttTopicStopMotor,0,1) != 0)
		{
			printf("SubscribeTopic OK!\r\n");
			sprintf(dispBuff,"SubscribeTopic OK!");
			LCD_ClearLine(LINE(4));	/* ����������� */
			ILI9341_DispStringLine_EN(LINE(4),dispBuff);
			sta++;
		}
		else
		{			
			printf("SubscribeTopic FAIL!\r\n");
			sprintf(dispBuff,"SubscribeTopic FAIL!");
			LCD_ClearLine(LINE(4));	/* ����������� */
			ILI9341_DispStringLine_EN(LINE(4),dispBuff);
		}	
	}
	
	Delay_ms(2000);
	
	//��ʾ
	if(sta == 4)
	{
		printf( "Please press the k1 \r\n" );  
		sprintf( dispBuff, "Please press the k1 " );
		LCD_ClearLine(LINE(5));	/* ����������� */
		ILI9341_DispStringLine_EN(LINE(5),dispBuff);
		Delay_ms(500);
		printf( "Press the k2 to stop beep\r\n" );  
		sprintf( dispBuff, "Press the k2 to stop beep" );
		LCD_ClearLine(LINE(6));	/* ����������� */
		ILI9341_DispStringLine_EN(LINE(6),dispBuff);
		
	}
	Delay_ms(500);
	
	
	#if 0  //��һ��
  while ( 1 )
	{
		printf("��ȡ�С���\n");
		Delay_ms(5000);
		//_mqtt.SendHeart();
		Delay_ms(200);
		if ( DHT11_Read_TempAndHumidity ( & DHT11_Data ) != SUCCESS )       //��ȡ DHT11 ��ʪ����Ϣ
		{
			sprintf ( cStr, "Read DHT11 ERROR!\r\n" );
			printf ( "%s", cStr );     //��ӡ��ȡ DHT11 ��ʪ����Ϣ	
			LCD_ClearLine(LINE(line));	/* ����������� */
			ILI9341_DispStringLine_EN(LINE(line++),cStr);
		}
		else
		{
			sprintf ( cStr, "\r\n��ȡDHT11�ɹ�!\r\n\r\nʪ��Ϊ%d.%d ��RH ���¶�Ϊ %d.%d�� \r\n\r\n", 
			DHT11_Data .humi_int, DHT11_Data .humi_deci, DHT11_Data .temp_int, DHT11_Data.temp_deci );
			printf ( "%s", cStr );     //��ӡ��ȡ DHT11 ��ʪ����Ϣ
			Delay_ms(100);
			
			sprintf(dispBuff,"AQI: %d.%d ��Temp: %d.%d",
							DHT11_Data .humi_int, DHT11_Data .humi_deci, DHT11_Data .temp_int, DHT11_Data.temp_deci);
			
			LCD_ClearLine(LINE(line));	/* ����������� */
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
			//������Ϣ
			_mqtt.PublishData(mqttTopicProPost,mqtt_message,0);
		
		}
		//����7�� ����
		if(line>=7)
		{
			line = 0;
			ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	//����
		}

		//8266������ս���aliyun��Ϣ
	 mqttMessage_Handler();
	 Delay_ms(100);
	}
	#else
	#if 1  //�޸İ�
	while(1)                            
	{	   
		
		//������� ˵��������� ��ʼ��¼ʱ��
		if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON  )
		{
			// ʹ�ܼ�����
			TIM_Cmd(BASIC_TIM, ENABLE);	
			printf( "�����Ѵ���\n" );
			sprintf ( dispBuff, "MASK ON" );
			LCD_ClearLine(LINE(7));	/* ����������� */
			ILI9341_DispStringLine_EN(LINE(7),dispBuff);
			Delay_ms(2000);
			ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	//����

			while ( 1 )
			{
				printf("��ȡ�С���\n");
				//_mqtt.SendHeart();
				if ( DHT11_Read_TempAndHumidity ( & DHT11_Data ) != SUCCESS )       //��ȡ DHT11 ��ʪ����Ϣ
				{
					printf ( "Read DHT11 ERROR!\r\n" );     //��ӡ��ȡ DHT11 ��ʪ����Ϣ	
					sprintf ( cStr, "Read DHT11 ERROR!" );
					LCD_ClearLine(LINE(line));	/* ����������� */
					ILI9341_DispStringLine_EN(LINE(line++),cStr);
				}
				else
				{
					sprintf ( cStr, "\r\n��ȡDHT11�ɹ�!\r\n\r\nʪ��Ϊ%d.%d ��RH ���¶�Ϊ %d.%d�� \r\n\r\n", 
					DHT11_Data .humi_int, DHT11_Data .humi_deci, DHT11_Data .temp_int, DHT11_Data.temp_deci );
					printf ( "%s", cStr );     //��ӡ��ȡ DHT11 ��ʪ����Ϣ
					Delay_ms(100);
					
					sprintf(dispBuff,"AQI: %d.%d ��Temp: %d.%d",
									DHT11_Data .humi_int, DHT11_Data .humi_deci, DHT11_Data .temp_int, DHT11_Data.temp_deci);
					
					LCD_ClearLine(LINE(line));	/* ����������� */
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
					//�����ϱ�
					_mqtt.PublishData(mqttTopicProPost,mqtt_message,0);
					//������Ϣ
					printf("������Ϣ�ɹ�\n");

				}
				//����7�� ����
				if(line>=7)
				{
					line = 0;
					ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	//����
				}

			  //8266������ս���aliyun��Ϣ
			  mqttMessage_Handler();
			  Delay_ms(100);
				
				if ( time >= 20000 ) /* 60000*60 * 1 ms = 1h ʱ�䵽 ���ֹ��� */
				{
					// ʧ�ܼ�����
					TIM_Cmd(BASIC_TIM, DISABLE);	
					BEEP_ON();
					time = 0;
					sprintf ( dispBuff, "press k2 and change mask" );
					LCD_ClearLine(LINE(8));	/* ����������� */
					ILI9341_DispStringLine_EN(LINE(8),dispBuff);
					
					//������״̬�ϱ�
					sprintf(mqtt_message,
					"{\"method\":\"thing.event.property.post\",\"id\":\"630262306\",\"params\":{\
					  \"Motor\":%d,\
					},\"version\":\"1.0.0\"}", 1 );
					//�����ϱ�
					_mqtt.PublishData(mqttTopicProPost,mqtt_message,0);
					
					Delay_ms(100);
					//�¼��ϱ�
					sprintf(mqtt_message,
					"{\"method\":\"thing.event.Shake.post\",\"id\":\"630262306\",\"params\":{\
					  \"motor\":%d,\
						},\"version\":\"1.0.0\"}",
						1
					);
					//�¼��ϱ�
					_mqtt.PublishData(mqttTopicEventPost,mqtt_message,0);
					printf("�¼��ϱ�\r\n");
				}
				//����k2ֹͣ������
				if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON  )
				{
					// ʧ�ܼ�����
					TIM_Cmd(BASIC_TIM, DISABLE);
					BEEP_OFF();
					time = 0;
					break;
				}
				//�������k1 ˵������ժ��
				if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON  )
				{
					// ʧ�ܼ�����
					TIM_Cmd(BASIC_TIM, DISABLE);	
					BEEP_OFF();
					printf("������ժ��\n");
					sprintf ( cStr, "MASK OFF" );
					printf ( "%s", cStr );    
					LCD_ClearLine(LINE(line));	/* ����������� */
					ILI9341_DispStringLine_EN(LINE(line++),cStr);
					time = 0;

					break;
				}
				
			}
		} 

	}
	
	#else  //ʹ���ⲿ�жϵ���û������

	while(1)                            
	{	   
		//������� ˵��������� ��ʼ��¼ʱ��
		if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON  )
		{
			// ʹ�ܼ�����
			TIM_Cmd(BASIC_TIM, ENABLE);	
			printf("�����Ѵ���\n");
			sprintf ( dispBuff, "MASK ON" );
			LCD_ClearLine(LINE(7));	/* ����������� */
			ILI9341_DispStringLine_EN(LINE(7),dispBuff);
			ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	//����
			
			while ( 1 )
			{
				printf("��ȡ�С���\n");
				//_mqtt.SendHeart();
				Delay_ms(5000);
				if ( DHT11_Read_TempAndHumidity ( & DHT11_Data ) != SUCCESS )       //��ȡ DHT11 ��ʪ����Ϣ
				{
					sprintf ( cStr, "Read DHT11 ERROR!\r\n" );
					printf ( "%s", cStr );     //��ӡ��ȡ DHT11 ��ʪ����Ϣ	
					LCD_ClearLine(LINE(line));	/* ����������� */
					ILI9341_DispStringLine_EN(LINE(line++),cStr);
				}
				else
				{
					sprintf ( cStr, "\r\n��ȡDHT11�ɹ�!\r\n\r\nʪ��Ϊ%d.%d ��RH ���¶�Ϊ %d.%d�� \r\n\r\n", 
					DHT11_Data .humi_int, DHT11_Data .humi_deci, DHT11_Data .temp_int, DHT11_Data.temp_deci );
					printf ( "%s", cStr );     //��ӡ��ȡ DHT11 ��ʪ����Ϣ
					Delay_ms(100);
					
					sprintf(dispBuff,"AQI: %d.%d ��Temp: %d.%d",
									DHT11_Data .humi_int, DHT11_Data .humi_deci, DHT11_Data .temp_int, DHT11_Data.temp_deci);
					
					LCD_ClearLine(LINE(line));	/* ����������� */
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
					//������Ϣ
					_mqtt.PublishData(mqttTopicProPost,mqtt_message,0);
					printf("������Ϣ�ɹ�\n");

				}
				//����7�� ����
				if(line>=7)
				{
					line = 0;
					ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	//����
				}

			  //8266������ս���aliyun��Ϣ
			  mqttMessage_Handler();
			  Delay_ms(100);
				
				if ( time >= 30000 ) /* 60000 * 1 ms = 1min ʱ�䵽 ���ֹ��� */
				{
					// ʧ�ܼ�����
					TIM_Cmd(BASIC_TIM, DISABLE);	
					BEEP_ON();
					time = 0;
					sprintf ( dispBuff, "press k2 and change mask" );
					LCD_ClearLine(LINE(8));	/* ����������� */
					ILI9341_DispStringLine_EN(LINE(8),dispBuff);
				}
				//����k2ֹͣ������
				if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON  )
				{
					// ʧ�ܼ�����
					TIM_Cmd(BASIC_TIM, DISABLE);
					BEEP_OFF();
					time = 0;
					break;
				}
				//�������k1 ˵������ժ��
				if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON  )
				{
					// ʧ�ܼ�����
					TIM_Cmd(BASIC_TIM, DISABLE);	
					BEEP_OFF();
					printf("������ժ��\n");
					sprintf ( cStr, "MASK OFF" );
					printf ( "%s", cStr );    
					LCD_ClearLine(LINE(line));	/* ����������� */
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
//�������ж�δ�õ�
void KEY1_IRQHandler(void)
{
  //ȷ���Ƿ������EXTI Line�ж�
	if(EXTI_GetITStatus(KEY1_INT_EXTI_LINE) != RESET) 
	{

    //����жϱ�־λ
		EXTI_ClearITPendingBit(KEY1_INT_EXTI_LINE);     
	}  
}

void KEY2_IRQHandler(void)
{
  //ȷ���Ƿ������EXTI Line�ж�
	if(EXTI_GetITStatus(KEY2_INT_EXTI_LINE) != RESET) 
	{

    //����жϱ�־λ
		EXTI_ClearITPendingBit(KEY2_INT_EXTI_LINE);     
	}  
}
//8266�ж�
void macESP8266_USART_INT_FUN ( void )
{		
	u8 temp;
	uint8_t ucCh;//���ڴ���mqtt

	static u8 rxlen = 0;
	
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) //�жϽ������ݼĴ����Ƿ�������
	{
		ucCh  = USART_ReceiveData( macESP8266_USARTx );
		temp = ucCh;
		//		USART1->DR = temp;//������������1��ӡWIFIģ�鷢�͵�����
		if(rxlen>=255) rxlen=0;
		rxbuf[rxlen++] = temp;
		rxlen%=sizeof(rxbuf);//rxbuf.length()==256
		if ( strEsp8266_Fram_Record .InfBit .FramLength < ( RX_BUF_MAX_LEN - 1 ) )                       //Ԥ��1���ֽ�д������
			strEsp8266_Fram_Record .Data_RX_BUF [ strEsp8266_Fram_Record .InfBit .FramLength ++ ]  = ucCh;
	}

	if(USART_GetITStatus(USART3, USART_IT_IDLE))
	{
		strEsp8266_Fram_Record .InfBit .FramFinishFlag = 1;
		
		ucCh = USART_ReceiveData( macESP8266_USARTx );                                                              //�������������жϱ�־λ(�ȶ�USART_SR��Ȼ���USART_DR)
		temp = ucCh;
		ucTcpClosedFlag = strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "CLOSED\r\n" ) ? 1 : 0;
		
		temp = USART3->SR;
		_mqtt.rxlen = rxlen;
		rxlen = 0;
	}
}
//��ʱ���ж�
void  BASIC_TIM_IRQHandler (void)
{
	if ( TIM_GetITStatus( BASIC_TIM, TIM_IT_Update) != RESET ) 
	{	
		time++;
		TIM_ClearITPendingBit(BASIC_TIM , TIM_FLAG_Update);  		 
	}		 	
}


//����mqtt��Ϣ
void mqttMessage_Handler(void)
{			
	Delay_ms(4000);
	if(_mqtt.rxlen>20)
	{
		int i,j=0;
		//printf("�յ�����mqtt����Ϣ�������С���\r\n\r\n");
		//�����������ַ�
		for(i=0;i<_mqtt.rxlen;i++)
		{
			for(j=0;j<9;j++)
			{
				
				if(_mqtt.rxbuf[i+j] != stopFan[j] && _mqtt.rxbuf[i+j] != stopMotor[j]) 
				{
					break;
				}
			}
			//���÷���
			if(j==7) 
			{
				printf("\r\n\r\n���÷���ֹͣ����\r\n\r\n");
				sprintf(dispBuff,"invoke service:stopFan");
				LCD_ClearLine(LINE(line));	/* ����������� */
				ILI9341_DispStringLine_EN(LINE(line++),dispBuff);
				macLED1_ON ();
				Delay_ms(1000);
				macLED1_OFF();
				break;
			}
			if(j==9) 
			{
				BEEP_OFF();
				printf("\r\n\r\n���÷���ֹͣ��\r\n\r\n");
				sprintf(dispBuff,"invoke service:stopMotor");
				LCD_ClearLine(LINE(line));	/* ����������� */
				ILI9341_DispStringLine_EN(LINE(line++),dispBuff);
				macLED2_ON();
				Delay_ms(1000);
				macLED2_OFF();
				break;
			}
			
			
		}
		//��ս������ַ�
		memset(_mqtt.rxbuf,0,_mqtt.rxlen);
		_mqtt.rxlen = 0;

	}
}



/*********************************************END OF FILE**********************/
