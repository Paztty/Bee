#include "main_activity.h"
#include "main.h"
#include "ADS1220.h"
#include "math.h"
#include "ssd1306.h"
#include "SampleFilter.h"


#define NOS 50
// #define Fillter_order 5


extern UART_HandleTypeDef huart1;
extern SPI_HandleTypeDef hspi1;

extern ADS1220_t ADS1220;

uint32_t led_sevice_freq = 1000;
uint32_t notfication_sevice_freq = 100;
uint32_t get_adc_sevice_freq = 10;

uint32_t ADS_SPS, SSD1306_FPS, uart_fl,ADS_fl;

uint8_t bt_counter[] = {0,0,0}, stable_fl = 1, stable_mode;

float_t acc, acc_show, acc_show_pr, noload;
char notfication_text[10];
float_t can_chinh[NOS], step_size, window_value;

float Fillter_input[Fillter_order] = {1,1,1,1,1,1}, Fillter_output[Fillter_order] = {0,0,0,0,0,0};
float Fillter_HSa[Fillter_order] = {/*b = */ -2.49631017988277e-18	-0.00785585409502368,	0.0401711752634344,	-0.103314801557551,	0.170762156469434,	0.800474647839413,	0.170762156469434,	-0.103314801557551,	0.0401711752634344,	-0.00785585409502368,	-2.49631017988277e-18};

uint32_t calender[4];



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

// Sample Convert Int to String ------------------------------------------------
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


// Sample Convert Float to String ------------------------------------------------
void ftoa(float n, char* res, int afterpoint) 
	{
  if (n < 0)
	n = 0;
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

void main_calendar()
	{
		for(int i = 0; i < 4; i++)
				calender[i]++;
	}



void init_system()
{	
		ssd1306_Init();
		ssd1306_Fill(Black);
		ssd1306_SetCursor(10,32);
	  	ssd1306_WriteString("Strarting...", Font_7x10 , White);
		ssd1306_UpdateScreen();
		
		HAL_Delay(1000);
	
		ssd1306_Fill(Black);
		ssd1306_SetCursor(10,10);
	  	ssd1306_WriteString("DO NOT PUT ANY", Font_7x10 , White);
		ssd1306_SetCursor(10,25);
	  	ssd1306_WriteString("THINGS ON THE", Font_7x10 , White);
		ssd1306_SetCursor(10,40);
	  	ssd1306_WriteString("SCALE", Font_7x10 , White);
		ssd1306_UpdateScreen();
		HAL_Delay(1000);
	
		ADS1220.DRDY_PIN = GPIO_PIN_3;
		ADS1220.DRDY_PORT = GPIOA;
		ADS1220.CS_PIN = GPIO_PIN_4;
		ADS1220.CS_PORT = GPIOA;
	  	ADS1220.hspi = hspi1;

		ssd1306_Fill(Black);
		ssd1306_SetCursor(10,32);
	  	ssd1306_WriteString("Starting ....", Font_7x10 , White);
		ssd1306_UpdateScreen();
		
		ADS1220_init();

		ADS1220_set_data_rate(DR_1000SPS);
    	ADS1220_set_pga_gain(PGA_GAIN_128);
		ADS1220_select_mux_channels(MUX_AIN0_AIN1);
		ADS1220_Start();
		HAL_Delay(1000);
		
		for (int i = 0; i < 2*NOS ; i++)
		{
			get_adc(1);
			ADS_fl = 0;
			HAL_Delay(10);
		}

		for (int i = 0; i < Fillter_order; i++)
		{
			Fillter_input[i] = adc_read();
			Fillter_output[i] = Fillter_input[i];
		}
		

		noload = acc + 0.1;
		step_size = ADS1220_Calib(noload, 66.845062, 143);
					
					
		ssd1306_Fill(Black);
		ssd1306_SetCursor(10,32);
	  	ssd1306_WriteString("All done!", Font_7x10 , White);
		ssd1306_UpdateScreen();		
		HAL_Delay(1000);
		
		
		ssd1306_Fill(Black);
		ssd1306_SetCursor(5,5);
	  	ssd1306_WriteString("WEIGHT IN GAM:", Font_7x10 , White);
		ssd1306_SetCursor(2,50);
	  	ssd1306_WriteString("10th order fillter", Font_7x10 ,White );
		ssd1306_SetCursor(0,20);
		ssd1306_WriteString( "                                 ", Font_11x18, Black );
		ssd1306_UpdateScreen();			
		
		
  //Configure for differential measurement between AIN0 and AIN1
  //Start continuous conversion mode

}

void main_activity()	
{
	get_adc(calender[3]);
	notfication(calender[1]);
	led_blink(calender[0]);
	dataPerSencond(calender[2]);
}

void get_adc(uint32_t get_adc_tick)
{
uint32_t adc_value;
	
	if(get_adc_tick <= get_adc_sevice_freq && ADS_fl == 0)
	{
		
		adc_value = ADS1220_Read_WaitForData();
		
		if (adc_value != 0){

				acc = ADS1220_convertToMilliV(adc_value);
 	
			for(int i = 0; i < NOS - 1 ; i++)
				{
				can_chinh [i] = can_chinh[i+1];
				}		
				can_chinh[NOS-1] = acc;
				acc = 0;
			
			for(int i = 0; i < NOS ; i++)
				{
				acc = acc + can_chinh[i];
				}	
				acc = acc / NOS;
			}	
		
		if (stable_mode == 0){
			window_value = 1 / (6 + 2/(acc_show_pr - noload));
			if(fabs(acc - acc_show_pr) > window_value && stable_fl == 1 )	
				{
					acc_show_pr = acc;
				}
			
			if( stable_fl == 0 )
				{
					if(fabs(acc - acc_show_pr) <= 0.01 || fabs(acc - acc_show_pr) > 2)
					{
							acc_show_pr = acc;
					}
					stable_fl = 1;
				}
			//acc_show_pr = Fillter();
			}
		
		if( stable_mode == 2 )
		{
			acc_show_pr = ADS1220_convertToMilliV(adc_value);
		}
		
		if(stable_mode == 1 )
		{
			acc_show_pr = acc;
		}

		
		ADS_SPS++;
		ADS_fl = 1;
		
	}
	
	if(get_adc_tick > get_adc_sevice_freq)
	{
			ADS_fl = 0;
			calender[3] = 1;
	}
}


void led_blink (uint32_t led_tick)
{
	if (led_tick < 10)
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
	}
	
	if (led_tick > 10)
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);
	}
	if (led_tick > 500)
		calender[0] = 0;
	
}

