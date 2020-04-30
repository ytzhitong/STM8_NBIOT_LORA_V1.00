/**
  *****************************************************************************
  *                            ʱ��оƬPCF8563����
  *
  *                       (C) Copyright 2000-2020, ***
  *                            All Rights Reserved
  *****************************************************************************
  *
  * @File    : pcf8563.c
  * @By      : Sam Chan
  * @Version : V1.0
  * @Date    : 2012 / 08 / 28
  *
  *****************************************************************************
  *                                   Update
  * @Version : V1.0.1
  * @By      : Sam Chan
  * @Date    : 2013 / 10 / 20
  * @Brief   : A��������ʾʱ�����ڸ�ʽ����
  *            B�����Ӷ�ȡʱ�䴦��������ȡ����ʱ��������Ϣֱ��ת����ASCII���浽ʱ���ʽ������
  *            C������ʱ�����ڴ���������ʾ���ߴ�ӡ�����ڵĻ�ֱ����ʾ���ߴ�ӡʱ���ʽ���鼴��
  *
  * @Version : V1.0.2
  * @By      : Sam Chan
  * @Date    : 2014 / 02 / 26
  * @Brief   : ������ṹΪ16λ��ֵ����ֵλ������20xx��19xx
  *
  * @Version : V1.0.3
  * @By      : Sam Chan
  * @Date    : 2014 / 03 / 09
  * @Brief   : ����PCF8563�Ƿ���ڼ�⺯��
  *
  * @Version : V1.0.4
  * @By      : Sam Chan
  * @Date    : 2014 / 05 / 10
  * @Brief   : A�����ӵ���Ĭ�ϲ���������������ֲ
  *            B�����Ӷ�C++������֧��
  *
  * @Version : V1.0.5
  * @By      : Sam Chan
  * @Date    : 2014 / 07 / 19
  * @Brief   : A��������ʾʱ��ʱbug����ʾ�ַ������������������ʾ�˲�����ʾ���ַ�
  *            B������ֱ������ʱ�亯����������������USMART�����Ĺ���ֱ�ӵ���
  *
  * @Version : V2.0
  * @By      : Sam
  * @Date    : 2015 / 05 / 15
  * @Brief   : ȫ���޸Ĵ��룬���Ӵ�BIN��BCDת������
  *
  *****************************************************************************
**/


#include "pcf8563.h"

_PCF8563_Time_Typedef PCF8563_Time;
_PCF8563_Date_Typedef PCF8563_Date;

/******************************************************************************
                            ������صı�������
******************************************************************************/

unsigned char buffer[4];

#define IICSPEED 100000       //100K�ٶȣ�ʱ���ٶ�����1M��400K�ٶȣ�ʱ���ٶ�����4M
#define PCF8563_ADDRESS    0xA2

/*******************************************************************************
****��������:IIC��ʼ������
****��������:
****�汾:V1.0
****����:14-2-2014
****��ڲ���:��
****���ڲ���:��
****˵��:
********************************************************************************/
void InitIIC(void)
{
  CLK_PeripheralClockConfig(CLK_Peripheral_I2C,ENABLE);
  GPIO_Init(GPIOC, GPIO_Pin_1 | GPIO_Pin_0, GPIO_Mode_Out_OD_HiZ_Fast);
  I2C_DeInit();
  I2C_Init(IICSPEED, PCF8563_ADDRESS, I2C_DutyCycle_2, I2C_Ack_Enable, I2C_AcknowledgedAddress_7bit);  
  I2C_Cmd(ENABLE);
}


/**
  *****************************************************************************
  * @Name   : ��BINת��ΪBCD
  *
  * @Brief  : none
  *
  * @Input  : BINValue: ����BIN
  *
  * @Output : none
  *
  * @Return : BCD��ʽ��ֵ
  *****************************************************************************
**/
static unsigned char RTC_BinToBcd2(unsigned char BINValue)
{
	unsigned char bcdhigh = 0;
	
	while (BINValue >= 10)
	{
		bcdhigh++;
		BINValue -= 10;
	}
	
	return ((unsigned char)(bcdhigh << 4) | BINValue);
}

/**
  *****************************************************************************
  * @Name   : ��BCDת��ΪBIN
  *
  * @Brief  : none
  *
  * @Input  : BCDValue: ����BCD
  *
  * @Output : none
  *
  * @Return : BIN��ʽ��ֵ
  *****************************************************************************
**/
static unsigned char RTC_Bcd2ToBin(unsigned char BCDValue)
{
	unsigned char tmp = 0;
	
	tmp = ((unsigned char)(BCDValue & (unsigned char)0xF0) >> (unsigned char)0x04) * 10;
	return (tmp + (BCDValue & (unsigned char)0x0F));
}

/**
  *****************************************************************************
  * @Name   : PCF8563ĳ�Ĵ���д��һ���ֽ�����
  *
  * @Brief  : none
  *
  * @Input  : REG_ADD��Ҫ�����Ĵ�����ַ
  *           dat��    Ҫд�������
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
void PCF8563_Write_Byte(unsigned char REG_ADD, unsigned char dat)
{    
    //I2C_AcknowledgeConfig(ENABLE);
          /* �ȴ����� */
    while(I2C_GetFlagStatus(I2C_FLAG_BUSY));
    
    /* ����ʼλ */
    I2C_GenerateSTART(ENABLE);
    
    /* ����EV5 ��������������һ��Ӧ���ź�*/
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT));  
    
    /* ����I2C��������ַ��I2C���豸Ϊдģʽ*/
    I2C_Send7bitAddress(PCF8563_ADDRESS, I2C_Direction_Transmitter);
  
    /* ����EV6 ��������������һ��Ӧ���ź�*/
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
     
    I2C_SendData((u8)(REG_ADD)); 
    /* ����EV8 ��������������һ��Ӧ���ź�*/
     while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTING)); 
   
    /* �����豸д����*/
      I2C_SendData(dat);
      
      /* ����EV6 ��������������һ��Ӧ���ź�*/
     while(!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED));
      I2C_AcknowledgeConfig(ENABLE);
   
    /* ������λ*/
    I2C_GenerateSTOP(ENABLE); 
} 

