/*
 * Oled12864_ssd1306.c
 *
 *  Created on: 2017��5��20��
 *      Author: jinzhifeng
 *      https://github.com/jinzhifeng
 */

#include "oled12864_ssd1306.h"
#include "Oledfont.h"


//Oled���Դ�
//��Ÿ�ʽ����.
//[0]0 1 2 3 ... 127
//[1]0 1 2 3 ... 127
//[2]0 1 2 3 ... 127
//[3]0 1 2 3 ... 127
//[4]0 1 2 3 ... 127
//[5]0 1 2 3 ... 127
//[6]0 1 2 3 ... 127
//[7]0 1 2 3 ... 127


void Oled_WriteCommand(uint8_t OledCommand)
{
	IIC_Start();
	IIC_WriteOneByte(0x78);         //Slave address,SA0=0
	IIC_WriteOneByte(0x00);			//write command
	IIC_WriteOneByte(OledCommand);
	IIC_Stop();
}

void Oled_WriteData(uint8_t OledData)
{
	IIC_Start();
	IIC_WriteOneByte(0x78);			//D/C#=0; R/W#=0
	IIC_WriteOneByte(0x40);			//write data
	IIC_WriteOneByte(OledData);
	IIC_Stop();
}

void fill_picture(uint8_t fill_Data)
{
	uint8_t m, n;
	for (m = 0; m < 8; m++)
	{
		Oled_WriteCommand(0xb0 + m);		//page0-page1
		Oled_WriteCommand(0x00);		//low column start address
		Oled_WriteCommand(0x10);		//high column start address
		for (n = 0; n < 128; n++)
		{
			Oled_WriteData(fill_Data);
		}
	}
}

//��������
void Oled_Set_Pos(uint8_t x, uint8_t y)
{
	Oled_WriteCommand(0xb0 + y);
	Oled_WriteCommand(((x & 0xf0) >> 4) | 0x10);
	Oled_WriteCommand((x & 0x0f));
}

//����Oled��ʾ
void Oled_Display_On(void)
{
	Oled_WriteCommand(0X8D);  //SET DCDC����
	Oled_WriteCommand(0X14);  //DCDC ON
	Oled_WriteCommand(0XAF);  //DISPLAY ON
}

//�ر�Oled��ʾ
void Oled_Display_Off(void)
{
	Oled_WriteCommand(0X8D);  //SET DCDC����
	Oled_WriteCommand(0X10);  //DCDC OFF
	Oled_WriteCommand(0XAE);  //DISPLAY OFF
}

//��������,������,������Ļ�Ǻ�ɫ��!��û����һ��!!!
void Oled_Clear(void)
{
	uint8_t i, n;
	for (i = 0; i < 8; i++)
	{
		Oled_WriteCommand(0xb0 + i);    //����ҳ��ַ��0~7��
		Oled_WriteCommand(0x00);      //������ʾλ�á��е͵�ַ
		Oled_WriteCommand(0x10);      //������ʾλ�á��иߵ�ַ
		for (n = 0; n < 128; n++)
			Oled_WriteData(0);
	} //������ʾ
}

void Oled_On(void)
{
	uint8_t i, n;
	for (i = 0; i < 8; i++)
	{
		Oled_WriteCommand(0xb0 + i);    //����ҳ��ַ��0~7��
		Oled_WriteCommand(0x00);      //������ʾλ�á��е͵�ַ
		Oled_WriteCommand(0x10);      //������ʾλ�á��иߵ�ַ
		for (n = 0; n < 128; n++)
			Oled_WriteData(1);
	} //������ʾ
}

//��ָ��λ����ʾһ���ַ�,���������ַ�
//x:0~127
//y:0~63
//mode:0,������ʾ;1,������ʾ
//size:ѡ������ 16/12
void Oled_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t Char_Size)
{
	uint8_t c = 0, i = 0;
	c = chr - ' '; //�õ�ƫ�ƺ��ֵ
	if (x > Max_Column - 1)
	{
		x = 0;
		y = y + 2;
	}
	if (Char_Size == 16)
	{
		Oled_Set_Pos(x, y);
		for (i = 0; i < 8; i++)
			Oled_WriteData(F8X16[c * 16 + i]);
		Oled_Set_Pos(x, y + 1);
		for (i = 0; i < 8; i++)
			Oled_WriteData(F8X16[c * 16 + i + 8]);
	}
	else
	{
		Oled_Set_Pos(x, y);
		for (i = 0; i < 6; i++)
			Oled_WriteData(F6x8[c][i]);
	}
}

