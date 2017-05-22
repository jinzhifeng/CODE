/*
 * rgbled_P9813.c
 *
 *  Created on: 2017��5��18��
 *      Author: jinzhifeng
 *      https://github.com/jinzhifeng
 */
#include "rgbled_p9813.h"

//�Ƚ����ݿ���0��Ȼ����32��ʱ�����壬�൱�ڷ���32��bit 0
void Send32BitZero(void)
{
	int8_t i;
	RGBLed_P9813_Data_Clr(); //ǰ32 λ��0��Ϊ��ʼ֡����Cin ������ʱ���룬����ʱ��DINҪ����CIN��
	for(i = 0; i != 32; i++)
	{
		RGBLed_P9813_Clk_Clr();	//CLK = 0
		RGBLed_P9813_Delay();
		RGBLed_P9813_Clk_Set();	//CLK = 1
		RGBLed_P9813_Delay();
	}
}

//������������ķ���ֵ
uint8_t CovertToOnesComplement(uint8_t Data)
{
	uint8_t tempData = 0;
	tempData =  ((~Data) & 0xC0) >> 6; //��Data����ת���ɷ���
	return tempData;
}


void RGBLed_P9813_SendRGBData(uint8_t rDATA, uint8_t gDATA, uint8_t bDATA)
{
	uint32_t rgbdatTemp = 0;
	int8_t i;
	//�Ƚ�R��G��B��ֵ�ֱ�ȡ����
	rgbdatTemp |= (uint32_t)0x03 << 30; //��־λΪ������1��
	rgbdatTemp |= (uint32_t)CovertToOnesComplement(bDATA) << 28;
	rgbdatTemp |= (uint32_t)CovertToOnesComplement(gDATA) << 26;
	rgbdatTemp |= (uint32_t)CovertToOnesComplement(rDATA) << 24;
	rgbdatTemp |= (uint32_t)bDATA << 16;
	rgbdatTemp |= (uint32_t)gDATA << 8;
	rgbdatTemp |= rDATA;
//32λ���ݸ�ʽ��11 00 00 00 00 00 0000 g7 g6 g5 g4 g3 g2 g1 g0 r7 r6 r5 r4 r3 r2 r1 r0
//
	for(i = 0; i != 32; i++)
	{
		if(rgbdatTemp & 0x80000000)
			RGBLed_P9813_Data_Set();
		else RGBLed_P9813_Data_Clr();
		rgbdatTemp <<= 1;	//�ȴ���λ�󴫵�λ

		RGBLed_P9813_Clk_Clr();	//CLK = 0
		RGBLed_P9813_Delay();
		RGBLed_P9813_Clk_Set();	//CLK = 1
		RGBLed_P9813_Delay();
	}
}


void RGBLed_P9813_Init(void)
{
	Send32BitZero();
	RGBLed_P9813_SetRGBValue(0, 0, 0);
	RGBLed_P9813_SetRGBValue(0, 0, 0);
}

void RGBLed_P9813_SetRGBValue(uint8_t rValue, uint8_t gValue, uint8_t bValue)
{
	RGBLed_PowerON();
	Send32BitZero();
	RGBLed_P9813_SendRGBData(rValue, gValue, bValue);
	RGBLed_P9813_SendRGBData(rValue, gValue, bValue);
}
