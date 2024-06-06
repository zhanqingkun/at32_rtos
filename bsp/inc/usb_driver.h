#ifndef __USB_DRIVER_H
#define __USB_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif
#include "at32f435_437_board.h"
#include <stdarg.h>
#include "string.h"
#include "usb_conf.h"
#include "usb_core.h"
#include "usbd_int.h"
#include "cdc_class.h"
#include "cdc_desc.h"

void usb_usart_config(linecoding_type linecoding);
void usb_vcp_printf(void *dev, const char *format, ...);
void usb_clock48m_select(usb_clk48_s clk_s);
void usb_gpio_config(void);
void usb_low_power_wakeup_config(void);
void usb_delay_ms(uint32_t ms);

#ifdef __cplusplus
}
#endif

#endif

