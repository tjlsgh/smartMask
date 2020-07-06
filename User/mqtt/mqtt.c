#include "mqtt.h"
#include "bsp_esp8266.h"

#if 1
typedef enum
{
	//���� 	    ֵ 			������������ 	����
	M_RESERVED1	=0	,	//	��ֹ	����
	M_CONNECT		,	//	�ͻ��˵������	�ͻ����������ӷ����
	M_CONNACK		,	//	����˵��ͻ���	���ӱ���ȷ��
	M_PUBLISH		,	//	������������	������Ϣ
	M_PUBACK		,	//	������������	QoS 1��Ϣ�����յ�ȷ��
	M_PUBREC		,	//	������������	�����յ�����֤������һ����
	M_PUBREL		,	//	������������	�����ͷţ���֤�����ڶ�����
	M_PUBCOMP		,	//	������������	QoS 2��Ϣ������ɣ���֤������������
	M_SUBSCRIBE		,	//	�ͻ��˵������	�ͻ��˶�������
	M_SUBACK		,	//	����˵��ͻ���	����������ȷ��
	M_UNSUBSCRIBE	,	//	�ͻ��˵������	�ͻ���ȡ����������
	M_UNSUBACK		,	//	����˵��ͻ���	ȡ�����ı���ȷ��
	M_PINGREQ		,	//	�ͻ��˵������	��������
	M_PINGRESP		,	//	����˵��ͻ���	������Ӧ
	M_DISCONNECT	,	//	�ͻ��˵������	�ͻ��˶Ͽ�����
	M_RESERVED2		,	//	��ֹ	����
}_typdef_mqtt_message;



//���ӳɹ���������Ӧ 20 02 00 00
//�ͻ��������Ͽ����� e0 00
const u8 parket_connetAck[] = {0x20,0x02,0x00,0x00};
const u8 parket_disconnet[] = {0xe0,0x00};
const u8 parket_heart[] = {0xc0,0x00};
const u8 parket_heart_reply[] = {0xc0,0x00};
const u8 parket_subAck[] = {0x90,0x03};

static void Mqtt_SendBuf(u8 *buf,u16 len);

static void Init(u8 *prx,u16 rxlen,u8 *ptx,u16 txlen);
static u8 Connect(char *ClientID,char *Username,char *Password);
static u8 SubscribeTopic(char *topic,u8 qos,u8 whether);
static u8 PublishData(char *topic, char *message, u8 qos);
static void SentHeart(void);
static void Disconnect(void);

_typdef_mqtt _mqtt = 
{
	0,0,
	0,0,
	Init,
	Connect,
	SubscribeTopic,
	PublishData,
	SentHeart,
	Disconnect,
};

static void Init(u8 *prx,u16 rxlen,u8 *ptx,u16 txlen)
{
	_mqtt.rxbuf = prx;_mqtt.rxlen = rxlen;
	_mqtt.txbuf = ptx;_mqtt.txlen = txlen;
	
	memset(_mqtt.rxbuf,0,_mqtt.rxlen);
	memset(_mqtt.txbuf,0,_mqtt.txlen);
	
	//�������������Ͽ�
	_mqtt.Disconnect();Delay_ms(100);
	_mqtt.Disconnect();Delay_ms(100);
}


