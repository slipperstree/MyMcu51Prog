/*-----------------------------------------------------------------------
|                            FILE DESCRIPTION                           |
-----------------------------------------------------------------------*/
/*----------------------------------------------------------------------
  - File name     : STC8x_SPI.c
  - Author        : zeweni
  - Update date   : 2020.02.06
  -	Copyright(C)  : 2020-2021 zeweni. All rights reserved.
-----------------------------------------------------------------------*/
/*------------------------------------------------------------------------
|                            COPYRIGHT NOTICE                            |
------------------------------------------------------------------------*/
/*
 * Copyright (C) 2021, zeweni (17870070675@163.com)

 * This file is part of 8051 ELL low-layer libraries.

 * 8051 ELL low-layer libraries is free software: you can redistribute 
 * it and/or modify it under the terms of the Apache-2.0 License.

 * 8051 ELL low-layer libraries is distributed in the hope that it will 
 * be useful,but WITHOUT ANY WARRANTY; without even the implied warranty 
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
 * Apache-2.0 License for more details.

 * You should have received a copy of the Apache-2.0 License.8051 ELL 
 * low-layer libraries. If not, see <http://www.apache.org/licenses/>.
**/
/*-----------------------------------------------------------------------
|                               INCLUDES                                |
-----------------------------------------------------------------------*/
#include "STC8x_SPI.h"

/*-----------------------------------------------------------------------
|                                 DATA                                  |
-----------------------------------------------------------------------*/
/* None */
/*-----------------------------------------------------------------------
|                               FUNCTION                                |
-----------------------------------------------------------------------*/
#if (PER_LIB_SPI_CTRL == 1)
	
	#if (PER_LIB_SPI_INIT_CTRL == 1)
	
		/**
		 * @brief     SPI初始化函数。
		 * @details   SPI initialization function. 
		 * @param[in] spix  SPI初始化结构体句柄，你需要定义它，并其地址传参。  
		 *            you need to definean associated initialization handle,
		 *            And pass it by its address.  
		 * @return    FSC_SUCCESS 返回成功。Return to success.
		 * @return    FSC_FAIL    返回失败。Return to fail.
		**/
		FSCSTATE SPI_Init(const SPIInit_Type* spix)
		{
			SPDAT = 0;
			SPSTAT = SPIF | WCOL;

			// B7=SSIG:SS 引脚功能控制位
			//    0:SS 引脚确定器件是主机还是从机
			//    1:忽略 SS 引脚功能，使用 MSTR 确定器件是主机还是从机
			// B4=MSTR:器件主/从模式选择位
			// 	  设置主机模式:
			//       若 SSIG=0，则 SS 管脚必须为高电平且设置 MSTR 为 1
			//       若 SSIG=1，则只需要设置 MSTR 为 1(忽略 SS 管脚的电平) 
			//	  设置从机模式:
			//       若 SSIG=0，则 SS 管脚必须为低电平(与 MSTR 位无关)
			//       若 SSIG=1，则只需要设置 MSTR 为 0(忽略 SS 管脚的电平)
			// CPHA:SPI 时钟相位控制
			//    0:数据 SS 管脚为低电平驱动第一位数据并在 SCLK 的后时钟沿改变数据，前时钟沿采样数据(必 须 SSIG=0)
			//    1:数据在 SCLK 的前时钟沿驱动，后时钟沿采样
			SPCTL = (SPCTL & 0x6F) | (spix -> Type);  		//0x6F=01101111
			SPCTL = (SPCTL & 0xFC) | (spix -> ClkSrc);		//0xFC=11111100
			SPCTL = (SPCTL & 0xF3) | (spix -> Mode << 2);	//0xF3=11110011
			SPCTL = (SPCTL & 0xDF) | (spix -> Tran << 5);	//0xDF=11011111
			SPCTL = (SPCTL & 0xBF) | (spix -> Run << 6);  	//0xBF=10111111
			return FSC_SUCCESS;
		}
		
	#endif
		
	#if (PER_LIB_SPI_WORK_CTRL == 1)
		
		/**
		 * @brief     SPI发送数据（一个字节）函数。
		 * @details   SPI send data function. 
		 * @param[in] dat   要发送的数据。 data of SPI.
		 * @return    FSC_SUCCESS 返回成功。Return to success.
		 * @return    FSC_FAIL    返回失败。Return to fail.
		**/
		FSCSTATE SPI_Send_Data(uint8_t dat)
		{
			SPDAT = dat;			//Data register assignment
			while (!(SPSTAT & 0x80));  //Query completion flag
			SPSTAT = 0xC0;         //Clear interrupt flag
			return FSC_SUCCESS;
		}


		/**
		 * @brief     SPI接收数据（一个字节）函数。
		 * @details   SPI receive data function. 
		 * @param     None.
		 * @return    [uint8_t] 接收的数据。 receive data. 
		**/
		uint8_t SPI_Receive_Data(void)
		{
			while (!(SPSTAT & 0x80));  //Query completion flag
			SPSTAT = 0xC0;         //Clear interrupt flag
			return SPDAT;  //Data register assignment
		}


		/**
		 * @brief     SPI中断初始化函数。
		 * @details   SPI init NVIC function.  
		 * @param[in] pri 中断优先级。interrupt priority.
		 * @param[in] run 使能控制位。enable control. 
		 * @return    FSC_SUCCESS 返回成功。Return to success.
		 * @return    FSC_FAIL    返回失败。Return to fail.
		**/
		FSCSTATE NVIC_SPI_Init(NVICPri_Type pri,BOOL run)
		{
			NVIC_SPI_PRI(pri);
			IE2 = (IE2 & 0xFD) | (run << 1);
			return FSC_SUCCESS;
		}


		/**
		 * @brief     SPI切换复用IO函数。
		 * @details   SPI switch out port control function.  
		 * @param[in] port 复用IO枚举体。IO switch enumerator.
		 * @return    FSC_SUCCESS 返回成功。Return to success.
		 * @return    FSC_FAIL    返回失败。Return to fail.
		**/
		FSCSTATE GPIO_SPI_SWPort(GPIOSWPort_Type port)
		{
			P_SW1 = (P_SW1 & 0xF3) | (port << 2);
			return FSC_FAIL;
		}
		
	#endif
		
#endif
/*-----------------------------------------------------------------------
|                   END OF FLIE.  (C) COPYRIGHT zeweni                  |
-----------------------------------------------------------------------*/



