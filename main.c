
/* Includes ------------------------------------------------------------------*/
#include "stm8l10x.h"
#include "AM2320.h"
#include "pcf8563.h"
#include "Uart.h"
#include "stdio.h"        //包含C库函数
#include <time.h>
#include <string.h>

//定义LED接口
#define LED_GPIO_PORT  GPIOB
#define LED_GPIO_PINS  GPIO_Pin_1

//定义输入接口
#define KEY_GPIO_PORT  GPIOB
#define KEY_GPIO_PINS  GPIO_Pin_5

//定义lora唤醒口
#define LORA_WAKE_PORT  GPIOC
#define LORA_WAKE_PINS  GPIO_Pin_4

//定义被lora唤醒口
#define LORA_HOSTWAKE_PORT  GPIOA
#define LORA_HOSTWAKE_PINS  GPIO_Pin_2

//定义被时钟唤醒口
#define PCF8563_PORT      GPIOB
#define PCF8563_INT_PINS  GPIO_Pin_3

u8 LoraWakeFlag=0;

#define add_start  0x9800
u8  MemCnt=0;      //已存数据组数
u32 EndAddr=0;

u16 ByteNumGlo=0;
u16 ByteNumLoc=0;
u8  Data[200];


uint16_t  hum;
short    temp;
uint8_t   ts;

u32  TimeStamp;
struct tm stm;

extern _PCF8563_Time_Typedef PCF8563_Time;
extern _PCF8563_Date_Typedef PCF8563_Date;
unsigned char Century;
_PCF8563_Timer_Typedef PCF8563_Timer={10,PCF_Timer_F1,PCF_Timer_Open,PCF_Time_INT_Open};