/**
  *****************************************************************************
  * @Name   : PCF8563ĳ�Ĵ�����ȡһ���ֽ�����
  *
  * @Brief  : none
  *
  * @Input  : REG_ADD��Ҫ�����Ĵ�����ַ
  *
  * @Output : none
  *
  * @Return : ��ȡ�õ��ļĴ�����ֵ
  *****************************************************************************
**/
unsigned char PCF8563_Read_Byte(unsigned char REG_ADD)
{

   unsigned char tmp;
   I2C_AcknowledgeConfig(ENABLE);
   
   while(I2C_GetFlagStatus(I2C_FLAG_BUSY));
   I2C_GenerateSTART(ENABLE);
    /* ����EV5 ��������������һ��Ӧ���ź�*/
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT)); 
    
    /* ����I2C��������ַ��I2C���豸Ϊдģʽ*/
    I2C_Send7bitAddress(PCF8563_ADDRESS, I2C_Direction_Transmitter);
    /* ����EV6 ��������������һ��Ӧ���ź�*/
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
    
    I2C_SendData(REG_ADD); 
    /* ����EV8 ��������������һ��Ӧ���ź�*/
    while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTING)); 
    
    /* ����ʼλ */ 
    I2C_GenerateSTART(ENABLE);
    /* ����EV5 ��������������һ��Ӧ���ź�*/
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT)); 
  
    /* ����I2C��������ַ��I2C���豸Ϊ��ģʽ*/
    I2C_Send7bitAddress(PCF8563_ADDRESS, I2C_Direction_Receiver);
    /* ����EV6 ��������������һ��Ӧ���ź�*/
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
    
    if(I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_RECEIVED))
    { 
        
         tmp = I2C_ReceiveData(); 
       
         I2C_AcknowledgeConfig(DISABLE);
         I2C_GenerateSTOP(ENABLE); 
         
         return tmp;
    }
    
    return 0;
}

/**
  *****************************************************************************
  * @Name   : PCF8563д���������
  *
  * @Brief  : none
  *
  * @Input  : REG_ADD��Ҫ�����Ĵ�����ʼ��ַ
  *           num��    д����������
  *           *WBuff�� д�����ݻ���
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
void PCF8563_Write_nByte(unsigned char REG_ADD, unsigned char num, unsigned char *pBuff)
{  
   I2C_AcknowledgeConfig(ENABLE);
          /* �ȴ����� */
    while(I2C_GetFlagStatus(I2C_FLAG_BUSY));
    
    /* ����ʼλ */
    I2C_GenerateSTART(ENABLE);
    
    /* ����EV5 ��������������һ��Ӧ���ź�*/
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT));  
    
    /* ����I2C��������ַ��I2C���豸Ϊдģʽ*/
    I2C_Send7bitAddress(PCF8563_ADDRESS, I2C_Direction_Transmitter);
  
    /* ����EV6 ��������������һ��Ӧ���ź�*/
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
      
    I2C_SendData((u8)(REG_ADD)); 
    /* ����EV8 ��������������һ��Ӧ���ź�*/
     while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTING)); 
    
    /* ���������豸д����*/
    while(num--)  
    {
      
      I2C_SendData(*pBuff); 
      /* ����EV6 ��������������һ��Ӧ���ź�*/
      while(!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED));
     
      I2C_AcknowledgeConfig(ENABLE);
  
      /*ָ��ָ����һ����ַ*/
      pBuff++;    
      
    }
  
    /* ������λ*/
    I2C_GenerateSTOP(ENABLE); 
}

/**
  *****************************************************************************
  * @Name   : PCF8563��ȡ��������
  *
  * @Brief  : none
  *
  * @Input  : REG_ADD��Ҫ�����Ĵ�����ʼ��ַ
  *           num��    ��ȡ��������
  *
  * @Output : *WBuff�� ��ȡ���ݻ���
  *
  * @Return : none
  *****************************************************************************
**/
void PCF8563_Read_nByte(unsigned char REG_ADD, unsigned char num, unsigned char *pBuff)
{
   I2C_AcknowledgeConfig(ENABLE);
           /* �ȴ����� */
   while(I2C_GetFlagStatus(I2C_FLAG_BUSY));
   
    /* ����ʼλ */
    I2C_GenerateSTART(ENABLE);
    /* ����EV5 ��������������һ��Ӧ���ź�*/
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT)); 
  
    /* ����I2C��������ַ��I2C���豸Ϊдģʽ*/
    I2C_Send7bitAddress(PCF8563_ADDRESS, I2C_Direction_Transmitter);
    /* ����EV6 ��������������һ��Ӧ���ź�*/
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
   
    I2C_SendData((u8)(REG_ADD)); 
    /* ����EV8 ��������������һ��Ӧ���ź�*/
    while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  
    /* ����ʼλ */ 
    I2C_GenerateSTART(ENABLE);
    /* ����EV5 ��������������һ��Ӧ���ź�*/
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT)); 
  
    /* ����I2C��������ַ��I2C���豸Ϊ��ģʽ*/
    I2C_Send7bitAddress(PCF8563_ADDRESS, I2C_Direction_Receiver);
    /* ����EV6 ��������������һ��Ӧ���ź�*/
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
    /*�����ڴӴ��豸��ȡ���� */
    while(num)  
    {
        /* ����EV6 */
      if(I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_RECEIVED))
      {      
        /*�� EEPROM ��ȡһ���ֽ�*/
        *pBuff = I2C_ReceiveData();
        /* ָ��ָ���¸�����ֽڵĵ�ַ*/
        pBuff++;  
        /* �������һ���ֽ�*/
        if(num == 1)
        {
        /* ����ҪӦ��*/
          I2C_AcknowledgeConfig(DISABLE);
        /* ������λ*/
          I2C_GenerateSTOP(ENABLE); 
                   
        }
        else 
        {
          /* �������һ���ֽ�����豸����Ӧ���ź�*/
          I2C_AcknowledgeConfig(ENABLE);
        }
          
        num--;

      }   
    }
}

/**
  *****************************************************************************
  * @Name   : PCF8563����Ƿ����
  *
  * @Brief  : ��ʱ������ʱ�Ĵ���д��һ����ֵ�ٶ�ȡ�������Աȣ���ͬ��ȷ����ͬ�����
  *
  * @Input  : none
  *
  * @Output : none
  *
  * @Return : 0: ����
  *           1: PCF8563���������
  *****************************************************************************
**/
unsigned char PCF8563_Check(void)
{
	unsigned char test_value = 0;
	unsigned char Time_Count = 0;  //��ʱ������ʱ���ݻ���

	if(PCF8563_Read_Byte(PCF8563_Address_Timer) & 0x80)  //������˶�ʱ�������ȹر�
	{
		PCF8563_Write_Byte(PCF8563_Address_Timer, PCF_Timer_Close);  //�ȹرն�ʱ��
		Time_Count = PCF8563_Read_Byte(PCF8563_Address_Timer_VAL);  //�ȱ������ֵ
	}

	PCF8563_Write_Byte(PCF8563_Address_Timer_VAL, PCF8563_Check_Data);  //д����ֵ
	for(test_value = 0;test_value < 50;test_value++)  {}  //��ʱһ��ʱ���ٶ�ȡ
	test_value = PCF8563_Read_Byte(PCF8563_Address_Timer_VAL);  //�ٶ�ȡ����

	if(Time_Count != 0)  //�����˶�ʱ�����ܣ���ָ�
	{
		PCF8563_Write_Byte(PCF8563_Address_Timer_VAL, Time_Count);  //�ָ��ֳ�
		PCF8563_Write_Byte(PCF8563_Address_Timer, PCF_Timer_Open);  //������ʱ��
	}

	if(test_value != PCF8563_Check_Data)  return 1;  //�������������
	
	return 0;  //����
}

