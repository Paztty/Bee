
/**
  ******************************************************************************
  * @file    combros_peripheral_spi.c
  * @author  Combros
  * @version 1.0
  * @date    8/31/2019 9:33:33 PM
  * @brief   $brief$     
  ******************************************************************************
  */
	
/* Includes ------------------------------------------------------------------*/
#include "ADS1220.h"
#include "stm32f1xx_hal.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define 	SPI_TIMEOUT			1000
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

	uint8_t m_config_reg0;
	uint8_t m_config_reg1;
	uint8_t m_config_reg2;
	uint8_t m_config_reg3;

  unsigned char  Config_Reg0;
  unsigned char  Config_Reg1;
  unsigned char  Config_Reg2;
  unsigned char  Config_Reg3;
	uint8_t	NewDataAvailable;
	
  ADS1220_t ADS1220;
	
	extern UART_HandleTypeDef huart1;
	extern SPI_HandleTypeDef hspi1;


/* Private function prototypes -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void print_db()
{
	uint8_t m[] = "ADS1220 init \r\n";
	HAL_UART_Transmit(&huart1, m , sizeof(m), 100);
}


	
uint8_t SPI_TransferData(uint8_t data)
		{
	unsigned char writeCommand[1];
	unsigned char readValue[1];
	writeCommand[0] = data;
	HAL_SPI_TransmitReceive(&hspi1, (uint8_t*)&writeCommand, (uint8_t*)&readValue, 1, 100);
	return readValue[0];
	}
		


	
	
uint8_t ADS1220_readRegister(uint8_t address)
	{
  uint8_t data;
	address = RREG|(address<<2);
		
	HAL_GPIO_WritePin(ADS1220.CS_PORT, ADS1220.CS_PIN, LOW);	
	HAL_Delay(5);
	SPI_TransferData(address);	
  data = SPI_TransferData(SPI_MASTER_DUMMY);
	HAL_Delay(5);
	HAL_GPIO_WritePin(ADS1220.CS_PORT, ADS1220.CS_PIN, HIGH);
	HAL_Delay(5);
  return data;
		
	}
	
	
	
void ADS1220_writeRegister(uint8_t address, uint8_t value)
	{
	address = WREG|(address<<2);
		
	HAL_GPIO_WritePin(ADS1220.CS_PORT, ADS1220.CS_PIN, LOW);
	HAL_Delay(5);
	SPI_TransferData(address);
	SPI_TransferData(value);    
	HAL_Delay(5);		
	HAL_GPIO_WritePin(ADS1220.CS_PORT, ADS1220.CS_PIN, HIGH);
	HAL_Delay(5);
	}	
	
	
void ADS1220_Command(uint8_t data_in)
	{
		HAL_GPIO_WritePin(ADS1220.CS_PORT, ADS1220.CS_PIN, LOW);
		HAL_Delay(2);
		HAL_GPIO_WritePin(ADS1220.CS_PORT, ADS1220.CS_PIN, HIGH);
		HAL_Delay(2);
		HAL_GPIO_WritePin(ADS1220.CS_PORT, ADS1220.CS_PIN, LOW);
		HAL_Delay(2);
		SPI_TransferData(data_in);
		HAL_Delay(2);
		HAL_GPIO_WritePin(ADS1220.CS_PORT, ADS1220.CS_PIN, HIGH);
	}

void ADS1220_Reset()
	{
		ADS1220_Command(RESET);
	}
	
	
void ADS1220_Start()
	{
		ADS1220_Command(START);
	}
 
	
	
void ADS1220_init()
	{
  print_db();
	NewDataAvailable = 0;
		
  /*SPI.transfer(WREG);           //WREG command (43h, 08h, 04h, 10h, and 00h)
  SPI.transfer(0x01);      	
  SPI.transfer(0x04);     
  SPI.transfer(0x10);    
  SPI.transfer(0x00);   
  */
    m_config_reg0 = 0x00;   //Default settings: AINP=AIN0, AINN=AIN1, Gain 1, PGA enabled
    m_config_reg1 = 0x04;   //Default settings: DR=20 SPS, Mode=Normal, Conv mode=continuous, Temp Sensor disabled, Current Source off
    m_config_reg2 = 0x10;   //Default settings: Vref internal, 50/60Hz rejection, power open, IDAC off
    m_config_reg3 = 0x00;   //Default settings: IDAC1 disabled, IDAC2 disabled, DRDY pin only

	 ADS1220_writeRegister( CONFIG_REG0_ADDRESS , m_config_reg0);
   ADS1220_writeRegister( CONFIG_REG1_ADDRESS , m_config_reg1);
   ADS1220_writeRegister( CONFIG_REG2_ADDRESS , m_config_reg2);
   ADS1220_writeRegister( CONFIG_REG3_ADDRESS , m_config_reg3);
	printf("Config_Reg :\r\n 0x%x\r\n", m_config_reg0);	
	printf(" 0x%x\r\n", m_config_reg1);	
	printf(" 0x%x\r\n", m_config_reg2);	
	printf(" 0x%x\r\n", m_config_reg3);		
  HAL_Delay(10);
  /*
  SPI.transfer(RREG);           //RREG
  data = SPI.transfer(SPI_MASTER_DUMMY);
  //Serial.println(data);
  data = SPI.transfer(SPI_MASTER_DUMMY); 
  //Serial.println(data);
  data = SPI.transfer(SPI_MASTER_DUMMY); 
  //Serial.println(data);
  data = SPI.transfer(SPI_MASTER_DUMMY); 
  //Serial.println(data);
  */
 //print_db();
  Config_Reg0 = ADS1220_readRegister(CONFIG_REG0_ADDRESS);
  Config_Reg1 = ADS1220_readRegister(CONFIG_REG1_ADDRESS);
  Config_Reg2 = ADS1220_readRegister(CONFIG_REG2_ADDRESS);
  Config_Reg3 = ADS1220_readRegister(CONFIG_REG3_ADDRESS);

	printf("Config_Reg :\r\n 0x%x\r\n", Config_Reg0);
	printf("0x%x\r\n",Config_Reg1);
	printf("0x%x\r\n",Config_Reg2);
	printf("0x%x\r\n",Config_Reg3);
	printf(" ");
  HAL_GPIO_WritePin(ADS1220.CS_PORT, ADS1220.CS_PIN,HIGH); //release chip, signal end transfer
  HAL_Delay(100);
	//print_db();
}




