
//骑远飞电子科技 QYF-NanoRiceF4开发板
//作者：正点原子 @ALIENTEK 
//改编：H.Q

#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h" 
#include "timer.h" 
#include "ov2640.h" 
#include "dcmi.h" 
#include "sram.h"	

u16 Parallel_Data_Buffer1[38400] __attribute__((at(0X68000000)));//测试用数组
u16 Parallel_Data_Buffer2[38400] __attribute__((at(0X68000000)));//测试用数组
u16 display[76800] __attribute__((at(0X68000000)));//显存，由4端dma数据组成

int zhencount=0;
volatile u32 jpeg_data_len=0; 			//buf中的JPEG有效数据长度 
 
//RGB565测试
//RGB数据直接显示在LCD上面
void rgb565_test(void)
{ 
	LCD_Clear(WHITE);
  POINT_COLOR=RED; 
	LCD_ShowString(30,50,200,16,16,"QiYuanFei NanoRiceF4");	
	delay_ms(1000);
	OV2640_RGB565_Mode();	//RGB565模式
	OV2640_Special_Effects(2);
	OV2640_Light_Mode(1);
	OV2640_Auto_Exposure(4);
	My_DCMI_Init();			//DCMI配置
	DMA_ITConfig(DMA2_Stream1, DMA_IT_TC, ENABLE);
	DCMI_DMA_Init((u32)Parallel_Data_Buffer1,38400,DMA_MemoryDataSize_HalfWord,DMA_MemoryInc_Enable);//DCMI DMA配置  
	OV2640_OutSize_Set(240,160);
	LCD_Set_Window(0,0,240,320);
	LCD_WriteRAM_Prepare();     		//开始写入GRAM	 	  
	DCMI_Start(); 		//启动传输
	delay_ms(10);		   
} 
int main(void)
{ 

	FSMC_SRAM_Init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);  //初始化延时函数
	uart_init(115200);		//初始化串口波特率为115200
	LED_Init();					//初始化LED 
 	LCD_Init();					//LCD初始化  
 	POINT_COLOR=RED;//设置字体为红色 
	LCD_ShowString(30,70,200,16,16,"QiYuanFei NanoRiceF4");	
	LCD_ShowString(30,90,200,16,16,"Binocular Camera");  
  LCD_ShowString(30,90,200,16,16,"2018.09.01");	
	delay_ms(2000);
	while(OV2640_Init())//初始化OV2640
	{
		LCD_ShowString(30,130,240,16,16,"OV2640 ERR");
		delay_ms(200);
	  LCD_Fill(30,130,239,170,WHITE);
		delay_ms(200);
	}
	LCD_ShowString(30,130,200,16,16,"OV2640 OK");  
	rgb565_test();	  
	while(1)
	{
	}
	

}
