#ifndef INIT_HARDWARE_H_
#define INIT_HARDWARE_H_
     
#include <stdint.h>
#include "stm32f0xx.h"


typedef enum {
    LED_R = 0,
    LED_G = 1
} led_t;

//------------------------------------------------------------------------------

#define LEDn	2

#define LEDR_PIN                         GPIO_Pin_0
#define LEDR_GPIO_PORT                   GPIOA
#define LEDR_GPIO_CLK                    RCC_AHBPeriph_GPIOA

#define LEDG_PIN                         GPIO_Pin_1
#define LEDG_GPIO_PORT                   GPIOA
#define LEDG_GPIO_CLK                    RCC_AHBPeriph_GPIOA

// reset camera ov7670 rst active = LOW
#define CAM_RST_PIN                      GPIO_Pin_5
#define CAM_RST_GPIO_PORT                GPIOA
#define CAM_RST_GPIO_CLK                 RCC_AHBPeriph_GPIOA



void init_hardware(void);
void led_init( led_t _led );
void led_on( led_t _led );
void led_off( led_t _led );
void led_toggle( led_t _led );
void cam_rst_init( void );
void cam_rst_low( void );
void cam_rst_high( void );

#endif
