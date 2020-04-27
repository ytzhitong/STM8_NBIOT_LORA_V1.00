/*******************************************************************************
****�ļ�������DS18B20 STM8L�µ�����
****��Ȩ��Ϣ������Ƕ��ʽϵͳ
*******************************************************************************/
#include "stm8l10x.h"
#include "AM2320.h"
 

void delay_us(u32 nCount)
{
//  for(char i=0;i<5;i++) 

  {
    while (nCount != 0) 
    {
      nCount--; 
    } 
  } 


} 

uint8_t AM2320_read_byte(void)
{
	uint16_t j = 0;
	uint8_t data = 0, bit = 0;

    for(uint8_t i = 0; i < 8; i++)
    {
        // �жϵ͵�ƽʱ��
        while(!CHECK_IP_1WIRE)
        {
            // ����55us,���ִ���������
            if(++j>=55)
            {
                break;
            }
        }
        // ��ʱ30us
        delay_10us(3);

        // ��ȡ����״̬
        bit = CHECK_IP_1WIRE;
        j = 0;
        // �жϸߵ�ƽʱ��
        while(CHECK_IP_1WIRE)
        {
            // 
            if(++j >= 55)
            {
                break;
            }
        }
        data <<= 1;
        data |= bit;
    }
    return data;
}


uint8_t  AM2320_get_value(uint16_t* hum,short* temp)
{
	u16 i=0;
	u16 j=0;
        u16 k=0;
	uint8_t HumHigh, HumLow, TempHigh, TempLow, TempChecksum, Temp;

	SET_DIR_1WIRE;
	CLR_OP_1WIRE;		//����1000us
	delay_10us(100);

	CLR_DIR_1WIRE;
       
        nop();
        
    while(CHECK_IP_1WIRE) //�жϴӻ��Ƿ��е͵�ƽ��Ӧ�ź�
    {
       k++;
       if(i>200)
    	   break;
    }
    
    nop();

    while(!CHECK_IP_1WIRE) //�жϴӻ��Ƿ��е͵�ƽ��Ӧ�ź�
    {
       i++;
       if(i>85)
   	   break;
    }
    
    nop();

    while(CHECK_IP_1WIRE) //�жϴӻ��Ƿ��иߵ�ƽ��Ӧ�ź�
    {
        j++;
        if(j>85)
             break;
    }

    if(i>80||j>80||i<20||j<20) //��Ӧ�쳣
    return 1;

    // ��������
    HumHigh   = AM2320_read_byte();
    HumLow    = AM2320_read_byte();
    TempHigh  = AM2320_read_byte();
    TempLow   = AM2320_read_byte();
    TempChecksum = AM2320_read_byte();

    Temp = (uint8_t)(HumHigh + HumLow + TempHigh + TempLow);
    if(Temp!=TempChecksum)
    return 2;

    if((TempHigh>>7)==0x01)
    {
    	*temp =-(( (TempHigh&0x07)<<8)+ TempLow);
    }
    else
    {
    	*temp =(( TempHigh<<8)+ TempLow);
    }

    *hum =(( HumHigh<<8 )+ HumLow);


    return 0;
}