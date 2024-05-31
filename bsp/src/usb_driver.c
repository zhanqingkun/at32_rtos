#include "usb_driver.h"

// 封装成类似 printf 的函数 todo:添加互斥锁 避免在两个任务打印时，usb发送失败。
void usb_vcp_printf(void *dev, const char *format, ...) {
    char usb_buffer[256];
    va_list args;

    va_start(args, format);

 	// 使用 vsnprintf 将格式化的字符串写入缓冲区
    int data_len = vsnprintf(usb_buffer, sizeof(usb_buffer), format, args);

    va_end(args);

    if (data_len > 0 && data_len < sizeof(usb_buffer)) {
        usb_vcp_send_data(dev, (uint8_t *)usb_buffer, data_len);
		memset(usb_buffer, 0, data_len);
    }
}



/**
  * @brief  this function handles usart2  and linecoding config.
  * @param  linecoding: linecoding value
  * @retval none
  */
void usb_usart_config( linecoding_type linecoding)
{
  usart_stop_bit_num_type usart_stop_bit;
  usart_data_bit_num_type usart_data_bit;
  usart_parity_selection_type usart_parity_select;

  /* enable the usart2 and gpio clock */
  crm_periph_clock_enable(CRM_USART2_PERIPH_CLOCK, FALSE);
  crm_periph_clock_enable(CRM_USART2_PERIPH_CLOCK, TRUE);

  /* stop bit */
  switch(linecoding.format)
  {
    case 0x0:
      usart_stop_bit = USART_STOP_1_BIT;
      break;
  /* to be used when transmitting and receiving data in smartcard mode */
    case 0x1:
      usart_stop_bit = USART_STOP_1_5_BIT;
      break;
    case 0x2:
      usart_stop_bit = USART_STOP_2_BIT;
      break;
    default :
      break;
  }

    /* parity */
  switch(linecoding.parity)
  {
    case 0x0:
      usart_parity_select = USART_PARITY_NONE;
      break;
    case 0x1:
      usart_parity_select = USART_PARITY_ODD;
      break;
    case 0x2:
      usart_parity_select = USART_PARITY_EVEN;
      break;
    /* hardware usart not support parity for mark and space  */
    case 0x3:
    case 0x4:
      break;
    default :
      break;
  }

  if(USART_PARITY_NONE == usart_parity_select)
  {
    /* data bits */
    switch(linecoding.data)
    {
      /* hardware usart not support data bits for 5/6 */
      case 0x5:
      case 0x6:
        break;
      case 0x7:
        usart_data_bit = USART_DATA_7BITS;
        break;
      case 0x8:
        usart_data_bit = USART_DATA_8BITS;
        break;
      /* hardware usart not support data bits for 16 */
      case 0x10:
        break;
      default :
        break;
    }
  }
  else
  {
    /* data bits */
    switch(linecoding.data)
    {
      /* hardware usart not support data bits for 5/6 */
      case 0x5:
      case 0x6:
        break;
      case 0x7:
        usart_data_bit = USART_DATA_8BITS;
        break;
      case 0x8:
        usart_data_bit = USART_DATA_9BITS;
        break;
      /* hardware usart not support data bits for 16 */
      case 0x10:
        break;
      default :
        break;
    }
  }

  nvic_irq_enable(USART2_IRQn, 0, 0);

  /* configure usart2 param */
  usart_init(USART2, linecoding.bitrate, usart_data_bit, usart_stop_bit);
  usart_parity_selection_config(USART2, usart_parity_select);
  usart_transmitter_enable(USART2, TRUE);
  usart_receiver_enable(USART2, TRUE);

  /* enable usart2 interrupt */
  usart_interrupt_enable(USART2, USART_RDBF_INT, TRUE);
  usart_enable(USART2, TRUE);
}

/**
  * @brief  usb 48M clock select
  * @param  clk_s:USB_CLK_HICK, USB_CLK_HEXT
  * @retval none
  */
