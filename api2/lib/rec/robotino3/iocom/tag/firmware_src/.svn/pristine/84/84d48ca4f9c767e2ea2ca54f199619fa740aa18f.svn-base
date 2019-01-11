#ifndef _I2C_H_
#define _I2C_H_

typedef enum _I2C_Status
{
	I2C_OK					= 0,
	I2C_UNSUPPORTED_CHANNEL	= 1,
	I2C_TIMEOUT				= 2,
	I2C_START_FAILED		= 3,
	I2C_SEND_SLA_ADR_FAILED	= 4,
	I2C_WRITE_FAILED		= 5,
	I2C_READ_FAILED			= 6,
} I2C_Status;

I2C_Status i2c_init( unsigned int ch, unsigned int freq );
I2C_Status i2c_done( unsigned int ch );

I2C_Status i2c_readBuf( unsigned int ch, unsigned char slaveAddr, unsigned char reg, unsigned char* buffer, unsigned int length );
I2C_Status i2c_writeBuf( unsigned int ch, unsigned char slaveAddr, unsigned char reg, const unsigned char* buffer, unsigned int length );
I2C_Status i2c_writeByte( unsigned int ch, unsigned char slaveAddr, unsigned char reg, unsigned char value );

#endif
