#include "SHT2x.h"
#include "delay.h"
#include<stdio.h>

SHT2x_data SHT20;

u8 SHT2x_Init(void)
{
	 u8 err;
   IIC_SHT20_Init();
   err = SHT2x_SoftReset();
	 return err;
}

u8 SHT2x_SoftReset(void) //SHT20软件复位
{
	u8 err=0;
	IIC_SHT20_Start();
  IIC_SHT20_Send_Byte(0x80);
	err = IIC_SHT20_Wait_Ack();
	IIC_SHT20_Send_Byte(0xFE);
	err = IIC_SHT20_Wait_Ack();
	IIC_SHT20_Stop();
	return err;
}
float SHT2x_GetTempPoll(void)
{
    float TEMP;
    u8 ack, tmp1, tmp2;
    u16 ST;
    u16 i=0;
    IIC_SHT20_Start();				//发送IIC开始信号
    IIC_SHT20_Send_Byte(I2C_ADR_W);			//IIC发送一个字节 
		ack = IIC_SHT20_Wait_Ack();	
		IIC_SHT20_Send_Byte(TRIG_TEMP_MEASUREMENT_POLL);
		ack = IIC_SHT20_Wait_Ack();

    do {
        delay_ms(100);               
        IIC_SHT20_Start();				//发送IIC开始信号
        IIC_SHT20_Send_Byte(I2C_ADR_R);	
			  i++;
			  ack = IIC_SHT20_Wait_Ack();
			  if(i==1000)break;
    } while(ack!=0);
		
    tmp1 = IIC_SHT20_Read_Byte(1);
    tmp2 = IIC_SHT20_Read_Byte(1);
		IIC_SHT20_Read_Byte(0);
    IIC_SHT20_Stop();
    
    ST = (tmp1 << 8) | (tmp2 << 0);
	
    ST &= ~0x0003;
    TEMP = ((float)ST * 0.00268127) - 46.85;

    return (TEMP);	  
}

float SHT2x_GetHumiPoll(void)
{
    float HUMI;
    u8 ack, tmp1, tmp2;
    u16 SRH;
		u16 i=0;
    
    IIC_SHT20_Start();			          	//发送IIC开始信号
    IIC_SHT20_Send_Byte(I2C_ADR_W);			//IIC发送一个字节 
		ack = IIC_SHT20_Wait_Ack();	
		IIC_SHT20_Send_Byte(TRIG_HUMI_MEASUREMENT_POLL);
		ack = IIC_SHT20_Wait_Ack();  
  
    do {
       delay_ms(100);               
        IIC_SHT20_Start();				//发送IIC开始信号
        IIC_SHT20_Send_Byte(I2C_ADR_R);	
			  i++;
			  ack = IIC_SHT20_Wait_Ack();
			  if(i==100)break;
    } while(ack!=0);
    
    tmp1 = IIC_SHT20_Read_Byte(1);
   
    tmp2 = IIC_SHT20_Read_Byte(1);
    IIC_SHT20_Read_Byte(0);
    IIC_SHT20_Stop();
    
    SRH = (tmp1 << 8) | (tmp2 << 0);
    SRH &= ~0x0003;
    HUMI = ((float)SRH * 0.00190735) - 6;

    return (HUMI);
}

//初始化IIC
void IIC_SHT20_Init(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;
  //先使能外设IO PORTC时钟 
	 RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT ;   //输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
 
	IIC_SCL=1;
	IIC_SDA=1;

}

void SDA_SHT20_OUT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; //输出模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void SDA_SHT20_IN(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; //输入模式
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}


//产生IIC起始信号
void IIC_SHT20_Start(void)
{
	SDA_SHT20_OUT();     //sda线输出
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	delay_us(4);
 	IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC_SCL=0;//钳住I2C总线，准备发送或接收数据 
	delay_us(4);
}	  
//产生IIC停止信号
void IIC_SHT20_Stop(void)
{
	SDA_SHT20_OUT();//sda线输出
	IIC_SCL=0;
	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC_SCL=1; 
		delay_us(1);
	IIC_SDA=1;//发送I2C总线结束信号
	delay_us(4);							   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 IIC_SHT20_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_SHT20_IN();      //SDA设置为输入  
	IIC_SDA=1;delay_us(1);	   
	IIC_SCL=1;delay_us(1);	 
	while(READ_SDA)
	{
		ucErrTime++;
	//	delay_us(1);
		if(ucErrTime>250)
		{
			IIC_SHT20_Stop();
			return 1;
		}
	}
	IIC_SCL=0;//时钟输出0 	   
	return 0;  
} 
//产生ACK应答
void IIC_SHT20_Ack(void)
{
	IIC_SCL=0;
	SDA_SHT20_OUT();
	IIC_SDA=0;
	delay_us(20);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}
//不产生ACK应答		    
void IIC_SHT20_NAck(void)
{
	IIC_SCL=0;
	SDA_SHT20_OUT();
	IIC_SDA=1;
	delay_us(5);
	IIC_SCL=1;
	delay_us(5);
	IIC_SCL=0;
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void IIC_SHT20_Send_Byte(u8 txd)
{                        
    u8 t;   
	 SDA_SHT20_OUT(); 	    
    IIC_SCL=0;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        IIC_SDA=(txd&0x80)>>7;
        txd<<=1; 	  
		delay_us(5);   //对TEA5767这三个延时都是必须的
		IIC_SCL=1;
		delay_us(5); 
		IIC_SCL=0;	
		delay_us(5);
    }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 IIC_SHT20_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_SHT20_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
    IIC_SCL=0; 
    delay_us(5);
		IIC_SCL=1;
        receive<<=1;
        if(READ_SDA)receive++;   
		delay_us(5); 
    }					 
    if (!ack)
        IIC_SHT20_NAck();//发送nACK
    else
        IIC_SHT20_Ack(); //发送ACK   
    return receive;
}





























