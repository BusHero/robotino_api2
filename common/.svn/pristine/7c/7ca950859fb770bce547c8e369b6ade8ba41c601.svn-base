#include "rec/vema/VEMA.h"
#include "rec/pcan/Exception.h"
#include "rec/pcan/PCanDev.h"
#include <iostream>
#include <stdlib.h>

#ifdef WIN32
#include <windows.h>
#endif

using namespace rec::vema;

class MyCAN : public rec::pcan::PCanDev
{
public:
	MyCAN( VEMA* vema )
		: _vema( vema )
	{
	}

	void log( const std::string& message )
	{
		_vema->log( message );
	}

private:
	VEMA* _vema;
};

VEMA::VEMA()
: _canDevice( NULL )
, _canID(0x601)
{
	try
	{
		_canDevice = new MyCAN( this );
	}
	catch( const rec::pcan::Exception& e )
	{
		throw rec::vema::Exception( "CAN: " + e.getMessage() );
	}

	_intern2extern[0] = 0;
	_intern2extern[1] = 2;
	_intern2extern[2] = 1;
	_intern2extern[3] = 6;
	_intern2extern[4] = 5;
	_intern2extern[5] = 7;
	_intern2extern[6] = 4;
	_intern2extern[7] = 3;

	std::map<int,int>::const_iterator iter = _intern2extern.begin();
	while( _intern2extern.end() != iter )
	{
		_extern2intern[iter->second] = iter->first;
		++iter;
	}
}

VEMA::VEMA(uint32 canID)
	: _canID(canID)
{
}

VEMA::~VEMA()
{
	delete _canDevice;
}

void VEMA::setCANId(uint32 id)
{
	_canID = id;
}

void VEMA::sendCmd(unsigned char* data, unsigned int length)
{
	if (length == 0)
		throw vema::Exception("Too few command bytes.");

	/* CAN Message objects. */
	rec::pcan::CanMessage txMsg(_canID, rec::pcan::mtStandard, length, data);
	rec::pcan::CanMessage rxMsg;

	try
	{
		send(txMsg, txMsg);
	}
	catch (const vema::Exception& e)
	{
		std::cerr << e.what() << std::endl;
		throw vema::Exception("VEMA: Error during direct mode command.");
	}

}

void VEMA::startClosedLoop(VEMAComponent chnlNr)
{
	sendSimpleCmd(chnlNr, CMD_START_CLOSED_LOOP);
}

void VEMA::stopClosedLoop(VEMAComponent chnlNr)
{
	sendSimpleCmd(chnlNr, CMD_STOP_CLOSED_LOOP);
}

VEMAStatus VEMA::getStatus(VEMAComponent chnlNr)
{
	checkTarget(chnlNr);

	/* CAN transmit message object. */
	unsigned char msgBuf[2];
	msgBuf[0] = CMD_GET_STATUS | chnlNr;
	msgBuf[1] = checksum(msgBuf, 1);
	rec::pcan::CanMessage txMsg(_canID, rec::pcan::mtStandard, 2, msgBuf);

	/* CAN receive message object. */
	rec::pcan::CanMessage rxMsg;

	try
	{
		send(txMsg, rxMsg);
	}
	catch (const vema::Exception& e)
	{
		std::cerr << e.what() << std::endl;
		throw vema::Exception("VEMA: Error during getting channel status.");
	}

	return (VEMAStatus(rxMsg.data(1)));
}

VEMAError VEMA::getError(VEMAComponent chnlNr)
{
	checkTarget(chnlNr);

	/* CAN transmit message object. */
	unsigned char msgBuf[2];
	msgBuf[0] = CMD_GET_ERROR | chnlNr;
	msgBuf[1] = checksum(msgBuf, 1);
	rec::pcan::CanMessage txMsg(_canID, rec::pcan::mtStandard, 2, msgBuf);

	/* CAN receive message object. */
	rec::pcan::CanMessage rxMsg;

	try
	{
		send(txMsg, rxMsg);
	}
	catch (const vema::Exception& e)
	{
		std::cerr << e.what() << std::endl;
		throw vema::Exception("VEMA: Error during getting channel error state.");
	}

	return (VEMAError(rxMsg.data(1)));
}

int32 VEMA::getPressure(VEMAComponent chnlNr)
{
	checkTarget(chnlNr);

	chnlNr = static_cast<VEMAComponent>( _extern2intern[chnlNr-1]+1 );

	/* CAN transmit message object. */
	unsigned char msgBuf[2];
	msgBuf[0] = CMD_GET_PRESSURE | chnlNr;
	msgBuf[1] = checksum(msgBuf, 1);
	rec::pcan::CanMessage txMsg(_canID, rec::pcan::mtStandard, 2, msgBuf);

	/* CAN receive message object. */
	rec::pcan::CanMessage rxMsg;

	try
	{
		send(txMsg, rxMsg);
	}
	catch (const vema::Exception& e)
	{
		std::cerr << e.what() << std::endl;
		throw vema::Exception("VEMA: Error during getting pressure.");
	}

	/* Extract pressure from CAN message. */
	int32 pressure = (uint32)(rxMsg.data(1) & 0x07) << 8;
	pressure += (uint32)(rxMsg.data(2));
	if ((rxMsg.data(1) & 0x08) != 0)	// negative?
		pressure = -pressure;

	return pressure;
}

