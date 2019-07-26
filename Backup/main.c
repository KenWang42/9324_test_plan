/**
 * Copyright (c) 2014 - 2017, Nordic Semiconductor ASA
 * 
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 * 
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 * 
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 * 
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 * 
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

/** @file
 * @defgroup uart_e9324_main main.c
 * @{
 * @ingroup uart_9324
 * @brief main file of SX9324 testing function
 *
 * The file contains requirement set-up for each register of SX9324
 * Also, contains functions for testing procedure
 * 
 */

/* C11 std lib */
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* SDK_12.3.0 lib */
#include "app_uart.h"
#include "app_error.h"
#include "nrf_delay.h"
#include "nrf_drv_twi.h"
#include "nrf_drv_timer.h"
#include "nrf_gpiote.h"
#include "nrf_gpio.h"
#include "nrf.h"
#include "bsp.h"

/* SX9324 interface */
#include "9324_LIB.h"


/**
 * @brief Function for main application entry.
 */
/*
int main(void)
{
	uart_init();
	
	printf("Start!\r\n");
	
	twi_init();
	
	static uint8_t data_array[10];
	static uint16_t index = 0;
	while(true)
	{
		nrf_delay_us(100);
		uint8_t cr = NULL;
		while(app_uart_get(&cr) != NRF_SUCCESS);
		
		if(strcmp((char *)data_array, "reset") == 0)
		{
			printf("Reset!\r\n");
			memset(data_array, NULL, 10);
			index = 0;
			
			if(SX9324_init() == true)
				timer_set();
			else
				printf("Initial Error!\r\n");
		}
		else
		{
			data_array[index] = cr;
			index++;
		}
	}
}
*/

volatile bool  m_xfer_done = false;

int main(void){
	APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
	
	NRF_LOG_INFO("\r\nTWI sensor example\r\n");
	NRF_LOG_FLUSH();
	twi_init();
	
	while(true)
	{
		nrf_delay_ms(500);
		
		do
		{
			__WFE();
		}while(m_xfer_done == false);
		
		SX9324_read_whoami();
		NRF_LOG_FLUSH();
	}
}

/** @} */
