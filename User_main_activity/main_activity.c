#include "main_activity.h"
#include "main.h"
#include "ADS1220.h"
#include "math.h"
#include "ssd1306.h"
#include "SampleFilter.h"

#include "STM32F1xx"

extern UART_HandleTypeDef huart1;
extern SPI_HandleTypeDef hspi1;

extern ADS1220_t ADS1220;
uint32_t noLoad, fullLoad, step_load;

uint32_t led_sevice_freq = 1;
uint32_t notfication_sevice_freq = 500;
uint32_t get_adc_sevice_freq = 100;

SampleFilter f;
uint8_t t_filter_f = 0;
float acc;
char notfication_text[10];



// stdio with UART1 ------------------------------------------------------------
#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
  #define GETCHAR_PROTOTYPE int fgetc(FILE *f)
#endif /* __GNUC__ */
	PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
	HAL_UART_Transmit(&huart1, (uint8_t*)&ch,1,100);
  /* Loop until the end of transmission */

  return ch;
}



void reverse(char* str, int len) 
{ 
		int i = 0, j = len - 1, temp; 
		while (i < j) { 
				temp = str[i]; 
				str[i] = str[j]; 
				str[j] = temp; 
				i++; 
				j--; 
		} 
} 


int intToStr(int x, char str[], int d) 
{ 
		int i = 0; 
		while (x) { 
				str[i++] = (x % 10) + '0'; 
				x = x / 10; 
		} 
	
		// If number of digits required is more, then 
		// add 0s at the beginning 
		while (i < d) 
				str[i++] = '0'; 
	
		reverse(str, i); 
		str[i] = '\0'; 
		return i; 
} 

void ftoa(float n, char* res, int afterpoint) 
	{ 
	// Extract integer part 
	int ipart = (int)n; 

	// Extract floating part 
	float fpart = n - (float)ipart; 

	// convert integer part to string 
	int i = intToStr(ipart, res, 1); 

	// check for display option after point 
	if (afterpoint != 0)
		{ 
			res[i] = '.'; // add dot 

			// Get the value of fraction part upto given no. 
			// of points after dot. The third parameter  
			// is needed to handle cases like 233.007 
			fpart = fpart * pow(10, afterpoint); 

			intToStr((int)fpart, res + i + 1, afterpoint); 
		} 
} 
// END stdio with UART1 --------------------------------------------------------


void init_system()
{
	int c;

	ssd1306_Init();
	
	ssd1306_Fill(Black);
	ssd1306_SetCursor(10,32);
	ssd1306_WriteString("Strarting...", Font_7x10 , White);
	ssd1306_UpdateScreen();

	ADS1220.DRDY_PIN = GPIO_PIN_0;
	ADS1220.DRDY_PORT = GPIOB;
	ADS1220.CS_PIN = GPIO_PIN_4;
	ADS1220.CS_PORT = GPIOA;
	ADS1220.hspi = hspi1;

	ssd1306_Fill(Black);
	ssd1306_SetCursor(10,32);
	ssd1306_WriteString("Starting ADS....", Font_7x10 , White);
	ssd1306_UpdateScreen();

	ADS1220_init();
	
	ADS1220_set_data_rate(DR_1000SPS);
	ADS1220_set_pga_gain(PGA_GAIN_128);
	ADS1220_select_mux_channels(MUX_AIN0_AIN1);
	ADS1220_Start();
	HAL_Delay(1000);
	
	ssd1306_Fill(Black);
	ssd1306_SetCursor(10,32);
	ssd1306_WriteString("Starting Filter....", Font_7x10 , White);
	ssd1306_UpdateScreen();	
	
	SampleFilter_init(&f);
	
	ssd1306_SetCursor(10,32);
	ssd1306_WriteString("Load buffer......", Font_7x10 , White);
	ssd1306_UpdateScreen();		
		
	for(c = 0; c < SAMPLEFILTER_TAP_NUM; c++)
		{
			get_adc();
			HAL_Delay(100);
			ssd1306_SetCursor(60,43);
			ftoa((float)c/SAMPLEFILTER_TAP_NUM*100, notfication_text,1);
			ssd1306_WriteString(notfication_text, Font_7x10 , White);	
			ssd1306_WriteString(" %", Font_7x10 , White);
			ssd1306_UpdateScreen();		
		}
	
					
	HAL_Delay(100);
	ssd1306_Fill(Black);
	ssd1306_SetCursor(10,32);
	ssd1306_WriteString("All done!", Font_7x10 , White);
	ssd1306_UpdateScreen();		
	HAL_Delay(1000);
	
	ssd1306_Fill(Black);
	ssd1306_SetCursor(5,5);
	ssd1306_WriteString("Loadcell milivol:", Font_7x10 , White);
	ssd1306_SetCursor(1,50);
	ssd1306_WriteString("SPS:1000 ", Font_7x10 , White);
	//ssd1306_SetCursor(1,50);
	ssd1306_WriteString("Gain:128", Font_7x10 , White);
	ssd1306_UpdateScreen();			
		
		
  //Configure for differential measurement between AIN0 and AIN1

  //Start continuous conversion mode

}

void main_activity(uint32_t tick)	
{
	printf("RUNNING....\r\n");
	notfication(tick);
}

void get_adc()
{
	t_filter_f = 1;
	uint32_t data;
	float data2;
    data = ADS1220_Read_WaitForData();

	data2 = ADS1220_convertToMilliV(data);
	SampleFilter_put(&f, (double)data2);
	acc = SampleFilter_get(&f) + 200;
	t_filter_f = 0;
}


void led_blink (uint32_t led_tick)
{
	if (led_tick % led_sevice_freq == 0)
	{
		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
	}

}



void notfication(uint32_t notfication_tick)
{

	if(t_filter_f == 0)
		{
	char data2[10];
	if (notfication_tick % notfication_sevice_freq == 0)
	{
		ssd1306_SetCursor(30,20);
		ftoa(acc, data2, 4);
		ssd1306_WriteString( data2, Font_7x10 , White);
		ssd1306_WriteString( " mV          ", Font_7x10, White );
		ssd1306_UpdateScreen();	
	}
}
}

