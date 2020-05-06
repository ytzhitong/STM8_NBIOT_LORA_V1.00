///AT+NSOCL=0
#include "Uart.h"
#include "string.h"
#include "AM2320.h"
#include "pcf8563.h"

//#include <stdlib.h>
//#include <stdio.h>

UART_BUF buf_uart;
char *strx,*extstrx;
char atstr[BUFLEN];
int err;    //ȫ�ֱ���

/*******************************************************************************
****��ڲ�������Ҫ���͵��ַ���
****���ڲ�������
****������ע��USART���ͺ���
****��Ȩ��Ϣ������Ƕ��ʽϵͳ
*******************************************************************************/
void USART_SendStr(unsigned char *Str) 
{
        while(*Str!=0)//��Ϊ����
        {
            USART_SendData8(*Str);     //�������� 
            while(!USART_GetFlagStatus (USART_FLAG_TXE));//�ȴ��������
            Str++;//��һ������
        }
}

void Clear_Buffer(void)//��մ���2����
{
    buf_uart.index=0;
    memset(buf_uart.buf,0,BUFLEN);
}

int BC28_Init(void)
{
    int errcount = 0;
    err = 0;   

    USART_SendStr("AT+CGATT=1\r\n");//�������磬PDP
    delay_ms(300);
    strx=strstr((const char*)buf_uart.buf,(const char*)"OK");//��OK
    Clear_Buffer();	
    if(strx)
    {
        Clear_Buffer();	
        delay_ms(300);
    }
    USART_SendStr("AT+CGATT?\r\n");//��ѯ����״̬
    delay_ms(300);
    strx=strstr((const char*)buf_uart.buf,(const char*)"+CGATT:1");//��1 ��������ɹ� ��ȡ��IP��ַ��
    Clear_Buffer();	
    errcount = 0;
    while(strx==NULL)
    {
        errcount++;
        Clear_Buffer();	
        USART_SendStr("AT+CGATT?\r\n");//��ȡ����״̬
        delay_ms(300);
        strx=strstr((const char*)buf_uart.buf,(const char*)"+CGATT:1");//����1,����ע���ɹ�
        if(errcount>100)     //��ֹ��ѭ��
        {
            err=1;
            errcount = 0;
            break;
        }
    }

    return err;
}

_PCF8563_Time_Typedef PCF8563_Time_Set;
_PCF8563_Date_Typedef PCF8563_Date_Set;
//
void TimeSVN(void)
{
//     char* ptr = NULL;
   
     if(buf_uart.flag==1)
     {
       delay_ms(200);
       
//       for(int i=0;i<56;i++)
//       {
//         if((buf_uart.buf[i]=='#')&&(buf_uart.buf[i+1]=='#'))
//         {
//           PCF8563_Date_Set.RTC_Years=buf_uart.buf[i+4]*10+buf_uart.buf[i+5]
//         }
//       }
//             
       if(strstr(buf_uart.buf, "TimeSet") != NULL)//
       {
//         ptr = strstr(buf_uart.buf, "TimeSet");
//         ptr+=8;
//         PCF8563_Date_Set.RTC_Years = atoi(ptr)-2000;
//         
//         ptr+=5;
//         PCF8563_Date_Set.RTC_Months = atoi(ptr);
//      
//         ptr+=3;
//         PCF8563_Date_Set.RTC_Days = atoi(ptr);
//  
//         ptr+=3;
//         PCF8563_Time_Set.RTC_Hours = atoi(ptr);         
//
//         ptr+=3;
//         PCF8563_Time_Set.RTC_Minutes = atoi(ptr); 
//
//         ptr+=3;
//         PCF8563_Time_Set.RTC_Seconds = atoi(ptr); 

         PCF8563_SetDate(PCF_Format_BIN, PCF_Century_20xx,&PCF8563_Date_Set);
         PCF8563_SetTime(PCF_Format_BIN, &PCF8563_Time_Set);
         
       }
       
       buf_uart.flag=0;
       
     }
}

