#ifndef __ADS1220_H
#define __ADS1220_H

#ifdef MCU_STM32L0
#include "stm32l0xx_hal.h"
#endif
	 
#ifdef MCU_STM32L1
#include "stm32l1xx_hal.h"
#endif

#ifdef MCU_STM32L4
#include "stm32l4xx_hal.h"
#endif
	 
#ifdef MCU_STM32F1
#include "stm32f1xx_hal.h"
#endif

#ifdef MCU_STM32F2
#include "stm32f2xx_hal.h"
#endif
	 
#ifdef MCU_STM32F3
#include "stm32f3xx_hal.h"
#endif

#ifdef MCU_STM32F4
#include "stm32f4xx_hal.h"
#endif


#include "stm32f1xx_hal.h"

#define SPI_MASTER_DUMMY    0xFF
#define RESET               0x06   //Send the RESET command (06h) to make sure the ADS1220 is properly reset after power-up
#define START               0x08    //Send the START/SYNC command (08h) to start converting in continuous conversion mode
#define WREG  0x40
#define RREG  0x20

//Config registers
#define CONFIG_REG0_ADDRESS 0x00
#define CONFIG_REG1_ADDRESS 0x01
#define CONFIG_REG2_ADDRESS 0x02
#define CONFIG_REG3_ADDRESS 0x03

#define REG_CONFIG1_DR_MASK       0xE0
#define REG_CONFIG0_PGA_GAIN_MASK 0x0E
#define REG_CONFIG0_MUX_MASK      0xF0

#define DR_20SPS    0x00
#define DR_45SPS    0x20
#define DR_90SPS    0x40
#define DR_175SPS   0x60
#define DR_330SPS   0x80
#define DR_600SPS   0xA0
#define DR_1000SPS  0xC0

#define PGA_GAIN_1   0x00
#define PGA_GAIN_2   0x02
#define PGA_GAIN_4   0x04
#define PGA_GAIN_8   0x06
#define PGA_GAIN_16  0x08
#define PGA_GAIN_32  0x0A
#define PGA_GAIN_64  0x0C
#define PGA_GAIN_128 0x0E

#define MUX_AIN0_AIN1   0x00
#define MUX_AIN0_AIN2   0x10
#define MUX_AIN0_AIN3   0x20
#define MUX_AIN1_AIN2   0x30
#define MUX_AIN1_AIN3   0x40
#define MUX_AIN2_AIN3   0x50
#define MUX_AIN1_AIN0   0x60
#define MUX_AIN3_AIN2   0x70
#define MUX_AIN0_AVSS   0x80
#define MUX_AIN1_AVSS   0x90
#define MUX_AIN2_AVSS   0xA0
#define MUX_AIN3_AVSS   0xB0

#define MUX_SE_CH0      0x80
#define MUX_SE_CH1      0x90
#define MUX_SE_CH2      0xA0
#define MUX_SE_CH3      0xB0

//#define ADS1220_CS_PIN GPIO_PIN_4
//#define ADS1220_DRDY_PIN GPIO_PIN_3
//#define ADS1220_CS_PORT GPIOA
//#define ADS1220_DRDY_PORT GPIOA

#define _BV(bit) (1<<(bit))  

#define  HIGH GPIO_PIN_SET
#define  LOW 	GPIO_PIN_RESET 

#define PGA          1                 // Programmable Gain = 1
#define VREF         2.048            // Internal reference of 2.048V
#define VFSR         VREF/PGA
#define FULL_SCALE   (((long int)1<<23)-1)

/* Exported variables --------------------------------------------------------*/

	
typedef struct
{
	SPI_HandleTypeDef hspi;
	GPIO_TypeDef* CS_PORT;
	uint8_t  CS_PIN;
	GPIO_TypeDef* DRDY_PORT;
	uint8_t  DRDY_PIN;
} ADS1220_t;
	
/* Exported functions --------------------------------------------------------*/


uint8_t 		SPI_TransferData(uint8_t data);
void 			delayMicroseconds(uint32_t time);

void 			ADS1220_init(void);
void 			ADS1220_Command(unsigned char data_in);

void 			ADS1220_Reset(void);
void			ADS1220_Start(void);
uint8_t  		ADS1220_readRegister(uint8_t address);
void 			ADS1220_writeRegister(uint8_t address, uint8_t value);

void 		 	ADS1220_select_mux_channels(int channels_conf);
int32_t	 		ADS1220_Read_WaitForData(void);
int32_t  		ADS1220_Read_SingleShot_SingleEnded_WaitForData(uint8_t channel_no);

void		 	ADS1220_set_pga_gain(int pgagain);
void 		 	ADS1220_set_data_rate(int datarate);
void 		 	ADS1220_set_conv_mode_single_shot(void);
void     		ADS1220_set_conv_mode_continuous(void);

float    		ADS1220_convertToMilliV(int32_t i32data);
void    		ADS1220_Continuous_conversion_mode_ON(void);

void 		 	ADS1220_PGA_OFF(void);
void 			ADS1220_PGA_ON(void);	
	
#endif

