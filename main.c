
/* Includes ------------------------------------------------------------------*/
#include "stm8l10x.h"
#include "AM2320.h"
#include "pcf8563.h"
#include "stdio.h"        //包含C库函数
#include <time.h>
#include <string.h>

//定义LED接口
#define LED_GPIO_PORT  GPIOD
#define LED_GPIO_PINS  GPIO_Pin_0

//定义lora唤醒口
#define LORA_WAKE_PORT  GPIOC
#define LORA_WAKE_PINS  GPIO_Pin_4

//定义被lora唤醒口
#define LORA_HOSTWAKE_PORT  GPIOA
#define LORA_HOSTWAKE_PINS  GPIO_Pin_2

uint16_t  hum;
short    temp;
uint8_t   ts;

u32  TimeStamp;
struct tm stm;

extern _PCF8563_Time_Typedef PCF8563_Time;
extern _PCF8563_Date_Typedef PCF8563_Date;
unsigned char Century;

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

void DataOrganize(void)
{
  u16  i;
  u8  Buffer[9];
  u8  val  = 0; 
  u8  MemCnt=0;
  u32 add_start = 0x9800;
  u32 add = 0x9800;

  //读取已存数据的数量，MemCnt表示已存数量
  val=FLASH_ReadByte(add);
  while((val==0x55)&&(MemCnt<220))
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

  //判断是否发送数据，每存储60组发送一次
   if(MemCnt%60==0)
  {
//    FLASH_ProgramByte(0x9fff, 0x55);
    for(i=0;i<61*9+10;i++)
    {
      FLASH_EraseByte(add_start+i);
    }
         
  }
    
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
    
    GPIO_Init(LORA_WAKE_PORT, LORA_WAKE_PINS, GPIO_Mode_Out_OD_Low_Slow);//初始化LORA唤醒端口，开漏输出、低速
    GPIO_SetBits(LORA_WAKE_PORT,LORA_WAKE_PINS); //拉高lora唤醒引脚

    GPIO_Init(LORA_HOSTWAKE_PORT, LORA_HOSTWAKE_PINS, GPIO_Mode_In_FL_IT);//初始化LORA唤醒mcu端口，开漏输出、低速
    EXTI_DeInit (); //恢复中断的所有设置 
    EXTI_SetPinSensitivity (EXTI_Pin_2,EXTI_Trigger_Falling);//外部中断2，上升沿触发，向量号10
    
    CLK_PeripheralClockConfig (CLK_Peripheral_USART,ENABLE); //使能串口时钟
        //初始化USART的2个脚--TXD-RXD
    GPIO_Init(GPIOC,GPIO_Pin_3,GPIO_Mode_Out_PP_High_Fast);//TXD
    GPIO_Init(GPIOC,GPIO_Pin_2,GPIO_Mode_In_PU_No_IT);//RXD
    USART_Init(9600,USART_WordLength_8D,USART_StopBits_1,USART_Parity_No,(USART_Mode_TypeDef)(USART_Mode_Tx|USART_Mode_Rx));//USART初始化，9600，8N1
    USART_ITConfig (USART_IT_RXNE,ENABLE);//使能接收中断，中断向量号28
    USART_Cmd (ENABLE);//使能USART
    
    enableInterrupts();
   
    InitIIC();
//    PCF8563_Set_Times(PCF_Format_BIN,PCF_Century_20xx,20, 1, 15, 3,17,15);
    
    FLASH_SetProgrammingTime(FLASH_ProgramTime_Standard);//标准编程时间
    FLASH_Unlock(FLASH_MemType_Program);  //解除保护

    while(1)
    {
      
        ts=AM2320_get_value(&hum,&temp);
        
        PCF8563_GetDate(PCF_Format_BIN, &Century, &PCF8563_Date);
                
        PCF8563_GetTime(PCF_Format_BIN,&PCF8563_Time);

        delay_ms(1000);
    
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
 
        DataOrganize();
        
        delay_ms(10);
        
        GPIO_SetBits(LED_GPIO_PORT,LED_GPIO_PINS); //拉高
        
        //halt();//挂起，最低功耗

        delay_ms(4000);
    }
 
}
  
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/