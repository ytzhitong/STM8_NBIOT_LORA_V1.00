
/* Includes ------------------------------------------------------------------*/
#include "stm8l10x.h"
#include "AM2320.h"
#include "pcf8563.h"
#include "Uart.h"
#include "stdio.h"        //����C�⺯��
#include <string.h>

//����LED�ӿ�
#define LED_GPIO_PORT  GPIOB
#define LED_GPIO_PINS  GPIO_Pin_1

//��������ӿ�
#define KEY_GPIO_PORT  GPIOB
#define KEY_GPIO_PINS  GPIO_Pin_5

//����lora���ѿ�
#define LORA_WAKE_PORT  GPIOC
#define LORA_WAKE_PINS  GPIO_Pin_4

//���屻lora���ѿ�
#define LORA_HOSTWAKE_PORT  GPIOA
#define LORA_HOSTWAKE_PINS  GPIO_Pin_2

//���屻ʱ�ӻ��ѿ�
#define PCF8563_PORT      GPIOB
#define PCF8563_INT_PINS  GPIO_Pin_3

u8 LoraWakeFlag=0;

#define add_start  0x9800
u8  MemCnt=0;      //�Ѵ���������
u32 EndAddr=0;

u16 ByteNumGlo=0;
u16 ByteNumLoc=0;
u8  Data[200];


uint16_t  hum;
short    temp;
uint8_t   ts;

extern _PCF8563_Time_Typedef PCF8563_Time;
extern _PCF8563_Date_Typedef PCF8563_Date;
unsigned char Century;
_PCF8563_Timer_Typedef PCF8563_Timer={60,PCF_Timer_F1,PCF_Timer_Open,PCF_Time_INT_Open};

/*******************************************************************************
****��ڲ�������
****���ڲ�������
****������ע������ȷ��ʱ����
****��Ȩ��Ϣ������Ƕ��ʽϵͳ
*******************************************************************************/
void Delay(__IO uint16_t nCount)
{
    /* Decrement nCount value */
    while (nCount != 0)
    {
        nCount--;
    }  
}

void USART_SendNbyte(unsigned char *Str,unsigned char n) 
{
    for(unsigned char i=0;i<n;i++)
    {
            USART_SendData8(*Str);     //�������� 
            while(!USART_GetFlagStatus (USART_FLAG_TXE));//�ȴ��������
            Str++;//��һ������
    }
}

//������֯�������һ�����ݶ���׼�����ͣ������ֽ���,
//��λ��ǰ����λ�ں�
u16 DataReOrg(unsigned char *Str,unsigned char num)
{ 
  ByteNumLoc=num*11;
  EndAddr= add_start+MemCnt*11-1; 
 
  for(u16 i=0;i<ByteNumLoc;i++)
  {
     *Str=FLASH_ReadByte(EndAddr);
     Str++;
     EndAddr--;
  }
  
  return ByteNumLoc;
}

void DataErase(unsigned char num)
{
  ByteNumLoc=num*11;
  EndAddr= add_start+MemCnt*11-1;;
  
  for(u16 i=0;i<ByteNumLoc;i++)
  {
     FLASH_EraseByte(EndAddr);
     EndAddr--;
  }
  
  MemCnt=MemCnt-num;
}


void DataToLora(unsigned char num)
{ 
  
    GPIO_ResetBits(LORA_WAKE_PORT,LORA_WAKE_PINS); //����
    delay_ms(10);
  
    ByteNumGlo = DataReOrg(Data,num);
    
    USART_SendNbyte(Data,ByteNumGlo) ;
    
    DataErase(num);
    
    delay_ms(1000); 
    
    GPIO_SetBits(LORA_WAKE_PORT,LORA_WAKE_PINS); //����
    delay_ms(10);
}

//�浵���ݣ������Ѵ�����
void DataStore(void)
{
  u16  i;
  u8  Buffer[11];
  u8  val  = 0; 
  u32 add;

  add=add_start+0x0A;
  MemCnt=0;
  //��ȡ�Ѵ����ݵ�������MemCnt��ʾ�Ѵ�����
  val=FLASH_ReadByte(add);
  while((val==0x55)&&(MemCnt<180))
  {
    MemCnt++;
    add=add+11;
    val=FLASH_ReadByte(add);
  }
  
  //�洢���ݲ���
  Buffer[10] =0x55;
  Buffer[9] =PCF8563_Date.RTC_Years;
  Buffer[8] =PCF8563_Date.RTC_Months; 
  Buffer[7] =PCF8563_Date.RTC_Days;
  Buffer[6] =PCF8563_Time.RTC_Hours;
  Buffer[5] =PCF8563_Time.RTC_Minutes;
  Buffer[4] =PCF8563_Time.RTC_Seconds;
  Buffer[3] =temp>>8;
  Buffer[2] =temp;
  Buffer[1] =hum>>8;
  Buffer[0] =hum;
 
  add=add_start+MemCnt*11;
  for(i=0;i<11;i++)
  {
    FLASH_ProgramByte((add + i), Buffer[i]);
  }  
  MemCnt++;
}

