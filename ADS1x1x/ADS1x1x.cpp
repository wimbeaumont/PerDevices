#include "ADS1x1x.h"
#include "dev_interface_def.h"

//#include <cstdio>
/* see ADS1x1x.h for more info
 * (C) Wim Beaumont Universiteit Antwerpen 2020    
 */ 

#define VERSION_ADS1x1x_SRC "0.41"



/*=========================================================================
    CONVERSION DELAY (in mS)
    -----------------------------------------------------------------------*/
#define ADS1015_CONVERSIONDELAY (1) ///< Conversion delay
#define ADS1115_CONVERSIONDELAY (9) ///< Conversion delay
/*=========================================================================*/

/*=========================================================================
    POINTER REGISTER
    -----------------------------------------------------------------------*/
#define ADS1015_REG_POINTER_MASK (0x03)      ///< Point mask
#define ADS1015_REG_POINTER_CONVERT (0x00)   ///< Conversion
#define ADS1015_REG_POINTER_CONFIG (0x01)    ///< Configuration
#define ADS1015_REG_POINTER_LOWTHRESH (0x02) ///< Low threshold
#define ADS1015_REG_POINTER_HITHRESH (0x03)  ///< High threshold
/*=========================================================================*/

/*=========================================================================
    CONFIG REGISTER
    -----------------------------------------------------------------------*/
#define ADS1015_REG_CONFIG_OS_MASK (0x8000) ///< OS Mask
#define ADS1015_REG_CONFIG_OS_SINGLE (0x8000) ///< Write: Set to start a single-conversion
#define ADS1015_REG_CONFIG_OS_BUSY  (0x0000) ///< Read: Bit = 0 when conversion is in progress
#define ADS1015_REG_CONFIG_OS_NOTBUSY (0x8000) ///< Read: Bit = 1 when device is not performing a conversion

#define ADS1015_REG_CONFIG_MUX_MASK (0x7000) ///< Mux Mask
#define ADS1015_REG_CONFIG_MUX_DIFF_0_1                                        \
  (0x0000) ///< Differential P = AIN0, N = AIN1 (default)
#define ADS1015_REG_CONFIG_MUX_DIFF_0_3                                        \
  (0x1000) ///< Differential P = AIN0, N = AIN3
#define ADS1015_REG_CONFIG_MUX_DIFF_1_3                                        \
  (0x2000) ///< Differential P = AIN1, N = AIN3
#define ADS1015_REG_CONFIG_MUX_DIFF_2_3                                        \
  (0x3000) ///< Differential P = AIN2, N = AIN3
#define ADS1015_REG_CONFIG_MUX_SINGLE_0 (0x4000) ///< Single-ended AIN0
#define ADS1015_REG_CONFIG_MUX_SINGLE_1 (0x5000) ///< Single-ended AIN1
#define ADS1015_REG_CONFIG_MUX_SINGLE_2 (0x6000) ///< Single-ended AIN2
#define ADS1015_REG_CONFIG_MUX_SINGLE_3 (0x7000) ///< Single-ended AIN3

#define ADS1015_REG_CONFIG_PGA_MASK (0x0E00)   ///< PGA Mask
#define ADS1015_REG_CONFIG_PGA_6_144V (0x0000) ///< +/-6.144V range = Gain 2/3
#define ADS1015_REG_CONFIG_PGA_4_096V (0x0200) ///< +/-4.096V range = Gain 1
#define ADS1015_REG_CONFIG_PGA_2_048V                                          \
  (0x0400) ///< +/-2.048V range = Gain 2 (default)
#define ADS1015_REG_CONFIG_PGA_1_024V (0x0600) ///< +/-1.024V range = Gain 4
#define ADS1015_REG_CONFIG_PGA_0_512V (0x0800) ///< +/-0.512V range = Gain 8
#define ADS1015_REG_CONFIG_PGA_0_256V (0x0A00) ///< +/-0.256V range = Gain 16

#define ADS1015_REG_CONFIG_MODE_MASK (0x0100)   ///< Mode Mask
#define ADS1015_REG_CONFIG_MODE_CONTIN (0x0000) ///< Continuous conversion mode
#define ADS1015_REG_CONFIG_MODE_SINGLE                                         \
  (0x0100) ///< Power-down single-shot mode (default)

