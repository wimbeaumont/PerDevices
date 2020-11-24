#ifndef ADS1x1x_H
#define ADS1x1x_H


#include "stdbool.h"
#include "dev_interface_def.h"
#include "I2CInterface.h" 
#include "ADCInterface.h" 

#define VERSION_ADS1x1x_HDR "0.20"

/** ADS1x1x class.
 *  Used for interfacing with a ADS1x1x Analoge to digital converter of Texas Instruments
 *  
 *  A lot of the code is copied form  
 *  https://github.com/adafruit/Adafruit_ADS1X15
 *
 *  Note: There is an accompanying test suite program "ADS1x1x_test" that can be used to test this library.
 *  
 *  ver 0.1 : initial version not connected to the hardware  only basic ADC funnctions 
 *  ver 0.2 :  tested with ADS1015 and ADS1515 , only  single mode .
 *  hs mode not supported 
 *  allert/ ready not suported 
 * for detail info about the project  check 
 *  https://github.com/wimbeaumont/peripheral_dev_tst
 * (C) Wim Beaumont Universiteit Antwerpen 2020       
 *  
 */
class ADS1x1x : public ADCInterface {
    
    protected:
  // Instance-specific properties
  uint8_t m_i2cAddress;      ///< the I2C address
  uint8_t m_conversionDelay; ///< conversion deay
  uint16_t   m_gain;          ///< ADC gain
  uint8_t lastreg; 	/// last register written 
  uint8_t  m_bitshift;
  uint8_t  m_differential; // 1 for differential mode 
  uint16_t config ;
  void setMux(int ch);
  int status ;  
  /** mbed I2C interface driver. */
  I2CInterface* _i2c_interface;
  /** The full i2c device address. */
  int _device_address;
  int write(int reg ,  int value , int  nr_bytes  );
  int read(uint16_t& value ,  int nr_bytes, int reg  );
  int _full_range;
  void setDefaultSettings(void);
  float getfullVoltageRange( void);
  int getADCvalue(uint16_t & value, int ch);
    public:
   
   
    /** Create an ADS1x1x I2C interface
     * 
     * @param i2cinterface pointer to the  I2C interface 
     * @param device_address   slection  GND =0  , VDD =1 , SDA=2 , SCL=3 base address 0x48 
     * @param Vdd_  the power supply voltage ( == full scale voltage
     * @param  subtype  version of the ADC1x1x  (only 1115 is supported initally )
     * @param gain  , set the gain factor for all channels   0 (==2/3) , 1 ,2, 4 , 8 ,16
     * @param single_diff   , set channels in single 0 or differential 1 . 
     */
    ADS1x1x(I2CInterface* i2cinterface,  int device_address, int subtype=1115,int  gain=0 , int single_diff=0);
    
    void setGain(int gain);// set the gain factor for all channels   0 (==2/3) , 1 ,2, 4 , 8 ,16
    int getStatus(void) { return status;}  //returns the last i2c status	
    virtual int startConversion(int ch=0) {return 0;}
    virtual int statusConversion( int& status, int ch=0){status=1; return 0;};   
    virtual int getADCvalue(int &value, int ch=0);
    virtual int getVoltage(float &voltage, int ch=0);
    virtual int     getFullRange( ){return _full_range;}  	 


};

#endif