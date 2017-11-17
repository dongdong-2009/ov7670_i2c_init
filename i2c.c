//***************************************************************************//**
//EEPROM LIB v 1.0
//******************************************************************************/
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_i2c.h"
#include "stm32f0xx_rcc.h"
#include "main.h"

#include "i2c.h"

#define I2C_TIMING                   0x00210507 //384 KHz
//#define I2C_INTERFACE_TIMING          0x10805E89 // 16 KHz

#define I2C_FLAG_TIMEOUT             ((uint32_t)0xffff) // ((uint32_t)0x1000)
#define I2C_LONG_TIMEOUT             ((uint32_t)(10 * I2C_FLAG_TIMEOUT))

#define I2C_INTERFACE                I2C1  // interface number
#define I2C_CLK                      RCC_APB1Periph_I2C1
   
#define I2C_SCL_PIN                  GPIO_Pin_9                  /* PA.09 */
#define I2C_SCL_GPIO_PORT            GPIOA                       /* GPIOA */
#define I2C_SCL_GPIO_CLK             RCC_AHBPeriph_GPIOA
#define I2C_SCL_SOURCE               GPIO_PinSource9
#define I2C_SCL_AF                   GPIO_AF_4 // for stm32f030(050 staroe nazvanie)

#define I2C_SDA_PIN                  GPIO_Pin_10                 /* PA.10 */
#define I2C_SDA_GPIO_PORT            GPIOA                       /* GPIOA */
#define I2C_SDA_GPIO_CLK             RCC_AHBPeriph_GPIOA
#define I2C_SDA_SOURCE               GPIO_PinSource10
#define I2C_SDA_AF                   GPIO_AF_4 // for stm32f030(050 staroe nazvanie)

#define CAM_I2C_ADR                  0x42  // camera ov7670

static uint32_t  I2C_Timeout = I2C_LONG_TIMEOUT;

/***************************************************************************//**
 *  @brief  I2C Configuration
 ******************************************************************************/
void i2c_init(void)
{
// for 050.
/* Configure I2C1 pins: PA9->SCL and PA10->SDA */
    
    I2C_InitTypeDef  I2C_InitStructure;
    GPIO_InitTypeDef  GPIO_InitStructure;
  
    /* Configure the I2C clock source. The clock is derived from the HSI */
    RCC_I2CCLKConfig(RCC_I2C1CLK_HSI);
    
    /* I2C_INTERFACE_SCL_GPIO_CLK and I2C_INTERFACE_SDA_GPIO_CLK Periph clock enable */
    RCC_AHBPeriphClockCmd(I2C_SCL_GPIO_CLK | I2C_SDA_GPIO_CLK, ENABLE);
  
    /* I2C_INTERFACE Periph clock enable */
    RCC_APB1PeriphClockCmd(I2C_CLK, ENABLE);
  
    /* Connect PXx to I2C_SCL*/
    GPIO_PinAFConfig(I2C_SCL_GPIO_PORT, I2C_SCL_SOURCE, I2C_SCL_AF);
  
    /* Connect PXx to I2C_SDA*/
    GPIO_PinAFConfig(I2C_SDA_GPIO_PORT, I2C_SDA_SOURCE, I2C_SDA_AF);
  
    /* GPIO configuration */  
    /* Configure I2C_INTERFACE pins: SCL */
    GPIO_InitStructure.GPIO_Pin = I2C_SCL_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(I2C_SCL_GPIO_PORT, &GPIO_InitStructure);
  
    /* Configure I2C_INTERFACE pins: SDA */
    GPIO_InitStructure.GPIO_Pin = I2C_SDA_PIN;
    GPIO_Init(I2C_SDA_GPIO_PORT, &GPIO_InitStructure);

  
    /* I2C configuration */
    /* I2C_INTERFACE configuration */
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_AnalogFilter = I2C_AnalogFilter_Enable;
    I2C_InitStructure.I2C_DigitalFilter = 0x00;
    I2C_InitStructure.I2C_OwnAddress1 = 0x00;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_Timing = I2C_TIMING;
  
    /* Apply I2C_INTERFACE configuration after enabling it */
    I2C_Init(I2C_INTERFACE, &I2C_InitStructure);
   
    /* I2C_INTERFACE Peripheral Enable */
    I2C_Cmd(I2C_INTERFACE, ENABLE);
}