#define ADS1015_REG_CONFIG_DR_MASK (0x00E0)   ///< Data Rate Mask        1515 
#define ADS1015_REG_CONFIG_DR_128SPS (0x0000) ///< 128 samples per second 8
#define ADS1015_REG_CONFIG_DR_250SPS (0x0020) ///< 250 samples per second 16
#define ADS1015_REG_CONFIG_DR_490SPS (0x0040) ///< 490 samples per second 32
#define ADS1015_REG_CONFIG_DR_920SPS (0x0060) ///< 920 samples per second 64
#define ADS1015_REG_CONFIG_DR_1600SPS (0x0080) ///< 1600 samples per second 128
#define ADS1015_REG_CONFIG_DR_2400SPS (0x00A0) ///< 2400 samples per second 250
#define ADS1015_REG_CONFIG_DR_3300BSPS (0x00B0) ///< 2400 samples per second 475
#define ADS1015_REG_CONFIG_DR_3300SPS (0x00C0) ///< 3300 samples per second  860


#define ADS1015_REG_CONFIG_CMODE_MASK (0x0010) ///< CMode Mask
#define ADS1015_REG_CONFIG_CMODE_TRAD                                          \
  (0x0000) ///< Traditional comparator with hysteresis (default)
#define ADS1015_REG_CONFIG_CMODE_WINDOW (0x0010) ///< Window comparator

const uint16_t  ADS1015_REG_CONFIG_CPOL_MASK=0x0008; ///< CPol Mask
#define ADS1015_REG_CONFIG_CPOL_ACTVLOW                                        \
  (0x0000) ///< ALERT/RDY pin is low when active (default)
#define ADS1015_REG_CONFIG_CPOL_ACTVHI                                         \
  (0x0008) ///< ALERT/RDY pin is high when active

const uint16_t  ADS1015_REG_CONFIG_CLAT_MASK=0x0004 ;///< Determines if ALERT/RDY pin latches once asserted
#define ADS1015_REG_CONFIG_CLAT_NONLAT                                         \
  (0x0000) ///< Non-latching comparator (default)
#define ADS1015_REG_CONFIG_CLAT_LATCH (0x0004) ///< Latching comparator

#define ADS1015_REG_CONFIG_CQUE_MASK (0x0003) ///< CQue Mask
#define ADS1015_REG_CONFIG_CQUE_1CONV                                          \
  (0x0000) ///< Assert ALERT/RDY after one conversions
#define ADS1015_REG_CONFIG_CQUE_2CONV                                          \
  (0x0001) ///< Assert ALERT/RDY after two conversions
#define ADS1015_REG_CONFIG_CQUE_4CONV                                          \
  (0x0002) ///< Assert ALERT/RDY after four conversions
#define ADS1015_REG_CONFIG_CQUE_NONE                                           \
  (0x0003) ///< Disable the comparator and put ALERT/RDY in high state (default)
/*=========================================================================*/



ADS1x1x::ADS1x1x(I2CInterface* i2cinterface,  int device_address,   int subtype ,int  gain , int single_diff):
    getVersion( VERSION_ADS1x1x_HDR,VERSION_ADS1x1x_SRC, __TIME__, __DATE__), _i2c_interface(i2cinterface)
{   
    // Assemble the full I2C device address.
    // ADS1015_ADDRESS (0x48)  for ADDR = gnd so in 8 bits 0x90
    _device_address = 0x90; // Prime the full device address with the device code.
    _device_address |= (device_address<<1);
    m_bitshift=0;// default  16 bits 
    _full_range= 16*2048; // 2^4  * 2^12 / 2 (  /2 ==  -xx to +xx 
    if (subtype == 1015 ) {    m_conversionDelay = ADS1015_CONVERSIONDELAY; m_bitshift=4 ;_full_range = 2048;   }
    else {m_conversionDelay = ADS1115_CONVERSIONDELAY;} 
    if ( single_diff == 0) { m_differential=0 ; }  else {m_differential=1;}
    setDefaultSettings() ;
      setGain(gain);
}
  