/**
  *****************************************************************************
  * @Name   : PCF8563����
  *
  * @Brief  : none
  *
  * @Input  : none
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
void PCF8563_Start(void)
{
	unsigned char temp = 0;
	
	temp = PCF8563_Read_Byte(PCF8563_Address_Control_Status_1);  //��ȡ����/״̬�Ĵ���1
	if (temp & PCF_Control_ChipStop)
	{
		temp &= PCF_Control_ChipRuns;  //����оƬ
	}
	if ((temp & (1<<7)) == 0)  //��ͨģʽ
	{
		temp &= PCF_Control_TestcClose;  //��Դ��λģʽʧЧ
	}
	PCF8563_Write_Byte(PCF8563_Address_Control_Status_1, temp);  //��д����ֵ
}

/**
  *****************************************************************************
  * @Name   : PCF8563ֹͣ
  *
  * @Brief  : ʱ��Ƶ�����CLKOUT �� 32.768kHz ʱ����
  *
  * @Input  : none
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
void PCF8563_Stop(void)
{
	unsigned char temp = 0;
	
	temp = PCF8563_Read_Byte(PCF8563_Address_Control_Status_1);  //��ȡ����/״̬�Ĵ���1
	temp |= PCF_Control_ChipStop;  //ֹͣ����
	PCF8563_Write_Byte(PCF8563_Address_Control_Status_1, temp);  //��д����ֵ
}

/**
  *****************************************************************************
  * @Name   : PCF8563��������ģʽ
  *
  * @Brief  : none
  *
  * @Input  : Mode: ����ģʽ
  *                 = PCF_Mode_Normal
  *                 = PCF_Mode_EXT_CLK
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
void PCF8563_SetMode(unsigned char Mode)
{
	unsigned char temp = 0;
	
	temp = PCF8563_Read_Byte(PCF8563_Address_Control_Status_1);  //��ȡ�Ĵ���ֵ
	if (Mode == PCF_Mode_EXT_CLK)  //EXT_CLK����ģʽ
	{
		temp |= PCF_Control_Status_EXT_CLKMode;
	}
	else if (Mode == PCF_Mode_Normal)
	{
		temp &= PCF_Control_Status_NormalMode;
		temp &= ~(1<<3);  //��Դ��λ����ʧЧ
	}
	PCF8563_Write_Byte(PCF8563_Address_Control_Status_1, temp);
}

/**
  *****************************************************************************
  * @Name   : PCF8563���õ�Դ��λ���ܿ�����ر�
  *
  * @Brief  : none
  *
  * @Input  : NewState: ״̬��PCF8563_PowerResetEnablePCF8563_PowerResetDisable
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
void PCF8563_SetPowerReset(unsigned char NewState)
{
	unsigned char TestC = 0;
	
	TestC = PCF8563_Read_Byte(PCF8563_Address_Control_Status_1);  //��ȡ�Ĵ���ֵ
	TestC &= ~(1<<3);  //���֮ǰ����
	if (NewState == PCF8563_PowerResetEnable)  //��λ������Ч
	{
		TestC |= PCF8563_PowerResetEnable;
	}
	else if (NewState == PCF8563_PowerResetDisable)
	{
		TestC &= ~PCF8563_PowerResetEnable;  //ʧЧ����ͨģʽ��ֵ�߼�0����ʧЧ
	}
	PCF8563_Write_Byte(PCF8563_Address_Control_Status_1, TestC);  //д����ֵ
}

/**
  *****************************************************************************
  * @Name   : PCF8563�������Ƶ��
  *
  * @Brief  : none
  *
  * @Input  :*PCF_CLKOUTStruct: Ƶ�ʽṹָ��
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
void PCF8563_SetCLKOUT(_PCF8563_CLKOUT_Typedef* PCF_CLKOUTStruct)
{
	unsigned char tmp = 0;
	
	tmp = PCF8563_Read_Byte(PCF8563_Address_CLKOUT);  //��ȡ�Ĵ���ֵ
	tmp &= 0x7c;  //���֮ǰ����
	if (PCF_CLKOUTStruct->CLKOUT_NewState == PCF_CLKOUT_Open)
	{
		tmp |= PCF_CLKOUT_Open;
	}
	else
	{
		tmp &= PCF_CLKOUT_Close;
	}
	tmp |= PCF_CLKOUTStruct->CLKOUT_Frequency;
	
	PCF8563_Write_Byte(PCF8563_Address_CLKOUT, tmp);
}

/**
  *****************************************************************************
  * @Name   : PCF8563���ö�ʱ��
  *
  * @Brief  : none
  *
  * @Input  :*PCF_TimerStruct: ��ʱ���ṹָ��
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
void PCF8563_SetTimer(_PCF8563_Timer_Typedef* PCF_TimerStruct)
{
	unsigned char Timer_Ctrl = 0;
	unsigned char Timer_Value = 0;
	
	Timer_Ctrl = PCF8563_Read_Byte(PCF8563_Address_Timer);  //��Ŀ��ƼĴ���ֵ
	Timer_Value = PCF8563_Read_Byte(PCF8563_Address_Timer_VAL);  //��ȡ����ʱ��ֵ
//	PCF8563_Read_nByte(PCF8563_Address_Timer, 1, &Timer_Ctrl);
//      PCF8563_Read_nByte(PCF8563_Address_Timer_VAL, 1, &Timer_Value);
        
        //
	//��ֹͣ��ʱ��
	//
	Timer_Ctrl &= PCF_Timer_Close;
	//PCF8563_Write_Byte(PCF8563_Address_Timer, Timer_Ctrl);
        PCF8563_Write_nByte(PCF8563_Address_Timer,1, &Timer_Ctrl);

	
	Timer_Ctrl &= 0x7c;  //�����ʱ��֮ǰ����
	
	if (PCF_TimerStruct->RTC_Timer_NewState == PCF_Timer_Open)  //����
	{
		Timer_Ctrl |= PCF_Timer_Open;
		Timer_Ctrl |= PCF_TimerStruct->RTC_Timer_Frequency;  //�����µĹ���Ƶ��
		if (PCF_TimerStruct->RTC_Timer_Value)  //��Ҫ�����µļ���ֵ
		{
			Timer_Value = PCF_TimerStruct->RTC_Timer_Value;  //�����µļ���ֵ
		}
	}
	else
	{
		Timer_Ctrl &= PCF_Timer_Close;
	}
	//PCF8563_Write_Byte(PCF8563_Address_Timer_VAL, Timer_Value);  //д�뵹��ʱ��ֵ
        PCF8563_Write_nByte(PCF8563_Address_Timer_VAL,1, &Timer_Value);
	
	if (PCF_TimerStruct->RTC_Timer_Interrupt == PCF_Time_INT_Open)  //�������ж����
	{
		Timer_Value = PCF8563_Read_Byte(PCF8563_Address_Control_Status_2);  //��ȡ����/״̬�Ĵ���2��ֵ
		Timer_Value &= PCF_Time_INT_Close;  //�����ʱ���ж�ʹ��
		Timer_Value &= ~(1<<2);  //�����ʱ���жϱ�־
		Timer_Value &= ~(1<<4);  //�� TF ��Чʱ INT ��Ч (ȡ���� TIE ��״̬) 
		Timer_Value |= PCF_Time_INT_Open;  //������ʱ���ж����
		//PCF8563_Write_Byte(PCF8563_Address_Control_Status_2, Timer_Value);
                PCF8563_Write_nByte(PCF8563_Address_Control_Status_2,1, &Timer_Value);
	}
	else
	{
		Timer_Value = PCF8563_Read_Byte(PCF8563_Address_Control_Status_2);  //��ȡ����/״̬�Ĵ���2��ֵ
		Timer_Value &= PCF_Time_INT_Close;  //�����ʱ���ж�ʹ��
		Timer_Value |= PCF_Time_INT_Open;  //������ʱ���ж����
		//PCF8563_Write_Byte(PCF8563_Address_Control_Status_2, Timer_Value);
                PCF8563_Write_nByte(PCF8563_Address_Control_Status_2,1, &Timer_Value);
	}
	
	//PCF8563_Write_Byte(PCF8563_Address_Timer, Timer_Ctrl);  //���ö�ʱ�����ƼĴ���
        PCF8563_Write_nByte(PCF8563_Address_Timer,1, &Timer_Ctrl);
}

/**
  *****************************************************************************
  * @Name   : ����ʱ�䣬��Ҫ���ں�̨���ã����߳�ʼ��ʱ����
  *
  * @Brief  : ��Ĭ�Ͼ����ó�0x00�ˣ��β����治���֣�����ֵ��Χ��0 ~ 6
  *
  * @Input  : PCF_Format:  ���ݸ�ʽ
  *                        = PCF_Format_BIN
  *                        = PCF_Format_BCD
  *           PCF_Century: ����λ�趨
  *                        = PCF_Century_19xx
  *                        = PCF_Century_20xx
  *           Year:        ��
  *           Month:       ��
  *           Date:        ��
  *           Week:        ����
  *           Hour:        ʱ
  *           Minute:      ��
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
void PCF8563_Set_Times(unsigned char PCF_Format,\
                       unsigned char PCF_Century,\
	                   unsigned char Year, unsigned char Month, unsigned char Date, unsigned char Week,\
                       unsigned char Hour, unsigned char Minute, unsigned char Second)
{
	_PCF8563_Time_Typedef Time_InitStructure;
	_PCF8563_Date_Typedef Date_InitStructure;
	
	if (PCF_Format == PCF_Format_BIN)
	{
		//
		//�ж������Ƿ���Ϸ�Χ
		//
		if (Year > 99)   Year  = 0;  //�ָ�00��
		if (Month > 12)  Month = 1;  //�ָ�1��
		if (Date > 31)   Date  = 1;  //�ָ�1��
		if (Week > 6)    Week  = 1;  //�ָ�����һ
		
		if (Hour > 23)    Hour   = 0;  //�ָ�0Сʱ
		if (Minute > 59)  Minute = 0;  //�ָ�0����
                if (Second > 59)  Second = 0;  //�ָ�0��
		
		//
		//ת��һ��
		//
		Date_InitStructure.RTC_Years    = RTC_BinToBcd2(Year);
		Date_InitStructure.RTC_Months   = RTC_BinToBcd2(Month);
		Date_InitStructure.RTC_Days     = RTC_BinToBcd2(Date);
		Date_InitStructure.RTC_WeekDays = RTC_BinToBcd2(Week);
		
		Time_InitStructure.RTC_Hours    = RTC_BinToBcd2(Hour);
		Time_InitStructure.RTC_Minutes  = RTC_BinToBcd2(Minute);
                Time_InitStructure.RTC_Seconds  = RTC_BinToBcd2(Second);
	}
//	Time_InitStructure.RTC_Seconds = 0x00;  //�ָ�0��
	Time_InitStructure.RTC_Seconds &= PCF_Accuracy_ClockYes;  //��֤׼ȷ��ʱ��
	//
	//�ж�����λ
	//
	if (PCF_Century == PCF_Century_19xx)
	{
		Date_InitStructure.RTC_Months |= PCF_Century_SetBitC;
	}
	else
	{
		Date_InitStructure.RTC_Months &= ~PCF_Century_SetBitC;
	}
	//
	//д����Ϣ���Ĵ���
	//
	buffer[0] = Time_InitStructure.RTC_Seconds;
	buffer[1] = Time_InitStructure.RTC_Minutes;
	buffer[2] = Time_InitStructure.RTC_Hours;
	PCF8563_Write_nByte(PCF8563_Address_Seconds, 3, buffer);  //д��ʱ����Ϣ
	
	buffer[0] = Date_InitStructure.RTC_Days;
	buffer[1] = Date_InitStructure.RTC_WeekDays;
	buffer[2] = Date_InitStructure.RTC_Months;
	buffer[3] = Date_InitStructure.RTC_Years;
	PCF8563_Write_nByte(PCF8563_Address_Days,    4, buffer);  //д��������Ϣ
}

/**
   ============================================================================
                     #### ���мĴ���ȫ���������ܺ��� ####
   ============================================================================
**/