//���ӷ������Ĵ������
static u8 Connect(char *ClientID,char *Username,char *Password)
{	
	u8 cnt=3;
	u8 wait;
	
	int ClientIDLen = strlen(ClientID);
	int UsernameLen = strlen(Username);
	int PasswordLen = strlen(Password);
	int DataLen;
	_mqtt.txlen=0;
	//�ɱ䱨ͷ+Payload  ÿ���ֶΰ��������ֽڵĳ��ȱ�ʶ
	DataLen = 10 + (ClientIDLen+2) + (UsernameLen+2) + (PasswordLen+2);
	
	//�̶���ͷ
	//���Ʊ�������
	_mqtt.txbuf[_mqtt.txlen++] = 0x10;		
	
	//ʣ�೤��(�������̶�ͷ��)
	do
	{
		u8 encodedByte = DataLen % 128;
		DataLen = DataLen / 128;
		// if there are more data to encode, set the top bit of this byte
		if ( DataLen > 0 )
			encodedByte = encodedByte | 128;
		_mqtt.txbuf[_mqtt.txlen++] = encodedByte;
	}while ( DataLen > 0 );
    	
	//�ɱ䱨ͷ
	//Э����
	_mqtt.txbuf[_mqtt.txlen++] = 0;        		// Protocol Name Length MSB    
	_mqtt.txbuf[_mqtt.txlen++] = 4;        		// Protocol Name Length LSB    
	_mqtt.txbuf[_mqtt.txlen++] = 'M';        	// ASCII Code for M    
	_mqtt.txbuf[_mqtt.txlen++] = 'Q';        	// ASCII Code for Q    
	_mqtt.txbuf[_mqtt.txlen++] = 'T';        	// ASCII Code for T    
	_mqtt.txbuf[_mqtt.txlen++] = 'T';        	// ASCII Code for T    
	//Э�鼶��
	_mqtt.txbuf[_mqtt.txlen++] = 4;        		// MQTT Protocol version = 4    
	//���ӱ�־
	_mqtt.txbuf[_mqtt.txlen++] = 0xc2;        	// conn flags 
	_mqtt.txbuf[_mqtt.txlen++] = 0;        		// Keep-alive Time Length MSB    
	_mqtt.txbuf[_mqtt.txlen++] = 60;        	// Keep-alive Time Length LSB  60S������  

	_mqtt.txbuf[_mqtt.txlen++] = BYTE1(ClientIDLen);// Client ID length MSB    
	_mqtt.txbuf[_mqtt.txlen++] = BYTE0(ClientIDLen);// Client ID length LSB  	
	memcpy(&_mqtt.txbuf[_mqtt.txlen],ClientID,ClientIDLen);
	_mqtt.txlen += ClientIDLen;
    
	if(UsernameLen > 0)
	{   
		_mqtt.txbuf[_mqtt.txlen++] = BYTE1(UsernameLen);		//username length MSB    
		_mqtt.txbuf[_mqtt.txlen++] = BYTE0(UsernameLen);    	//username length LSB    
		memcpy(&_mqtt.txbuf[_mqtt.txlen],Username,UsernameLen);
		_mqtt.txlen += UsernameLen;
	}
	
	if(PasswordLen > 0)
	{    
		_mqtt.txbuf[_mqtt.txlen++] = BYTE1(PasswordLen);		//password length MSB    
		_mqtt.txbuf[_mqtt.txlen++] = BYTE0(PasswordLen);    	//password length LSB  
		memcpy(&_mqtt.txbuf[_mqtt.txlen],Password,PasswordLen);
		_mqtt.txlen += PasswordLen; 
	}    
	

	while(cnt--)
	{
		memset(_mqtt.rxbuf,0,_mqtt.rxlen);
		Mqtt_SendBuf(_mqtt.txbuf,_mqtt.txlen);
//		for(i=0;i<_mqtt.txlen;i++)
//		{
//			printf("%c",_mqtt.txbuf[i]);
//		}

		wait=30;//�ȴ�3sʱ��
		while(wait--)
		{
//			printf("%d %d,",_mqtt.rxbuf[0],_mqtt.rxbuf[1]);
			//CONNECT
			if(_mqtt.rxbuf[0]==parket_connetAck[0] && _mqtt.rxbuf[1]==parket_connetAck[1]) //���ӳɹ�			   
			{
				return 1;//���ӳɹ�
			}
//			if(strEsp8266_Fram_Record .Data_RX_BUF [0]  == parket_connetAck[0] 
//				&& strEsp8266_Fram_Record .Data_RX_BUF [1] == parket_connetAck[1])
//			{
//				return 1;
//			}
			Delay_ms(100);			
		}
	}
	return 0;
}


