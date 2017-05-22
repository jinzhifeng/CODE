/*
 * iic.c
 *
 *  Created on: 2017��5��16��
 *      Author: jinzhifeng
 *      https://github.com/jinzhifeng
 */
#include "iic.h"

void IIC_Start(void)
/**
* \brief       IIC_Start
*
* \param       None
*
* \retval      None
*/
/* I2c��ʼ�źţ���SCLΪ���ڼ䣬SDA�ɸߵ��͵����䣨��SCLΪ�ߣ�SDA�½������䣩*/
{
	IIC_SdaOutPutMode(); //SDA���ģʽ
	IIC_SCL_Set();	//SCL����

	IIC_SDA_Set(); //SDA�½���
	IIC_Delay();
	IIC_SDA_Clr();
	IIC_Delay();

    IIC_SCL_Clr();	//ǯסI2C���ߣ�׼�����ͻ��߽�������(Ҳ���Բ�����,����ÿ��дbit���߶�bit֮ǰҪ������)
}

void IIC_Stop(void)
/**
* \brief       IIC_Stop
*
* \param       None
*
* \retval      None
*/
/* I2cֹͣ�źţ���SCLΪ���ڼ䣬SDA�ɵ͵��ߵ����䣨��SCLΪ�ߣ�SDA���������䣩*/
{
	IIC_SdaOutPutMode(); //SDA���ģʽ
	IIC_SCL_Set();	//SCL����

	IIC_SDA_Clr(); //SDA����������
	IIC_Delay();
	IIC_SDA_Set();
	IIC_Delay();
}


uint8_t IIC_ReadOneByte(void)
/**
* \brief       ������������һ��8λ�ֽڵ�����
*
* \param       None
*
* \retval      ReadByte
*/
{
	uint8_t i, ReadOneByte = 0;
	IIC_SdaInPutMode(); //SDA������������Ϊ����ģʽ
	for(i = 0; i != 8; i++)
	{
		IIC_SCL_Set();
		IIC_Delay();
		ReadOneByte <<= 1;
		if(IIC_ReadSdaPin()) ReadOneByte++; //ReadOneByte = ReadOneByte | 0x01;
		IIC_SCL_Clr();
    IIC_Delay();
	}
	IIC_SdaOutPutMode(); //����8λ���ݺ�λΪ���ģʽ
	return ReadOneByte;
}

IIC_ACK_Typedef IIC_WriteOneByte(uint8_t WriteOneByte)
/**
* \brief       �������豸дһ��8λ�ֽڵ�����
*
* \param       WriteOneByte
*
* \retval      IIC_AckState	(IIC_ACK������������һ���ֽڳɹ���IIC_NACK������������һ���ֽ�ʧ��)
*/
/* ������ÿ����һ���ֽڣ�����ʱ������9�ڼ��ͷ������ߣ��ɽ���������һ��Ӧ���źš�
	Ӧ���ź�Ϊ�͵�ƽʱ���涨Ϊ��ЧӦ��λ��ACK���Ӧ��λ������ʾ�������Ѿ��ɹ��ؽ����˸��ֽڣ�
  Ӧ���ź�Ϊ�ߵ�ƽʱ���涨Ϊ��Ӧ��λ��NACK����һ���ʾ���������ո��ֽ�û�гɹ���
  ���ڷ�����ЧӦ��λACK��Ҫ���ǣ��������ڵ�9��ʱ������֮ǰ�ĵ͵�ƽ�ڼ佫SDA�����ͣ�����ȷ���ڸ�ʱ�ӵĸߵ�ƽ�ڼ�Ϊ�ȶ��ĵ͵�ƽ��
  ��������������������������յ����һ���ֽں󣬷���һ��NACK�źţ���֪ͨ���ط������������ݷ��ͣ����ͷ�SDA�ߣ��Ա����ؽ���������һ��ֹͣ�ź�P
*/
{
	uint8_t i;
	IIC_ACK_Typedef IIC_AckState;
	IIC_SdaOutPutMode(); //SDA���ģʽ
	for(i = 0; i != 8; i++)
	{
		if(WriteOneByte & 0x80)
		{
			IIC_SDA_Set();
		}
		else
		{
			IIC_SDA_Clr();
		}
		IIC_Delay();
		IIC_SCL_Set();
		IIC_Delay();
		IIC_SCL_Clr(); //ÿ��ʱ�������ڼ䴫��һ������λ��SDA�ϵ�����

		WriteOneByte <<= 1; //׼����һλ���Ӹ�λ����λд��
	}

	IIC_SdaInPutMode(); //SDA����ģʽ
	IIC_Delay();
	IIC_SCL_Set();


	if(IIC_ReadSdaPin() == 0)
	{
		IIC_AckState = IIC_ACK;
//		printf("IIC_ACK���豸�������ݳɹ���\r\n");
	}
	else
	{
		IIC_AckState = IIC_NACK;
		printf("IIC_NACK���豸��������ʧ�ܣ�\r\n");
	}
	IIC_Delay();
	IIC_SCL_Clr();
	IIC_Delay();
	IIC_SdaOutPutMode(); 	//SDA���ģʽ
	return IIC_AckState;	//�����豸Ӧ��λ
}

void IIC_SdaOutPutMode(void)  //SDA_GPIO������Ϊ���ģʽ
{
	  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __GPIOF_CLK_ENABLE();
  GPIO_InitStruct.Pin = Lcd1602_sda_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
  HAL_GPIO_Init(Lcd1602_sda_GPIO_Port, &GPIO_InitStruct);
}

void IIC_SdaInPutMode(void)  //SDA_GPIO������Ϊ����ģʽ��������
{
	  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __GPIOF_CLK_ENABLE();
  GPIO_InitStruct.Pin = Lcd1602_sda_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
  HAL_GPIO_Init(Lcd1602_sda_GPIO_Port, &GPIO_InitStruct);
}

volatile void IIC_DelayXus(uint32_t DelayXus)
/**
* \brief       ��ʱ�ӳ���,��ʱDelayXus΢��
*
* \param       DelayXus
*
* \retval      None
*/
{
  uint32_t dwCurCounter = 0;
  uint32_t dwPreTickVal = SysTick->VAL;
  uint32_t dwCurTickVal;
  DelayXus *= (HAL_RCC_GetHCLKFreq() / 1000000);
  for(;;)
  {
	dwCurTickVal = SysTick -> VAL;
    if(dwCurTickVal < dwPreTickVal)
	{
      dwCurCounter = dwCurCounter + dwPreTickVal - dwCurTickVal;
    }
    else
	{
      dwCurCounter = dwCurCounter + dwPreTickVal + SysTick->LOAD - dwCurTickVal;
    }
    dwPreTickVal = dwCurTickVal;
    if(dwCurCounter >= DelayXus)
	{
      return;
    }
  }
}
