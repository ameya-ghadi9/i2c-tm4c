/*
Author: Ameya Ghadi
*/

#include "Lib_i2c_trial.h"
#include "UART.h"

void InitI2C0(void)
{

	//enable I2C module-- I2C0
	SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);

	//reset I2C0 module
	SysCtlPeripheralReset(SYSCTL_PERIPH_I2C0);

	//enable GPIO peripheral that contains I2C0
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

	// Configure the pin muxing for I2C0 functions on port B2 and B3.
	GPIOPinConfigure(GPIO_PB2_I2C0SCL);
	GPIOPinConfigure(GPIO_PB3_I2C0SDA);

	// Select the I2C function for these pins.
	GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
	GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);

	// Enable and initialize the I2C0 master module.  Use the system clock for
	// the I2C0 module.  The last parameter sets the I2C data transfer rate.
	// If false the data rate is set to 100kbps and if true the data rate will
	// be set to 400kbps.
	I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), false);

	//clear I2C FIFOs
	HWREG(I2C0_BASE + I2C_O_FIFOCTL) = 80008000;
}


void Who_Am_I(void)
{
	uint32_t data;
	while(I2CMasterBusy(I2C0_BASE));
		/****Device ID*******/
	I2CMasterSlaveAddrSet(I2C0_BASE, 0x6B, false); //ST,(SAD+W)	

	I2CMasterDataPut(I2C0_BASE,0x0F); //SUB-- WHO AM I
	I2CMasterControl(I2C0_BASE,(I2C_MASTER_CMD_BURST_SEND_START));
	while(I2CMasterBusy(I2C0_BASE));// wait for transmission done

	I2CMasterSlaveAddrSet(I2C0_BASE, 0x6B, true); //ST,(SAD+R)	
	I2CMasterControl(I2C0_BASE,I2C_MASTER_CMD_SINGLE_RECEIVE);
	while(I2CMasterBusy(I2C0_BASE));// wait for transmission done
	while(I2C0_MCS_R&(I2C_MCS_BUSY|I2C_MCS_ADRACK|I2C_MCS_DATACK)){}
		
	data = (I2CMasterDataGet(I2C0_BASE));
}