//MQTT����/ȡ���������ݴ������
//topic       ���� 
//qos         ��Ϣ�ȼ� 
//whether     ����/ȡ�����������
static u8 SubscribeTopic(char *topic,u8 qos,u8 whether)
{    	
	u8 cnt=2;
	u8 wait;
	int topiclen = strlen(topic);
	
	int DataLen = 2 + (topiclen+2) + (whether?1:0);//�ɱ䱨ͷ�ĳ��ȣ�2�ֽڣ�������Ч�غɵĳ���
	_mqtt.txlen=0;

	//�̶���ͷ
	//���Ʊ�������
	if(whether) _mqtt.txbuf[_mqtt.txlen++] = 0x82; //��Ϣ���ͺͱ�־����
	else	_mqtt.txbuf[_mqtt.txlen++] = 0xA2;    //ȡ������

	//ʣ�೤��
	do
	{
		u8 encodedByte = DataLen % 128;
		DataLen = DataLen / 128;
		// if there are more data to encode, set the top bit of this byte
		if ( DataLen > 0 )
			encodedByte = encodedByte | 128;
		_mqtt.txbuf[_mqtt.txlen++] = encodedByte;
	}while ( DataLen > 0 );	
	
	//�ɱ䱨ͷ
	_mqtt.txbuf[_mqtt.txlen++] = 0;				//��Ϣ��ʶ�� MSB
	_mqtt.txbuf[_mqtt.txlen++] = 0x01;           //��Ϣ��ʶ�� LSB
	//��Ч�غ�
	_mqtt.txbuf[_mqtt.txlen++] = BYTE1(topiclen);//���ⳤ�� MSB
	_mqtt.txbuf[_mqtt.txlen++] = BYTE0(topiclen);//���ⳤ�� LSB   
	memcpy(&_mqtt.txbuf[_mqtt.txlen],topic,topiclen);
	_mqtt.txlen += topiclen;
	
	if(whether)
	{
			_mqtt.txbuf[_mqtt.txlen++] = qos;//QoS����
	}

	while(cnt--)
	{
		memset(_mqtt.rxbuf,0,_mqtt.rxlen);
		Mqtt_SendBuf(_mqtt.txbuf,_mqtt.txlen);
		wait=30;//�ȴ�3sʱ��
		while(wait--)
		{
			if(_mqtt.rxbuf[0]==parket_subAck[0] && _mqtt.rxbuf[1]==parket_subAck[1]) //���ĳɹ�			   
			{
				return 1;//���ĳɹ�
			}
			Delay_ms(100);			
		}
	}
	if(cnt) return 1;	//���ĳɹ�
	return 0;
}

//MQTT�������ݴ������
//topic   ���� 
//message ��Ϣ
//qos     ��Ϣ�ȼ� 
static u8 PublishData(char *topic, char *message, u8 qos)
{  
	int topicLength = strlen(topic);    
	int messageLength = strlen(message);     
	static u16 id=0;
	int DataLen;
	_mqtt.txlen=0;
	//��Ч�غɵĳ����������㣺�ù̶���ͷ�е�ʣ�೤���ֶε�ֵ��ȥ�ɱ䱨ͷ�ĳ���
	//QOSΪ0ʱû�б�ʶ��
	//���ݳ���             ������   ���ı�ʶ��   ��Ч�غ�
	if(qos)	DataLen = (2+topicLength) + 2 + messageLength;       
	else	DataLen = (2+topicLength) + messageLength;   

	//�̶���ͷ
	//���Ʊ�������
	_mqtt.txbuf[_mqtt.txlen++] = 0x30;    // MQTT Message Type PUBLISH  

	//ʣ�೤��
	do
	{
		u8 encodedByte = DataLen % 128;
		DataLen = DataLen / 128;
		// if there are more data to encode, set the top bit of this byte
		if ( DataLen > 0 )
			encodedByte = encodedByte | 128;
		_mqtt.txbuf[_mqtt.txlen++] = encodedByte;
	}while ( DataLen > 0 );	
	
	_mqtt.txbuf[_mqtt.txlen++] = BYTE1(topicLength);//���ⳤ��MSB
	_mqtt.txbuf[_mqtt.txlen++] = BYTE0(topicLength);//���ⳤ��LSB 
	memcpy(&_mqtt.txbuf[_mqtt.txlen],topic,topicLength);//��������
	_mqtt.txlen += topicLength;
        
	//���ı�ʶ��
	if(qos)
	{
		_mqtt.txbuf[_mqtt.txlen++] = BYTE1(id);
		_mqtt.txbuf[_mqtt.txlen++] = BYTE0(id);
		id++;
	}
	memcpy(&_mqtt.txbuf[_mqtt.txlen],message,messageLength);
	_mqtt.txlen += messageLength;
        
	Mqtt_SendBuf(_mqtt.txbuf,_mqtt.txlen);
	return _mqtt.txlen;
}

static void SentHeart(void)
{
	Mqtt_SendBuf((u8 *)parket_heart,sizeof(parket_heart));
}

static void Disconnect(void)
{
	Mqtt_SendBuf((u8 *)parket_disconnet,sizeof(parket_disconnet));
}

