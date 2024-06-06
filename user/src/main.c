/**
  **************************************************************************
  * @file     main.c
  * @brief    main program
  **************************************************************************
  *                       Copyright notice & Disclaimer
  *
  * The software Board Support Package (BSP) that is made available to
  * download from Artery official website is the copyrighted work of Artery.
  * Artery authorizes customers to use, copy, and distribute the BSP
  * software and its related documentation for the purpose of design and
  * development in conjunction with Artery microcontrollers. Use of the
  * software is governed by this copyright notice and the following disclaimer.
  *
  * THIS SOFTWARE IS PROVIDED ON "AS IS" BASIS WITHOUT WARRANTIES,
  * GUARANTEES OR REPRESENTATIONS OF ANY KIND. ARTERY EXPRESSLY DISCLAIMS,
  * TO THE FULLEST EXTENT PERMITTED BY LAW, ALL EXPRESS, IMPLIED OR
  * STATUTORY OR OTHER WARRANTIES, GUARANTEES OR REPRESENTATIONS,
  * INCLUDING BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.
  *
  **************************************************************************
  */

#include "at32f435_437_board.h"
#include "at32f435_437_clock.h"
#include "cm_backtrace.h"
#include "usb_driver.h"
#include "FreeRTOS.h"
#include "task.h"
#include "list.h"

/* usb global struct define */
#define HARDWARE_VERSION               "V1.0.0"
#define SOFTWARE_VERSION               "V0.1.0"

otg_core_type otg_core_struct;
extern linecoding_type linecoding;

TaskHandle_t Task1_Handle;
TaskHandle_t Task2_Handle;

void Task1_Entry( void *p_arg );
void Task2_Entry( void *p_arg );

void fault_test_by_div0(void) {
    volatile int * SCB_CCR = (volatile int *) 0xE000ED14; // SCB->CCR
    int x, y, z;

    *SCB_CCR |= (1 << 4); /* bit4: DIV_0_TRP. */

    x = 10;
    y = 0;
    z = x / y;
}

/**
  * @brief  main function.
  * @param  none
  * @retval none
  */
int main(void)
{
	nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);

	system_clock_config();

  	at32_board_init();

	/* hardware usart config: usart2 */
	usb_usart_config(linecoding);

	/* usb gpio config */
	usb_gpio_config();

	/* enable otgfs clock */
	crm_periph_clock_enable(OTG_CLOCK, TRUE);

	/* select usb 48m clcok source */
	usb_clock48m_select(USB_CLK_HEXT);

	/* enable otgfs irq */
	nvic_irq_enable(OTG_IRQ, 0, 0);

	/* init usb */
	usbd_init(&otg_core_struct,
				USB_FULL_SPEED_CORE_ID,
				USB_ID,
				&cdc_class_handler,
				&cdc_desc_handler);

	delay_ms(1000);

	printf("Hardware Init success\n");

	/* CmBacktrace initialize */
    cm_backtrace_init("CmBacktrace", HARDWARE_VERSION, SOFTWARE_VERSION);
	
	fault_test_by_div0();


	while(1)
  	{
  	}
}


/**
  * @brief  this function handles otgfs interrupt.
  * @param  none
  * @retval none
  */
void OTG_IRQ_HANDLER(void)
{
  	usbd_irq_handler(&otg_core_struct);
}

