/** @file
 * @defgroup uart_9324_main main.c
 * @{
 * @ingroup uart_9324
 * @brief main file of SX9324 test plan
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

/* LOG */
#define NRF_LOG_MODULE_NAME "APP"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"


/* SX9324 interface */
//#include "9324_LIB.h"

/* UART */
#define MAX_TEST_DATA_BYTES     (15U)                /**< max number of test bytes to be used for tx and rx. */
#define UART_TX_BUF_SIZE 256                         /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE 256                         /**< UART RX buffer size. */

/* TWI */
#define PSENSOR_SCL_PIN             6    // SCL signal pin
#define PSENSOR_SDA_PIN             5    // SDA signal pin
#define PSENSOR_ADDR          	    0x28U

/* GPIO */
#define LED_WHITE      1
#define LED_ORANGE    2

/*TWI Instance ID*/
#define TWI_INSTANCE_ID 0

/*Register*/
#define PSENSOR_RegDIffMsb  0x65
#define PSENSOR_RegUseMsb   0x61
#define PSENSOR_RegWhoAmI   0xFA
#define PSENSOR_RegIrqSrc   0x00
#define PSENSOR_RegPhaseSel 0x60

/*Register Value*/
#define PHASE_1 0x01
#define PHASE_2 0x02

/*Indicate if opreation on TWI has ended*/
static volatile bool  m_xfer_done = false;

/* TWI instance */
static const nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(TWI_INSTANCE_ID);

/* Buffer for samples read */
static uint8_t  m_sample[2];

/*The initial Status of registers and their values. Exported from GUI*/
#define CMD_COUNT 50
uint8_t PSENSOR_INIT_REGISTERS[CMD_COUNT][2] = {
{0x10,0x0A}, {0x11,0x26}, {0x14,0x00}, {0x15,0x00}, {0x20,0x20}, {0x23,0x00}, {0x24,0x47}, {0x26,0x00}, {0x27,0x47}, {0x28,0x37}, 
{0x29,0x37}, {0x2A,0x1F}, {0x2B,0x3D}, {0x2C,0x12}, {0x2D,0x08}, {0x30,0x09}, {0x31,0x09}, {0x32,0x20}, {0x33,0x20}, {0x34,0x0C}, 
{0x35,0x00}, {0x36,0x59}, {0x37,0x59}, {0x40,0x00}, {0x41,0x00}, {0x42,0x00}, {0x43,0x00}, {0x44,0x00}, {0x45,0x05}, {0x46,0x00}, 
{0x47,0x00}, {0x48,0x00}, {0x49,0x00}, {0x4A,0x33}, {0x4B,0x31}, {0x4C,0x31}, {0x4D,0x00}, {0x4E,0x80}, {0x4F,0x0C}, {0x50,0x14}, 
{0x51,0x70}, {0x52,0x20}, {0x53,0x00}, {0x54,0x00}, {0x02,0x00}, {0x03,0x00}, {0x06,0x00}, {0x05,0x6F}, {0x07,0x80}, {0x08,0x01}
};

/*Array and its pointer to contain PSENSOR values*/
int16_t PSENSOR_DIFF_VALUE[20];
int VALUE_COUNT = 0;
bool IS_DIFF = false;

/**
 * @brief Reading Specific Register Value of SX9324
 */
void SX9324_read_Reg(int Reg)
{
	m_xfer_done = false;
	ret_code_t err_code;
	
	uint8_t reg = Reg;
	err_code = nrf_drv_twi_tx(&m_twi, PSENSOR_ADDR, &reg, sizeof(reg), false);

	APP_ERROR_CHECK(err_code);
	while (m_xfer_done == false);
	nrf_delay_us(100);
	
	err_code = nrf_drv_twi_rx(&m_twi, PSENSOR_ADDR, m_sample, sizeof(m_sample));
  APP_ERROR_CHECK(err_code);
}


/**
 * @brief Function for setting SX9324 register with proper value
 * @return True if initialized success, False if not
 */
bool SX9324_init(void)
{
		ret_code_t err_code;
	  
		// SX9324_read_RegIrqSrc();
	  SX9324_read_Reg(PSENSOR_RegIrqSrc);
	  nrf_delay_ms(10);
	  
	  m_xfer_done = false;
		for(int i =0; i < CMD_COUNT; i++)
		{
//			printf("write reg 0x%x, ", PSENSOR_INIT_REGISTERS[i][0]);
//			if(i%10==0) printf("\n\r");
			err_code = nrf_drv_twi_tx(&m_twi, PSENSOR_ADDR, PSENSOR_INIT_REGISTERS[i],sizeof(PSENSOR_INIT_REGISTERS[i]), false);
			APP_ERROR_CHECK(err_code);
			
			nrf_delay_ms(10);
		}
		
		/*set 0x0F of 0x03 to 1: Set Compensate*/
		uint8_t CALIBRATION[2] = {0x03,0x0F};
		err_code = nrf_drv_twi_tx(&m_twi, PSENSOR_ADDR, CALIBRATION,sizeof(CALIBRATION), false);
		nrf_delay_ms(10);

		printf("\r\nSX9324 Initialized.\r\n");
		return true;
}

/**
 * @brief Setting loading phase of SX9324
 */