static void Mqtt_SendBuf(u8 *buf,u16 len)
{
	macESP8266_USARTx_SendBuf(buf,len);
}
#else
//�̶���ͷ��ʽ��10
unsigned char GetDataFixedHead(unsigned char MesType,unsigned char DupFlag,unsigned char QosLevel,unsigned char Retain)
{
	unsigned char dat = 0;
	dat = (MesType & 0x0f) << 4;
	dat |= (DupFlag & 0x01) << 3;
	dat |= (QosLevel & 0x03) << 1;
	dat |= (Retain & 0x01);
	return dat;//1
}
//unsigned char 8λ
void GetDataConnet(unsigned char *buff)//��ȡ���ӵ����ݰ���ȷ���ӷ���20 02 00 00
{
	unsigned int i,len,lennum = 0;
	unsigned char *msg;
	buff[0] = GetDataFixedHead(MQTT_TypeCONNECT,0,0,0);//MQTT_TypeCONNECT��1
	buff[2] = 0x00;
	buff[3] = 0x04;
	buff[4] = 'M';
	buff[5] = 'Q';
	buff[6] = 'T';
	buff[7] = 'T';
	buff[8] = 0x04;//Э�鼶�� Protocol Level
	buff[9] = 0 | (MQTT_StaCleanSession << 1) | (MQTT_StaWillFlag << 1)
							| (MQTT_StaWillQoS << 3) | (MQTT_StaWillRetain << 5) 
							|	(MQTT_StaPasswordFlag << 6) |(MQTT_StaUserNameFlag << 7);//���ӱ�־
	buff[10] = MQTT_KeepAlive >> 8;
	buff[11] = MQTT_KeepAlive;
	len = strlen(MQTT_ClientIdentifier);
	buff[12] = len >> 8;
	buff[13] = len;
	msg = MQTT_ClientIdentifier;
	for(i = 0;i<len;i++)
	{
		buff[14+i] =  msg[i];
	}
	lennum += len;
	if(MQTT_StaWillFlag)
	{
		len = strlen(MQTT_WillTopic);
		buff[13 + lennum + 1] = len >> 8;
		buff[13 + lennum + 2] = len;
		lennum += 2;
		msg = MQTT_WillTopic;
		for(i = 0;i<len;i++)
		{
			buff[14+lennum+i] =  msg[i];
		}
		lennum += len;
		len = strlen(MQTT_WillMessage);
		buff[12] = len >> 8;
		buff[13] = len;
		lennum += 2;
		msg = MQTT_WillMessage;
		for(i = 0;i<len;i++)
		{
			buff[14+lennum+i] =  msg[i];
		}
		lennum += len;
	}
	if(MQTT_StaUserNameFlag)
	{
		len = strlen(MQTT_UserName);
		buff[13 + lennum + 1] = len >> 8;
		buff[13 + lennum + 2] = len;
		lennum += 2;
		msg = MQTT_UserName;
		for(i = 0;i<len;i++)
		{
			buff[14+lennum+i] =  msg[i];
		}
		lennum += len;
		
	}
	if(MQTT_StaPasswordFlag)
	{
		len = strlen(MQTT_Password);
		buff[13 + lennum + 1] = len >> 8;
		buff[13 + lennum + 2] = len;
		lennum += 2;
		msg = MQTT_Password;
		for(i = 0;i<len;i++)
		{
			buff[14+lennum+i] =  msg[i];
		}
		lennum += len;
	}
	buff[1] = 13 + lennum - 1;
}
void GetDataDisConnet(unsigned char *buff)//��ȡ�Ͽ����ӵ����ݰ�
{
	buff[0] = 0xe0;
	buff[1] = 0;
}
void GetDataPINGREQ(unsigned char *buff)//������������ݰ��ɹ�����d0 00
{
	buff[0] = 0xc0;
	buff[1] = 0;
}
/*
	�ɹ�����90 0x 00 Num RequestedQoS
*/
void GetDataSUBSCRIBE(unsigned char *buff,const char *dat,unsigned int Num,unsigned char RequestedQoS)//������������ݰ� Num:������� RequestedQoS:��������Ҫ��0,1��2
{
	unsigned int i,len = 0,lennum = 0; 
	buff[0] = 0x82;
	len = strlen(dat);
	buff[2] = Num>>8;
	buff[3] = Num;
	buff[4] = len>>8;
	buff[5] = len;
	for(i = 0;i<len;i++)
	{
		buff[6+i] = dat[i];
	}
	lennum = len;
	buff[6 + lennum ] = RequestedQoS;
	buff[1] = lennum + 5;
}
void GetDataPUBLISH(unsigned char *buff,unsigned char dup, unsigned char qos,unsigned char retain,const char *topic ,const char *msg)//��ȡ������Ϣ�����ݰ�
{
	unsigned int i,len=0,lennum=0;
	buff[0] = GetDataFixedHead(MQTT_TypePUBLISH,dup,qos,retain);
	len = strlen(topic);
	buff[2] = len>>8;
	buff[3] = len;
	for(i = 0;i<len;i++)
	{
		buff[4+i] = topic[i];
	}
	lennum = len;
	len = strlen(msg);
	for(i = 0;i<len;i++)
	{
		buff[4+i+lennum] = msg[i];
	}
	lennum += len;
	buff[1] = lennum + 2;
}

#endif

/*********************************************END OF FILE********************************************/