/**
  *****************************************************************************
  * @Name   : �ж�ʱ����Ϣ�Ƿ���Ϸ�Χ���������ָ���ֵ
  *
  * @Brief  : ����ֵ��Χ��0 ~ 6
  *
  * @Input  : PCF_DataStruct: �Ĵ����ṹָ��
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
static void IS_PCF8563_Data(_PCF8563_Register_Typedef* PCF_DataStruct)
{
	if (PCF_DataStruct->Years > 99)           PCF_DataStruct->Years          = 0;  //�ָ�00��
	if (PCF_DataStruct->Months_Century > 12)  PCF_DataStruct->Months_Century = 1;  //�ָ�1��
	if (PCF_DataStruct->Days > 31)            PCF_DataStruct->Days           = 1;  //�ָ�1��
	if (PCF_DataStruct->WeekDays > 6)         PCF_DataStruct->WeekDays       = 1;  //�ָ�����һ
	
	if (PCF_DataStruct->Hours > 23)           PCF_DataStruct->Hours          = 0;  //�ָ�0Сʱ
	if (PCF_DataStruct->Minutes > 59)         PCF_DataStruct->Minutes        = 0;  //�ָ�0����
	if (PCF_DataStruct->Seconds > 59)         PCF_DataStruct->Seconds        = 0;  //�ָ�0��
}

/**
  *****************************************************************************
  * @Name   : PCF8563д��Ĵ���
  *
  * @Brief  : ������ֵ��Χ��: 0 ~ 6��ʮ���Ƹ�ʽ
  *
  * @Input  : PCF_Format:     ���ݸ�ʽ
  *                           = PCF_Format_BIN
  *                           = PCF_Format_BCD
  *           PCF_Century:    ����λ�趨
  *                           = PCF_Century_19xx
  *                           = PCF_Century_20xx
  *           PCF_DataStruct: �Ĵ����ṹָ��
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
void PCF8563_SetRegister(unsigned char PCF_Format, unsigned char PCF_Century, _PCF8563_Register_Typedef* PCF_DataStruct)
{
	if (PCF_Format == PCF_Format_BIN)  //ʮ���Ƹ�ʽ����Ҫת��һ��
	{
		//
		//�ж���ֵ�Ƿ��ڷ�Χ֮��
		//
		IS_PCF8563_Data(PCF_DataStruct);
		//
		//��Ҫת��һ��
		//
		PCF_DataStruct->Years          = RTC_BinToBcd2(PCF_DataStruct->Years);
		PCF_DataStruct->Months_Century = RTC_BinToBcd2(PCF_DataStruct->Months_Century);
		PCF_DataStruct->Days           = RTC_BinToBcd2(PCF_DataStruct->Days);
		PCF_DataStruct->WeekDays       = RTC_BinToBcd2(PCF_DataStruct->WeekDays);
		
		PCF_DataStruct->Hours          = RTC_BinToBcd2(PCF_DataStruct->Hours);
		PCF_DataStruct->Minutes        = RTC_BinToBcd2(PCF_DataStruct->Minutes);
		PCF_DataStruct->Seconds        = RTC_BinToBcd2(PCF_DataStruct->Seconds);
		
		PCF_DataStruct->Day_Alarm      = RTC_BinToBcd2(PCF_DataStruct->Day_Alarm);
		PCF_DataStruct->WeekDays_Alarm = RTC_BinToBcd2(PCF_DataStruct->WeekDays_Alarm);
		
		PCF_DataStruct->Hour_Alarm     = RTC_BinToBcd2(PCF_DataStruct->Hour_Alarm);
		PCF_DataStruct->Minute_Alarm   = RTC_BinToBcd2(PCF_DataStruct->Minute_Alarm);
	}
	//
	//�ж�����λ
	//
	if (PCF_Century == PCF_Century_19xx)
	{
		PCF_DataStruct->Months_Century |= PCF_Century_SetBitC;
	}
	else
	{
		PCF_DataStruct->Months_Century &= ~PCF_Century_SetBitC;
	}
	//
	//�ر��������壬�ж����
	//
	PCF_DataStruct->Timer_Control    &= ~(1<<7);
	PCF_DataStruct->CLKOUT_Frequency &= ~(1<<7);
	PCF_DataStruct->WeekDays_Alarm   &= ~(1<<7);
	PCF_DataStruct->Day_Alarm        &= ~(1<<7);
	PCF_DataStruct->Hour_Alarm       &= ~(1<<7);
	PCF_DataStruct->Minute_Alarm     &= ~(1<<7);
	PCF_DataStruct->Control_Status_2 &= ~(3<<0);
	//
	//д�����ݵ��Ĵ���
	//
	PCF8563_Write_nByte(PCF8563_Address_Control_Status_1, 16, (unsigned char*)PCF_DataStruct);
}

/**
  *****************************************************************************
  * @Name   : PCF8563��ȡ�Ĵ���
  *
  * @Brief  : ������ֵ��Χ��: 0 ~ 6��ʮ���Ƹ�ʽ
  *
  * @Input  : PCF_Format:     ���ݸ�ʽ
  *                           = PCF_Format_BIN
  *                           = PCF_Format_BCD
  *
  * @Output : *PCF_Century:   ����λ��0��21���ͣ�1:20����
  *           PCF_DataStruct: �Ĵ����ṹָ��
  *
  * @Return : none
  *****************************************************************************
**/
void PCF8563_GetRegister(unsigned char PCF_Format, unsigned char *PCF_Century, _PCF8563_Register_Typedef* PCF_DataStruct)
{
	//
	//��ȡȫ���Ĵ�����ֵ
	//
	PCF8563_Read_nByte(PCF8563_Address_Control_Status_1, 16, (unsigned char*)PCF_DataStruct);
	//
	//�ж�����λ��ֵ
	//
	if (PCF_DataStruct->Months_Century & PCF_Century_SetBitC)
	{
		*PCF_Century = 1;
	}
	else
	{
		*PCF_Century = 0;
	}
	//
	//������Чλ
	//
	PCF_DataStruct->Years          &= PCF8563_Shield_Years;
	PCF_DataStruct->Months_Century &= PCF8563_Shield_Months_Century;
	PCF_DataStruct->Days           &= PCF8563_Shield_Days;
	PCF_DataStruct->WeekDays       &= PCF8563_Shield_WeekDays;
	
	PCF_DataStruct->Hours          &= PCF8563_Shield_Hours;
	PCF_DataStruct->Minutes        &= PCF8563_Shield_Minutes;
	PCF_DataStruct->Seconds        &= PCF8563_Shield_Seconds;
	
	PCF_DataStruct->Minute_Alarm   &= PCF8563_Shield_Minute_Alarm;
	PCF_DataStruct->Hour_Alarm     &= PCF8563_Shield_Hour_Alarm;
	PCF_DataStruct->Day_Alarm      &= PCF8563_Shield_Day_Alarm;
	PCF_DataStruct->WeekDays_Alarm &= PCF8563_Shield_WeekDays_Alarm;
	
	//
	//�ж���Ҫ�����ݸ�ʽ
	//
	if (PCF_Format == PCF_Format_BIN)
	{
		PCF_DataStruct->Years           = RTC_Bcd2ToBin(PCF_DataStruct->Years);
		PCF_DataStruct->Months_Century  = RTC_Bcd2ToBin(PCF_DataStruct->Months_Century);
		PCF_DataStruct->Days            = RTC_Bcd2ToBin(PCF_DataStruct->Days);
		PCF_DataStruct->WeekDays        = RTC_Bcd2ToBin(PCF_DataStruct->WeekDays);
		
		PCF_DataStruct->Hours           = RTC_Bcd2ToBin(PCF_DataStruct->Hours);
		PCF_DataStruct->Minutes         = RTC_Bcd2ToBin(PCF_DataStruct->Minutes);
		PCF_DataStruct->Seconds         = RTC_Bcd2ToBin(PCF_DataStruct->Seconds);
		
		PCF_DataStruct->Day_Alarm       = RTC_Bcd2ToBin(PCF_DataStruct->Day_Alarm);
		PCF_DataStruct->WeekDays_Alarm  = RTC_Bcd2ToBin(PCF_DataStruct->WeekDays_Alarm);
		
		PCF_DataStruct->Hour_Alarm      = RTC_Bcd2ToBin(PCF_DataStruct->Hour_Alarm);
		PCF_DataStruct->Minute_Alarm    = RTC_Bcd2ToBin(PCF_DataStruct->Minute_Alarm);
	}
}