int32_t ADS1220_Read_WaitForData()
{
    static uint8_t SPI_Buff[3];
    int32_t mResult32=0;
    long int bit24;

		
    if(HAL_GPIO_ReadPin(ADS1220.DRDY_PORT, ADS1220.DRDY_PIN) == LOW)             //        Wait for DRDY to transition low
    {
		HAL_GPIO_WritePin(ADS1220.CS_PORT, ADS1220.CS_PIN, LOW);
		HAL_Delay(2);                       //Take CS low
  	for (int i = 0; i < 3; i++)
  	{
          SPI_Buff[i] = SPI_TransferData(SPI_MASTER_DUMMY);
  	}
		HAL_Delay(2);   
		HAL_GPIO_WritePin(ADS1220.CS_PORT, ADS1220.CS_PIN, HIGH);                //  Clear CS to high

        bit24 = SPI_Buff[0];
        bit24 = (bit24 << 8) | SPI_Buff[1];
        bit24 = (bit24 << 8) | SPI_Buff[2];                                 // Converting 3 bytes to a 24 bit int

        bit24= ( bit24 << 8 );
        mResult32 = ( bit24 >> 8 );                      // Converting 24 bit two's complement to 32 bit two's complement
    }
    return mResult32;
}

int32_t ADS1220_Read_SingleShot_SingleEnded_WaitForData(uint8_t channel_no)
{
    static uint8_t SPI_Buff[3];
    int32_t mResult32=0;
    long int bit24;

    ADS1220_select_mux_channels(channel_no);
	  HAL_Delay(100);
		ADS1220_Start();
	  HAL_Delay(100);
	
    if(HAL_GPIO_ReadPin(ADS1220.DRDY_PORT, ADS1220.DRDY_PIN) == LOW)             //        Wait for DRDY to transition low
    {
		    HAL_GPIO_WritePin(ADS1220.CS_PORT, ADS1220.CS_PIN, LOW);                        //Take CS low
				delayMicroseconds(100);
        for (int i = 0; i < 3; i++)
        {
          SPI_Buff[i] = SPI_TransferData(SPI_MASTER_DUMMY);
        }
        delayMicroseconds(100);
		    HAL_GPIO_WritePin(ADS1220.CS_PORT, ADS1220.CS_PIN, HIGH);                 //  Clear CS to high

        bit24 = SPI_Buff[0];
        bit24 = (bit24 << 8) | SPI_Buff[1];
        bit24 = (bit24 << 8) | SPI_Buff[2];                                 // Converting 3 bytes to a 24 bit int

        bit24= ( bit24 << 8 );
        mResult32 = ( bit24 >> 8);                      // Converting 24 bit two's complement to 32 bit two's complement
    }
    return mResult32;
}

void delayMicroseconds(uint32_t time)
{
        for (int i = 0; i <= time; i ++)
					{
						//__NOP;
					}
}

/////// config function
void ADS1220_PGA_ON(void)
{
    m_config_reg0 &= ~_BV(0);
    ADS1220_writeRegister(CONFIG_REG0_ADDRESS,m_config_reg0);
}

void ADS1220_PGA_OFF(void)
{
    m_config_reg0 |= _BV(0);
    ADS1220_writeRegister(CONFIG_REG0_ADDRESS,m_config_reg0);
}

void ADS1220_set_conv_mode_continuous()
{
    m_config_reg1 |= _BV(2);
    ADS1220_writeRegister(CONFIG_REG1_ADDRESS,m_config_reg1);
}

void ADS1220_set_conv_mode_single_shot()
{
    m_config_reg1 &= ~_BV(2);
    ADS1220_writeRegister(CONFIG_REG1_ADDRESS,m_config_reg1);
}

void ADS1220_set_data_rate(int datarate)
{
    m_config_reg1 &= ~REG_CONFIG1_DR_MASK;
    m_config_reg1 |= datarate;
    ADS1220_writeRegister(CONFIG_REG1_ADDRESS,m_config_reg1);
}

void ADS1220_select_mux_channels(int channels_conf)
{
    m_config_reg0 &= ~REG_CONFIG0_MUX_MASK;
    m_config_reg0 |= channels_conf;
    ADS1220_writeRegister(CONFIG_REG0_ADDRESS,m_config_reg0);
}

void ADS1220_set_pga_gain(int pgagain)
{
    m_config_reg0 &= ~REG_CONFIG0_PGA_GAIN_MASK;
    m_config_reg0 |= pgagain ;
    ADS1220_writeRegister(CONFIG_REG0_ADDRESS,m_config_reg0);
}

void ADS1220_Continuous_conversion_mode_ON()
{
  Config_Reg1 |= _BV(2);
  ADS1220_writeRegister(CONFIG_REG1_ADDRESS,Config_Reg1);
}

void ADS1220_Single_shot_mode_ON()
{
  Config_Reg1 &= ~_BV(2);
  ADS1220_writeRegister(CONFIG_REG1_ADDRESS,Config_Reg1);
}

float ADS1220_convertToMilliV(int32_t i32data)
{
    return (float)((i32data*VFSR*1000)/FULL_SCALE);
}
