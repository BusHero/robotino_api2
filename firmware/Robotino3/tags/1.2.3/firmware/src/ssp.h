#ifndef _SSP1_H_
#define _SSP1_H_

typedef enum _SSP_Status
{
	SSP_OK					= 0,
	SSP_UNSUPPORTED_CHANNEL	= 1,
} SSP_Status;

typedef enum _SSP_DSS
{
	SSP_DSS_4Bit	= 0x03,
	SSP_DSS_5Bit	= 0x04,
	SSP_DSS_6Bit	= 0x05,
	SSP_DSS_7Bit	= 0x06,
	SSP_DSS_8Bit	= 0x07,
	SSP_DSS_9Bit	= 0x08,
	SSP_DSS_10Bit	= 0x09,
	SSP_DSS_11Bit	= 0x0A,
	SSP_DSS_12Bit	= 0x0B,
	SSP_DSS_13Bit	= 0x0C,
	SSP_DSS_14Bit	= 0x0D,
	SSP_DSS_15Bit	= 0x0E,
	SSP_DSS_16Bit	= 0x0F,
} SSP_DSS;

typedef enum _SSP_FRF
{
	SSP_FRF_SPI			= 0x00,
	SSP_FRF_TI			= 0x10,
	SSP_FRF_Microwire	= 0x20,
} SSP_FRF;

typedef enum _SSP_CPOL
{
	SSP_CPOL_LOW	= 0x00,
	SSP_CPOL_HIGH	= 0x40,
} SSP_CPOL;

typedef enum _SSP_CPHA
{
	SSP_CPHA_1ST = 0x00,
	SSP_CPHA_2ND = 0x80,
} SSP_CPHA;

SSP_Status ssp_init( unsigned int ch, SSP_DSS dss, SSP_FRF frf, SSP_CPOL cpol, SSP_CPHA cpha, unsigned int frequency );
SSP_Status ssp_done( unsigned int ch );

void ssp_select_gyro( void );
void ssp_deselect_gyro( void );

SSP_Status ssp_readBuf( unsigned int ch, unsigned char reg, unsigned char* buffer, unsigned int length );
SSP_Status ssp_writeBuf( unsigned int ch, unsigned char reg, const unsigned char* buffer, unsigned int length );

#endif
