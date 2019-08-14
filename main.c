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
#define LED_ORANGEE    2

/* TWI Instance ID */
#define TWI_INSTANCE_ID 0

/* Register*/
#define PSENSOR_RegDIffMsb 0x65
#define PSENSOR_RegUseMsb 0x61
#define PSENSOR_RegWhoAmI  0xFA

/*Indicate if opreation on TWI has ended*/
static volatile bool  m_xfer_done = false;

/* TWI instance */
static const nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(TWI_INSTANCE_ID);

/* Buffer for samples read */
static uint8_t  m_sample[2];
//static uint8_t  m_buffer;

/* Timer */
static const nrf_drv_timer_t Sensor_Timer = NRF_DRV_TIMER_INSTANCE(0);



/**
 * @brief UART error handle
 */
void uart_error_handle(app_uart_evt_t * p_event)
{
    if (p_event->evt_type == APP_UART_COMMUNICATION_ERROR)
    {
        APP_ERROR_HANDLER(p_event->data.error_communication);
    }
    else if (p_event->evt_type == APP_UART_FIFO_ERROR)
    {
        APP_ERROR_HANDLER(p_event->data.error_code);
    }
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
               // data_handler(m_sample);
							 int16_t value ;
							 if((m_sample[0] >> 4) == 1)
							 {
								 //printf("nnn--%x%x  ",m_sample[0],m_sample[1]);
								 int16_t nData = (((int16_t)m_sample[0] << 8) | (int16_t)m_sample[1]) & 0x0FFF;

								 // negaitive
								  value = nData - 0x0FFF - 1 ;
							 }
							 else
							 {
								 value = ((uint16_t)m_sample[0] << 8) | (uint16_t)m_sample[1];
							 }
							
							 printf("Value: %d\r\n",value);
            }
            m_xfer_done = true;
            break;
        default:
            break;
    }
}


/**
 * @brief Function for setting SX9324 register with proper value
 */

#define CMD_COUNT 50
#define PSENSOER_RegIrqSrc 0x00
uint8_t PSENSOR_INIT_REGISTERS[CMD_COUNT][2] = {
{0x10,0x0A}, {0x11,0x26}, {0x14,0x00}, {0x15,0x00}, {0x20,0x20}, {0x23,0x00}, {0x24,0x47}, {0x26,0x00}, {0x27,0x47}, {0x28,0x37}, 
{0x29,0x37}, {0x2A,0x1F}, {0x2B,0x3D}, {0x2C,0x12}, {0x2D,0x08}, {0x30,0x09}, {0x31,0x09}, {0x32,0x20}, {0x33,0x20}, {0x34,0x0C}, 
{0x35,0x00}, {0x36,0x59}, {0x37,0x59}, {0x40,0x00}, {0x41,0x00}, {0x42,0x00}, {0x43,0x00}, {0x44,0x00}, {0x45,0x05}, {0x46,0x00}, 
{0x47,0x00}, {0x48,0x00}, {0x49,0x00}, {0x4A,0x33}, {0x4B,0x31}, {0x4C,0x31}, {0x4D,0x00}, {0x4E,0x80}, {0x4F,0x0C}, {0x50,0x14}, 
{0x51,0x70}, {0x52,0x20}, {0x53,0x00}, {0x54,0x00}, {0x02,0x00}, {0x03,0x00}, {0x06,0x00}, {0x05,0x6F}, {0x07,0x80}, {0x08,0x01}
};

void SX9324_read_RegIrqSrc(void)
{
	m_xfer_done = false;
	ret_code_t err_code;
	
	uint8_t reg = PSENSOER_RegIrqSrc;
	err_code = nrf_drv_twi_tx(&m_twi, PSENSOR_ADDR, &reg, sizeof(reg), false);

	APP_ERROR_CHECK(err_code);
	while (m_xfer_done == false);
	nrf_delay_us(100);
	
	printf("\r\nRead Reg_IrqSrc \r\n");
	
	err_code = nrf_drv_twi_rx(&m_twi, PSENSOR_ADDR, m_sample, sizeof(m_sample));
  APP_ERROR_CHECK(err_code);
}

