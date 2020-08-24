/*
Author: Ameya Ghadi
*/

#ifndef LIB_I2C_TRIAL_H_
#define LIB_I2C_TRIAL_H_

#include <stdint.h>
#include <stdbool.h>

#include "inc/tm4c123gh6pm.h"
#include "inc/hw_i2c.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"


void InitI2C0(void);
void Who_Am_I(void);

#endif