/**
   ============================================================================
                      #### ʱ����Ϣ�������ܺ��� ####
   ============================================================================
**/

/**
  *****************************************************************************
  * @Name   : PCF8563д��ʱ����Ϣ
  *
  * @Brief  : none
  *
  * @Input  : PCF_Format:     ���ݸ�ʽ
  *                           = PCF_Format_BIN
  *                           = PCF_Format_BCD
  *           PCF_DataStruct: ʱ��ṹָ��
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
void PCF8563_SetTime(unsigned char PCF_Format, _PCF8563_Time_Typedef* PCF_DataStruct)
{
	if (PCF_Format == PCF_Format_BIN)  //ʮ���Ƹ�ʽ����Ҫת��һ��
	{
		//
		//�ж���ֵ�Ƿ��ڷ�Χ֮��
		//
		if (PCF_DataStruct->RTC_Hours > 23)    PCF_DataStruct->RTC_Hours   = 0;  //�ָ�0Сʱ
		if (PCF_DataStruct->RTC_Minutes > 59)  PCF_DataStruct->RTC_Minutes = 0;  //�ָ�0����
		if (PCF_DataStruct->RTC_Seconds > 59)  PCF_DataStruct->RTC_Seconds = 0;  //�ָ�0��
		//
		//��Ҫת��һ��
		//
		PCF_DataStruct->RTC_Hours   = RTC_BinToBcd2(PCF_DataStruct->RTC_Hours);
		PCF_DataStruct->RTC_Minutes = RTC_BinToBcd2(PCF_DataStruct->RTC_Minutes);
		PCF_DataStruct->RTC_Seconds = RTC_BinToBcd2(PCF_DataStruct->RTC_Seconds);
	}
	//
	//��������
	//
	buffer[0] = PCF_DataStruct->RTC_Seconds;
	buffer[1] = PCF_DataStruct->RTC_Minutes;
	buffer[2] = PCF_DataStruct->RTC_Hours;
	//
	//д�����ݵ��Ĵ���
	//
	PCF8563_Write_nByte(PCF8563_Address_Seconds, 3, buffer);
}

/**
  *****************************************************************************
  * @Name   : PCF8563��ȡʱ����Ϣ
  *
  * @Brief  : none
  *
  * @Input  : PCF_Format:     ���ݸ�ʽ
  *                           = PCF_Format_BIN
  *                           = PCF_Format_BCD
  *
  * @Output : PCF_DataStruct: ʱ��ṹָ��
  *
  * @Return : none
  *****************************************************************************
**/
void PCF8563_GetTime(unsigned char PCF_Format, _PCF8563_Time_Typedef* PCF_DataStruct)
{
	//
	//��ȡ�Ĵ�����ֵ
	//
	PCF8563_Read_nByte(PCF8563_Address_Seconds, 3, buffer);
	//
	//������Чλ
	//
	buffer[0] &= PCF8563_Shield_Seconds;
	buffer[1] &= PCF8563_Shield_Minutes;
	buffer[2] &= PCF8563_Shield_Hours;
	
	//
	//�ж���Ҫ�����ݸ�ʽ
	//
	if (PCF_Format == PCF_Format_BIN)
	{
		PCF_DataStruct->RTC_Hours   = RTC_Bcd2ToBin(buffer[2]);
		PCF_DataStruct->RTC_Minutes = RTC_Bcd2ToBin(buffer[1]);
		PCF_DataStruct->RTC_Seconds = RTC_Bcd2ToBin(buffer[0]);
	}
	else if (PCF_Format == PCF_Format_BCD)
	{
		//
		//��������
		//
		PCF_DataStruct->RTC_Hours   = buffer[2];
		PCF_DataStruct->RTC_Minutes = buffer[1];
		PCF_DataStruct->RTC_Seconds = buffer[0];
	}
}