//m^n����
uint32_t Oled_pow(uint8_t m, uint8_t n)
{
	uint32_t result = 1;
	while (n--)
		result *= m;
	return result;
}

//��ʾ2������
//x,y :�������
//len :���ֵ�λ��
//size:�����С
//mode:ģʽ	0,���ģʽ;1,����ģʽ
//num:��ֵ(0~4294967295);
void Oled_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size2)
{
	uint8_t t, temp;
	uint8_t enshow = 0;
	for (t = 0; t < len; t++)
	{
		temp = (num / Oled_pow(10, len - t - 1)) % 10;
		if (enshow == 0 && t < (len - 1))
		{
			if (temp == 0)
			{
				Oled_ShowChar(x + (size2 / 2) * t, y, ' ', size2);
				continue;
			}
			else
				enshow = 1;

		}
		Oled_ShowChar(x + (size2 / 2) * t, y, temp + '0', size2);
	}
}

//��ʾһ���ַ��Ŵ�
void Oled_ShowString(uint8_t x, uint8_t y, uint8_t *chr, uint8_t Char_Size)
{
	uint8_t j = 0;
	while (chr[j] != '\0')
	{
		Oled_ShowChar(x, y, chr[j], Char_Size);
		x += 8;
		if (x > 120)
		{
			x = 0;
			y += 2;
		}
		j++;
	}
}

//��ʾ����
void Oled_ShowCHinese(uint8_t x, uint8_t y, uint8_t no)
{
	uint8_t t, adder = 0;
	Oled_Set_Pos(x, y);
	for (t = 0; t < 16; t++)
	{
		Oled_WriteData(Hzk[2 * no][t]);
		adder += 1;
	}
	Oled_Set_Pos(x, y + 1);
	for (t = 0; t < 16; t++)
	{
		Oled_WriteData(Hzk[2 * no + 1][t]);
		adder += 1;
	}
}

/***********������������ʾ��ʾBMPͼƬ128��64��ʼ������(x,y),x�ķ�Χ0��127��yΪҳ�ķ�Χ0��7*****************/
void Oled_DrawBMP(uint8_t x0, uint8_t y0, uint8_t x1,
		uint8_t Y1, uint8_t BMP[])
{
	unsigned int j = 0;
	uint8_t x, y;

	if (Y1 % 8 == 0)
		y = Y1 / 8;
	else
		y = Y1 / 8 + 1;
	for (y = y0; y < Y1; y++)
	{
		Oled_Set_Pos(x0, y);
		for (x = x0; x < x1; x++)
		{
			Oled_WriteData(BMP[j++]);
		}
	}
}

//��ʼ��SSD1306
void Oled_Init(void)
{
	Oled_WriteCommand(0xAE); //display off
	Oled_WriteCommand(0x00); //set low column address
	Oled_WriteCommand(0x10); //set high column address
	Oled_WriteCommand(0x40); //set start line address
	Oled_WriteCommand(0xB0); //set page address
	Oled_WriteCommand(0x81); //contract control
	Oled_WriteCommand(0xFF); //128
	Oled_WriteCommand(0xA1); //set segment remap
	Oled_WriteCommand(0xA6); //normal / reverse
	Oled_WriteCommand(0xA8); //set multiplex ratio(1 to 64)
	Oled_WriteCommand(0x3F); //1/32 duty
	Oled_WriteCommand(0xC8); //Com scan direction
	Oled_WriteCommand(0xD3); //set display offset
	Oled_WriteCommand(0x00); //

	Oled_WriteCommand(0xD5); //set osc division
	Oled_WriteCommand(0x80); //

	Oled_WriteCommand(0xD8); //set area color mode off
	Oled_WriteCommand(0x05); //

	Oled_WriteCommand(0xD9); //Set Pre-Charge Period
	Oled_WriteCommand(0xF1); //

	Oled_WriteCommand(0xDA); //set com pin configuartion
	Oled_WriteCommand(0x12); //

	Oled_WriteCommand(0xDB); //set Vcomh
	Oled_WriteCommand(0x30); //

	Oled_WriteCommand(0x8D); //set charge pump enable
	Oled_WriteCommand(0x14); //

	Oled_WriteCommand(0xAF); //--turn on Oled panel

}