//*******************************************************************8
// write byte 
//***************************************************************
uint8_t I2C_ByteWrite(uint8_t WriteAddr, uint8_t val)
{
    
    /* Configure slave address, nbytes, reload and generate start */
    I2C_TransferHandling(I2C_INTERFACE, CAM_I2C_ADR, 2, I2C_AutoEnd_Mode, I2C_Generate_Start_Write);
  
    /* Wait until TXIS flag is set */
    I2C_Timeout = I2C_LONG_TIMEOUT;  
    while(I2C_GetFlagStatus(I2C_INTERFACE, I2C_ISR_TXIS) == RESET)
    {
        if((I2C_Timeout--) == 0) return I2C_TIME_OUT;
    }
  
    /* Send memory address */
    I2C_SendData(I2C_INTERFACE, WriteAddr);
  
    /* Wait until TCR flag is set */
    I2C_Timeout = I2C_LONG_TIMEOUT;
    while(I2C_GetFlagStatus(I2C_INTERFACE, I2C_ISR_TXIS) == RESET)
    {
        if((I2C_Timeout--) == 0) return I2C_TIME_OUT;
    }
  
    /* Send memory data */
    I2C_SendData(I2C_INTERFACE, val);
  
    /* Wait until STOPF flag is set */
    I2C_Timeout = I2C_LONG_TIMEOUT;
    while(I2C_GetFlagStatus(I2C_INTERFACE, I2C_ISR_STOPF) == RESET){
        if((I2C_Timeout--) == 0) return I2C_TIME_OUT;
    }   
  
    /* Clear STOPF flag */
    I2C_ClearFlag(I2C_INTERFACE, I2C_ICR_STOPCF);
    
    return I2C_OK;    
}

//*********************************************************************************
// read byte
//*********************************************************************************
uint8_t I2C_ByteRead( uint8_t ReadAddr, uint8_t *val)
{
    uint8_t tmp = 0;

    //--------------------------------------------------------------------------
    // WRITE
    
    /* Configure slave address, nbytes, reload and generate start */
    I2C_TransferHandling(I2C_INTERFACE, CAM_I2C_ADR, 1, I2C_AutoEnd_Mode, I2C_Generate_Start_Write);
  
    /* Wait until TXIS flag is set */
    I2C_Timeout = I2C_LONG_TIMEOUT;
    while(I2C_GetFlagStatus(I2C_INTERFACE, I2C_ISR_TXIS) == RESET)
    {
      if((I2C_Timeout--) == 0) return I2C_TIME_OUT;
    }
  
    /* Send memory address */
    I2C_SendData(I2C_INTERFACE, ReadAddr);
    
    /* Wait until STOPF flag is set */
    I2C_Timeout = I2C_LONG_TIMEOUT;
    while(I2C_GetFlagStatus(I2C_INTERFACE, I2C_ISR_STOPF) == RESET)
    {
      if((I2C_Timeout--) == 0) return I2C_TIME_OUT;
    }   

    /* Clear STOPF flag */
    I2C_ClearFlag(I2C_INTERFACE, I2C_ICR_STOPCF);

    //--------------------------------------------------------------------------
    // READ
    
    /* Configure slave address, end mode and start condition */
    I2C_TransferHandling(I2C_INTERFACE, CAM_I2C_ADR, 1, I2C_AutoEnd_Mode, I2C_Generate_Start_Read);
    
    /* Wait until RXNE flag is set */
    I2C_Timeout = I2C_LONG_TIMEOUT;
    while(I2C_GetFlagStatus(I2C_INTERFACE, I2C_ISR_RXNE) == RESET)
    {
        if((I2C_Timeout--) == 0) return I2C_TIME_OUT;
    }
        
    /* Read data from RXDR */
    tmp = I2C_ReceiveData(I2C_INTERFACE);
   
    /* Wait until STOPF flag is set */
    I2C_Timeout = I2C_LONG_TIMEOUT;
    while(I2C_GetFlagStatus(I2C_INTERFACE, I2C_ISR_STOPF) == RESET)
    {
      if((I2C_Timeout--) == 0) return I2C_TIME_OUT;
    }
  
    /* Clear STOPF flag */
    I2C_ClearFlag(I2C_INTERFACE, I2C_ICR_STOPCF);
    
    *val = tmp;
    return I2C_OK;    
}
//*******************************************************************************
//*****************************************************************************