bool SX9324_init(void)
{
		ret_code_t err_code;
		int count = 0;
		int TimeOutCount = 2; // 1 sec
	  
		SX9324_read_RegIrqSrc();
	  nrf_delay_ms(500);
	  
	  m_xfer_done = false;
		for(int i =0; i < CMD_COUNT; i++)
		{
			//printf("\r\ntx reg 0x%x\r\n", PSENSOR_INIT_REGISTERS[i][0]);
			err_code = nrf_drv_twi_tx(&m_twi, PSENSOR_ADDR, PSENSOR_INIT_REGISTERS[i],sizeof(PSENSOR_INIT_REGISTERS[i]), false);
			APP_ERROR_CHECK(err_code);
			while (m_xfer_done == false)
			{
				count++;
				nrf_delay_ms(500);
				
				if(count >= TimeOutCount)
				{
				  printf("\r\nTime Out\r\n");
					return false;
				}
			}
			nrf_delay_us(100);
		}
		
		/*set bit 4 of 0x00 to 1: Calibration*/
		uint8_t CALIBRATION[2] = {0x00,0x10};
		err_code = nrf_drv_twi_tx(&m_twi, PSENSOR_ADDR, CALIBRATION,sizeof(CALIBRATION), false);
		nrf_delay_us(100);
		
		/*Set PH2 to RegDiff*/
	  uint8_t RegPhaseSel[2] = {0x60,0x02};
		err_code = nrf_drv_twi_tx(&m_twi, PSENSOR_ADDR, RegPhaseSel,sizeof(RegPhaseSel), false);
		nrf_delay_us(100);
		
		
		printf("\r\nSX9324 Initialized.\r\n");
		return true;
}

/**
 * @brief read value from PSENSOR_RegDIffVal
 */
void SX9324_read_DiffValue(void)
{
	m_xfer_done = false;
	ret_code_t err_code;
	
	uint8_t reg = PSENSOR_RegDIffMsb;
	err_code = nrf_drv_twi_tx(&m_twi, PSENSOR_ADDR, &reg, sizeof(reg), false);

	APP_ERROR_CHECK(err_code);
	while (m_xfer_done == false);
	nrf_delay_us(100);
	
	printf("\r\nRead Differential Value: \r\n");
	
	err_code = nrf_drv_twi_rx(&m_twi, PSENSOR_ADDR, m_sample, sizeof(m_sample));
  APP_ERROR_CHECK(err_code);
}

void SX9324_read_whoami()
{
	m_xfer_done = false;
	ret_code_t err_code;
	uint8_t reg = PSENSOR_RegWhoAmI;
	
	err_code = nrf_drv_twi_tx(&m_twi, PSENSOR_ADDR, &reg, sizeof(reg), false);

	APP_ERROR_CHECK(err_code);
	while (m_xfer_done == false);
	nrf_delay_us(100);
	
	printf("\r\nRead who am I\r\n");
	
	err_code = nrf_drv_twi_rx(&m_twi, PSENSOR_ADDR, m_sample, sizeof(m_sample));
  APP_ERROR_CHECK(err_code);
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
											 uart_error_handle,
											 APP_IRQ_PRIORITY_LOW,
											 err_code);

  APP_ERROR_CHECK(err_code);
  printf("\r\n UART initialized. \r\n");
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

/**
 * @brief Handler for timer events.
 */
void timer_event_handler(nrf_timer_event_t event_type, void* p_context)
{
    switch (event_type)
    {
        case NRF_TIMER_EVENT_COMPARE0:
						do
						{
						}while (m_xfer_done == false);
						SX9324_read_DiffValue();
            break;

        default:
            break;
    }
}


/**
 * @brief Timer initailize
 */
void timer_init()
{
	  uint32_t time_ms = 1000; //Time(in miliseconds) between events.
    uint32_t time_ticks;
    uint32_t err_code = NRF_SUCCESS;
	 
	  nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG;
    err_code = nrf_drv_timer_init(&Sensor_Timer, &timer_cfg, timer_event_handler);
    APP_ERROR_CHECK(err_code);

    time_ticks = nrf_drv_timer_ms_to_ticks(&Sensor_Timer, time_ms);

    nrf_drv_timer_extended_compare(
         &Sensor_Timer, NRF_TIMER_CC_CHANNEL0, time_ticks, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, true);

    nrf_drv_timer_enable(&Sensor_Timer);
	  printf("\r\nTimer Initialized.\r\n");
}


int main(void)
{
	LEDS_CONFIGURE(LEDS_MASK);
	LEDS_OFF(LEDS_MASK);

	uart_init();
	twi_init();
	timer_init();
	printf("\r\nStart Test\r\n");
	SX9324_init();
	
	nrf_gpio_cfg_output(LED_WHITE);
	nrf_gpio_pin_clear(LED_WHITE);
	nrf_gpio_cfg_output(LED_ORANGEE);
	nrf_gpio_pin_clear(LED_ORANGEE);
	while(true)
	{
		printf("\r\n Lights Switch \r\n");
		nrf_gpio_pin_toggle(LED_ORANGEE);
		nrf_delay_ms(1000);
		nrf_gpio_pin_toggle(LED_WHITE);
	}
	
}

/** @} */
