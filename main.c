#include <stdint.h>
#include "hardware.h"
#include "sys_timer.h"
#include "time_hal.h"
#include "i2c.h"

#define CAM_ADR_ID_MSB  (0x0a) //ov7670
#define CAM_ID_MSB      (0x76) //ov7670

int main()
{
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f0xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f0xx.c file
     */     
    uint8_t res;
    uint8_t b;
    
    sys_timer_setup();
    init_hardware();
    
    
    res = I2C_ByteRead( CAM_ADR_ID_MSB, &b);
    if (res != I2C_OK && b != CAM_ID_MSB){
        while(1){
            led_on( LED_R );
            delay_ms(200);
            led_off( LED_R );
            delay_ms(200);
        }
    }
    
    led_on( LED_G ); // OK
    
    
//    res = I2C_ByteWrite( 0x01, 0xaa);
//    res = I2C_ByteRead( 0x01, &b);
        
    
    while(1);
}
