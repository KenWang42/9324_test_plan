#include "9324_LIB.h"



/* TWI instance */
static const nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(TWI_INSTANCE_ID);

/* Buffer for samples read */
static uint8_t m_sample[2];

/* Timer */
static const nrf_drv_timer_t Sensor_Timer = NRF_DRV_TIMER_INSTANCE(0);

static const uint8_t PSENSOR_INIT_REGISTERS[CMD_COUNT][2] = {
																								 PSENSOR_RegGnrlCtrl1,
																								 PSENSOR_RegAfeCrtl0,
																								 PSENSOR_RegAfeCrtl4,
																								 PSENSOR_RegAfeCrtl7,
																								 PSENSOR_RegAfePh0,
																								 PSENSOR_RegAfePh1,
																								 PSENSOR_RegAfePh2,
																								 PSENSOR_RegAfePh3,
																								 PSENSOR_RegAfeCrtl8,
																								 PSENSOR_RegAfeCrtl9,
																								 PESNSOR_RegProxCrtl0,
																								 PESNSOR_RegProxCrtl1,
																								 PESNSOR_RegProxCrtl2,
																								 PESNSOR_RegProxCrtl3,
																								 PESNSOR_RegProxCrtl4,
																								 PESNSOR_RegProxCrtl6,
																								 PESNSOR_RegProxCrtl7,
																								 PSENSOR_RegAdvCrtl5,
																								 PSENSOR_RegAdvCrtl10,
																								 PSENSOR_RegAdvCrtl11,
																								 PSENSOR_RegAdvCrtl12,
																								 PSENSOR_RegAdvCrtl14,
																								 PSENSOR_RegAdvCrtl15,
																								 PSENSOR_RegAdvCrtl16,
																								 PSENSOR_RegAdvCrtl17,
																								 PSENSOR_RegAdvCrtl18,
																								 PSENSOR_RegIrqMsk,
																								 //PSENSOR_RegIrqCfg1,
																								 //PSENSOR_RegIrqCfg2
																								};

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
 * @brief Initialize of SX9324
 */
bool SX9324_init()
{
	ret_code_t err_code;
	int count = 0;
	int time_out_count = 2; // 1 sec
	
	m_xfer_done = false;
	for(int i = 0; i < CMD_COUNT; i++)
	{
		err_code = nrf_drv_twi_tx(&m_twi, PSENSOR_ADDR, PSENSOR_INIT_REGISTERS[i], sizeof(PSENSOR_INIT_REGISTERS[i]), false);
		APP_ERROR_CHECK(err_code);
		while(m_xfer_done == false)
		{
			count++;
			nrf_delay_ms(500);
			
			if(count >= time_out_count) // Timeout
			{
				NRF_LOG_INFO("\r\nSX9324 Timeout\r\n");
	      NRF_LOG_FLUSH();
				return false;
			}
		}
		nrf_delay_us(100);
	}
	nrf_delay_us(100);
	
	return true;
}


/**
 * @brief read value from PSENSOR_RegDIffVal
 */
void SX9324_read_DiffValue()
{
	m_xfer_done = false;
	ret_code_t err_code;
	
	uint8_t reg = (PSENSOR_RegDIffVal);
	err_code = nrf_drv_twi_tx(&m_twi, PSENSOR_ADDR, &reg, sizeof(reg), false);
	APP_ERROR_CHECK(err_code);
	while (m_xfer_done == false);
	nrf_delay_us(100);
	
	err_code = nrf_drv_twi_rx(&m_twi, PSENSOR_ADDR, m_sample, sizeof(m_sample));
	
}


/**
 * @brief Handler for timer events.
 */
void timer_event_handler(nrf_timer_event_t event_type, void* p_context)
{
    //static uint32_t i;
   // uint32_t led_to_invert = (1 << leds_list[(i++) % LEDS_NUMBER]);

    switch (event_type)
    {
        case NRF_TIMER_EVENT_COMPARE0:

						do
						{
							//__WFE();

						}while (m_xfer_done == false);
						
						SX9324_read_DiffValue();
		
            break;

        default:
            //Do nothing.
            break;
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
}


/**
 * @brief Timer initailize
 */
void timer_set()
{
	 uint32_t time_ms = 100; //Time(in miliseconds) between consecutive compare events.
   uint32_t time_ticks;
   uint32_t err_code = NRF_SUCCESS;
	
	 //Configure TIMER_LED for generating simple light effect - leds on board will invert his state one after the other.
    nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG;
    err_code = nrf_drv_timer_init(&Sensor_Timer, &timer_cfg, timer_event_handler);
    APP_ERROR_CHECK(err_code);

    time_ticks = nrf_drv_timer_ms_to_ticks(&Sensor_Timer, time_ms);

    nrf_drv_timer_extended_compare(
         &Sensor_Timer, NRF_TIMER_CC_CHANNEL0, time_ticks, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, true);

    nrf_drv_timer_enable(&Sensor_Timer);
}

void SX9324_read_whoami()
{
	m_xfer_done = false;
	ret_code_t err_code;
	
	uint8_t reg = PSENSOR_RegWhoAmI;
	err_code = nrf_drv_twi_tx(&m_twi, PSENSOR_ADDR, &reg,sizeof(reg), false);
	APP_ERROR_CHECK(err_code);
	while (m_xfer_done == false);
	nrf_delay_us(100);
	
	NRF_LOG_INFO("\r\nRead who am I\r\n");
	NRF_LOG_FLUSH();
	err_code = nrf_drv_twi_rx(&m_twi, PSENSOR_ADDR, m_sample, sizeof(m_sample));
  APP_ERROR_CHECK(err_code);
}