void ADS1x1x::setGain(int gain) {
 config &= ~ADS1015_REG_CONFIG_PGA_MASK; 
  switch (gain) {
    	case 1 : m_gain = ADS1015_REG_CONFIG_PGA_4_096V;
    	break; 
    	case  2 : m_gain =ADS1015_REG_CONFIG_PGA_2_048V;
    	break; 
    	case 4 : m_gain =ADS1015_REG_CONFIG_PGA_1_024V;
    	break; 
   	case 8 : m_gain =ADS1015_REG_CONFIG_PGA_0_512V;
    	break; 
   	case 16 : m_gain =ADS1015_REG_CONFIG_PGA_0_256V;
    	break; 
	default :    m_gain =ADS1015_REG_CONFIG_PGA_6_144V; /* +/- 6.144V range (limited to VDD +0.3V max!) */
    }
    config |= m_gain; 
 }  


float ADS1x1x::getfullVoltageRange( void) {
 uint16_t gainbits = config &  ADS1015_REG_CONFIG_PGA_MASK;
 float vrange=0; 
 switch (gainbits) {
  	case  ADS1015_REG_CONFIG_PGA_6_144V :  vrange= 6.144;
	break;
 	case  ADS1015_REG_CONFIG_PGA_4_096V :  vrange= 4.096;
	break;
 	case  ADS1015_REG_CONFIG_PGA_2_048V :  vrange= 2.048;
	break;
 	case  ADS1015_REG_CONFIG_PGA_1_024V :  vrange= 1.024;
	break;
 	case  ADS1015_REG_CONFIG_PGA_0_512V :  vrange= .512;
	break;
 	case  ADS1015_REG_CONFIG_PGA_0_256V :  vrange= .256;
	break; 
 }
 return vrange;	
}	 		
 


int ADS1x1x::getMuxCh(void) {
	int ch ;
	uint16_t muxreg = config & ADS1015_REG_CONFIG_MUX_MASK; 
	if ( m_differential  == 1) {
		if ( muxreg == ADS1015_REG_CONFIG_MUX_DIFF_0_1 ) ch= 0;
		else if ( muxreg == ADS1015_REG_CONFIG_MUX_DIFF_2_3 ) ch= 1;
		     else if ( muxreg == ADS1015_REG_CONFIG_MUX_DIFF_0_3) ch= 2; 
		          else if ( muxreg == ADS1015_REG_CONFIG_MUX_DIFF_1_3 ) ch= 3;
		               else ch=-1; // something wrong not differential ? 
	}else {if ( muxreg ==  ADS1015_REG_CONFIG_MUX_SINGLE_0) ch= 0;
		else if ( muxreg ==ADS1015_REG_CONFIG_MUX_SINGLE_1  ) ch= 1;
		     else if ( muxreg == ADS1015_REG_CONFIG_MUX_SINGLE_2 ) ch= 2; 
		          else if ( muxreg == ADS1015_REG_CONFIG_MUX_SINGLE_3) ch= 3;
		               else ch=-1; // something wrong not single ended  ? 
	}
	return ch; 
}	

void ADS1x1x::setMux( int ch ) {
	config &= ~ADS1015_REG_CONFIG_MUX_MASK; 
	if ( m_differential  == 1) {
	  switch (ch) {
	  case (1):
	    config |= ADS1015_REG_CONFIG_MUX_DIFF_2_3;
	    break;
	  case (2):
	    config |= ADS1015_REG_CONFIG_MUX_DIFF_0_3;
	    break;
	  case (3):
	    config |= ADS1015_REG_CONFIG_MUX_DIFF_1_3;
	    break;
	    default :  // so includes   case (0):
	    config |= ADS1015_REG_CONFIG_MUX_DIFF_0_1 ;
	  }	
	}
	else { // Set single-ended input channel
	  switch (ch) {
	  case (1):
	    config |= ADS1015_REG_CONFIG_MUX_SINGLE_1;
	    break;
	  case (2):
	    config |= ADS1015_REG_CONFIG_MUX_SINGLE_2;
	    break;
	  case (3):
	    config |= ADS1015_REG_CONFIG_MUX_SINGLE_3;
	    break;
	    default :  // so includes   case (0):
	    config |= ADS1015_REG_CONFIG_MUX_SINGLE_0;
	  }
	}
}

