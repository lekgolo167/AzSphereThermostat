#include "HDC1080.h"

// DHTS11 ensor Function Definitions

/* ------------------------------------------------------------ */
/*				HDC1080 Sensor Function Definitions				*/
/* ------------------------------------------------------------ */


/* ------------------------------------------------------------ */
/*				Procedure Definitions							*/
/* ------------------------------------------------------------ */



/* ------------------------------------------------------------ */
/*        writeRegI2C
**
**        Synopsis:
**				writeRegI2C(bConfig);
**
**        Parameters:
**				uint8_t bReg - the register address to be written to
**				uint16_t bVal - the bytes to be written
**
**        Return Values:
**                void 
**
**        Errors:
**			none
**
**        Description:
**			This function writes to a register over I2C. 
**
*/

void writeRegI2C(uint8_t bReg, uint16_t bVal)
{
	Wire.beginTransmission(HDC1080_I2C_ADDR); // start transmission to device 
	Wire.write(bReg);		        // send register address
	Wire.write((bVal>>8)&0xff); // send upper byte
	Wire.write((bVal)&0xff);    // send lower byte
	Wire.endTransmission(); 	  // end transmission	
}

/* ------------------------------------------------------------ */
/*        readRegI2C
**
**        Synopsis:
**				readRegI2C(bReg, rVal, delay_ms);
**
**        Parameters:
**				uint8_t bReg - the register address to be written to
**				uint16_t* rVal - the return location for the read bytes
**				unsigned int delay_ms - the number of milliseconds required for the HYGRO to convert the desired data
**
**        Return Values:
**                bool success - whether valid data has been successfully captured
**
**        Errors:
**			failure on bad rVal pointer
**
**        Description:
**			This function reads a register over I2C. 
**
*/
 bool readRegI2C(uint8_t bReg, uint16_t &rVal, unsigned long delay_ms)
 {
	int n, i;
	Wire.beginTransmission(HYGROI2C_I2C_ADDR);
	Wire.write(bReg); // send register address
	if (delay_ms > 0)
		delay(delay_ms); // wait for conversion to complete
	n = Wire.requestFrom(HYGROI2C_I2C_ADDR, 2);
	if (n != 2) {
		for (i=0; i<n; i++)
			Wire.read(); // ensure any bad bytes aren't left in the buffer
		return false;
	}
	while (Wire.available()) {
		rVal <<= 8;
		rVal |= (uint16_t)Wire.read(); // shift and set in received bytes, most significant first
	}
	return true;
}


/* ------------------------------------------------------------ */
/*        begin
**
**        Synopsis:
**				myHYGROI2C.begin();
**
**        Parameters:
**
**        Return Values:
**                void 
**
**        Errors:
**
**        Description:
**				This function initializes the I2C interface #1 that is used to communicate with PmodAD2.
**
*/
void begin()
{
	Wire.begin();
	delay(15);
	writeRegI2C(HYGROI2C_CONFIG_REG, 0x00); // use non-sequential acquisition mode, all other config bits are default
}

/* ------------------------------------------------------------ */
/*        getTemperature
**
**        Synopsis:
**				myHYGROI2C.getTemperature();
**
**        Parameters:
**
**        Return Values:
**                float deg_c - the temperature reading in degrees celsius
**
**        Errors: - modify to manage read failures
**
**        Description:
**				This function captures a temperature reading from the Pmod HYGRO.
**
*/
float getTemperature()
{
	uint16_t raw_t;
	float deg_c;
	readRegI2C(HYGROI2C_TMP_REG, raw_t, 7); // conversion time for temperature at 14 bit resolution is 6.5 ms
	deg_c = (float)raw_t / 0x10000;
	deg_c *= 165.0;
	deg_c -= 40.0; // conversion provided in reference manual
	return deg_c;
}

/* ------------------------------------------------------------ */
/*        getHumidity
**
**        Synopsis:
**				HYGROI2C.getHumidity();
**
**        Parameters:
**
**        Return Values:
**                float per_rh - the humidity reading in percent relative humidity.
**
**        Errors: - modify to manage read failures
**
**        Description:
**				This function captures a humidity reading from the Pmod HYGRO.
**
*/
float getHumidity() {
	uint16_t raw_h;
	float per_rh;
	readRegI2C(HYGROI2C_HUM_REG, raw_h, 7); // conversion time for humidity at 14 bit resolution is 6.35 ms
	per_rh = (float)raw_h / 0x10000;
	per_rh *= 100.0; // conversion provided in reference manual
	return per_rh;
}

/* ------------------------------------------------------------ */
/*        tempF2C
**
**        Synopsis:
**				HYGROI2C.tempF2C(deg_f);
**
**        Parameters:
**				float deg_f - the temperature in degrees fahrenheit
**        Return Values:
**              float deg_c - the temperature in degrees celsius
**
**        Errors:
**
**        Description:
**				This function converts a fahrenheit temperature to celsius
**
*/
float tempF2C(float deg_f)
{
	return (deg_f - 32) / 1.8;
}

/* ------------------------------------------------------------ */
/*        tempC2F
**
**        Synopsis:
**				HYGROI2C.tempC2F(deg_c);
**
**        Parameters:
**              float deg_c - the temperature in degrees celsius
**        Return Values:
**				float deg_f - the temperature in degrees fahrenheit
**
**        Errors:
**
**        Description:
**				This function converts a celsius temperature to fahrenheit
**
*/
float tempC2F(float deg_c)
{
	return deg_c * 1.8 + 32;
}