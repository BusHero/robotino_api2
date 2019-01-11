#ifndef WIN32

#include <fcntl.h>
#include <libpcan.h>
#include <stdio.h>
#include <iostream>

#include "rec/pcan/PCanDev.h"
#include "rec/pcan/Exception.h"

namespace rec
{
	namespace pcan
	{
		class PCanDevImpl
		{
			public:       
				PCanDevImpl( PCanDev* dev, CANBaudRate baudRate = br1M, CANMsgType msgType = mtStandard);
				~PCanDevImpl();

				bool busReady();
				void write(CanMessage& message);
				void read(CanMessage& message);
				std::string versionInfo() const;
				void msgFilter(uint32 fromID, uint32 toID, CANMsgType msgType);
				void resetFilter();

			private:
				PCanDev* _dev;
				HANDLE _devHandle;
				static const unsigned int RESPONSE_TIME = 100000;	// microseconds
		};
	}
}

using namespace rec::pcan;

PCanDevImpl::PCanDevImpl( PCanDev* dev, CANBaudRate baudRate, CANMsgType msgType )
: _dev( dev )
, _devHandle(0)
{
	HANDLE tmpHandle = 0;

	/* Open PCAN device. */
	tmpHandle = LINUX_CAN_Open("/dev/pcanusb0", O_RDWR);
	if (tmpHandle == 0)
	{
		throw pcan::Exception("PCAN: error on opening device.");
	}

	/* Clear status. */
	CAN_Status(tmpHandle);

	/* Initializing (setting baud rate and message type). */
	if ((CANResult)CAN_Init(tmpHandle, (WORD)baudRate, (int)msgType) != CAN_ERR_OK)
	{
		throw pcan::Exception("PCAN: error on initializing device.");
	}

	_devHandle = tmpHandle;
}

PCanDevImpl::~PCanDevImpl()
{
	if (_devHandle != 0)
	{
		if ((CANResult)CAN_Close(_devHandle) == CAN_ERR_OK)
			_devHandle = 0;
		else
			throw pcan::Exception("PCAN: error on closing device.");
	}
}

bool PCanDevImpl::busReady()
{
	if (_devHandle == 0)
		throw pcan::Exception("PCAN: device not opened.");

	return ((CANResult)CAN_Status(_devHandle) == canOK);
}

void PCanDevImpl::write(CanMessage& message)
{
	if (_devHandle == 0)
		throw pcan::Exception("PCAN: device not opened.");

	TPCANMsg pcanMsg;
	pcanMsg.ID = (DWORD)message.id();
	pcanMsg.MSGTYPE = (BYTE)message.type();
	pcanMsg.LEN = (BYTE)message.length();
	memcpy(pcanMsg.DATA, message.data(), message.length());

	CANResult wrRes = (CANResult)CAN_Write(_devHandle, &pcanMsg);
	if (wrRes == canOK)
	{
		/* Log outgoing CAN Message. */
		_dev->log( "Write:" + message.toString() );
		//LOG_OUT(pcanMsg.DATA, pcanMsg.LEN)
	}
	else
	{
		std::cerr << "PCAN: status = " << wrRes << std::endl;
		throw pcan::Exception("PCAN: error on sending data.");
	}
}

void PCanDevImpl::read(CanMessage& message)
{
	if (_devHandle == 0)
		throw pcan::Exception("PCAN: device not opened.");

	TPCANRdMsg pcanMsg;
	CANResult rdRes;

	rdRes = (CANResult)LINUX_CAN_Read_Timeout(_devHandle, &pcanMsg, RESPONSE_TIME);

	if (rdRes == canOK)
	{
		message.setId((uint32)pcanMsg.Msg.ID);
		message.setType((CANMsgType)pcanMsg.Msg.MSGTYPE);
		message.setData((unsigned char*)pcanMsg.Msg.DATA, (int)pcanMsg.Msg.LEN);

		/* Log incoming CAN message. */
		_dev->log( "Read:" + message.toString() );
		//LOG_IN(pcanMsg.Msg.DATA, pcanMsg.Msg.LEN)
	}
	else
	{
		std::cerr << "PCAN: error on reading data (status = " << rdRes << ")" << std::endl;
		throw pcan::Exception("PCAN: error on reading data.");
	}
}

std::string PCanDevImpl::versionInfo() const
{
	if (_devHandle == 0)
		throw pcan::Exception("PCAN: device not opened.");

	char verStr[VERSIONSTRING_LEN];
	if ((CANResult)CAN_VersionInfo(_devHandle, verStr) != CAN_ERR_OK)
		throw pcan::Exception("PCAN: error on requesting driver version.");

	return std::string(verStr);
}

void PCanDevImpl::msgFilter(uint32 fromID, uint32 toID, CANMsgType msgType)
{
	if (_devHandle == 0)
		throw pcan::Exception("PCAN: device not opened.");

	if ((CANResult)CAN_MsgFilter(_devHandle, (DWORD)fromID, (DWORD)toID, (int)msgType) != CAN_ERR_OK)
		throw pcan::Exception("PCAN: error on adding message filter.");
}

void PCanDevImpl::resetFilter()
{
	if (_devHandle == 0)
		throw pcan::Exception("PCAN: device not opened.");

	if ((CANResult)CAN_ResetFilter(_devHandle) != CAN_ERR_OK)
		throw pcan::Exception("PCAN: error on resetting message filter.");
}

PCanDev::PCanDev()
	: _impl( new PCanDevImpl( this ) )
{
}

PCanDev::~PCanDev()
{
	delete _impl;
}

bool PCanDev::busReady()
{
	return _impl->busReady();
}

void PCanDev::write(CanMessage& message)
{
	_impl->write(message);
}

void PCanDev::read(CanMessage& message)
{
	_impl->read(message);
}

std::string PCanDev::versionInfo() const
{
	return _impl->versionInfo();
}

void PCanDev::msgFilter(uint32 fromID, uint32 toID, CANMsgType msgType)
{
	_impl->msgFilter(fromID, toID, msgType);
}

void PCanDev::resetFilter()
{
	_impl->resetFilter();
}


#endif	// WIN32