/*******************************************************************************
****入口参数：无
****出口参数：无
****函数备注：不精确延时函数
****版权信息：蓝旗嵌入式系统
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
            USART_SendData8(*Str);     //发送数据 
            while(!USART_GetFlagStatus (USART_FLAG_TXE));//等待发送完毕
            Str++;//下一个数据
    }
}

//数据组织，从最后一组数据读起，准备发送，返回字节数,
//高位在前，低位在后
u16 DataReOrg(unsigned char *Str,unsigned char num)
{ 
  ByteNumLoc=num*9;
  EndAddr= add_start+MemCnt*9-1; 
 
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
  ByteNumLoc=num*9;
  EndAddr= add_start+MemCnt*9-1;;
  
  for(u16 i=0;i<ByteNumLoc;i++)
  {
     FLASH_EraseByte(EndAddr);
     EndAddr--;
  }
  
  MemCnt=MemCnt-num;
}


//存档数据，返回已存数量
void DataStore(void)
{
  u16  i;
  u8  Buffer[9];
  u8  val  = 0; 
  u32 add = 0x9808;

  //读取已存数据的数量，MemCnt表示已存数量
  val=FLASH_ReadByte(add);
  while((val==0x55)&&(MemCnt<200))
  {
    MemCnt++;
    add=add+9;
    val=FLASH_ReadByte(add);
  }
  
  //存储数据操作
  Buffer[8] =0x55;
  Buffer[7] =TimeStamp>>24;
  Buffer[6] =TimeStamp>>16;
  Buffer[5] =TimeStamp>>8;
  Buffer[4] =TimeStamp;
  Buffer[3] =temp>>8;
  Buffer[2] =temp;
  Buffer[1] =hum>>8;
  Buffer[0] =hum;
 
  add=add_start+MemCnt*9;
  for(i=0;i<9;i++)
  {
    FLASH_ProgramByte((add + i), Buffer[i]);
  }  
  MemCnt++;
}

/*******************************************************************************
****入口参数：无
****出口参数：无
****函数备注：主函数，PD0接LED
****版权信息：蓝旗嵌入式系统
*******************************************************************************/
void main(void)
{  
    CLK_MasterPrescalerConfig(CLK_MasterPrescaler_HSIDiv1);
 
    GPIO_Init(LED_GPIO_PORT, LED_GPIO_PINS, GPIO_Mode_Out_PP_Low_Slow);//初始化LED端口，上拉输出低电平、低速
    GPIO_SetBits(LED_GPIO_PORT,LED_GPIO_PINS); //拉高
    
    GPIO_Init(KEY_GPIO_PORT, KEY_GPIO_PINS, GPIO_Mode_In_FL_No_IT);//初始化KEY端口
    
    GPIO_Init(LORA_WAKE_PORT, LORA_WAKE_PINS, GPIO_Mode_Out_OD_Low_Slow);//初始化LORA唤醒端口，开漏输出、低速
    GPIO_SetBits(LORA_WAKE_PORT,LORA_WAKE_PINS); //拉高lora唤醒引脚

    GPIO_Init(LORA_HOSTWAKE_PORT, LORA_HOSTWAKE_PINS, GPIO_Mode_In_FL_IT);//初始化LORA唤醒mcu端口
    GPIO_Init(PCF8563_PORT, PCF8563_INT_PINS, GPIO_Mode_In_FL_IT);//
    EXTI_DeInit (); //恢复中断的所有设置 
    EXTI_SetPinSensitivity (EXTI_Pin_2,EXTI_Trigger_Rising);//外部中断2，上升沿触发，向量号10
    EXTI_SetPinSensitivity (EXTI_Pin_3,EXTI_Trigger_Falling);
        
    CLK_PeripheralClockConfig (CLK_Peripheral_USART,ENABLE); //使能串口时钟
        //初始化USART的2个脚--TXD-RXD
    GPIO_Init(GPIOC,GPIO_Pin_3,GPIO_Mode_Out_PP_High_Fast);//TXD
    GPIO_Init(GPIOC,GPIO_Pin_2,GPIO_Mode_In_PU_No_IT);//RXD
    USART_Init(9600,USART_WordLength_8D,USART_StopBits_1,USART_Parity_No,(USART_Mode_TypeDef)(USART_Mode_Tx|USART_Mode_Rx));//USART初始化，9600，8N1
    USART_ITConfig (USART_IT_RXNE,ENABLE);//使能接收中断，中断向量号28
    USART_Cmd (ENABLE);//使能USART  
   
    enableInterrupts();
   
    InitIIC();
    
    delay_ms(100);
               
    FLASH_SetProgrammingTime(FLASH_ProgramTime_Standard);//标准编程时间
    FLASH_Unlock(FLASH_MemType_Program);  //解除保护
    
    while(1)
    {
            
        while(GPIO_ReadInputDataBit(KEY_GPIO_PORT,KEY_GPIO_PINS)!=0)//读GP输入状态
        {
           TimeSVN();   //对时模式
        }

        ts=AM2320_get_value(&hum,&temp);       
        PCF8563_GetDate(PCF_Format_BIN, &Century, &PCF8563_Date);                
        PCF8563_GetTime(PCF_Format_BIN,&PCF8563_Time);

        delay_ms(500);
    
        GPIO_ResetBits(LED_GPIO_PORT,LED_GPIO_PINS); //拉di

        ts=AM2320_get_value(&hum,&temp);        
        PCF8563_GetDate(PCF_Format_BIN, &Century, &PCF8563_Date);                
        PCF8563_GetTime(PCF_Format_BIN,&PCF8563_Time);    
    
        stm.tm_year=PCF8563_Date.RTC_Years+100;
        stm.tm_mon =PCF8563_Date.RTC_Months-1;
        stm.tm_mday=PCF8563_Date.RTC_Days;
        stm.tm_hour=PCF8563_Time.RTC_Hours;
        stm.tm_min =PCF8563_Time.RTC_Minutes;
        stm.tm_sec =PCF8563_Time.RTC_Seconds;
        TimeStamp=mktime(&stm);


        PCF8563_SetTimer(&PCF8563_Timer);         
        //DataStore();

        
//        if(LoraWakeFlag==1)
//        {
//          LoraWakeFlag=0;
//          USART_SendNbyte("lora wake",10) ;
//        }
     
//        
//        ByteNumGlo = DataReOrg(Data,MemCnt);
//        
//        USART_SendNbyte(Data,ByteNumGlo) ;
//        
//        DataErase(MemCnt);
//                           
        GPIO_SetBits(LED_GPIO_PORT,LED_GPIO_PINS); //拉高
                
//        halt();//挂起，最低功耗

        delay_ms(1000);
    }
 
}
  
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/