void  ADS1x1x::setDefaultSettings(void) {
  // Start with default values
    config =0 ;
    config =
      ADS1015_REG_CONFIG_CQUE_NONE |    // Disable the comparator (default val)
      ADS1015_REG_CONFIG_CLAT_NONLAT |  // Non-latching (default val)
      ADS1015_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
      ADS1015_REG_CONFIG_CMODE_TRAD |   // Traditional comparator (default val)
      ADS1015_REG_CONFIG_DR_490SPS |   // 490 samples per second (default)
      ADS1015_REG_CONFIG_MODE_SINGLE;   // Single-shot mode (default)
    
      
  
	
}

int   ADS1x1x::setConversionMode(int mode) {
	// check mode 
	uint16_t regstat = config & ADS1015_REG_CONFIG_MODE_MASK;
	if (mode ) { if (regstat > 0)  return 0;} // already Single-shot mode 
	else {  if (regstat == 0)   return 0;} // already continous mode 
	if ( mode ) {
		config |=  ADS1015_REG_CONFIG_MODE_MASK; //set Single-shot mode 
	}else {
		config &=  ~ADS1015_REG_CONFIG_MODE_MASK; //set continous mode 	
	}
	status= write(ADS1015_REG_POINTER_CONFIG , config , 2 );
	return status;
}


int ADS1x1x::getADCvalue(uint16_t & value, int ch){
	uint16_t regstat = config & ADS1015_REG_CONFIG_MODE_MASK;
	if (regstat ) status =getADCvalueSingleShot( value,  ch);
	else status = getADCvalueContinous( value,  ch);
	return status; 
}

int ADS1x1x::getADCvalueContinous(uint16_t & value, int ch){
	// for continous mode it doesn't make much sense to switch all the time the channel
	// so check if the config has the correct channel selected 
	// check if ch is set in the config 
	int chck = getMuxCh();
	if ( chck < 0) return -600;  // not correct config  for single ended or differential  
	
	if ( ch != chck ) {
        	setMux(ch); 
	       	status= write(ADS1015_REG_POINTER_CONFIG , config , 2 );
       		_i2c_interface->wait_for_ms(5); // need some time to fill the conversion register 
	 }
	status-=read( value,2,  ADS1015_REG_POINTER_CONVERT) ;	// no need for waiting as the last full conversion is read
	return status;
}


int ADS1x1x::getADCvalueSingleShot(uint16_t & value, int ch){
	uint16_t regstat;
	// assumes config register is correct except the mux 
        setMux(ch); 
       	status= write(ADS1015_REG_POINTER_CONFIG , config , 2 );
	if ( status ) return status-300;
	_i2c_interface->wait_for_ms(3);// not sure if this is needed 	
	// Set 'start single-conversion' bit	
	config |= ADS1015_REG_CONFIG_OS_SINGLE;
	status= write(ADS1015_REG_POINTER_CONFIG , config , 2 );
	if ( status ) return status-100;
	// Wait for the conversion to complete
	//_i2c_interface->wait_for_ms(m_conversionDelay);
	int cntdwn= 100;
	while (cntdwn) { 	
		status= read(regstat,2 , ADS1015_REG_POINTER_CONFIG);
		//printf("reg :0x%04x \n\r",regstat);
		if ( status ) return status-400;
		if( regstat & ADS1015_REG_CONFIG_OS_SINGLE) break;//no conv
		_i2c_interface->wait_for_ms(m_conversionDelay);
		cntdwn--;
	}
	if ( cntdwn ==0 ) { return status-500; }
	status=read( value,2,  ADS1015_REG_POINTER_CONVERT) ;
	//value = value >> m_bitShift ;
	// no bit shift needed , the also for the 1015 the output 16 bits , 4 lsb always 0 
	//if (m_bitShift != 0) {
	// Shift 12-bit results right 4 bits for the ADS1015,
	// making sure we keep the sign bit intact
	//    if (value > 0x07FF) {
	      // negative number - extend the sign to 16th bit
	 //     value |= 0xF000;
	 //   }	
	 //}
    return status;
}


int  ADS1x1x::getADCvalue(int& value, int ch){
	uint16_t v16;
	status= getADCvalue( v16,  ch)		;
	value= (int) v16;
	return status;
}	


int ADS1x1x::getVoltage(float &voltage, int ch){
  uint16_t  value;
  int status =getADCvalue(value,ch);
  int16_t zvalue=  (int16_t) value; // take the sign 
  
  voltage  = getfullVoltageRange()*  (float) zvalue / (float) _full_range ;
  return status; 
 }   