void notfication(uint32_t notfication_tick)
{
	char data2[10];
	
	if (notfication_tick > notfication_sevice_freq)
	{
	calender[1] = 0;
  SSD1306_FPS = 0;
	}
		if (notfication_tick < notfication_sevice_freq && SSD1306_FPS == 0)
	{
		
		
		//acc_show = acc;	
		acc_show = ADS1220_ConverToGram(acc_show_pr, step_size, noload);			
		
		ssd1306_SetCursor(10,20);
		
		ftoa(acc_show, data2, 4);
		//intToStr(acc_show , data2, 5);
		
	  	ssd1306_WriteString( data2, Font_11x18 , Black);
		ssd1306_WriteString( " Gam    ", Font_11x18, Black );	

	  	ssd1306_SetCursor(2,50);
		switch (stable_mode)
		{
			case 0:
			{
				  ssd1306_WriteString("10th order fillter                    ", Font_7x10 ,White );
			}
			case 1:
			{
				  ssd1306_WriteString("2th order fillter                    ", Font_7x10 ,White );
			}
			case 2:
			{
				  ssd1306_WriteString("No Fillter                     ", Font_7x10 ,White );
			}
		}
//		ssd1306_UpdateScreen();
		
		ssd1306_UpdateScreen();
		SSD1306_FPS = 1;
	}
}

void dataPerSencond(uint32_t tick)
{
		if(tick < led_sevice_freq && uart_fl == 0)
			{
			 printf("Tick: %d ------ noload value: %f  ------  value windown: %f ---- Weight milivol: %f --- Step size: %f \r\n",HAL_GetTick(), noload ,window_value , acc, step_size);
				// stable_fl = 0;
			uart_fl = 1;
				// ADS_SPS = 0;	
				printf("\nInput array data: ");
			for (uint8_t i = 0; i < Fillter_order; i++)
			{
				printf(" %f,  ", Fillter_input[i]);
			}
			printf("\noutput array data: ");
			for (uint8_t i = 0; i < Fillter_order; i++)
			{
				printf(" %f,  ", Fillter_output[i]);
			}

			}
		if(tick > led_sevice_freq)
		{			
			calender[2] = 0;
			uart_fl = 0;
		}
}
	
float adc_read()
{
	uint32_t adc_value = ADS1220_Read_WaitForData();
	while(adc_value == 0)
		{
			adc_value = ADS1220_Read_WaitForData();
		}
			return ADS1220_convertToMilliV(adc_value);		
}	

float Fillter()
{
	float equal;
	// update input sample
	for (uint8_t i = Fillter_order - 1 ; i > 0; i--)
	{
		Fillter_input[i] = Fillter_input[i - 1];
	}	
	Fillter_input[0] = adc_read();
	
	// butterworld fillter calculator
	equal = (Fillter_input[0] * Fillter_HSa[0]);
	for (uint8_t i = 1; i < Fillter_order; i++)
	{
		equal = equal + (Fillter_input[i] * Fillter_HSa[i]);// - (Fillter_output[i] * Fillter_HSa[0][i]);
	}
	
	//update output equal
	for (uint8_t i = Fillter_order - 1 ; i > 0; i--)
	{
		Fillter_output[i] = Fillter_output[i - 1];
	}
	Fillter_output[0] = equal;
	
	return Fillter_output[0];
}	