void VEMA::getAllPressures(int32* values)
{
	int32 tmp = 0;
	unsigned char msgBuf[2];
	rec::pcan::CanMessage txMsg;
	rec::pcan::CanMessage rxMsg;

	/* ////////// Channel Group 1 ////////// */

	/* CAN transmit message object. */
	msgBuf[0] = CMD_GET_PRESSURE | ChannelGroup1;
	msgBuf[1] = checksum(msgBuf, 1);
	txMsg = rec::pcan::CanMessage(_canID, rec::pcan::mtStandard, 2, msgBuf);

	try
	{
		send(txMsg, rxMsg);
	}
	catch (const vema::Exception& e)
	{
		std::cerr << e.what() << std::endl;
		throw vema::Exception("VEMA: Error during getting pressure.");
	}

	/* Extract pressure value for channel 1. */
	tmp = (uint32)(rxMsg.data(1) & 0x7F) << 4;
	tmp += (uint32)(rxMsg.data(2) & 0xF0) >> 4;
	if ((rxMsg.data(1) & 0x80) != 0)	// negative?
		tmp = -tmp;
	values[0] = tmp;

	/* Extract pressure value for channel 2. */
	tmp = (uint32)(rxMsg.data(2) & 0x07) << 8;
	tmp += (uint32)rxMsg.data(3);
	if ((rxMsg.data(2) & 0x08) != 0)	// negative?
		tmp = -tmp;
	values[1] = tmp;

	/* Extract pressure value for channel 3. */
	tmp = (uint32)(rxMsg.data(4) & 0x7F) << 4;
	tmp += (uint32)(rxMsg.data(5) & 0xF0) >> 4;
	if ((rxMsg.data(4) & 0x80) != 0)	// negative?
		tmp = -tmp;
	values[2] = tmp;

	/* Extract pressure value for channel 4. */
	tmp = (uint32)(rxMsg.data(5) & 0x07) << 8;
	tmp += (uint32)rxMsg.data(6);
	if ((rxMsg.data(5) & 0x08) != 0)	// negative?
		tmp = -tmp;
	values[3] = tmp;

	/* ////////// Channel Group 2 ////////// */

	msgBuf[0] = CMD_GET_PRESSURE | ChannelGroup2;
	msgBuf[1] = checksum(msgBuf, 1);
	txMsg = rec::pcan::CanMessage(_canID, rec::pcan::mtStandard, 2, msgBuf);

	try
	{
		send(txMsg, rxMsg);
	}
	catch (const vema::Exception& e)
	{
		std::cerr << e.what() << std::endl;
		throw vema::Exception("VEMA: Error during getting pressure.");
	}

	/* Extract pressure value for channel 5. */
	tmp = (uint32)(rxMsg.data(1) & 0x7F) << 4;
	tmp += (uint32)(rxMsg.data(2) & 0xF0) >> 4;
	if ((rxMsg.data(1) & 0x80) != 0)	// negative?
		tmp = -tmp;
	values[4] = tmp;

	/* Extract pressure value for channel 6. */
	tmp = (uint32)(rxMsg.data(2) & 0x07) << 8;
	tmp += (uint32)rxMsg.data(3);
	if ((rxMsg.data(2) & 0x08) != 0)	// negative?
		tmp = -tmp;
	values[5] = tmp;

	/* Extract pressure value for channel 7. */
	tmp = (uint32)(rxMsg.data(4) & 0x7F) << 4;
	tmp += (uint32)(rxMsg.data(5) & 0xF0) >> 4;
	if ((rxMsg.data(4) & 0x80) != 0)	// negative?
		tmp = -tmp;
	values[6] = tmp;

	/* Extract pressure value for channel 8. */
	tmp = (uint32)(rxMsg.data(5) & 0x07) << 8;
	tmp += (uint32)rxMsg.data(6);
	if ((rxMsg.data(5) & 0x08) != 0)	// negative?
		tmp = -tmp;
	values[7] = tmp;

	int tmpval[8];
	memcpy( tmpval, values, 8 * sizeof( int ) );
	for( int i=0; i<8; ++i )
	{
		*(values+i) = *(tmpval+_intern2extern[i]);
	}
}

