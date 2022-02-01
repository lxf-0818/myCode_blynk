/* 
	ADS1115_sample.c - 12/9/2013. Written by David Purdie as part of the openlabtools initiative
	Initiates and reads a single sample from the ADS1115 (without error handling)
        Changed to cpp for blynk Leon Rosenfeld 
	https://training.ti.com/sites/default/files/docs/adcs-introduction-to-i2c-example-presentation.pdf
*/

#include <stdio.h>
#include <fcntl.h> // open
#include <inttypes.h> // uint8_t, etc
#include <linux/i2c-dev.h> // I2C bus definitions
// Added the 2 includes needed for g++ compile lxf
#include <sys/ioctl.h>
#include <unistd.h>
//
#define START_SINGLE_CONVERION 0x80 //Bit 15   Config register 
#define GAIN_1 0x02 		    //Bit 11-9 Config          4.096
#define SINGLE_SHOT_MODE 0x01       //Bit 8 
#define DISABLE_COMPARTOR 0x03      //Bit 1-0 
#define NO_MAIN 1
#include <iostream>
using namespace std;

int readADC(float results[]) {
  	cout <<"Multi channel ADC read" << endl;	
  	int ADS_address = 0x48;		// Address of our device on the I2C bus
  	int I2CFile;
  
 	 uint8_t readBuf[2];			// 2 byte buffer to store the data read from the I2C device
  
  	int16_t val;				// Stores the 16 bit value of our ADC conversion
  
  	uint8_t writeBuf[3];			// Buffer to store the 3 bytes that we write to the I2C device
  	I2CFile = open("/dev/i2c-1", O_RDWR);	// Open the I2C device
  
  	ioctl(I2CFile, I2C_SLAVE, ADS_address);  // Specify the address of the I2C Slave to communicate with

  	int chanArray[4] ={0x40,0x50,0x60,0x70}; // ADS1115 has 4 channels 

  	writeBuf[1] = START_SINGLE_CONVERION | GAIN_1 | SINGLE_SHOT_MODE ;   // This sets the 8 MSBs of the config register (bits 15-8) to 10000011 lxf
  	writeBuf[2] = DISABLE_COMPARTOR;  				     // This sets the 8 LSBs of the config register (bits 7-0) to 00000011

  	for (int index =0;index<4;index++) {

  		// Initialize the buffer used to read data from the ADS1115 to 0
  		readBuf[0]= 0;		
  		readBuf[1]= 0;
	  
  		// These three bytes are written to the ADS1115 to set the config register and start a conversion 
  		writeBuf[0] = 1;			// This sets the pointer register so that the following two bytes write to the config register
  		writeBuf[1]  |= chanArray[index];   	// This OR's the  config register (bits 14-12) for the channel
  
	  
  		// Write writeBuf to the ADS1115, the 3 specifies the number of bytes we are writing,
  		// this begins a single conversion
  		write(I2CFile, writeBuf, 3);	

  		// Wait for the conversion to complete, this requires bit 15 to change from 0->1
  		while ((readBuf[0] & 0x80) == 0)	// readBuf[0] contains 8 MSBs of config register, AND with 10000000 to select bit 15
	  		read(I2CFile, readBuf, 2);	// Read the config register into readBuf
	  

  		writeBuf[0] = 0;			// Set pointer register to 0 to read from the conversion register
  		write(I2CFile, writeBuf, 1);
  
  		read(I2CFile, readBuf, 2);		// Read the contents of the conversion register into readBuf
  		val = readBuf[0] << 8 | readBuf[1];	// Combine the two bytes of readBuf into a single 16 bit result 
		results[index] = (float)val*4.096/32767.0; //return voltage to caller
  		//printf("Channel %d Voltage Reading %f (V) \n",index, (float)val*4.096/32767.0);	// Print the result to terminal, first convert from binary value to mV
  	}
  	close(I2CFile);
  
  	return 0;

}

