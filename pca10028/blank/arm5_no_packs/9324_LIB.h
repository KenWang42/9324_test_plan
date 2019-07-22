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

/* Timer */
const nrf_drv_timer_t Sensor_Timer = NRF_DRV_TIMER_INSTANCE(0);

/* TWI */
#define PSENSOR_SCL_PIN             6    // SCL signal pin
#define PSENSOR_SDA_PIN             5    // SDA signal pin
#define PSENSOR_ADDR          	    0x28U

/* TWI Instance ID */
#define TWI_INSTANCE_ID 0

/* Register*/
#define PSENSOR_RegDIffVal 0x35