void SX9324_set_phase(int phase){
	  ret_code_t err_code;
    /*Set PH2 to RegDiff*/
	  uint8_t RegPhaseSel[2] = {PSENSOR_RegPhaseSel,phase};
		err_code = nrf_drv_twi_tx(&m_twi, PSENSOR_ADDR, RegPhaseSel,sizeof(RegPhaseSel), false);
		APP_ERROR_CHECK(err_code);
		nrf_delay_us(100);
}


/**
 * @brief UART event handler
 * @function if input "r" or "R" on the keyboard the program will restart
 */
void uart_event_handler(app_uart_evt_t * p_event)
{
	  uint8_t cr;

	  if (p_event->evt_type == APP_UART_DATA_READY)
    {
			app_uart_get(&cr);
			
			if (cr == 'r' || cr == 'R')
			{
				NVIC_SystemReset();
			}

			if (cr == 'o' || cr == 'O')
			{
			    printf("\r\nOrange Light\r\n");
				nrf_gpio_pin_clear(LED_ORANGE);
				nrf_gpio_pin_set(LED_WHITE);
				nrf_delay_ms(100);
			}

			if (cr == 'w' || cr == 'W')
			{
			    printf("\r\nWhite Light\r\n");
				nrf_gpio_pin_clear(LED_WHITE);
				nrf_gpio_pin_set(LED_ORANGE);
				nrf_delay_ms(100);
			}

	}
    else if (p_event->evt_type == APP_UART_COMMUNICATION_ERROR)
    {
        APP_ERROR_HANDLER(p_event->data.error_communication);
    }
    else if (p_event->evt_type == APP_UART_FIFO_ERROR)
    {
        APP_ERROR_HANDLER(p_event->data.error_code);
    }
}


/**
 * @brief UART initailize
 */
void uart_init()
{
	uint32_t err_code;
	const app_uart_comm_params_t comm_params =
		{
				RX_PIN_NUMBER,
				TX_PIN_NUMBER,
				RTS_PIN_NUMBER,
				CTS_PIN_NUMBER,
				APP_UART_FLOW_CONTROL_ENABLED,
				false,
				UART_BAUDRATE_BAUDRATE_Baud115200
		};

	APP_UART_FIFO_INIT(&comm_params,
											 UART_RX_BUF_SIZE,
											 UART_TX_BUF_SIZE,
											 uart_event_handler,
											 APP_IRQ_PRIORITY_LOW,
											 err_code);

  APP_ERROR_CHECK(err_code);
  printf("\r\nUART initialized.\r\n");
}


/**
 * @brief TWI events handler.
 */
void twi_handler(nrf_drv_twi_evt_t const * p_event, void * p_context)
{
    switch (p_event->type)
    {
        case NRF_DRV_TWI_EVT_DONE:
            if (p_event->xfer_desc.type == NRF_DRV_TWI_XFER_RX)
            {
							 int16_t value ;
							 if((m_sample[0] >> 4) == 1)
							 {
								 //printf("nnn--%x%x  ",m_sample[0],m_sample[1]);
								 int16_t nData = (((int16_t)m_sample[0] << 8) | (int16_t)m_sample[1]) & 0x0FFF;

								 //negaitive
								  value = nData - 0x0FFF - 1 ;
							 }
							 else
							 {
								 value = ((uint16_t)m_sample[0] << 8) | (uint16_t)m_sample[1];
							 }
							 
							 if (IS_DIFF)
							{
							   PSENSOR_DIFF_VALUE[VALUE_COUNT] = value;
							   VALUE_COUNT += 1;
							   printf("Value: %d\r\n", value);
							}

            }
            m_xfer_done = true;
            break;
        default:
            break;
    }
}


/**
 * @brief TWI initailize
 */
void twi_init ()
{
    ret_code_t err_code;

    const nrf_drv_twi_config_t twi_psensor_config = {
       .scl                = PSENSOR_SCL_PIN,
       .sda                = PSENSOR_SDA_PIN,
       .frequency          = NRF_TWI_FREQ_400K,
       .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
       .clear_bus_init     = false
    };

    err_code = nrf_drv_twi_init(&m_twi, &twi_psensor_config, twi_handler, NULL);
    APP_ERROR_CHECK(err_code);

    nrf_drv_twi_enable(&m_twi);
	  printf("\r\nTWI Initialized\r\n");
}


int main(void)
{
	uart_init();
	twi_init();

	printf("\r\nStart Test\r\n");
	
	SX9324_init();
	/*Set up LED*/
	nrf_gpio_cfg_output(LED_WHITE);
	nrf_gpio_pin_set(LED_WHITE);
	nrf_gpio_cfg_output(LED_ORANGE);
	nrf_gpio_pin_set(LED_ORANGE);
	
	nrf_delay_ms(100);
	
	IS_DIFF = true;
	printf("\r\nRead Phase 1\r\n");
	SX9324_set_phase(PHASE_1);
	for(int i = 0; i < 10; i++){
		SX9324_read_Reg(PSENSOR_RegDIffMsb);
		nrf_delay_ms(50);
	}
	
	printf("\r\nRead Phase 2\r\n");
	SX9324_set_phase(PHASE_2);
	for(int i = 0; i < 10; i++){
		SX9324_read_Reg(PSENSOR_RegDIffMsb);
		nrf_delay_ms(50);
	}
	IS_DIFF = false;

	printf("\r\nEnd Test\r\n");
	for (int i = 0; i < 20; i++){
	  printf("%d ,", PSENSOR_DIFF_VALUE[i]);
		nrf_delay_us(100);
	}
	
	while(true)
	{
		// Do Nothing
	}
	
}