/*******************************************************************************
****��ڲ�������
****���ڲ�������
****������ע����������PD0��LED
****��Ȩ��Ϣ������Ƕ��ʽϵͳ
*******************************************************************************/
void main(void)
{  
    CLK_MasterPrescalerConfig(CLK_MasterPrescaler_HSIDiv1);
 
    GPIO_Init(LED_GPIO_PORT, LED_GPIO_PINS, GPIO_Mode_Out_PP_Low_Slow);//��ʼ��LED�˿ڣ���������͵�ƽ������
    GPIO_SetBits(LED_GPIO_PORT,LED_GPIO_PINS); //����
    
    GPIO_Init(KEY_GPIO_PORT, KEY_GPIO_PINS, GPIO_Mode_In_FL_No_IT);//��ʼ��KEY�˿�
    
    GPIO_Init(LORA_WAKE_PORT, LORA_WAKE_PINS, GPIO_Mode_Out_OD_Low_Slow);//��ʼ��LORA���Ѷ˿ڣ���©���������
    GPIO_SetBits(LORA_WAKE_PORT,LORA_WAKE_PINS); //����lora��������

    GPIO_Init(LORA_HOSTWAKE_PORT, LORA_HOSTWAKE_PINS, GPIO_Mode_In_FL_IT);//��ʼ��LORA����mcu�˿�
    GPIO_Init(PCF8563_PORT, PCF8563_INT_PINS, GPIO_Mode_In_FL_IT);//
    EXTI_DeInit (); //�ָ��жϵ��������� 
    EXTI_SetPinSensitivity (EXTI_Pin_2,EXTI_Trigger_Rising);//�ⲿ�ж�2�������ش�����������10
    EXTI_SetPinSensitivity (EXTI_Pin_3,EXTI_Trigger_Falling);
        
    CLK_PeripheralClockConfig (CLK_Peripheral_USART,ENABLE); //ʹ�ܴ���ʱ��
        //��ʼ��USART��2����--TXD-RXD
    GPIO_Init(GPIOC,GPIO_Pin_3,GPIO_Mode_Out_PP_High_Fast);//TXD
    GPIO_Init(GPIOC,GPIO_Pin_2,GPIO_Mode_In_PU_No_IT);//RXD
    USART_Init(115200,USART_WordLength_8D,USART_StopBits_1,USART_Parity_No,(USART_Mode_TypeDef)(USART_Mode_Tx|USART_Mode_Rx));//USART��ʼ����9600��8N1
    USART_ITConfig (USART_IT_RXNE,ENABLE);//ʹ�ܽ����жϣ��ж�������28
    USART_Cmd (ENABLE);//ʹ��USART  
   
    enableInterrupts();
   
    InitIIC();
    
    delay_ms(1000);
               
    FLASH_SetProgrammingTime(FLASH_ProgramTime_Standard);//��׼���ʱ��
    FLASH_Unlock(FLASH_MemType_Program);  //�������
    
    while(1)
    {
      
        GPIO_ResetBits(LED_GPIO_PORT,LED_GPIO_PINS); //��di
            
        while(GPIO_ReadInputDataBit(KEY_GPIO_PORT,KEY_GPIO_PINS)!=0)//��GP����״̬
        {
           TimeSVN();   //��ʱģʽ
        }

        ts=AM2320_get_value(&hum,&temp);       
        PCF8563_GetDate(PCF_Format_BIN, &Century, &PCF8563_Date);                
        PCF8563_GetTime(PCF_Format_BIN,&PCF8563_Time);

        delay_ms(500);
    
        GPIO_SetBits(LED_GPIO_PORT,LED_GPIO_PINS); //����

        ts=AM2320_get_value(&hum,&temp);        
        PCF8563_GetDate(PCF_Format_BIN, &Century, &PCF8563_Date);                
        PCF8563_GetTime(PCF_Format_BIN,&PCF8563_Time);    

        PCF8563_SetTimer(&PCF8563_Timer);         
        DataStore();
        
        if(LoraWakeFlag==1)
        {
          LoraWakeFlag=0;
                    
          u8 DataNum=MemCnt/15;
          u8 DataRem=MemCnt%15;
          
          for(u8 i=0;i<DataNum;i++)
          {
              DataToLora(15);             
          }
          
          DataToLora(DataRem);         
          
        }
                               
        GPIO_ResetBits(LED_GPIO_PORT,LED_GPIO_PINS); //����
                
        halt();//������͹���

        delay_ms(1000);
    }
 
}
  
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/