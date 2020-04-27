/**
  *****************************************************************************
  *                            ʱ��оƬPCF8563����
  *
  *                       (C) Copyright 2000-2020, ***
  *                            All Rights Reserved
  *****************************************************************************
  *
  * @File    : pcf8563.h
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
  *            B�����Ӷ�ȡʱ�䴦����������ȡ����ʱ��������Ϣֱ��ת����ASCII���浽ʱ���ʽ������
  *            C������ʱ�����ڴ�����������ʾ���ߴ�ӡ�����ڵĻ�ֱ����ʾ���ߴ�ӡʱ���ʽ���鼴��
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


#ifndef _pcf8563_h_ 
#define _pcf8563_h_


/**
  ****************************** Support C++ **********************************
**/
#ifdef __cplusplus
	extern "C"{
#endif		
/**
  *****************************************************************************
**/
		

/******************************************************************************
                             �ⲿ����ͷ�ļ�
                  Ӧ�õ���ͬ������ͷ�ļ����������޸ļ���                        
******************************************************************************/

#include "stm8l10x.h"

/******************************************************************************
                                 �����궨��
******************************************************************************/

#define PCF8563_Check_Data                       (unsigned char)0x55  //����ã�����������ֵ

#define PCF8563_Write                            (unsigned char)0xa2  //д����
#define PCF8563_Read                             (unsigned char)0xa3  //����������ã�PCF8563_Write + 1��

//
//��Դ��λ����
//
#define PCF8563_PowerResetEnable                 (unsigned char)0x08
#define PCF8563_PowerResetDisable                (unsigned char)0x09

//
//����λ��������
//
#define PCF_Century_SetBitC                      (unsigned char)0x80

#define PCF_Century_19xx                         (unsigned char)0x03
#define PCF_Century_20xx                         (unsigned char)0x04

//
//���ݸ�ʽ
//
#define PCF_Format_BIN                           (unsigned char)0x01
#define PCF_Format_BCD                           (unsigned char)0x02

//
//����PCF8563ģʽ��
//
#define PCF_Mode_Normal                          (unsigned char)0x05
#define PCF_Mode_EXT_CLK                         (unsigned char)0x06

#define PCF_Mode_INT_Alarm                       (unsigned char)0x07
#define PCF_Mode_INT_Timer                       (unsigned char)0x08

/******************************************************************************
                             �����Ĵ�����ַ�궨��
******************************************************************************/

#define PCF8563_Address_Control_Status_1         (unsigned char)0x00  //����/״̬�Ĵ���1
#define PCF8563_Address_Control_Status_2         (unsigned char)0x01  //����/״̬�Ĵ���2

#define PCF8563_Address_CLKOUT                   (unsigned char)0x0d  //CLKOUTƵ�ʼĴ���
#define PCF8563_Address_Timer                    (unsigned char)0x0e  //��ʱ�����ƼĴ���
#define PCF8563_Address_Timer_VAL                (unsigned char)0x0f  //��ʱ���������Ĵ���

#define PCF8563_Address_Years                    (unsigned char)0x08  //��
#define PCF8563_Address_Months                   (unsigned char)0x07  //��
#define PCF8563_Address_Days                     (unsigned char)0x05  //��
#define PCF8563_Address_WeekDays                 (unsigned char)0x06  //����
#define PCF8563_Address_Hours                    (unsigned char)0x04  //Сʱ
#define PCF8563_Address_Minutes                  (unsigned char)0x03  //����
#define PCF8563_Address_Seconds                  (unsigned char)0x02  //��

#define PCF8563_Alarm_Minutes                    (unsigned char)0x09  //���ӱ���
#define PCF8563_Alarm_Hours                      (unsigned char)0x0a  //Сʱ����
#define PCF8563_Alarm_Days                       (unsigned char)0x0b  //�ձ���
#define PCF8563_Alarm_WeekDays                   (unsigned char)0x0c  //���ڱ���

/******************************************************************************
                               �������κ궨��
******************************************************************************/

#define PCF8563_Shield_Control_Status_1          (unsigned char)0xa8
#define PCF8563_Shield_Control_Status_2          (unsigned char)0x1f

#define PCF8563_Shield_Seconds                   (unsigned char)0x7f
#define PCF8563_Shield_Minutes                   (unsigned char)0x7f
#define PCF8563_Shield_Hours                     (unsigned char)0x3f

#define PCF8563_Shield_Days                      (unsigned char)0x3f
#define PCF8563_Shield_WeekDays                  (unsigned char)0x07
#define PCF8563_Shield_Months_Century            (unsigned char)0x1f
#define PCF8563_Shield_Years                     (unsigned char)0xff

#define PCF8563_Shield_Minute_Alarm              (unsigned char)0x7f
#define PCF8563_Shield_Hour_Alarm                (unsigned char)0x3f
#define PCF8563_Shield_Day_Alarm                 (unsigned char)0x3f
#define PCF8563_Shield_WeekDays_Alarm            (unsigned char)0x07

#define PCF8563_Shield_CLKOUT_Frequency          (unsigned char)0x03
#define PCF8563_Shield_Timer_Control             (unsigned char)0x03
#define PCF8563_Shield_Timer_Countdown_Value     (unsigned char)0xff

/******************************************************************************
                              PCF8563�Ĵ����ṹ����                          
******************************************************************************/

/**
   ==================================================================
                               ȫ���Ĵ����ṹ
   ==================================================================
**/
typedef struct
{
	unsigned char Control_Status_1;  //���ƼĴ���1
	unsigned char Control_Status_2;  //���ƼĴ���2
	unsigned char Seconds;  //��Ĵ���
	unsigned char Minutes;  //���ӼĴ���
	unsigned char Hours;  //Сʱ�Ĵ���
	unsigned char Days;  //���ڼĴ���
	unsigned char WeekDays;  //���ڼĴ�������ֵ��Χ��0 ~ 6
	unsigned char Months_Century;  //�·ݼĴ�����bit7Ϊ����λ��0��ָ��������Ϊ20xx��1��ָ��������Ϊ19xx
	unsigned char Years;  //��Ĵ���
	unsigned char Minute_Alarm;  //���ӱ����Ĵ���
	unsigned char Hour_Alarm;  //Сʱ�����Ĵ���
	unsigned char Day_Alarm;  //���ڱ����Ĵ���
	unsigned char WeekDays_Alarm;  //���ڱ����Ĵ�������ֵ��Χ��0 ~ 6
	unsigned char CLKOUT_Frequency;  //Ƶ�ʹܽ�������ƼĴ���
	unsigned char Timer_Control;  //��ʱ�����ƼĴ���
	unsigned char Timer_Countdown_Value;  //��ʱ�������Ĵ���
}_PCF8563_Register_Typedef;

/**
   ==================================================================
                               ʱ����Ϣ�ṹ
   ==================================================================
**/
typedef struct
{
	unsigned char RTC_Hours;  //Сʱ
	unsigned char RTC_Minutes;  //����
	unsigned char RTC_Seconds;  //����
	unsigned char Reseved;  //����
}_PCF8563_Time_Typedef;

/**
   ==================================================================
                               ������Ϣ�ṹ
   ==================================================================
**/
typedef struct
{
	unsigned char RTC_Years;  //���
	unsigned char RTC_Months;  //�·�
	unsigned char RTC_Days;  //����
	unsigned char RTC_WeekDays;  //���ڣ���ֵ��Χ��0 ~ 6
}_PCF8563_Date_Typedef;

extern _PCF8563_Time_Typedef PCF8563_Time;
extern _PCF8563_Date_Typedef PCF8563_Date;

/**
   ==================================================================
                               ������Ϣ�ṹ
   ==================================================================
**/
typedef struct
{
	unsigned short Reseved;  //����
	unsigned char RTC_AlarmDays;  //��������
	unsigned char RTC_AlarmWeekDays;  //�������壬��ֵ��Χ��0 ~ 6
	unsigned char RTC_AlarmHours;  //Сʱ����
	unsigned char RTC_AlarmMinutes;  //��������
	unsigned char RTC_AlarmNewState;  //���忪��
	                                  //��ֵ��RTC_AlarmNewState_Open��RTC_AlarmNewState_Close��RTC_AlarmNewState_Open_INT_Enable
	                                  //ֻʹ������һ������
	unsigned char RTC_AlarmType;  //�������ͣ�
	                              //��ֵ��RTC_AlarmType_Minutes��RTC_AlarmType_Hours��RTC_AlarmType_Days��RTC_AlarmType_WeekDays
	                              //����������ʹ򿪣����û��ϵ�ϲ�
}_PCF8563_Alarm_Typedef;

//
//���忪��
//
#define RTC_AlarmNewState_Open                   (u8)0x01
#define RTC_AlarmNewState_Close                  (u8)0x02  //����رգ����ҹر��ж����
#define RTC_AlarmNewState_Open_INT_Enable        (u8)0x04  //���忪���������ж����
//
//������������
//
#define RTC_AlarmType_Minutes                    (unsigned char)0x01
#define RTC_AlarmType_Hours                      (unsigned char)0x02
#define RTC_AlarmType_Days                       (unsigned char)0x04
#define RTC_AlarmType_WeekDays                   (unsigned char)0x08

/**
   ==================================================================
                               Ƶ�������Ϣ�ṹ
   ==================================================================
**/
typedef struct
{
	unsigned char CLKOUT_Frequency;  //���Ƶ��ѡ��
	                                 //PCF_CLKOUT_F32768 --> ���32.768KHz
                                     //PCF_CLKOUT_F1024  --> ���1024Hz
                                     //PCF_CLKOUT_F32    --> ���32Hz
                                     //PCF_CLKOUT_F1     --> ���1Hz
	unsigned char CLKOUT_NewState;  //���״̬
	                                //PCF_CLKOUT_Close   --> �������ֹ����ɸ��迹
	                                //PCF_CLKOUT_Open    --> �����Ч
}_PCF8563_CLKOUT_Typedef;

//
//Ƶ������Ĵ��� --> 0x0d
//
#define PCF_CLKOUT_Open                          (unsigned char)(1<<7)     //CLKOUT�����Ч
#define PCF_CLKOUT_Close                         (unsigned char)(~(1<<7))  //CLKOUT�������ֹ����ɸ��迹

#define PCF_CLKOUT_F32768                        (unsigned char)0x00  //���32.768KHz
#define PCF_CLKOUT_F1024                         (unsigned char)0x01  //���1024Hz
#define PCF_CLKOUT_F32                           (unsigned char)0x02  //���32Hz
#define PCF_CLKOUT_F1                            (unsigned char)0x03  //���1Hz

/**
   ==================================================================
                               ��ʱ����Ϣ�ṹ
   ==================================================================
**/
typedef struct
{
	unsigned char RTC_Timer_Value;  //��ʱ����������ֵ�����ö�ʱʱ��������Ҫ��ֱ����0����
	unsigned char RTC_Timer_Frequency;  //��ʱ������Ƶ��
	                                    //PCF_Timer_F4096 --> ��ʱ��ʱ��Ƶ��Ϊ4096Hz
	                                    //PCF_Timer_F64   --> ��ʱ��ʱ��Ƶ��Ϊ64Hz
	                                    //PCF_Timer_F1    --> ��ʱ��ʱ��Ƶ��Ϊ1Hz
	                                    //PCF_Timer_F160  --> ��ʱ��ʱ��Ƶ��Ϊ1/60Hz
	unsigned char RTC_Timer_NewState;  //����״̬
	                                   //PCF_Timer_Close  --> �ر�
	                                   //PCF_Timer_Open   --> ����
	unsigned char RTC_Timer_Interrupt;  //�Ƿ������ж����
	                                    //PCF_Time_INT_Close --> �ر�
	                                    //PCF_Time_INT_Open  --> ����
}_PCF8563_Timer_Typedef;

//
//��ʱ�����ƼĴ��� --> 0x0e
//
#define PCF_Timer_Open                           (unsigned char)(1<<7)     //��ʱ����Ч
#define PCF_Timer_Close                          (unsigned char)(~(1<<7))  //��ʱ����Ч

#define PCF_Timer_F4096                          (unsigned char)0x00  //��ʱ��ʱ��Ƶ��Ϊ4096Hz
#define PCF_Timer_F64                            (unsigned char)0x01  //��ʱ��ʱ��Ƶ��Ϊ64Hz
#define PCF_Timer_F1                             (unsigned char)0x02  //��ʱ��ʱ��Ƶ��Ϊ1Hz
#define PCF_Timer_F160                           (unsigned char)0x03  //��ʱ��ʱ��Ƶ��Ϊ1/60Hz

//
//�ж��������
//
#define PCF_Time_INT_Open                        (unsigned char)(1<<0)     //��ʱ���ж���Ч
#define PCF_Time_INT_Close                       (unsigned char)(~(1<<0))  //��ʱ���ж���Ч

/******************************************************************************
                                 �����궨��
******************************************************************************/

//
//����/״̬�Ĵ���1 --> 0x00
//
#define PCF_Control_Status_NormalMode            (unsigned char)(~(1<<7))  //��ͨģʽ
#define PCF_Control_Status_EXT_CLKMode           (unsigned char)(1<<7)     //EXT_CLK����ģʽ
#define PCF_Control_ChipRuns                     (unsigned char)(~(1<<5))  //оƬ����
#define PCF_Control_ChipStop                     (unsigned char)(1<<5)     //оƬֹͣ���У�����оƬ��Ƶ���첽���߼�0
#define PCF_Control_TestcClose                   (unsigned char)(~(1<<3))  //��Դ��λ����ʧЧ����ͨģʽʱ���߼�0��
#define PCF_Control_TestcOpen                    (unsigned char)(1<<3)     //��Դ��λ������Ч

//
//����/״̬�Ĵ���2 --> 0x01
//
#define PCF_Control_TI_TF1                       (unsigned char)(~(1<<4))  //��TF��ЧʱINT��Ч����ȡ����TIE��״̬��
#define PCF_Control_TI_TF2                       (unsigned char)(1<<4)     //INT������Ч����ȡ����TIE��״̬��
                                                                           //ע�⣺��AF��AIE����Чʱ����INTһֱ��Ч
#define PCF_Control_ClearAF                      (unsigned char)(~(1<<3))  //�������
#define PCF_Control_ClearTF                      (unsigned char)(~(1<<2))
                                                                           //����������ʱ��AF��ֵ�߼�1���ڶ�ʱ������������ʱ��
                                                                           //TF��ֵ�߼�1�������ڱ�������дǰһֱ����ԭ��ֵ��
                                                                           //����ʱ���ͱ����ж϶�����ʱ���ж�Դ��AF��TF������
                                                                           //��Ҫʹ���һ����־λ����ֹ��һ��־λ����д��Ӧ�����߼�
                                                                           //ָ��AND
#define PCF_Alarm_INT_Open                       (unsigned char)(1<<1)     //�����ж���Ч
#define PCF_Alarm_INT_Close                      (unsigned char)(~(1<<1))  //�����ж���Ч

//
//��Ĵ��� --> 0x02
//
#define PCF_Accuracy_ClockNo                     (unsigned char)(1<<7)     //����֤׼ȷ��ʱ��/��������
#define PCF_Accuracy_ClockYes                    (unsigned char)(~(1<<7))  //��֤׼ȷ��ʱ��/��������

//
//��������Ĵ��� --> 0x09
//
#define PCF_Alarm_MinutesOpen                    (unsigned char)(~(1<<7))  //���ӱ�����Ч
#define PCF_Alarm_MinutesClose                   (unsigned char)(1<<7)     //���ӱ�����Ч

//
//Сʱ����Ĵ��� --> 0x0a
//
#define PCF_Alarm_HoursOpen                      (unsigned char)(~(1<<7))  //Сʱ������Ч
#define PCF_Alarm_HoursClose                     (unsigned char)(1<<7)     //Сʱ������Ч

//
//��������Ĵ��� --> 0x0b
//
#define PCF_Alarm_DaysOpen                       (unsigned char)(~(1<<7))  //�ձ�����Ч
#define PCF_Alarm_DaysClose                      (unsigned char)(1<<7)     //�ձ�����Ч

//
//��������Ĵ��� --> 0x0c
//
#define PCF_Alarm_WeekDaysOpen                   (unsigned char)(~(1<<7))  //���ڱ�����Ч
#define PCF_Alarm_WeekDaysClose                  (unsigned char)(1<<7)     //���ڱ�����Ч

/******************************************************************************
                                  �ⲿ���ܺ���                      
******************************************************************************/

void InitIIC(void);

void          PCF8563_Write_Byte                 (unsigned char REG_ADD, unsigned char dat);  //PCF8563ĳ�Ĵ���д��һ���ֽ�����
unsigned char PCF8563_Read_Byte                  (unsigned char REG_ADD);  //PCF8563ĳ�Ĵ�����ȡһ���ֽ�����
void          PCF8563_Write_nByte                (unsigned char REG_ADD, unsigned char num, unsigned char *pBuff);  //PCF8563д���������
void          PCF8563_Read_nByte                 (unsigned char REG_ADD, unsigned char num, unsigned char *pBuff);  //PCF8563��ȡ��������

unsigned char PCF8563_Check                      (void);  //PCF8563����Ƿ����
void          PCF8563_Start                      (void);  //PCF8563����
void          PCF8563_Stop                       (void);  //PCF8563ֹͣ

void          PCF8563_SetMode                    (unsigned char Mode);  //PCF8563��������ģʽ
void          PCF8563_SetPowerReset              (unsigned char NewState);  //PCF8563���õ�Դ��λ���ܿ�����ر�

void          PCF8563_SetCLKOUT                  (_PCF8563_CLKOUT_Typedef* PCF_CLKOUTStruct);  //PCF8563�������Ƶ��
void          PCF8563_SetTimer                   (_PCF8563_Timer_Typedef* PCF_TimerStruct);  //PCF8563���ö�ʱ��

void          PCF8563_Set_Times                  (unsigned char PCF_Format,\
                                                  unsigned char PCF_Century,\
	                                              unsigned char Year, unsigned char Month, unsigned char Date, unsigned char Week,\
                                                  unsigned char Hour, unsigned char Minute, unsigned char Second);  //����ʱ�䣬��Ҫ���ں�̨���ã����߳�ʼ��ʱ����

void          PCF8563_SetRegister                (unsigned char PCF_Format, unsigned char PCF_Century, _PCF8563_Register_Typedef* PCF_DataStruct);  //PCF8563д��Ĵ���
void          PCF8563_GetRegister                (unsigned char PCF_Format, unsigned char *PCF_Century, _PCF8563_Register_Typedef* PCF_DataStruct);  //PCF8563��ȡ�Ĵ���

void          PCF8563_SetTime                    (unsigned char PCF_Format, _PCF8563_Time_Typedef* PCF_DataStruct);  //PCF8563д��ʱ����Ϣ
void          PCF8563_GetTime                    (unsigned char PCF_Format, _PCF8563_Time_Typedef* PCF_DataStruct);  //PCF8563��ȡʱ����Ϣ

void          PCF8563_SetDate                    (unsigned char PCF_Format, unsigned char PCF_Century, _PCF8563_Date_Typedef* PCF_DataStruct);  //PCF8563д��������Ϣ
void          PCF8563_GetDate                    (unsigned char PCF_Format, unsigned char *PCF_Century, _PCF8563_Date_Typedef* PCF_DataStruct);  //PCF8563��ȡ������Ϣ

void          PCF8563_SetAlarm                   (unsigned char PCF_Format, _PCF8563_Alarm_Typedef* PCF_DataStruct);  //PCF8563д��������Ϣ
void          PCF8563_GetAlarm                   (unsigned char PCF_Format, _PCF8563_Alarm_Typedef* PCF_DataStruct);  //PCF8563��ȡ������Ϣ


/**
  ****************************** Support C++ **********************************
**/
#ifdef __cplusplus
	}
#endif
/**
  *****************************************************************************
**/


#endif  /* end pcf8563.h */