/**
   ============================================================================
                         #### ������Ϣ�������ܺ��� ####
   ============================================================================
**/

/**
  *****************************************************************************
  * @Name   : PCF8563д��������Ϣ
  *
  * @Brief  : ������ֵ��Χ��: 0 ~ 6��ʮ���Ƹ�ʽ
  *
  * @Input  : PCF_Format:     ���ݸ�ʽ
  *                           = PCF_Format_BIN
  *                           = PCF_Format_BCD
  *           PCF_Century:    ����λ�趨
  *                           = PCF_Century_19xx
  *                           = PCF_Century_20xx
  *           PCF_DataStruct: ���ڽṹָ��
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
void PCF8563_SetDate(unsigned char PCF_Format, unsigned char PCF_Century, _PCF8563_Date_Typedef* PCF_DataStruct)
{
	if (PCF_Format == PCF_Format_BIN)  //ʮ���Ƹ�ʽ����Ҫת��һ��
	{
		//
		//�ж���ֵ�Ƿ��ڷ�Χ֮��
		//
		if (PCF_DataStruct->RTC_Years > 99)    PCF_DataStruct->RTC_Years    = 0;  //�ָ�00��
		if (PCF_DataStruct->RTC_Months > 12)   PCF_DataStruct->RTC_Months   = 1;  //�ָ�1��
		if (PCF_DataStruct->RTC_Days > 31)     PCF_DataStruct->RTC_Days     = 1;  //�ָ�1��
		if (PCF_DataStruct->RTC_WeekDays > 6)  PCF_DataStruct->RTC_WeekDays = 1;  //�ָ�����һ
		//
		//��Ҫת��һ��
		//
		PCF_DataStruct->RTC_Years    = RTC_BinToBcd2(PCF_DataStruct->RTC_Years);
		PCF_DataStruct->RTC_Months   = RTC_BinToBcd2(PCF_DataStruct->RTC_Months);
		PCF_DataStruct->RTC_Days     = RTC_BinToBcd2(PCF_DataStruct->RTC_Days);
		PCF_DataStruct->RTC_WeekDays = RTC_BinToBcd2(PCF_DataStruct->RTC_WeekDays);
	}
	//
	//�ж�����λ
	//
	if (PCF_Century == PCF_Century_19xx)
	{
		PCF_DataStruct->RTC_Months |= PCF_Century_SetBitC;
	}
	else
	{
		PCF_DataStruct->RTC_Months &= ~PCF_Century_SetBitC;
	}
	//
	//���ݿ���
	//
	buffer[0] = PCF_DataStruct->RTC_Days;
	buffer[1] = PCF_DataStruct->RTC_WeekDays;
	buffer[2] = PCF_DataStruct->RTC_Months;
	buffer[3] = PCF_DataStruct->RTC_Years;
	//
	//д�����ݵ��Ĵ���
	//
	PCF8563_Write_nByte(PCF8563_Address_Days, 4, buffer);
}

/**
  *****************************************************************************
  * @Name   : PCF8563��ȡ������Ϣ
  *
  * @Brief  : ������ֵ��Χ��: 0 ~ 6��ʮ���Ƹ�ʽ
  *
  * @Input  : PCF_Format:     ���ݸ�ʽ
  *                           = PCF_Format_BIN
  *                           = PCF_Format_BCD
  *
  * @Output : *PCF_Century:   ����λ��0��21���ͣ�1:20����
  *           PCF_DataStruct: ���ڽṹָ��
  *
  * @Return : none
  *****************************************************************************
**/
void PCF8563_GetDate(unsigned char PCF_Format, unsigned char *PCF_Century, _PCF8563_Date_Typedef* PCF_DataStruct)
{
	//
	//��ȡȫ���Ĵ�����ֵ
	//
	PCF8563_Read_nByte(PCF8563_Address_Days, 4, buffer);
	//
	//�ж�����λ��ֵ
	//
	if (buffer[2] & PCF_Century_SetBitC)
	{
		*PCF_Century = 1;
	}
	else
	{
		*PCF_Century = 0;
	}
	//
	//������Чλ
	//
	buffer[0] &= PCF8563_Shield_Days;
	buffer[1] &= PCF8563_Shield_WeekDays;
	buffer[2] &= PCF8563_Shield_Months_Century;
	buffer[3] &= PCF8563_Shield_Years;
	
	//
	//�ж���Ҫ�����ݸ�ʽ
	//
	if (PCF_Format == PCF_Format_BIN)
	{
		PCF_DataStruct->RTC_Years    = RTC_Bcd2ToBin(buffer[3]);
		PCF_DataStruct->RTC_Months   = RTC_Bcd2ToBin(buffer[2]);
		PCF_DataStruct->RTC_Days     = RTC_Bcd2ToBin(buffer[0]);
		PCF_DataStruct->RTC_WeekDays = RTC_Bcd2ToBin(buffer[1]);
	}
	else if (PCF_Format == PCF_Format_BCD)
	{
		//
		//��������
		//
		PCF_DataStruct->RTC_Years    = buffer[3];
		PCF_DataStruct->RTC_Months   = buffer[2];
		PCF_DataStruct->RTC_Days     = buffer[0];
		PCF_DataStruct->RTC_WeekDays = buffer[1];
	}
}