int32 VEMA::setPressure(VEMAComponent chnlNr, int32 pressure)
{
	checkTarget(chnlNr);

	chnlNr = static_cast<VEMAComponent>( _extern2intern[chnlNr-1]+1 );

	/* CAN transmit message object. */
	unsigned char msgBuf[4];
	uint32 tmpPressure = abs(pressure);
	msgBuf[0] = CMD_SET_PRESSURE | chnlNr;
	msgBuf[1] = (unsigned char)(tmpPressure >> 8) & 0x07;
	msgBuf[2] = (unsigned char)(tmpPressure & 0xFF);
	if (pressure < 0)
		msgBuf[1] |= 0x08;	// set sign bit
	msgBuf[3] = checksum(msgBuf, 3);
	rec::pcan::CanMessage txMsg(_canID, rec::pcan::mtStandard, 4, msgBuf);

	/* CAN receive message object. */
	rec::pcan::CanMessage rxMsg;

	try
	{
		send(txMsg, rxMsg);
	}
	catch (const vema::Exception& e)
	{
		std::cerr << e.what() << std::endl;
		throw vema::Exception("VEMA: Error during setting pressure.");
	}

	/* Extract current pressure from CAN message. */
	int32 currentPressure = (uint32)(rxMsg.data(1) & 0x07) << 8;
	currentPressure += (uint32)(rxMsg.data(2));
	if ((rxMsg.data(1) & 0x08) != 0)	// negative?
		currentPressure = -currentPressure;

	return currentPressure;
}

void VEMA::setAllPressures(int* values)
{
	int tmpval[8];
	memcpy( tmpval, values, 8 * sizeof( int ) );
	for( int i=0; i<8; ++i )
	{
		*(values+i) = *(tmpval+_extern2intern[i]);
	}

	uint32 tmp = 0;
	unsigned char msgBuf[8];
	rec::pcan::CanMessage txMsg;
	rec::pcan::CanMessage rxMsg;

	/* ////////// Channel Group 1 ////////// */

	/* CAN transmit message object. */
	msgBuf[0] = CMD_SET_PRESSURE | ChannelGroup1;

	/* Set pressure value for channel 1. */
	tmp = abs(values[0]);
	msgBuf[1] = (unsigned char)(tmp >> 4);
	if (values[0] < 0)	// sign
		msgBuf[1] |= 0x80;
	msgBuf[2] = (tmp & 0x0000000F) << 4;

	/* Set pressure value for channel 2. */
	tmp = abs(values[1]);
	msgBuf[2] |= tmp >> 8;
	if (values[1] < 0)	// sign
		msgBuf[2] |= 0x08;
	msgBuf[3] = tmp & 0x000000FF;

	/* Set pressure value for channel 3. */
	tmp = abs(values[2]);
	msgBuf[4] = (unsigned char)(tmp >> 4);
	if (values[2] < 0)	// sign
		msgBuf[4] |= 0x80;
	msgBuf[5] = (tmp & 0x0000000F) << 4;

	/* Set pressure value for channel 4. */
	tmp = abs(values[3]);
	msgBuf[5] |= tmp >> 8;
	if (values[3] < 0)	// sign
		msgBuf[5] |= 0x08;
	msgBuf[6] = tmp & 0x000000FF;

	/* Calculate and append checksum. */
	msgBuf[7] = checksum(msgBuf, 7);
	txMsg = rec::pcan::CanMessage(_canID, rec::pcan::mtStandard, 8, msgBuf);

	try
	{
		send(txMsg, rxMsg);
	}
	catch (const vema::Exception& e)
	{
		std::cerr << e.what() << std::endl;
		throw vema::Exception("VEMA: Error during setting pressures (group 1).");
	}

	/* CAN transmit message object. */
	msgBuf[0] = CMD_SET_PRESSURE | ChannelGroup2;

	/* Set pressure value for channel 5. */
	tmp = abs(values[4]);
	msgBuf[1] = (unsigned char)(tmp >> 4);
	if (values[4] < 0)	// sign
		msgBuf[1] |= 0x80;
	msgBuf[2] = (tmp & 0x0000000F) << 4;

	/* Set pressure value for channel 6. */
	tmp = abs(values[5]);
	msgBuf[2] |= tmp >> 8;
	if (values[5] < 0)	// sign
		msgBuf[2] |= 0x08;
	msgBuf[3] = tmp & 0x000000FF;

	/* Set pressure value for channel 7. */
	tmp = abs(values[6]);
	msgBuf[4] = (unsigned char)(tmp >> 4);
	if (values[6] < 0)	// sign
		msgBuf[4] |= 0x80;
	msgBuf[5] = (tmp & 0x0000000F) << 4;

	/* Set pressure value for channel 8. */
	tmp = abs(values[7]);
	msgBuf[5] |= tmp >> 8;
	if (values[7] < 0)	// sign
		msgBuf[5] |= 0x08;
	msgBuf[6] = tmp & 0x000000FF;

	/* Calculate and append checksum. */
	msgBuf[7] = checksum(msgBuf, 7);
	txMsg = rec::pcan::CanMessage(_canID, rec::pcan::mtStandard, 8, msgBuf);

	try
	{
		send(txMsg, rxMsg);
	}
	catch (const vema::Exception& e)
	{
		std::cerr << e.what() << std::endl;
		throw vema::Exception("VEMA: Error during setting pressures (group 2).");
	}
}

