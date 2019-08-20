/* Timer */
static const nrf_drv_timer_t Sensor_Timer = NRF_DRV_TIMER_INSTANCE(0);

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
						SX9324_read_Reg(PSENSOR_RegDIffMsb);
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
	  uint32_t time_ms = 100; //Time(in miliseconds) between events.
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