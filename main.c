#include <stdint.h>
#include <stdbool.h>

#include "driverlib/sysctl.h"

#include "PLL.h"
#include "SysTick.h"
#include "UART.h"
#include "Lib_i2c_trial.h"
#include "Accelerometer.h"

int16_t g_ax;
int16_t g_ay;
int16_t g_az;

int main(void)
{
	
		PLL_Init(Bus20MHz);
		SysTick_Init();
		UART_Init();
		InitI2C0();

		SysTick_Wait10ms(5);
		
		Who_Am_I();
		SysTick_Wait10ms(5);
		
		Power_Up_Accelerometer();
		SysTick_Wait10ms(5);
		
		while(1)
		{
			Read_Accelerometer_Raw();
			Read_Accelerometer_Calibrated_Ax();
			Read_Accelerometer_Calibrated_Ay();
			Read_Accelerometer_Calibrated_Az();
			SysTick_Wait10ms(1);
		}

}