void VEMA::resetError(VEMAComponent chnlNr)
{
	sendSimpleCmd(chnlNr, CMD_RESET_ERROR);
}

void VEMA::pressurize(VEMAComponent chnlNr)
{
	sendSimpleCmd(chnlNr, CMD_PRESSURIZE);
}

void VEMA::exhaust(VEMAComponent chnlNr)
{
	sendSimpleCmd(chnlNr, CMD_EXHAUST);
}

void VEMA::closeValve(VEMAComponent chnlNr)
{
	sendSimpleCmd(chnlNr, CMD_CLOSE_VALVE);
}

void VEMA::startCalibration(VEMAComponent chnlNr)
{
	sendSimpleCmd(chnlNr, CMD_START_CALIBRATION);
}

void VEMA::setCalibration(VEMAComponent chnlNr)
{
	sendSimpleCmd(chnlNr, CMD_SET_CALIBRATION);
}

void VEMA::setDigitalOut(unsigned int nr)
{
	if (nr < 1 || nr > 2)
		throw vema::Exception("Invalid digital output specified.");

	/* Set output. */
	sendSimpleCmd((VEMAComponent)nr, CMD_SET_DIGITAL_OUT);
}

void VEMA::resetDigitalOut(unsigned int nr)
{
	if (nr < 1 || nr > 2)
		throw vema::Exception("Invalid digital output specified.");

	/* Set output. */
	sendSimpleCmd((VEMAComponent)nr, CMD_RESET_DIGITAL_OUT);
}

/*!
 * ********** Private functions **********
 */

void VEMA::checkTarget(VEMAComponent target) const
{
	if ((target < Channel1) || (target > AllChannels) || (target > Channel8 && target < ChannelGroup1))
		throw vema::Exception("Invalid VEMA component specified.");
}

unsigned char VEMA::checksum(unsigned char* data, unsigned int length)
{
	unsigned char sum = 0;
	while (length > 0)
		sum += data[-1 + length--];
	return (0xFF - sum);
}

void VEMA::sendSimpleCmd(VEMAComponent chnlNr, unsigned char cmd)
{
	checkTarget(chnlNr);

	/* CAN transmit message object. */
	unsigned char msgBuf[2];
	msgBuf[0] = cmd | chnlNr;
	msgBuf[1] = checksum(msgBuf, 1);
	rec::pcan::CanMessage txMsg(_canID, rec::pcan::mtStandard, 2, msgBuf);

	/* CAN receive message object. */
	rec::pcan::CanMessage rxMsg;

	try
	{
		send(txMsg, rxMsg);
	}
	catch (const vema::Exception& e)
	{
		std::cerr << e.what() << std::endl;
		throw vema::Exception("VEMA: error.");
	}

}

void VEMA::send(rec::pcan::CanMessage& txMsg, rec::pcan::CanMessage& rxMsg)
{
	try
	{
		/* Send CAN Message. */
		_canDevice->write(txMsg);

#ifdef WIN32
		Sleep(RESPONSE_TIME);
#endif

		/* Read response. */
		_canDevice->read(rxMsg);

		/* Check first response byte. */
		unsigned char cmdByte = txMsg.data(0);
		unsigned char responseCmdByte = rxMsg.data(0);

		if (cmdByte == 0xFF - responseCmdByte)		// complement on one?
		{
			/* Valve terminal returned an error. */
			std::cerr << "VEMA: error response." << std::endl;
			throw vema::Exception("VEMA: error response from valve terminal.");
		}
		else if (responseCmdByte != cmdByte)
		{
			/* Unknown/invalid response. */
			std::cerr << "VEMA: unexpected response." << std::endl;
			throw vema::Exception("VEMA: unexpected response from valve terminal.");
		}

		/* Check checksum */
		unsigned char sum = 0;
		for (unsigned int i = 0; i < rxMsg.length(); i++)
			sum += rxMsg.data(i);
		if (sum != 0xFF)
		{
			/* Unknown/invalid response. */
			std::cerr << "VEMA: response checksum error." << std::endl;
			throw vema::Exception("VEMA: valve terminal response checksum error.");
		}
	}
	catch (const pcan::Exception& e)
	{
		std::cerr << "VEMA: " << e.what() << std::endl;
		throw vema::Exception("VEMA: CAN communication error with valve terminal.");
	}

}
