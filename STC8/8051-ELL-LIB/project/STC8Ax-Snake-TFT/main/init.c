/*-----------------------------------------------------------------------
|                            FILE DESCRIPTION                           |
-----------------------------------------------------------------------*/
/*----------------------------------------------------------------------
  - File name     : init.c
  - Author        : zeweni
  - Update date   : 2020.01.11
  -	Copyright(C)  : 2020-2021 zeweni. All rights reserved.
-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------
|                               INCLUDES                                |
-----------------------------------------------------------------------*/
#include "main.h"
/*-----------------------------------------------------------------------
|                                 DATA                                  |
-----------------------------------------------------------------------*/

/*--------------------------------------------------------
| @Description: MCU peripheral initialization function   |
--------------------------------------------------------*/

static void STC8x_SYSCLK_Config(void);
static void STC8x_UART_Config(void);
static void STC8x_GPIO_Config(void);
static void STC8x_TIMER_Config(void);
static void STC8x_SPI_Config(void);


/*-----------------------------------------------------------------------
|                               FUNCTION                                |
-----------------------------------------------------------------------*/

/**
  * @name    STC8x_System_Init
  * @brief   MCU initialization function
  * @param   None
  * @return  None
***/
void STC8x_System_Init(void)
{
	DELAY_POS(); /* Power on stability delay */	
	STC8x_SYSCLK_Config(); /* Initialize system clock */
	delay_init();
	
	STC8x_GPIO_Config();
	STC8x_UART_Config();
	STC8x_TIMER_Config();
	STC8x_SPI_Config();


	/*
		Add hardware driver initialization code here.
	*/
	
	
	NVIC_GLOBAL_ENABLE();
}

/**
  * @name    STC8x_SYSCLK_Config
  * @brief   MCU SYSCLK initialization function
  * @param   None
  * @return  None
***/
static void STC8x_SYSCLK_Config(void)
{
	SYSCLK_InitType SYSCLK_InitStruct={0}; /* Declare structure */

	SYSCLK_InitStruct.MCLKSrc = XOSC;
	SYSCLK_InitStruct.SCLKDiv = 0; /* if SCLKDiv = 0, Not output */
	SYSCLK_InitStruct.SCLKOutPin = SCLK_OUT_P16;
	SYSCLK_Init(&SYSCLK_InitStruct);
}



/**
  * @name    STC8x_GPIO_Config
  * @brief   MCU GPIO initialization function
  * @param   None
  * @return  None
***/
static void STC8x_GPIO_Config(void)
{
	// GPIO_MODE_WEAK_PULL(GPIO_P0,Pin_All);
	// GPIO_MODE_WEAK_PULL(GPIO_P1,Pin_All);
	GPIO_MODE_WEAK_PULL(GPIO_P2,Pin_All);
	// GPIO_MODE_WEAK_PULL(GPIO_P3,Pin_All);
	// GPIO_MODE_WEAK_PULL(GPIO_P4,Pin_All);
	// GPIO_MODE_WEAK_PULL(GPIO_P5,Pin_All);

	// LED
	GPIO_MODE_OUT_PP(GPIO_P5,Pin_5);

	RST_P54_ENABLE();
	
    /* Run lamp */
	//GPIO_MODE_OUT_PP(GPIO_P2,Pin_0);  //P20
	
}

/**
  * @name    STC8x_TIMER_Config
  * @brief   MCU TIMER initialization function
  * @param   None
  * @return  None
***/
static void STC8x_TIMER_Config(void)
{
	TIMER_InitType TIMER_InitStruct={0};
	
	TIMER_InitStruct.Mode = TIMER_16BitAutoReload;
	TIMER_InitStruct.Time = 1000;     //1ms
	TIMER_InitStruct.Run = ENABLE;
	TIMER0_Init(&TIMER_InitStruct);
	NVIC_TIMER0_Init(NVIC_PR0,ENABLE);
}

/**
  * @name    STC8x_UART_Config
  * @brief   MCU UART initialization function
  * @param   None
  * @return  None
***/
static void STC8x_UART_Config(void)
{
	UART_InitType UART_InitStruct = {0};
	
	/* UART1 TXD */
	GPIO_MODE_OUT_PP(GPIO_P3,Pin_1);
	/* UART1 RXD */
	GPIO_MODE_IN_FLOATING(GPIO_P3,Pin_0); 

	UART_InitStruct.Mode = UART_8bit_BRTx;
	UART_InitStruct.BRTGen = UART_BRT_TIM1;
	UART_InitStruct.BRTMode = UART_BRT_1T;
	UART_InitStruct.BaudRate = 115200;
	UART_InitStruct.RxEnable = ENABLE;
	UART1_Init(&UART_InitStruct);
	NVIC_UART1_Init(NVIC_PR0,ENABLE);
}

static void STC8x_SPI_Config(void)
{
	SPIInit_Type SPI_InitStruct = {0};

	// 推挽输出。 CS / SDA / SCL / RST / DC
	GPIO_MODE_OUT_PP(GPIO_P1, Pin_2 | Pin_3 | Pin_5 | Pin_0 | Pin_1);
	
	// 设置映射，默认为映射1
	//       	   映射1	映射2	 映射3	 映射4
	//   CS(SS) 	P12		P22		P74		P35
	//   SDA(MOSI)	P13		P23		P75		P34
	//   XXX(MISO)	P14		P24		P76		P33
	//   SCL(SCLK)	P15		P25		P77		P32
	//GPIO_SPI_SWPort(SW_Port1);
	
	SPI_InitStruct.Type = SPI_Type_Master;
	SPI_InitStruct.ClkSrc = SPI_SCLK_DIV_4;
	SPI_InitStruct.Mode = SPI_Mode_0; // Mode0: CPOL=0 / CPHA=0   Mode2: CPOL=1 / CPHA=0
	SPI_InitStruct.Tran = SPI_Tran_MSB;
	SPI_InitStruct.Run = ENABLE;
	SPI_Init(&SPI_InitStruct);
}

/*-----------------------------------------------------------------------
|                   END OF FLIE.  (C) COPYRIGHT zeweni                  |
-----------------------------------------------------------------------*/