int ADS1x1x::read(uint16_t& value , int nr_bytes , int  reg  ) { 
// assumes the readpointer is set 
    char data[2];
    int status = 3 ; // nr_bytes out of range 
    if ( reg != lastreg) {
    	status=write(reg , 0 , 0);
    }	
    if(nr_bytes >0 && nr_bytes < 3) {
        // Read the raw data from the device.
        status = _i2c_interface->read(_device_address, data, nr_bytes, false);
        if ( status == 0) {
            value = (int) data[0];
            if ( nr_bytes == 2) {
                value=value << 8;
                value |= data[1];                
            }
        } // else result == i2c error code
    } // else status =  nr_bytes out of range 
    return status;
}  
        
 




int ADS1x1x::write(int reg ,  int value , int  nr_bytes ){   
    char data[3];
    int status ;
    //Assemble our three bytes of data - Refer to ADS1x1x ref manual, section 6.
    lastreg=reg;
    data[0] = (int8_t)reg;    
    if ( nr_bytes ==2) {
        data[2] = (int8_t) value; //LSB
        value= value >>8;        
        data[1] = (int8_t) value; //MSB   
    }
    else { 
        data[1] = (int8_t) value;   // so if nr_bytes =0  
    }
    nr_bytes++;
    status =  _i2c_interface->write(_device_address, data, nr_bytes, false);
        
    return status;
}
 
    int ADS1x1x::setAlertPinMode( int mode  , int upperlimit , int lowerlimit , int cnt, int pol  ){
    	switch (mode) {
    		case 1  : cnt =0;//not to be 3 
    			upperlimit=0XFFFF; //MSB =1
    			lowerlimit=0;   // MSB =0     			
    		break;
    		case 2 : case 5 : config &=  ~ADS1015_REG_CONFIG_CMODE_MASK;
    			if( mode ==2 ) { config&= ~ADS1015_REG_CONFIG_CLAT_MASK;} //not latched
    			else { config |= ADS1015_REG_CONFIG_CLAT_MASK;}
    		break;
    		case 3: case 6 : config |=  ADS1015_REG_CONFIG_CMODE_MASK;//window mode 
			if( mode ==3 ) { config&= ~ADS1015_REG_CONFIG_CLAT_MASK;} //not latched
    			else { config |= ADS1015_REG_CONFIG_CLAT_MASK;}
    		break;
    		default  : config |= ADS1015_REG_CONFIG_CQUE_NONE; // so including 0  
    			   mode=0;
    		break;
    	}

	if ( cnt < 0 ||  cnt > 2)  cnt=3; //disable allert pin
	if ( mode > 0 ) { 
		config &= ~ADS1015_REG_CONFIG_CQUE_NONE;
		config  |= cnt ; 
		if ( pol == 1 ) config |= ADS1015_REG_CONFIG_CPOL_MASK;
		else config &= ~ADS1015_REG_CONFIG_CPOL_MASK;
		status= write(ADS1015_REG_POINTER_LOWTHRESH ,(int16_t ) lowerlimit, 2 );
		status+= write(ADS1015_REG_POINTER_HITHRESH ,(int16_t ) upperlimit, 2 );
		
	}
	status+= write(ADS1015_REG_POINTER_CONFIG , config , 2 );
	return status; 
    }	 
 
      uint16_t ADS1x1x::getDigValue(float volt)  {
	float voltage= getfullVoltageRange();
	voltage = volt /voltage ;
	uint16_t digv;
	if ( volt <  0) voltage= -voltage ; 
	digv= (uint16_t)  0x7FFF * voltage;
	if ( volt < 0 )  {  digv= ~digv ; digv+=1; } 
	return digv;
    }
    
// next only for debugging 
	
int ADS1x1x::getReg(uint16_t reg, uint16_t & value){
	status= read(value,2 , reg);
	return status;
}
	
	
	
// int ADS1x1x::decode_config_reg( uint16_t value){	

int ADS1x1x::chkShadowReg(uint16_t  &check) {
	uint16_t value;
	status=getReg(1,value);
	check = ~config & ~value ; 
	value = value & config;
	check=value | check;
	check |= 0x8000;
	return status;
}   		
		
  
