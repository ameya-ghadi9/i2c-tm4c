#ifndef _SYSTICK_H_
#define _SYSTICK_H_

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"


void SysTick_Init(void);
void SysTick_Wait(uint32_t delay);
void SysTick_Wait10ms(uint32_t delay);


#endif