void usb_clock48m_select(usb_clk48_s clk_s)
{
  if(clk_s == USB_CLK_HICK)
  {
    crm_usb_clock_source_select(CRM_USB_CLOCK_SOURCE_HICK);

    /* enable the acc calibration ready interrupt */
    crm_periph_clock_enable(CRM_ACC_PERIPH_CLOCK, TRUE);

    /* update the c1\c2\c3 value */
    acc_write_c1(7980);
    acc_write_c2(8000);
    acc_write_c3(8020);
#if (USB_ID == 0)
    acc_sof_select(ACC_SOF_OTG1);
#else
    acc_sof_select(ACC_SOF_OTG2);
#endif
    /* open acc calibration */
    acc_calibration_mode_enable(ACC_CAL_HICKTRIM, TRUE);
  }
  else
  {
    switch(system_core_clock)
    {
      /* 48MHz */
      case 48000000:
        crm_usb_clock_div_set(CRM_USB_DIV_1);
        break;

      /* 72MHz */
      case 72000000:
        crm_usb_clock_div_set(CRM_USB_DIV_1_5);
        break;

      /* 96MHz */
      case 96000000:
        crm_usb_clock_div_set(CRM_USB_DIV_2);
        break;

      /* 120MHz */
      case 120000000:
        crm_usb_clock_div_set(CRM_USB_DIV_2_5);
        break;

      /* 144MHz */
      case 144000000:
        crm_usb_clock_div_set(CRM_USB_DIV_3);
        break;

      /* 168MHz */
      case 168000000:
        crm_usb_clock_div_set(CRM_USB_DIV_3_5);
        break;

      /* 192MHz */
      case 192000000:
        crm_usb_clock_div_set(CRM_USB_DIV_4);
        break;

      /* 216MHz */
      case 216000000:
        crm_usb_clock_div_set(CRM_USB_DIV_4_5);
        break;

      /* 240MHz */
      case 240000000:
        crm_usb_clock_div_set(CRM_USB_DIV_5);
        break;

      /* 264MHz */
      case 264000000:
        crm_usb_clock_div_set(CRM_USB_DIV_5_5);
        break;

      /* 288MHz */
      case 288000000:
        crm_usb_clock_div_set(CRM_USB_DIV_6);
        break;

      default:
        break;

    }
  }
}

/**
  * @brief  this function config gpio.
  * @param  none
  * @retval none
  */
void usb_gpio_config(void)
{
  gpio_init_type gpio_init_struct;

  crm_periph_clock_enable(OTG_PIN_GPIO_CLOCK, TRUE);
  gpio_default_para_init(&gpio_init_struct);

  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;

  /* dp and dm */
  gpio_init_struct.gpio_pins = OTG_PIN_DP | OTG_PIN_DM;
  gpio_init(OTG_PIN_GPIO, &gpio_init_struct);

  gpio_pin_mux_config(OTG_PIN_GPIO, OTG_PIN_DP_SOURCE, OTG_PIN_MUX);
  gpio_pin_mux_config(OTG_PIN_GPIO, OTG_PIN_DM_SOURCE, OTG_PIN_MUX);

#ifdef USB_SOF_OUTPUT_ENABLE
  crm_periph_clock_enable(OTG_PIN_SOF_GPIO_CLOCK, TRUE);
  gpio_init_struct.gpio_pins = OTG_PIN_SOF;
  gpio_init(OTG_PIN_SOF_GPIO, &gpio_init_struct);
  gpio_pin_mux_config(OTG_PIN_SOF_GPIO, OTG_PIN_SOF_SOURCE, OTG_PIN_MUX);
#endif

  /* otgfs use vbus pin */
#ifndef USB_VBUS_IGNORE
  gpio_init_struct.gpio_pins = OTG_PIN_VBUS;
  gpio_init_struct.gpio_pull = GPIO_PULL_DOWN;
  gpio_pin_mux_config(OTG_PIN_GPIO, OTG_PIN_VBUS_SOURCE, OTG_PIN_MUX);
  gpio_init(OTG_PIN_GPIO, &gpio_init_struct);
#endif


}
#ifdef USB_LOW_POWER_WAKUP
/**
  * @brief  usb low power wakeup interrupt config
  * @param  none
  * @retval none
  */
void usb_low_power_wakeup_config(void)
{
  exint_init_type exint_init_struct;

  crm_periph_clock_enable(CRM_SCFG_PERIPH_CLOCK, TRUE);
  exint_default_para_init(&exint_init_struct);

  exint_init_struct.line_enable = TRUE;
  exint_init_struct.line_mode = EXINT_LINE_INTERRUPUT;
  exint_init_struct.line_select = OTG_WKUP_EXINT_LINE;
  exint_init_struct.line_polarity = EXINT_TRIGGER_RISING_EDGE;
  exint_init(&exint_init_struct);

  nvic_irq_enable(OTG_WKUP_IRQ, 0, 0);
}

/**
  * @brief  this function handles otgfs wakup interrupt.
  * @param  none
  * @retval none
  */
void OTG_WKUP_HANDLER(void)
{
  exint_flag_clear(OTG_WKUP_EXINT_LINE);
}

#endif

/**
  * @brief  usb delay millisecond function.
  * @param  ms: number of millisecond delay
  * @retval none
  */
void usb_delay_ms(uint32_t ms)
{
  /* user can define self delay function */
  delay_ms(ms);
}

