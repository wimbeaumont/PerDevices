#ifndef __DUMMYI2CINTERFACE_H  
#define __DUMMYI2CINTERFACE_H   

#define DummyI2CINTERFACE_HDR_VER "1.11"

#include "I2CInterface.h"
#include <unistd.h>  //for the ms 
/*
 *  This is a dummy implementation of the I2CInterface class for software developments
 *  This file make part of the PeriperalDevice package see repository  
 *  https://github.com/wimbeaumont/PeripheralDevices
 *  For more info see 	the README.md in the top of repository 
 *  ver 0.10  init version 
 *  ver 1.00  for new version of the I2Cinterface 
 *  ver 1.10  had to add I2C_dummy_buffersize  to get it working on gcc 4.4 
 *  ver 1.11  added wait us
 *
 * (C) Wim Beaumont Universiteit Antwerpen 2019 -- 2023 
 * 
 * License see
 * https://github.com/wimbeaumont/PeripheralDevices/blob/master/LICENSE
*/ 

#define I2C_dummy_buffersize 1000
class DummyI2CInterface :public I2CInterface  {
    private:
	//const int  I2C_dummy_buffersize=1000;
   
	char lbuffer[I2C_dummy_buffersize] ; // dummy space to store write and read from 
    int  lfreq; // = 100000;
    public :
    
    DummyI2CInterface(void ) :
	getVersion( DummyI2CINTERFACE_HDR_VER,DummyI2CINTERFACE_HDR_VER, __TIME__, __DATE__){
	      lfreq=100000;// to get gcc 4.4 working 
    } ;

virtual int    frequency (int hz){ lfreq=hz ; return lfreq;};
virtual int     read (int address, char *data, int length, bool repeated=false){
		int returnvalue=-1;
  		  if (length <  I2C_dummy_buffersize){
			for ( int lc =0; lc < length ; lc++){
				data[lc]=lbuffer[lc]; // can be done more efficient but I2C is not so fast
			}	
			returnvalue=length;
	          };
		  return returnvalue;
		}
	
 virtual int    read (int &data, int ack){data=lbuffer[0];return 0;};// Read a single byte from the I2C bus.
 virtual int    write (int address, const char *data, int length, bool repeated=false){
		int returnvalue=-1;
		   if (length <  I2C_dummy_buffersize){
			for ( int lc =0; lc < length ; lc++){
				lbuffer[lc]=data[lc]; // can be done more efficient but I2C is not so fast
			}	
			returnvalue=length;
	           };
		   return returnvalue;
		};
virtual int     write (int data){ lbuffer[0]=data ; return 0;  };//  Write single byte out on the I2C bus.
virtual int     start (void) {return 0;}; // Creates a start condition on the I2C bus.
virtual int     stop  (void) {return 0;}; // Creates a stop condition on the I2C bus.
virtual int     transfer (int address, const char *tx_buffer, int tx_length, char *rx_buffer, int rx_length, void* callbackfunction,  bool repeated=false){
            return  -1;  // seems transfer not supported in mbed or not correctly called below
           //  return i2cdev.transfer (address, tx_buffer,  tx_length, rx_buffer,  rx_length, callback,  event,  repeated);
        };    
virtual void wait_for_ms(int x)  {  usleep(1000*x); }
virtual void wait_for_us(int x)  { usleep(x); }

    } ;


#endif
