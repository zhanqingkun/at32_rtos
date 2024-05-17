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
#include "list.h"
/** @addtogroup AT32F437_periph_template
  * @{
  */

/** @addtogroup 437_LED_toggle LED_toggle
  * @{
  */

#define DELAY                            100
#define FAST                             1
#define SLOW                             4

uint8_t g_speed = FAST;

List_t 		List_Test;
ListItem_t	List_Item1;
ListItem_t	List_Item2;
ListItem_t	List_Item3;
/**
  * @brief  main function.
  * @param  none
  * @retval none
  */
int main(void)
{
	system_clock_config();

  	at32_board_init();
	printf("This is a list test");

	/* 链表根节点初始化 */
    vListInitialise( &List_Test );

	  /* 节点1初始化 */
    vListInitialiseItem( &List_Item1 );
    List_Item1.xItemValue = 1;
    
    /* 节点2初始化 */    
    vListInitialiseItem( &List_Item2 );
    List_Item2.xItemValue = 2;
    
    /* 节点3初始化 */
    vListInitialiseItem( &List_Item3 );
    List_Item3.xItemValue = 3;
    
    /* 将节点插入链表，按照升序排列 */
    vListInsert( &List_Test, &List_Item2 );
    vListInsert( &List_Test, &List_Item1 );
    vListInsert( &List_Test, &List_Item3 );    

	while(1)
  	{
		at32_led_toggle(LED2);
		delay_ms(g_speed * DELAY);
		at32_led_toggle(LED3);
		delay_ms(g_speed * DELAY);
		at32_led_toggle(LED4);
		delay_ms(g_speed * DELAY);
  	}
}

/**
  * @}
  */

/**
  * @}
  */
