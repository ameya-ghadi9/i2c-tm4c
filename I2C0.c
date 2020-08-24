// I2C0.c
// Runs on LM4F120/TM4C123
// Provide a function that initializes, sends, and receives the I2C0 module
// interfaced with an HMC6352 compass or TMP102 thermometer.
// Daniel Valvano
// May 3, 2015

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2015
   Section 8.6.4 Programs 8.5, 8.6 and 8.7

 Copyright 2015 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

// I2C0SCL connected to PB2 and to pin 4 of HMC6352 compass or pin 3 of TMP102 thermometer
// I2C0SDA connected to PB3 and to pin 3 of HMC6352 compass or pin 2 of TMP102 thermometer
// SCL and SDA lines pulled to +3.3 V with 10 k resistors (part of breakout module)
// ADD0 pin of TMP102 thermometer connected to GND
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "UART.h"

#define I2C_MCS_ACK             0x00000008  // Data Acknowledge Enable
#define I2C_MCS_DATACK          0x00000008  // Acknowledge Data
#define I2C_MCS_ADRACK          0x00000004  // Acknowledge Address
#define I2C_MCS_STOP            0x00000004  // Generate STOP
#define I2C_MCS_START           0x00000002  // Generate START
#define I2C_MCS_ERROR           0x00000002  // Error
#define I2C_MCS_RUN             0x00000001  // I2C Master Enable
#define I2C_MCS_BUSY            0x00000001  // I2C Busy
#define I2C_MCR_MFE             0x00000010  // I2C Master Function Enable

#define MAXRETRIES              5           // number of receive attempts before giving up
void I2C_Init(void){
  SYSCTL_RCGCI2C_R |= 0x0001;           // enable and provide clock to I2C0
  SYSCTL_RCGCGPIO_R |= 0x0002;          // enable and provide clock to I2C0
  while((SYSCTL_PRGPIO_R&0x0002) == 0){};// ready?

  GPIO_PORTB_AFSEL_R |= (3<<2);           //enable alt funct on PB2,3
  GPIO_PORTB_ODR_R |= (1<<3);             //enable open drain on PB3 only- SDA
  GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R&0xFFFF00FF)+0x00003300;
  I2C0_MCR_R = I2C_MCR_MFE;      // master function enable
  I2C0_MTPR_R = 39;              //configure for 100 kbps clock
  // calculated from datasheet-> PLL bus frequency = 80MHz
}

uint32_t I2C_Set_Slave_Address_Mode(int8_t slave_addr)
{
	 while(I2C0_MCS_R&I2C_MCS_BUSY){};// wait for I2C ready
		 
	I2C0_MSA_R = slave_addr << 1;    // MSA[7:1] is slave address
	I2C0_MSA_R &= ~(1<<0);					//for transmit, 0th bit = 0
		 
   I2C0_MCS_R |= 7;// generate start/restart  // master enable
				
  while(I2C0_MCS_R&I2C_MCS_BUSY){};// wait for transmission done
                                          // return error bits
	//UART_OutUHex(I2C0_MCS_R&(I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR));
  return (I2C0_MCS_R&(I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR));
}

uint32_t I2C_Set_Sub_Address_Mode(int8_t sub_reg_addr)
{
	while(I2C0_MCS_R&I2C_MCS_BUSY){};// wait for I2C ready
	I2C0_MSA_R = sub_reg_addr;
  I2C0_MCS_R = (0	| I2C_MCS_STOP		// generate start/restart
                       | I2C_MCS_RUN);    // master enable
				
  while(I2C0_MCS_R&I2C_MCS_BUSY){};// wait for transmission done
                                          // return error bits
  return (I2C0_MCS_R&(I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR));
}

uint16_t I2C_Read(int8_t slave_address)
{
	while(I2C0_MCS_R&I2C_MCS_BUSY){};// wait for I2C ready
	  uint8_t data1,data2;
  int retryCounter = 1;
  do{
    while(I2C0_MCS_R&I2C_MCS_BUSY){};// wait for I2C ready
    I2C0_MSA_R = slave_address << 1;    // MSA[7:1] is slave address
    I2C0_MSA_R |= 0x01;              // MSA[0] is 1 for receive
    I2C0_MCS_R = (0
                         | I2C_MCS_ACK      // positive data ack
                         | I2C_MCS_START    // generate start/restart
                         | I2C_MCS_RUN);    // master enable
    while(I2C0_MCS_R&I2C_MCS_BUSY){};// wait for transmission done
    data1 = (I2C0_MDR_R&0xFF);       // MSB data sent first
    I2C0_MCS_R = (0
                         | I2C_MCS_STOP     // generate stop
                         | I2C_MCS_RUN);    // master enable
		I2C0_MCS_R &= ~(I2C_MCS_ACK);
    while(I2C0_MCS_R&I2C_MCS_BUSY){};// wait for transmission done
    data2 = (I2C0_MDR_R&0xFF);       // LSB data sent last
    retryCounter = retryCounter + 1;        // increment retry counter
	}                                         // repeat if error
  while(((I2C0_MCS_R&(I2C_MCS_ADRACK|I2C_MCS_ERROR)) != 0) && (retryCounter <= MAXRETRIES));
  return (data1<<8)+data2;                  // usually returns 0xFFFF on error
	
}


