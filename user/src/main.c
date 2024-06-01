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
#include "usb_driver.h"
#include "FreeRTOS.h"
#include "task.h"
#include "list.h"

/* usb global struct define */
otg_core_type otg_core_struct;
extern linecoding_type linecoding;
/*
*************************************************************************
*                        任务控制块 & STACK 
*************************************************************************
*/
TaskHandle_t Task1_Handle;
TaskHandle_t Task2_Handle;

void Task1_Entry( void *p_arg );
void Task2_Entry( void *p_arg );

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

	usb_vcp_printf(&otg_core_struct.dev, "Hardware Init success\n");

	/* enter critical */
  	taskENTER_CRITICAL();

	 /* create task1 */
  	if (xTaskCreate((TaskFunction_t )Task1_Entry, 
					(const char*    )"Task1",
					(uint16_t  		)512,
					(void*          )NULL,
					(UBaseType_t    )2,
					(TaskHandle_t*  )&Task1_Handle) != pdPASS) {
		usb_vcp_printf(&otg_core_struct.dev,"Task1 could not be created as there was insufficient heap memory remaining.\r\n");
	} else {
		usb_vcp_printf(&otg_core_struct.dev,"Task1 was created successfully.\r\n");
	}

	/* create task2 */
  	if (xTaskCreate((TaskFunction_t )Task2_Entry, 
					(const char*    )"Task2",
					(uint16_t  		)512,
					(void*          )NULL,
					(UBaseType_t    )2,
					(TaskHandle_t*  )&Task2_Handle) != pdPASS) {
		usb_vcp_printf(&otg_core_struct.dev,"Task2 could not be created as there was insufficient heap memory remaining.\r\n");
	} else {
		usb_vcp_printf(&otg_core_struct.dev,"Task2 was created successfully.\r\n");
	}
	/* exit critical */
	taskEXIT_CRITICAL();

	/* start scheduler */
	vTaskStartScheduler();

	while(1)
  	{
  	}
}

/* 任务1 */
void Task1_Entry( void *p_arg )
{
	for( ;; )
	{
		at32_led_toggle(LED3);
		usb_vcp_printf(&otg_core_struct.dev, "Task1 running\n");
		vTaskDelay(500);
	}
}

/* 任务2 */
void Task2_Entry( void *p_arg )
{
	for( ;; )
	{
		at32_led_toggle(LED4);
		vTaskDelay(500);
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

