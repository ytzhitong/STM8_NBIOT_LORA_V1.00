#include "stm8l10x.h"

void Clear_Buffer(void);//��ջ���	
int  BC28_Init(void);
void BC28_Senddata(uint8_t *len,uint8_t *data);

#define BUFLEN 30      //���黺���С
typedef struct _UART_BUF
{
    char buf [BUFLEN+1];               
    unsigned int index ;
    unsigned char flag;
}UART_BUF;

extern UART_BUF buf_uart;

typedef struct
{
   uint8_t CSQ;    
   uint8_t Socketnum;   //���
   uint8_t reclen[10];   //��ȡ�����ݵĳ���
   uint8_t res;      
   uint8_t recdatalen[10];
   uint8_t recdata[100];
} BC28;

void TimeSVN(void);