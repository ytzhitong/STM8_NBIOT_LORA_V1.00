#ifndef _AM2320_H_
#define _AM2320_H_

#define DQ_PORT		GPIOB  //设置DQ引脚
#define DQ_PIN		GPIO_Pin_0
#define SET_OP_1WIRE 	GPIO_SetBits(DQ_PORT, DQ_PIN)
#define CLR_OP_1WIRE 	GPIO_ResetBits(DQ_PORT, DQ_PIN)
#define SET_DIR_1WIRE	GPIO_Init(DQ_PORT,DQ_PIN,GPIO_Mode_Out_PP_Low_Fast)
#define CLR_DIR_1WIRE 	GPIO_Init(DQ_PORT,DQ_PIN,GPIO_Mode_In_FL_No_IT)
#define CHECK_IP_1WIRE  GPIO_ReadInputDataBit(DQ_PORT, DQ_PIN)

extern void delay_10us(u16 n_10us);
extern void delay_ms(u16 n_ms);

uint8_t  AM2320_get_value(uint16_t* hum,short* temp); //读取温湿度值

#endif 