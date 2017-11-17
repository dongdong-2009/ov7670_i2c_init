/*
 * Подпрограммы работы с аппаратурой
 *
 */

#include <stdint.h>
#include <string.h>
#include "stm32f0xx.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_rcc.h"
#include "main.h"
#include "hardware.h"
#include "sys_timer.h"
#include "i2c.h"

#ifndef DEBUG
#define DEBUG 0
#endif

  
GPIO_TypeDef* GPIO_PORT[LEDn] = {LEDR_GPIO_PORT, LEDG_GPIO_PORT};
const uint16_t GPIO_PIN[LEDn] = {LEDR_PIN, LEDG_PIN};
const uint32_t GPIO_CLK[LEDn] = {LEDR_GPIO_CLK, LEDG_GPIO_CLK};

//******************************************************************************
// Настройка портов Светодиодов
//******************************************************************************
void led_init( led_t led )
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  
//---------------------------------
  /* GPIOC Periph clock enable */
  RCC_AHBPeriphClockCmd(GPIO_CLK[ led ], ENABLE);

  /* Configure PC10 and PC11 in output pushpull mode */
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN[ led ];
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIO_PORT[ led ], &GPIO_InitStructure);

}

//******************************************************************************
// Включение светодиодов
//******************************************************************************
void led_on( led_t led)
{
  GPIO_PORT[ led ]->BSRR = GPIO_PIN[ led ];   
}

//******************************************************************************
// ВЫключение светодиода
//******************************************************************************
void led_off( led_t led)
{
  GPIO_PORT[ led ]->BRR = GPIO_PIN[ led ];   
}

//******************************************************************************
// Смена состояния светодиода
//******************************************************************************
void led_toggle( led_t led )
{
  GPIO_PORT[ led ]->ODR ^= GPIO_PIN[ led ];
}

//******************************************************************************
// Настройка портов RESET камеры
//******************************************************************************
void cam_rst_init( void )
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  
//---------------------------------
  /* GPIOC Periph clock enable */
  RCC_AHBPeriphClockCmd(CAM_RST_GPIO_CLK, ENABLE);

  /* Configure PC10 and PC11 in output pushpull mode */
  GPIO_InitStructure.GPIO_Pin = CAM_RST_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(CAM_RST_GPIO_PORT, &GPIO_InitStructure);
  
}

//******************************************************************************
// RESET = 0
//******************************************************************************
void cam_rst_low( void )
{
    CAM_RST_GPIO_PORT->BRR = CAM_RST_PIN; 
}

//******************************************************************************
// RESET = 1
//******************************************************************************
void cam_rst_high( void )
{
    CAM_RST_GPIO_PORT->BSRR = CAM_RST_PIN;
}

//******************************************************************************
// Начальная инициальзация переферии
//******************************************************************************
void init_hardware(void)
{

  	led_init( LED_R );
	led_init( LED_G );

	led_off( LED_R );
	led_off( LED_G );
    
    cam_rst_init();
    i2c_init();

    // camera reset  -\___2ms___/----300ms---  (rst active=0)
    cam_rst_low();
    delay_ms(2);
    cam_rst_high();
    delay_ms(300);
    
//    printf_d("Start IWDG(watchdog) interval 280ms.\r\n");
    
	/* IWDG timeout equal to 280 ms (the timeout may varies due to LSI frequency dispersion) */
	/* Enable write access to IWDG_PR and IWDG_RLR registers */
//	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

	/* IWDG counter clock: 40KHz(LSI) / 32 = 1.25 KHz */
//	IWDG_SetPrescaler(IWDG_Prescaler_32);

	/* Set counter reload value to 349 */
//	IWDG_SetReload(349);

	/* Reload IWDG counter */
//	IWDG_ReloadCounter();

	/* Enable IWDG (the LSI oscillator will be enabled by hardware) */
//	IWDG_Enable();
}
