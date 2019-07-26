// C11 std lib
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// SDK_12.3.0 lib
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

/* UART */
#define MAX_TEST_DATA_BYTES     (15U)                /**< max number of test bytes to be used for tx and rx. */
#define UART_TX_BUF_SIZE 256                         /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE 256                         /**< UART RX buffer size. */

/* TWI */
#define PSENSOR_SCL_PIN             6    // SCL signal pin
#define PSENSOR_SDA_PIN             5    // SDA signal pin
#define PSENSOR_ADDR          	    0x28U

/* GPIO */
#define LED_WHITE      3
#define LED_ORANGEE    4

/* TWI Instance ID */
#define TWI_INSTANCE_ID 0

/* Register*/
#define PSENSOR_RegDIffVal 0x65
#define PSENSOR_RegWhoAmI  0xFA

/* Initial Register Setting */
// The value come form the GUI
#define PSENSOR_RegGnrlCtrl1       {0x11, 0x26}
#define PSENSOR_RegAfeCrtl0        {0x20, 0x20}
#define PSENSOR_RegAfeCrtl4        {0x24, 0x47}
#define PSENSOR_RegAfeCrtl7        {0x27, 0x47}
#define PSENSOR_RegAfePh0          {0x28, 0x37}
#define PSENSOR_RegAfePh1          {0x29, 0x37} // CS1 
#define PSENSOR_RegAfePh2          {0x2A, 0x1F} // CS2
#define PSENSOR_RegAfePh3          {0x2B, 0x3D}
#define PSENSOR_RegAfeCrtl8        {0x2C, 0x12}
#define PSENSOR_RegAfeCrtl9        {0x2D, 0x08}
#define PESNSOR_RegProxCrtl0       {0x30, 0x0A}
#define PESNSOR_RegProxCrtl1       {0x31, 0x12}
#define PESNSOR_RegProxCrtl2       {0x32, 0x20}
#define PESNSOR_RegProxCrtl3       {0x33, 0x20}
#define PESNSOR_RegProxCrtl4       {0x34, 0x0C}
#define PESNSOR_RegProxCrtl6       {0x36, 0x14}
#define PESNSOR_RegProxCrtl7       {0x37, 0x14}
#define PSENSOR_RegAdvCrtl5        {0x45, 0x05}
#define PSENSOR_RegAdvCrtl10       {0x4A, 0x33}
#define PSENSOR_RegAdvCrtl11       {0x4B, 0x33}
#define PSENSOR_RegAdvCrtl12       {0x4C, 0x33}
#define PSENSOR_RegAdvCrtl14       {0x4E, 0x80}
#define PSENSOR_RegAdvCrtl15       {0x4F, 0x0C}
#define PSENSOR_RegAdvCrtl16       {0x50, 0x14}
#define PSENSOR_RegAdvCrtl17       {0x51, 0x70}
#define PSENSOR_RegAdvCrtl18       {0x52, 0x20}
#define PSENSOR_RegIrqMsk          {0x05, 0x6F}
//#define PSENSOR_RegIrqCfg1         {0x07, 0x80}
//#define PSENSOR_RegIrqCfg2         {0x08, 0x01}

#define CMD_COUNT 27

/*Indicate if opreation on TWI has ended*/
extern volatile bool  m_xfer_done;



/**
 * @brief Discern if the uart event is belong to the error category
 * @param[in] p_event    ptr of uart Event
 */
void uart_error_handle(app_uart_evt_t * p_event);

/**
 * @brief Initialize of SX9324 by writing certain value in the registers
 * @return true if SX9324 does not time out
 * @return false if SX9324 time out
 */
bool SX9324_init(void);		

/**
 * @brief read the value of Psensor and write it to rx of UART
 */
void SX9324_read_DiffValue(void);	
/**
 * @brief Handler for timer events.
 * @param[in] event_type    instance of timer event																						
 */
void timer_event_handler(nrf_timer_event_t event_type, void* p_context);

/**
 * @brief TWI events handler.
 * @param[in] p_event    ptr of instance of twi event
 */
void twi_handler(nrf_drv_twi_evt_t const * p_event, void * p_context);

/**
 * @brief UART initailize
 */
void uart_init(void);

/**
 * @brief TWI initailize
 */
void twi_init (void);

/**
 * @brief Timer initailize
 */
void timer_set(void);

/**
 * @brief Timer initailize
 */
 void SX9324_read_whoami(void);