/**
   ============================================================================
                         #### ������Ϣ�������ܺ��� ####
   ============================================================================
**/

/**
  *****************************************************************************
  * @Name   : PCF8563д��������Ϣ
  *
  * @Brief  : ������ֵ��Χ��: 0 ~ 6��ʮ���Ƹ�ʽ
  *
  * @Input  : PCF_Format:     ���ݸ�ʽ
  *                           = PCF_Format_BIN
  *                           = PCF_Format_BCD
  *           PCF_DataStruct: ����ṹָ��
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
void PCF8563_SetAlarm(unsigned char PCF_Format, _PCF8563_Alarm_Typedef* PCF_DataStruct)
{
	unsigned char Alarm_State = 0;
	unsigned char Alarm_Interrupt = 0;  //����/״̬�Ĵ��������жϻ���
	
	if (PCF_Format == PCF_Format_BIN)  //ʮ���Ƹ�ʽ����Ҫת��һ��
	{
		//
		//�ж���ֵ�Ƿ��ڷ�Χ֮��
		//
		if (PCF_DataStruct->RTC_AlarmMinutes > 59)  PCF_DataStruct->RTC_AlarmMinutes  = 0;  //�ָ�0����
		if (PCF_DataStruct->RTC_AlarmHours > 23)    PCF_DataStruct->RTC_AlarmHours    = 0;  //�ָ�0Сʱ
		if (PCF_DataStruct->RTC_AlarmDays > 31)     PCF_DataStruct->RTC_AlarmDays     = 1;  //�ָ�1��
		if (PCF_DataStruct->RTC_AlarmWeekDays > 6)  PCF_DataStruct->RTC_AlarmWeekDays = 1;  //�ָ�����һ
		//
		//��Ҫת��һ��
		//
		PCF_DataStruct->RTC_AlarmMinutes  = RTC_BinToBcd2(PCF_DataStruct->RTC_AlarmMinutes);
		PCF_DataStruct->RTC_AlarmHours    = RTC_BinToBcd2(PCF_DataStruct->RTC_AlarmHours);
		PCF_DataStruct->RTC_AlarmDays     = RTC_BinToBcd2(PCF_DataStruct->RTC_AlarmDays);
		PCF_DataStruct->RTC_AlarmWeekDays = RTC_BinToBcd2(PCF_DataStruct->RTC_AlarmWeekDays);
	}
	//
	//�ж��Ƿ�������ѡ��
	//
	if (PCF_DataStruct->RTC_AlarmNewState == RTC_AlarmNewState_Open)  //ֻ������
	{
		Alarm_State = 1;
	}
	else if (PCF_DataStruct->RTC_AlarmNewState == RTC_AlarmNewState_Open_INT_Enable)  //�����岢���ж����
	{
		Alarm_State = 2;
	}
	else if (PCF_DataStruct->RTC_AlarmNewState == RTC_AlarmNewState_Close)  //�ر����壬���ҹر��ж����
	{
		Alarm_State = 3;
	}
	//
	//��ȡ����/״̬�Ĵ���2ֵ
	//
	Alarm_Interrupt = PCF8563_Read_Byte(PCF8563_Address_Control_Status_2);
	Alarm_Interrupt &= PCF_Alarm_INT_Close;  //�ȹر��ж����
	Alarm_Interrupt &= PCF_Control_ClearAF;;  //�����־
	PCF8563_Write_Byte(PCF8563_Address_Control_Status_2, Alarm_Interrupt);
	
	//
	//���ݿ������ͽ�����Ӧ����
	//
	if (Alarm_State == 1 || Alarm_State == 2)  //������
	{
		if (PCF_DataStruct->RTC_AlarmType & RTC_AlarmType_Days)      PCF_DataStruct->RTC_AlarmDays     &= PCF_Alarm_DaysOpen;  //��������
		if (PCF_DataStruct->RTC_AlarmType & RTC_AlarmType_Hours)     PCF_DataStruct->RTC_AlarmHours    &= PCF_Alarm_HoursOpen;  //Сʱ����
		if (PCF_DataStruct->RTC_AlarmType & RTC_AlarmType_Minutes)   PCF_DataStruct->RTC_AlarmMinutes  &= PCF_Alarm_MinutesOpen;  //��������
		if (PCF_DataStruct->RTC_AlarmType & RTC_AlarmType_WeekDays)  PCF_DataStruct->RTC_AlarmWeekDays &= PCF_Alarm_WeekDaysOpen;  //��������
	}
	if (Alarm_State == 3)  //�ر�
	{
		if (PCF_DataStruct->RTC_AlarmType & RTC_AlarmType_Days)      PCF_DataStruct->RTC_AlarmDays     |= PCF_Alarm_DaysClose;  //��������
		if (PCF_DataStruct->RTC_AlarmType & RTC_AlarmType_Hours)     PCF_DataStruct->RTC_AlarmHours    |= PCF_Alarm_HoursClose;  //Сʱ����
		if (PCF_DataStruct->RTC_AlarmType & RTC_AlarmType_Minutes)   PCF_DataStruct->RTC_AlarmMinutes  |= PCF_Alarm_MinutesClose;  //��������
		if (PCF_DataStruct->RTC_AlarmType & RTC_AlarmType_WeekDays)  PCF_DataStruct->RTC_AlarmWeekDays |= PCF_Alarm_WeekDaysClose;  //��������
	}
	//
	//�ж��Ƿ����ж����
	//
	if (Alarm_State == 2)
	{
		Alarm_Interrupt |= PCF_Alarm_INT_Open;
		Alarm_Interrupt &= PCF_Control_ClearAF;;  //�����־
	}
	//
	//��������
	//
	buffer[0] = PCF_DataStruct->RTC_AlarmMinutes;
	buffer[1] = PCF_DataStruct->RTC_AlarmHours;
	buffer[2] = PCF_DataStruct->RTC_AlarmDays;
	buffer[3] = PCF_DataStruct->RTC_AlarmWeekDays;
	//
	//д�����ݵ��Ĵ���
	//
	PCF8563_Write_nByte(PCF8563_Alarm_Minutes, 4, buffer);
	//
	//д�����/״̬�Ĵ���2��ֵ
	//
	PCF8563_Write_Byte(PCF8563_Address_Control_Status_2, Alarm_Interrupt);
}

/**
  *****************************************************************************
  * @Name   : PCF8563��ȡ������Ϣ
  *
  * @Brief  : ������ֵ��Χ��: 0 ~ 6��ʮ���Ƹ�ʽ��ֻ�Ƿ���������Ĵ�����ֵ������λ���ж����ʲô�Ĳ�����
  *
  * @Input  : PCF_Format:     ���ݸ�ʽ
  *                           = PCF_Format_BIN
  *                           = PCF_Format_BCD
  *
  * @Output : PCF_DataStruct: ����ṹָ��
  *
  * @Return : none
  *****************************************************************************
**/
void PCF8563_GetAlarm(unsigned char PCF_Format, _PCF8563_Alarm_Typedef* PCF_DataStruct)
{
	//
	//��ȡȫ���Ĵ�����ֵ
	//
	PCF8563_Read_nByte(PCF8563_Alarm_Minutes, 4, buffer);
	//
	//������Чλ�����ӱ���ֵȫ������
	//
	buffer[0] &= PCF8563_Shield_Minute_Alarm;  //���ӱ���ֵ
	buffer[1] &= PCF8563_Shield_Hour_Alarm;  //Сʱ����ֵ
	buffer[2] &= PCF8563_Shield_Day_Alarm;  //���ڱ���ֵ
	buffer[3] &= PCF8563_Shield_WeekDays_Alarm;  //���ڱ���ֵ
	
	//
	//�ж���Ҫ�����ݸ�ʽ
	//
	if (PCF_Format == PCF_Format_BIN)
	{
		PCF_DataStruct->RTC_AlarmDays     = RTC_Bcd2ToBin(buffer[2]);
		PCF_DataStruct->RTC_AlarmHours    = RTC_Bcd2ToBin(buffer[1]);
		PCF_DataStruct->RTC_AlarmMinutes  = RTC_Bcd2ToBin(buffer[0]);
		PCF_DataStruct->RTC_AlarmWeekDays = RTC_Bcd2ToBin(buffer[3]);
	}
	else if (PCF_Format == PCF_Format_BCD)
	{
		//
		//��������
		//
		PCF_DataStruct->RTC_AlarmDays     = buffer[2];
		PCF_DataStruct->RTC_AlarmHours    = buffer[1];
		PCF_DataStruct->RTC_AlarmMinutes  = buffer[0];
		PCF_DataStruct->RTC_AlarmWeekDays = buffer[3];
	}
}
