
#ifndef __MAIN_ACTIVITY_H
#define __MAIN_ACTIVITY_H

#ifdef __cplusplus
extern "C" {
#endif
/*Include file...............................................................................................*/
#include "stm32f1xx_hal.h"



/*End include.......................................................................................................*/

#ifdef __cplusplus
}
#endif

#define  HIGH GPIO_PIN_SET
#define  LOW 	GPIO_PIN_RESET 


#endif /* __MAIN_ACTIVITY_H */



void init_system(void);
void main_activity(uint32_t tick);

void led_blink(uint32_t led_tick);
void notfication(uint32_t notfication_tick);
void get_adc(uint32_t get_adc_tick);

