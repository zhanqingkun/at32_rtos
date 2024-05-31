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
#include "usb_conf.h"
#include "usb_core.h"
#include "usbd_int.h"
#include "cdc_class.h"
#include "cdc_desc.h"

/* usb global struct define */
otg_core_type otg_core_struct;
extern linecoding_type linecoding;
extern List_t pxReadyTasksLists[ configMAX_PRIORITIES ];
/*
*************************************************************************
*                        任务控制块 & STACK 
*************************************************************************
*/
TaskHandle_t Task1_Handle;
#define TASK1_STACK_SIZE                    20
StackType_t Task1Stack[TASK1_STACK_SIZE];
TCB_t Task1TCB;

TaskHandle_t Task2_Handle;
#define TASK2_STACK_SIZE                    20
StackType_t Task2Stack[TASK2_STACK_SIZE];
TCB_t Task2TCB;


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
    /* 初始化与任务相关的列表，如就绪列表 */
    prvInitialiseTaskLists();

    /* 创建任务 */
    Task1_Handle = xTaskCreateStatic( (TaskFunction_t)Task1_Entry,   /* 任务入口 */
					                  (char *)"Task1",               /* 任务名称，字符串形式 */
					                  (uint32_t)TASK1_STACK_SIZE ,   /* 任务栈大小，单位为字 */
					                  (void *) NULL,                 /* 任务形参 */
					                  (StackType_t *)Task1Stack,     /* 任务栈起始地址 */
					                  (TCB_t *)&Task1TCB );          /* 任务控制块 */
    /* 将任务添加到就绪列表 */                                 
    vListInsertEnd( &( pxReadyTasksLists[1] ), &( ((TCB_t *)(&Task1TCB))->xStateListItem ) );
                                
    Task2_Handle = xTaskCreateStatic( (TaskFunction_t)Task2_Entry,   /* 任务入口 */
					                  (char *)"Task2",               /* 任务名称，字符串形式 */
					                  (uint32_t)TASK2_STACK_SIZE ,   /* 任务栈大小，单位为字 */
					                  (void *) NULL,                 /* 任务形参 */
					                  (StackType_t *)Task2Stack,     /* 任务栈起始地址 */
					                  (TCB_t *)&Task2TCB );          /* 任务控制块 */
    /* 将任务添加到就绪列表 */                                 
    vListInsertEnd( &( pxReadyTasksLists[2] ), &( ((TCB_t *)(&Task2TCB))->xStateListItem ) );
                                      
    /* 启动调度器，开始多任务调度，启动成功则不返回 */
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
		at32_led_toggle(LED4);
		usb_vcp_printf(&otg_core_struct.dev, "Task1 running\n");
		vTaskDelay(500);
	}
}

/* 任务2 */
void Task2_Entry( void *p_arg )
{
	for( ;; )
	{
		at32_led_toggle(LED3);
//		usb_vcp_printf(&otg_core_struct.dev, "Task2 running\n");
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

