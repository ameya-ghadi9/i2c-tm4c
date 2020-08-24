/*
Author: Ameya Ghadi
*/

#include "Accelerometer.h"
#include "Lib_i2c_trial.h"

//ACCELEROMETER RESOLUTION = 2/32768
//Full scale range +/- 2g i.e. 4g corresponds to 2^16 bits
//2g corresponds to 2^15 bits
//To avoid converting this value into a floating point and thus involving floating point math
//Scale up resolution by mulitplying by 10^7
//((2/32768)*10^7) = 610.35 = 610

#define A_RES										((float)2/32768)	//Resolution
#define A_RES_INVERSE						16384							//To add/subtract 1g value from Az reading
#define A_RES_SCALED						(int16_t)(610)		//To be used if avoiding floating point math
//static int16_t data[3];

static uint8_t CTRL_REG6_XL = 0;	//Accelerometer Config

int8_t data_msb[3] = {0,0,0};
uint8_t data_lsb[3] = {0,0,0};

int16_t Accelero_Raw[3] = {0,0,0};
float Accelero_Calibrated[3] = {0,0,0};

void Power_Up_Accelerometer(void)
{
	/***************************************************************/

	uint32_t data;
		
	data = (I2CMasterDataGet(I2C0_BASE));
	I2CMasterSlaveAddrSet(I2C0_BASE, 0x6B, false);
	
	I2CMasterDataPut(I2C0_BASE, 0x20);
	I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);
	//wait for MCU to finish transaction
	while(I2CMasterBusy(I2C0_BASE));	

	I2CMasterDataPut(I2C0_BASE, 0xC0/*Get_Accelero_Config()*/); //init data
	I2CMasterControl(I2C0_BASE,I2C_MASTER_CMD_BURST_SEND_FINISH);
	while(I2CMasterBusy(I2C0_BASE));// wait for transmission done
	while(I2C0_MCS_R&(I2C_MCS_BUSY|I2C_MCS_ADRACK|I2C_MCS_DATACK)){}
}

void Read_Accelerometer_Raw(void)
{

	while(I2CMasterBusy(I2C0_BASE));
	
	I2CMasterSlaveAddrSet(I2C0_BASE, 0x6B, false); //ST,(SAD+W)
	I2CMasterDataPut(I2C0_BASE,0x28); //SUB
	I2CMasterControl(I2C0_BASE,(I2C_MASTER_CMD_BURST_SEND_START));	
	while(I2C0_MCS_R&(I2C_MCS_BUSY|I2C_MCS_ADRACK|I2C_MCS_DATACK)){};// wait for transmission done
	//and address ack from slave
		
	I2CMasterSlaveAddrSet(I2C0_BASE, 0x6B, true);//SR,(SAD+R)		
	I2CMasterControl(I2C0_BASE,I2C_MASTER_CMD_BURST_RECEIVE_START);
		
	while(I2C0_MCS_R&(I2C_MCS_BUSY|I2C_MCS_ADRACK)){};// wait for transmission done		
	while(I2CMasterBusy(I2C0_BASE));
	data_lsb[0] = (I2CMasterDataGet(I2C0_BASE));	//Ax LSB
	
	I2CMasterControl(I2C0_BASE,I2C_MASTER_CMD_BURST_RECEIVE_CONT);	
	while(I2CMasterBusy(I2C0_BASE));
	data_msb[0] = (I2CMasterDataGet(I2C0_BASE));	//Ax MSB
	
	I2CMasterControl(I2C0_BASE,I2C_MASTER_CMD_BURST_RECEIVE_CONT);	
	while(I2CMasterBusy(I2C0_BASE));
	data_lsb[1] = (I2CMasterDataGet(I2C0_BASE));	//Ay LSB
				
	I2CMasterControl(I2C0_BASE,I2C_MASTER_CMD_BURST_RECEIVE_CONT);	
	while(I2CMasterBusy(I2C0_BASE));
	data_msb[1] = (I2CMasterDataGet(I2C0_BASE));	//Ay MSB

	I2CMasterControl(I2C0_BASE,I2C_MASTER_CMD_BURST_RECEIVE_CONT);	
	while(I2CMasterBusy(I2C0_BASE));
	data_lsb[2] = (I2CMasterDataGet(I2C0_BASE));	//Az LSB
	
	I2CMasterControl(I2C0_BASE,I2C_MASTER_CMD_BURST_SEND_FINISH);		
	while(I2CMasterBusy(I2C0_BASE));
	data_msb[2] = (I2CMasterDataGet(I2C0_BASE));	//Az MSB		
			
	//read accelerometer
	Accelero_Raw[0] = (data_msb[0]<<(int8_t)8)|data_lsb[0];		//Ax
	Accelero_Raw[1] = (data_msb[1]<<(int8_t)8)|data_lsb[1];		//Ay
	Accelero_Raw[2] = ((data_msb[2]<<(int8_t)8)|data_lsb[2]) + A_RES_INVERSE; //Az
	
}


void Read_Accelerometer_Calibrated_Ax(void)
{
	Accelero_Calibrated[0] = (A_RES*Accelero_Raw[0]);
	//return (Accelero_Calibrated[0]);
}

void Read_Accelerometer_Calibrated_Ay(void)
{
	Accelero_Calibrated[1] = (A_RES*Accelero_Raw[1]);
	//return (Accelero_Calibrated[1]);
}

void Read_Accelerometer_Calibrated_Az(void)
{
	Accelero_Calibrated[2] = (A_RES*Accelero_Raw[2]);
	//return (Accelero_Calibrated[2]);
}

