#include "SparkFunBQ27441.h"
#include "BQ27441_Definitions.h"
#include "stm32l1xx_i2c.h"
#include "stm32l1xx.h"
#include <stdbool.h>
//#include "main.c"

extern I2C_TypeDef * I2CPERIPHSEL;
/*****************************************************************************
 ************************** Initialisation Functions *************************
 *****************************************************************************/

void I2C_start(uint8_t address, uint8_t direction, uint8_t repeat)
{
 if (!repeat) {
	 while(I2C_GetFlagStatus(I2CPERIPHSEL, I2C_FLAG_BUSY));
 }
 I2C_GenerateSTART(I2CPERIPHSEL, ENABLE);
 while(!I2C_CheckEvent(I2CPERIPHSEL, I2C_EVENT_MASTER_MODE_SELECT)) ;
 I2C_Send7bitAddress(I2CPERIPHSEL, address, direction);
 if (direction== I2C_Direction_Transmitter) {
 while(!I2C_CheckEvent(I2CPERIPHSEL, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
 }
 else if(direction == I2C_Direction_Receiver) {
 while(!I2C_CheckEvent(I2CPERIPHSEL, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
 }
}

void I2C_stop()
{
 I2C_GenerateSTOP(I2CPERIPHSEL, ENABLE);
}


// Configures the design capacity of the connected battery.
bool setCapacity(uint16_t capacity)
{
	// Write to STATE subclass (82) of BQ27441 extended memory.
	// Offset 0x0A (10)
	// Design capacity is a 2-byte piece of data - MSB first
	// Unit: mAh
	uint8_t capMSB = capacity >> 8;
	uint8_t capLSB = capacity & 0x00FF;
	uint8_t capacityData[2] = {capMSB, capLSB};
	return writeExtendedData(BQ27441_ID_STATE, 10, capacityData, 2);
}

// Configures the design energy of the connected battery.
bool setDesignEnergy(uint16_t energy)
{
	// Write to STATE subclass (82) of BQ27441 extended memory.
	// Offset 0x0C (12)
	// Design energy is a 2-byte piece of data - MSB first
	// Unit: mWh
	uint8_t enMSB = energy >> 8;
	uint8_t enLSB = energy & 0x00FF;
	uint8_t energyData[2] = {enMSB, enLSB};
	return writeExtendedData(BQ27441_ID_STATE, 12, energyData, 2);
}

// Configures the terminate voltage.
bool setTerminateVoltage(uint16_t voltage)
{
	// Write to STATE subclass (82) of BQ27441 extended memory.
	// Offset 0x0F (16)
	// Termiante voltage is a 2-byte piece of data - MSB first
	// Unit: mV
	// Min 2500, Max 3700
	if(voltage<2500) voltage=2500;
	if(voltage>3700) voltage=3700;
	
	uint8_t tvMSB = voltage >> 8;
	uint8_t tvLSB = voltage & 0x00FF;
	uint8_t tvData[2] = {tvMSB, tvLSB};
	return writeExtendedData(BQ27441_ID_STATE, 16, tvData, 2);
}

// Configures taper rate of connected battery.
bool setTaperRate(uint16_t rate)
{
	// Write to STATE subclass (82) of BQ27441 extended memory.
	// Offset 0x1B (27)
	// Termiante voltage is a 2-byte piece of data - MSB first
	// Unit: 0.1h
	// Max 2000
	if(rate>2000) rate=2000;
	uint8_t trMSB = rate >> 8;
	uint8_t trLSB = rate & 0x00FF;
	uint8_t trData[2] = {trMSB, trLSB};
	return writeExtendedData(BQ27441_ID_STATE, 27, trData, 2);
}

/*****************************************************************************
 ********************** Battery Characteristics Functions ********************
 *****************************************************************************/

// Reads and returns the battery voltage
uint16_t voltage(void)
{
	return readWord(BQ27441_COMMAND_VOLTAGE);
}

// Reads and returns the specified current measurement
int16_t current(current_measure type)
{
	int16_t current = 0;
	switch (type)
	{
	case AVG:
		current = (int16_t) readWord (BQ27441_COMMAND_AVG_CURRENT);
		break;
	case STBY:
		current = (int16_t)  readWord(BQ27441_COMMAND_STDBY_CURRENT);
		break;
	case MAX:
		current = (int16_t)  readWord(BQ27441_COMMAND_MAX_CURRENT);
		break;
	}
	
	return current;
}

// Reads and returns the specified capacity measurement
uint16_t capacity(capacity_measure type)
{
	uint16_t capacity = 0;
	switch (type)
	{
	case REMAIN:
		return  readWord(BQ27441_COMMAND_REM_CAPACITY);
		break;
	case FULL:
		return  readWord(BQ27441_COMMAND_FULL_CAPACITY);
		break;
	case AVAIL:
		capacity =  readWord(BQ27441_COMMAND_NOM_CAPACITY);
		break;
	case AVAIL_FULL:
		capacity =  readWord(BQ27441_COMMAND_AVAIL_CAPACITY);
		break;
	case REMAIN_F: 
		capacity =  readWord(BQ27441_COMMAND_REM_CAP_FIL);
		break;
	case REMAIN_UF:
		capacity =  readWord(BQ27441_COMMAND_REM_CAP_UNFL);
		break;
	case FULL_F:
		capacity =  readWord(BQ27441_COMMAND_FULL_CAP_FIL);
		break;
	case FULL_UF:
		capacity =  readWord(BQ27441_COMMAND_FULL_CAP_UNFL);
		break;
	case DESIGN:
		capacity =  readWord(BQ27441_EXTENDED_CAPACITY);
	}
	
	return capacity;
}

// Reads and returns measured average power
int16_t power()
{
	return (int16_t)  readWord(BQ27441_COMMAND_AVG_POWER);
}

// Reads and returns specified state of charge measurement
uint16_t soc(soc_measure type)
{
	uint16_t socRet = 0;
	switch (type)
	{
	case FILTERED:
		socRet =  readWord(BQ27441_COMMAND_SOC);
		break;
	case UNFILTERED:
		socRet =  readWord(BQ27441_COMMAND_SOC_UNFL);
		break;
	}
	
	return socRet;
}

// Reads and returns specified state of health measurement
uint8_t soh(soh_measure type)
{
	uint16_t sohRaw =  readWord(BQ27441_COMMAND_SOH);
	uint8_t sohStatus = sohRaw >> 8;
	uint8_t sohPercent = sohRaw & 0x00FF;
	
	if (type == PERCENT)	
		return sohPercent;
	else
		return sohStatus;
}

// Reads and returns specified temperature measurement
uint16_t temperature(temp_measure type)
{
	uint16_t temp = 0;
	switch (type)
	{
	case BATTERY:
		temp =  readWord(BQ27441_COMMAND_TEMP);
		break;
	case INTERNAL_TEMP:
		temp =  readWord(BQ27441_COMMAND_INT_TEMP);
		break;
	}
	return temp;
}

/*****************************************************************************
 ************************** GPOUT Control Functions **************************
 *****************************************************************************/
// Get GPOUT polarity setting (active-high or active-low)
bool GPOUTPolarity()
{
	uint16_t opConfigRegister = opConfig();
	
	return (opConfigRegister & BQ27441_OPCONFIG_GPIOPOL);
}

// Set GPOUT polarity to active-high or active-low
bool setGPOUTPolarity(bool activeHigh)
{
	uint16_t oldOpConfig = opConfig();
	
	// Check to see if we need to update opConfig:
	if ((activeHigh && (oldOpConfig & BQ27441_OPCONFIG_GPIOPOL)) ||
        (!activeHigh && !(oldOpConfig & BQ27441_OPCONFIG_GPIOPOL)))
		return true;
		
	uint16_t newOpConfig = oldOpConfig;
	if (activeHigh)
		newOpConfig |= BQ27441_OPCONFIG_GPIOPOL;
	else
		newOpConfig &= ~(BQ27441_OPCONFIG_GPIOPOL);
	
	return writeOpConfig(newOpConfig);
}

// Get GPOUT function (BAT_LOW or SOC_INT)
bool GPOUTFunction()
{
	uint16_t opConfigRegister = opConfig();
	
	return (opConfigRegister & BQ27441_OPCONFIG_BATLOWEN);	
}

// Set GPOUT function to BAT_LOW or SOC_INT
bool setGPOUTFunction(gpout_function function)
{
	uint16_t oldOpConfig = opConfig();
	
	// Check to see if we need to update opConfig:
	if ((function && (oldOpConfig & BQ27441_OPCONFIG_BATLOWEN)) ||
        (!function && !(oldOpConfig & BQ27441_OPCONFIG_BATLOWEN)))
		return true;
	
	// Modify BATLOWN_EN bit of opConfig:
	uint16_t newOpConfig = oldOpConfig;
	if (function)
		newOpConfig |= BQ27441_OPCONFIG_BATLOWEN;
	else
		newOpConfig &= ~(BQ27441_OPCONFIG_BATLOWEN);

	// Write new opConfig
	return writeOpConfig(newOpConfig);
}

// Get SOC1_Set Threshold - threshold to set the alert flag
uint8_t SOC1SetThreshold()
{
	return readExtendedData(BQ27441_ID_DISCHARGE, 0);
}

// Get SOC1_Clear Threshold - threshold to clear the alert flag
uint8_t SOC1ClearThreshold()
{
	return readExtendedData(BQ27441_ID_DISCHARGE, 1);
}

// Set the SOC1 set and clear thresholds to a percentage
bool setSOC1Thresholds(uint8_t set, uint8_t clear)
{
	uint8_t thresholds[2];
	thresholds[0] = constrain(set, 0, 100);
	thresholds[1] = constrain(clear, 0, 100);
	return writeExtendedData(BQ27441_ID_DISCHARGE, 0, thresholds, 2);
}

// Get SOCF_Set Threshold - threshold to set the alert flag
uint8_t SOCFSetThreshold()
{
	return readExtendedData(BQ27441_ID_DISCHARGE, 2);
}

// Get SOCF_Clear Threshold - threshold to clear the alert flag
uint8_t SOCFClearThreshold()
{
	return readExtendedData(BQ27441_ID_DISCHARGE, 3);
}

// Set the SOCF set and clear thresholds to a percentage
bool setSOCFThresholds(uint8_t set, uint8_t clear)
{
	uint8_t thresholds[2];
	thresholds[0] = constrain(set, 0, 100);
	thresholds[1] = constrain(clear, 0, 100);
	return writeExtendedData(BQ27441_ID_DISCHARGE, 2, thresholds, 2);
}

// Check if the SOC1 flag is set
bool socFlag()
{
	uint16_t flagState = flags();
	
	return flagState & BQ27441_FLAG_SOC1;
}

// Check if the SOCF flag is set
bool socfFlag()
{
	uint16_t flagState = flags();
	
	return flagState & BQ27441_FLAG_SOCF;
	
}

// Check if the ITPOR flag is set
bool itporFlag()
{
	uint16_t flagState = flags();
	
	return flagState & BQ27441_FLAG_ITPOR;
}

// Check if the FC flag is set
bool fcFlag()
{
	uint16_t flagState = flags();
	
	return flagState & BQ27441_FLAG_FC;
}

// Check if the CHG flag is set
bool chgFlag()
{
	uint16_t flagState = flags();
	
	return flagState & BQ27441_FLAG_CHG;
}

// Check if the DSG flag is set
bool dsgFlag()
{
	uint16_t flagState = flags();
	
	return flagState & BQ27441_FLAG_DSG;
}

// Get the SOC_INT interval delta
uint8_t sociDelta()
{
	return readExtendedData(BQ27441_ID_STATE, 26);
}

// Set the SOC_INT interval delta to a value between 1 and 100
bool setSOCIDelta(uint8_t delta)
{
	uint8_t soci = constrain(delta, 0, 100);
	return writeExtendedData(BQ27441_ID_STATE, 26, &soci, 1);
}

// Pulse the GPOUT pin - must be in SOC_INT mode
bool pulseGPOUT()
{
	return executeControlWord(BQ27441_CONTROL_PULSE_SOC_INT);
}

/*****************************************************************************
 *************************** Control Sub-Commands ****************************
 *****************************************************************************/

// Read the device type - should be 0x0421
uint16_t deviceType()
{
	return readControlWord(BQ27441_CONTROL_DEVICE_TYPE);
}

// Enter configuration mode - set userControl if calling from an Arduino sketch
// and you want control over when to exitConfig
bool enterConfig(bool userControl)
{
	if (userControl) _userConfigControl = true;
	
	if (sealed())
	{
		_sealFlag = true;
		unseal(); // Must be unsealed before making changes
	}
	
	if (executeControlWord(BQ27441_CONTROL_SET_CFGUPDATE))
	{
		int16_t timeout = BQ72441_I2C_TIMEOUT;
		while ((timeout--) && (!(flags() & BQ27441_FLAG_CFGUPMODE)))
			//delay(1);
		if (timeout > 0)
			return true;
	}
	
	return false;
}

// Exit configuration mode with the option to perform a resimulation
bool exitConfig(bool resim)
{
	// There are two methods for exiting config mode:
	//    1. Execute the EXIT_CFGUPDATE command
	//    2. Execute the SOFT_RESET command
	// EXIT_CFGUPDATE exits config mode _without_ an OCV (open-circuit voltage)
	// measurement, and without resimulating to update unfiltered-SoC and SoC.
	// If a new OCV measurement or resimulation is desired, SOFT_RESET or
	// EXIT_RESIM should be used to exit config mode.
	if (resim)
	{
		if (softReset())
		{
			int16_t timeout = BQ72441_I2C_TIMEOUT;
			while ((timeout--) && ((flags() & BQ27441_FLAG_CFGUPMODE)))
				//delay(1);
			if (timeout > 0)
			{
				if (_sealFlag) seal(); // Seal back up if we IC was sealed coming in
				return true;
			}
		}
		return false;
	}
	else
	{
		return executeControlWord(BQ27441_CONTROL_EXIT_CFGUPDATE);
	}	
}

// Read the flags() command
uint16_t flags()
{
	return  readWord(BQ27441_COMMAND_FLAGS);
}

// Read the CONTROL_STATUS subcommand of control()
uint16_t status()
{
	return readControlWord(BQ27441_CONTROL_STATUS);
}

/***************************** Private Functions *****************************/

// Check if the BQ27441-G1A is sealed or not.
bool sealed()
{
	uint16_t stat = status();
	return stat & BQ27441_STATUS_SS;
}

// Seal the BQ27441-G1A
bool seal()
{
	return executeControlWord(BQ27441_CONTROL_SEALED);
}

// UNseal the BQ27441-G1A
bool unseal()
{
	// To unseal the BQ27441, write the key to the control
	// command. Then immediately write the same key to control again.
	if (executeControlWord(BQ27441_UNSEAL_KEY))
	{
		return executeControlWord(BQ27441_UNSEAL_KEY);
	}
	return false;
}

// Read the 16-bit opConfig register from extended data
uint16_t opConfig()
{
	return  readWord(BQ27441_EXTENDED_OPCONFIG);
}

// Write the 16-bit opConfig register in extended data
bool writeOpConfig(uint16_t value)
{
	uint8_t opConfigMSB = value >> 8;
	uint8_t opConfigLSB = value & 0x00FF;
	uint8_t opConfigData[2] = {opConfigMSB, opConfigLSB};
	
	// OpConfig register location: BQ27441_ID_REGISTERS id, offset 0
	return writeExtendedData(BQ27441_ID_REGISTERS, 0, opConfigData, 2);
}

// Issue a soft-reset to the BQ27441-G1A
bool softReset()
{
	return executeControlWord(BQ27441_CONTROL_SOFT_RESET);
}

// Read a 16-bit command word from the BQ27441-G1A
uint16_t readWord(uint16_t subAddress)
{
	uint8_t data[2];
	ReadBytes(subAddress, 1, 2, data);
	return ((uint16_t) data[1] << 8) | data[0];
}

// Read a 16-bit subcommand() from the BQ27441-G1A's control()
uint16_t readControlWord(uint16_t function)
{
	uint8_t subCommandMSB = (function >> 8);
	uint8_t subCommandLSB = (function & 0x00FF);
	uint8_t command[2] = {subCommandLSB, subCommandMSB};
	uint8_t data[2] = {0, 0};
	
	I2C_start(BQ72441_I2C_ADDRESS, I2C_Direction_Transmitter, 0);
	WriteBytes((uint8_t) 0, 1);
	WriteBytes(command, 2);
	I2C_stop();
	
	if (ReadBytes((uint8_t) 0, 1, 2, data))
	{
		return ((uint16_t)data[1] << 8) | data[0];
	}
	
	return false;
}

// Execute a subcommand() from the BQ27441-G1A's control()
bool executeControlWord(uint16_t function)
{
	uint8_t subCommandMSB = (function >> 8);
	uint8_t subCommandLSB = (function & 0x00FF);
	uint8_t command[2] = {subCommandLSB, subCommandMSB};
	
	I2C_start(BQ72441_I2C_ADDRESS, I2C_Direction_Transmitter, 0);
	WriteBytes((uint8_t) 0, 1);
	WriteBytes(command, 2);
	I2C_stop();

	return true;
}

/*****************************************************************************
 ************************** Extended Data Commands ***************************
 *****************************************************************************/
 
// Issue a BlockDataControl() command to enable BlockData access
bool blockDataControl()
{
	uint8_t enableByte = 0x00;
	I2C_start(BQ72441_I2C_ADDRESS, I2C_Direction_Transmitter, 0);
	WriteBytes(BQ27441_EXTENDED_CONTROL, 1);
	WriteBytes( &enableByte, 1);
	I2C_stop();
	return true;
}

// Issue a DataClass() command to set the data class to be accessed
bool blockDataClass(uint8_t id)
{

	I2C_start(BQ72441_I2C_ADDRESS, I2C_Direction_Transmitter, 0);
	WriteBytes(BQ27441_EXTENDED_DATACLASS, 1);
	WriteBytes(&id, 1);
	I2C_stop();
	return true;
}

// Issue a DataBlock() command to set the data block to be accessed
bool blockDataOffset(uint8_t offset)
{

	I2C_start(BQ72441_I2C_ADDRESS, I2C_Direction_Transmitter, 0);
	WriteBytes(BQ27441_EXTENDED_DATABLOCK, 1);
	WriteBytes(&offset, 1);
	I2C_stop();
	return true;
}

// Read the current checksum using BlockDataCheckSum()
uint8_t blockDataChecksum()
{
	uint8_t csum;
	ReadBytes(BQ27441_EXTENDED_CHECKSUM, 1, 1, &csum);
	return csum;
}

// Use BlockData() to read a byte from the loaded extended data
uint8_t readBlockData(uint8_t offset)
{
	uint8_t ret;
	uint8_t address = offset + BQ27441_EXTENDED_BLOCKDATA;
	ReadBytes(address, 1, 1, &ret);
	return ret;
}

// Use BlockData() to write a byte to an offset of the loaded data
bool writeBlockData(uint8_t offset, uint8_t data)
{
	uint8_t address = offset + BQ27441_EXTENDED_BLOCKDATA;
	I2C_start(BQ72441_I2C_ADDRESS, I2C_Direction_Transmitter, 0);
	WriteBytes(&address, 1);
	WriteBytes( &data, 1);
	I2C_stop();
	return true;
}

// Read all 32 bytes of the loaded extended data and compute a 
// checksum based on the values.
uint8_t computeBlockChecksum()
{
	uint8_t data[32];
	ReadBytes(BQ27441_EXTENDED_BLOCKDATA, 1, 32, data);

	uint8_t csum = 0;
	for (int i=0; i<32; i++)
	{
		csum += data[i];
	}
	csum = 255 - csum;
	
	return csum;
}

// Use the BlockDataCheckSum() command to write a checksum value
bool writeBlockChecksum(uint8_t csum)
{
	I2C_start(BQ72441_I2C_ADDRESS, I2C_Direction_Transmitter, 0);
	WriteBytes((uint8_t *) BQ27441_EXTENDED_CHECKSUM, 1);
	WriteBytes(&csum, 1);
	I2C_stop();
	return true;
}

// Read a byte from extended data specifying a class ID and position offset
uint8_t readExtendedData(uint8_t classID, uint8_t offset)
{
	uint8_t retData = 0;
	if (!_userConfigControl) enterConfig(false);
		
	if (!blockDataControl()) // // enable block data memory control
		return false; // Return false if enable fails
	if (!blockDataClass(classID)) // Write class ID using DataBlockClass()
		return false;
	
	blockDataOffset(offset / 32); // Write 32-bit block offset (usually 0)
	
	computeBlockChecksum(); // Compute checksum going in
	uint8_t oldCsum = blockDataChecksum();
	/*for (int i=0; i<32; i++)
		Serial.print(String(readBlockData(i)) + " ");*/
	retData = readBlockData(offset % 32); // Read from offset (limit to 0-31)
	
	if (!_userConfigControl) exitConfig(true);
	
	return retData;
}

// Write a specified number of bytes to extended data specifying a 
// class ID, position offset.
bool writeExtendedData(uint8_t classID, uint8_t offset, uint8_t * data, uint8_t len)
{
	if (len > 32)
		return false;
	
	if (!_userConfigControl) enterConfig(false);
	
	if (!blockDataControl()) // // enable block data memory control
		return false; // Return false if enable fails
	if (!blockDataClass(classID)) // Write class ID using DataBlockClass()
		return false;
	
	blockDataOffset(offset / 32); // Write 32-bit block offset (usually 0)
	computeBlockChecksum(); // Compute checksum going in
	uint8_t oldCsum = blockDataChecksum();

	// Write data bytes:
	for (int i = 0; i < len; i++)
	{
		// Write to offset, mod 32 if offset is greater than 32
		// The blockDataOffset above sets the 32-bit block
		writeBlockData((offset % 32) + i, data[i]);
	}
	
	// Write new checksum using BlockDataChecksum (0x60)
	uint8_t newCsum = computeBlockChecksum(); // Compute the new checksum
	writeBlockChecksum(newCsum);

	if (!_userConfigControl) exitConfig(true);
	
	return true;
}

/*****************************************************************************
 ************************ I2C Read and Write Routines ************************
 *****************************************************************************/

// Read a specified number of bytes over I2C at a given subAddress
int16_t ReadBytes(uint8_t subAddress, uint8_t write_count, uint8_t read_count, uint8_t * data)
{
	int16_t timeout = BQ72441_I2C_TIMEOUT;
	I2C_start(BQ72441_I2C_ADDRESS, I2C_Direction_Transmitter, 0);
	WriteBytes(&subAddress, write_count);
	I2C_start(BQ72441_I2C_ADDRESS, I2C_Direction_Receiver, 1);
	for (int i = 0; i < read_count; i++) {
		if (i !=  (read_count - 1)) {
			 I2C_AcknowledgeConfig(I2CPERIPHSEL, ENABLE);
			 while(!I2C_CheckEvent(I2CPERIPHSEL, I2C_EVENT_MASTER_BYTE_RECEIVED));
			 data[i]=I2C_ReceiveData(I2CPERIPHSEL);

		}
		else {
			 I2C_AcknowledgeConfig(I2CPERIPHSEL, DISABLE);
			 while(!I2C_CheckEvent(I2CPERIPHSEL, I2C_EVENT_MASTER_BYTE_RECEIVED));
			 data[i]=I2C_ReceiveData(I2CPERIPHSEL);
		}
	}
	I2C_stop();
	return timeout;
}

// Write a specified number of bytes over I2C to a given subAddress
void WriteBytes(uint8_t * subAddress, uint8_t count)
{
	for (int i = 0; i < count; i++) {
		 I2C_SendData(I2CPERIPHSEL, subAddress[i]);
		 while(!I2C_CheckEvent(I2CPERIPHSEL, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	}
}

