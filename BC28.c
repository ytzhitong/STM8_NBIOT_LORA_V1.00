///AT+NSOCL=0
#include "BC28.h"
#include "string.h"
#include "AM2320.h"

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

