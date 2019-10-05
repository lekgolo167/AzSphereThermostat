#include "HDC1080.h"

/* ------------------------------------------------------------ */
/*				HDC1080 Sensor Function Definitions				*/
/* ------------------------------------------------------------ */

/// <summary>
///     This struct is a delay between write and read. Sensor requires about 15 ms to convert data and send it on the bus
/// </summary>
const struct timespec delay = { 0, 20000000 };

struct HDC1080* HDC1080_sensor;

/// <summary>
///     This function converts a 2 byte array into a uint16_t.
/// </summary>
uint16_t ByteTo2Bytes(const uint8_t* raw_data)
{
	return (raw_data[0] << 8) | raw_data[1];
}

/// <summary>
///    Checks the number of transferred bytes for I2C functions and prints an error
///    message if the functions failed or if the number of bytes is different than
///    expected number of bytes to be transferred.
/// </summary>
/// <returns>true on success, or false on failure</returns>
bool CheckTransferSize(const char *desc, size_t expectedBytes, ssize_t actualBytes)
{
	if (actualBytes < 0) {
		Log_Debug("ERROR: %s: errno=%d (%s)\n", desc, errno, strerror(errno));
		return false;
	}

	if (actualBytes != (ssize_t)expectedBytes) {
		Log_Debug("ERROR: %s: transferred %zd bytes; expected %zd\n", desc, actualBytes,
			expectedBytes);
		return false;
	}

	return true;
}

/// <summary>
///     This function initializes the HDC1080.
/// </summary>
void HDC1080Begin(struct HDC1080* hdc1080_ptr)
{
	HDC1080_sensor = hdc1080_ptr;
	HDC1080_sensor->humidity = 0.0;
	HDC1080_sensor->temp_C = 0.0;

	// Send config register address then
	// Send setup bytes, software reset bit ON, all others are default
	const uint8_t HDC1080_config[] = { HDC1080_CONFIG_REG,  0x80, 0x00 };

	ssize_t transferredBytes = I2CMaster_Write(ISU_2_Fd, HDC1080_I2C_ADDR, HDC1080_config, sizeof(HDC1080_config));
	if(CheckTransferSize("GET init FAIL", sizeof(HDC1080_config), transferredBytes));
		Log_Debug("HDC1080 Initialization Completed\n");

}

/// <summary>
///     This function captures a temperature reading from the HDC1080.
/// </summary>
bool HDC1080GetTemperature()
{
	uint8_t raw_temperature[] = {0x0, 0x0};
	const uint8_t i2c_commands[] = { HDC1080_TMP_REG };

	// Request temperature information
	ssize_t transferredBytes = I2CMaster_Write(ISU_2_Fd, HDC1080_I2C_ADDR, i2c_commands, sizeof(i2c_commands));
	if (!CheckTransferSize("HDC1080: Failed to write for Temperature request", sizeof(i2c_commands), transferredBytes))
		return false;

	// Wait for sensor to convert data
	nanosleep(&delay, NULL);

	// Read temperature bytes
	transferredBytes = I2CMaster_Read(ISU_2_Fd, HDC1080_I2C_ADDR, raw_temperature, 2);
	if (!CheckTransferSize("HDC1080: Failed to read the Temperature bytes", sizeof(raw_temperature), transferredBytes))
		return false;

	// Convert raw data
	uint16_t raw_t = ByteTo2Bytes(raw_temperature);
	HDC1080_sensor->temp_C = (float)raw_t / 0x10000;
	HDC1080_sensor->temp_C *= 165.0;
	HDC1080_sensor->temp_C -= 40.0; // conversion provided in reference manual

	return true;
}

/// <summary>
///     This function captures a relative humidity reading from the HDC1080.
/// </summary>
bool HDC1080GetHumidity()
{
	uint8_t raw_humidity[] = { 0x0, 0x0 };
	const uint8_t i2c_commands[] = { HDC1080_HUM_REG };

	// Request humidity information
	ssize_t transferredBytes = I2CMaster_Write(ISU_2_Fd, HDC1080_I2C_ADDR, i2c_commands , sizeof(i2c_commands));
	if (!CheckTransferSize("HDC1080: Failed to write for Humidity request", sizeof(i2c_commands), transferredBytes))
		return false;

	// Wait for sensor to convert data
	nanosleep(&delay, NULL);

	// Read humidity bytes
	transferredBytes = I2CMaster_Read(ISU_2_Fd, HDC1080_I2C_ADDR, raw_humidity, sizeof(raw_humidity));
	if (!CheckTransferSize("HDC1080: Failed to read the Humidity bytes", sizeof(raw_humidity), transferredBytes))
		return false;

	// Convert raw data
	uint16_t raw_h = ByteTo2Bytes(raw_humidity);
	HDC1080_sensor->humidity = (float)raw_h / 0x10000;
	HDC1080_sensor->humidity *= 100.0; // conversion provided in reference manual

	return true;
}

/// <summary>
///     This function converts a celsius temperature to fahrenheit.
/// </summary>
float tempC2F()
{
	return HDC1080_sensor->temp_C * 1.8 + 32;
}

/// <summary>
///     This function converts a fahrenheit temperature to celsius.
/// </summary>
float tempF2C(float deg_F)
{
	return (deg_F - 32) * (5/9);
}