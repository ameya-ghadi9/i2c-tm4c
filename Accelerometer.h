/*
Author: Ameya Ghadi
*/

#ifndef _ACCELEROMETER_H_
#define _ACCELEROMETER_H_

#include <stdint.h>
#include <stdbool.h>

#define ODR_XL	6
#define FS_XL	0
#define BW_SCAL_ODR 0
#define BW_XL	0

extern int8_t data_msb[3];
extern uint8_t data_lsb[3];
extern int16_t Accelero_Raw[3];
extern float Accelero_Calibrated[3];

//void Set_Accelero_Config(void);
//uint8_t Get_Accelero_Config(void);

void Power_Up_Accelerometer(void);

void Read_Accelerometer_Raw(void);

//float Read_Accelerometer_Calibrated_Ax(void);
//float Read_Accelerometer_Calibrated_Ay(void);
//float Read_Accelerometer_Calibrated_Az(void);

void Read_Accelerometer_Calibrated_Ax(void);
void Read_Accelerometer_Calibrated_Ay(void);
void Read_Accelerometer_Calibrated_Az(void);

#endif
