//#include "mqttTest.h"

////串口1发送一个字节
////void macESP8266_USARTx_SendByte(u8 val)
////{
////    USART_SendData(macESP8266_USARTx, val);
////    while (USART_GetFlagStatus(macESP8266_USARTx, USART_FLAG_TC) == RESET);	//等待发送完成
////}
//////串口1发送一个数据包
////void macESP8266_USARTx_SendBuf(u8 *buf,u8 len)
////{
////    while(len--)	macESP8266_USARTx_SendByte(*buf++);
////}


//int32_t mqtt_transport_sendPacketBuffer(uint8_t* buf, uint32_t buflen)
//{
//    int rc = 0;
//    
//    //SendBuffer(UART4, buf, buflen);
//    macESP8266_USARTx_SendBuf(buf,buflen);
//    return rc;
//}

//int32_t transport_getdata(uint8_t* buf, int32_t count)
//{
////    int32_t n, rc = 0;
////    
////    memcpy(buf, (void*)&RxBuffer4[readBufLen], count);
////		
////    readBufLen += count;
////    
////    return count;
//	//return (int32_t)ESP8266_ReceiveString ( ENABLE );
//}

//uint8_t mqtt_connect(void)
//{
//    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
//    int rc = 0;
//    uint8_t buf[200];
//    int buflen = sizeof(buf);
//    int mysock = 0;
//    MQTTString topicString = MQTTString_initializer;
//    int len = 0;

//    data.clientID.cstring = "1234|securemode=3,signmethod=hmacsha1,timestamp=999|";
//    data.keepAliveInterval = 20;      //seconds
//    data.cleansession = 1;
//    data.username.cstring = "Smart_Mask&a104fcohulX";
//    data.password.cstring = "88A834ED73F726745FDBB4B7AB47D6FACA3FA8AE";
//    data.MQTTVersion = 4;

//    len = MQTTSerialize_connect((unsigned char *)buf, buflen, &data);

////    len += MQTTSerialize_disconnect((unsigned char *)(buf + len), buflen - len);

//    rc = mqtt_transport_sendPacketBuffer(buf, len);
//    printf("len:%d|rc:%d\r\n", len, rc);
//    
//    //reset_receive_buf();
//    
//    /* wait for connack */
//    if (MQTTPacket_read(buf, buflen, transport_getdata) == CONNACK)
//    {
//        unsigned char sessionPresent, connack_rc;
//        if (MQTTDeserialize_connack(&sessionPresent, &connack_rc, buf, buflen) != 1 || connack_rc != 0)
//        {
//            printf("Unable to connect, return code %d\n", connack_rc);
//            return 0;
//        }
//        else 
//        {
//            printf("MQTT CONNECT!\n");
//        }
//    }
//    else
//        printf("no connack\n");
//        return 0;
//}


//void mqtt_publish(char *pPubTopic, char *pMessage)
//{
//    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
//    int rc = 0;
//    uint8_t buf[200];
//    int buflen = sizeof(buf);
//    int mysock = 0;
//    MQTTString topicString = MQTTString_initializer;
//    char* payload = pMessage;
//    int payloadlen = strlen(payload);
//    int len = 0;

//    topicString.cstring = pPubTopic;
//    len += MQTTSerialize_publish((unsigned char *)(buf + len), buflen - len, 0, 0, 0, 0, topicString, (unsigned char *)payload, payloadlen);

////    len += MQTTSerialize_disconnect((unsigned char *)(buf + len), buflen - len);

//		//!!!
//    rc = mqtt_transport_sendPacketBuffer(buf, len);

//}

//uint8_t mqtt_subscribe(char *pSubTopic)
//{
//    uint8_t buf[200] = {0};
//    uint32_t buflen = sizeof(buf);
//    int32_t msgid = 1;
//    int32_t req_qos = 0;
//    uint32_t rc, len = 0;
//    MQTTString topicString = MQTTString_initializer;
//    topicString.cstring = pSubTopic;
//    
//    len = MQTTSerialize_subscribe(buf, buflen, 0, msgid, 1, &topicString, &req_qos);

//		//!!!
//    rc = mqtt_transport_sendPacketBuffer(buf, len);

//    //reset_receive_buf();
//    if (MQTTPacket_read(buf, buflen, transport_getdata) == SUBACK) /* wait for suback */
//    {
//        unsigned short submsgid;
//        int subcount;
//        int granted_qos;

//        rc = MQTTDeserialize_suback(&submsgid, 1, &subcount, &granted_qos, buf, buflen);
//        if (granted_qos != 0)
//        {
//            printf("granted qos != 0, %d\n", granted_qos);
//            return 0;
//        }
//        else
//        {
//            printf("granted qos = 0\n");
//            return 1;
//        }
//    }
//    else
//        printf("no suback received!\r\n");
//        return 0;
//}

