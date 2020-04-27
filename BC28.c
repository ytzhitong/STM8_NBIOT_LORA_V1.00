///AT+NSOCL=0
#include "BC28.h"
#include "string.h"
#include "AM2320.h"

UART_BUF buf_uart;
char *strx,*extstrx;
char atstr[BUFLEN];
int err;    //全局变量

/*******************************************************************************
****入口参数：需要发送的字符串
****出口参数：无
****函数备注：USART发送函数
****版权信息：蓝旗嵌入式系统
*******************************************************************************/
void USART_SendStr(unsigned char *Str) 
{
        while(*Str!=0)//不为结束
        {
            USART_SendData8(*Str);     //发送数据 
            while(!USART_GetFlagStatus (USART_FLAG_TXE));//等待发送完毕
            Str++;//下一个数据
        }
}

void Clear_Buffer(void)//清空串口2缓存
{
    buf_uart.index=0;
    memset(buf_uart.buf,0,BUFLEN);
}

int BC28_Init(void)
{
    int errcount = 0;
    err = 0;   

    USART_SendStr("AT+CGATT=1\r\n");//激活网络，PDP
    delay_ms(300);
    strx=strstr((const char*)buf_uart.buf,(const char*)"OK");//返OK
    Clear_Buffer();	
    if(strx)
    {
        Clear_Buffer();	
        delay_ms(300);
    }
    USART_SendStr("AT+CGATT?\r\n");//查询激活状态
    delay_ms(300);
    strx=strstr((const char*)buf_uart.buf,(const char*)"+CGATT:1");//返1 表明激活成功 获取到IP地址了
    Clear_Buffer();	
    errcount = 0;
    while(strx==NULL)
    {
        errcount++;
        Clear_Buffer();	
        USART_SendStr("AT+CGATT?\r\n");//获取激活状态
        delay_ms(300);
        strx=strstr((const char*)buf_uart.buf,(const char*)"+CGATT:1");//返回1,表明注网成功
        if(errcount>100)     //防止死循环
        {
            err=1;
            errcount = 0;
            break;
        }
    }

    return err;
}

