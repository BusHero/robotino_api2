#ifdef WIN32

#include <windows.h>
#include <iostream>
#include "Pcan_usb.h"

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
		};

	}
}

using namespace rec::pcan;

PCanDevImpl::PCanDevImpl( PCanDev* dev, CANBaudRate baudRate, CANMsgType msgType )
: _dev( dev )
{
	/* Clear status. */
	if ((CANResult)CAN_Status() == CAN_ERR_NODRIVER)
	{
		throw pcan::Exception("No VxD: No PCAN driver loaded.");
	}

	/* Get version info. */
	char verStr[255];
	if ((CANResult)CAN_VersionInfo(verStr) == CAN_ERR_OK)
	{
		printf("Driver version: %s\n", verStr);
	}
	else
	{
		throw pcan::Exception("Error on requesting PCAN driver version. CAN device connected?");
	}

	/* Initializing (setting baud rate and message type). */
	if ((CANResult)CAN_Init((WORD)baudRate, (int)msgType) != CAN_ERR_OK)
	{
		throw pcan::Exception("Error on initializing PCAN device.");
	}
}


PCanDevImpl::~PCanDevImpl()
{
	if ((CANResult)CAN_Close() != CAN_ERR_OK)
	{
		throw pcan::Exception("Error on closing PCAN device.");
	}
}

bool PCanDevImpl::busReady()
{
	return ((CANResult)CAN_Status() == canOK);
}

void PCanDevImpl::write(CanMessage& message)
{
	TPCANMsg pcanMsg;
	pcanMsg.ID = (DWORD)message.id();
	pcanMsg.MSGTYPE = (BYTE)message.type();
	pcanMsg.LEN = (BYTE)message.length();
	memcpy(pcanMsg.DATA, message.data(), message.length());

	CANResult wrRes = (CANResult)CAN_Write(&pcanMsg);
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
	TPCANMsg pcanMsg;
	CANResult rdRes = (CANResult)CAN_Read(&pcanMsg);

	if (rdRes == canOK)
	{
		message.setId((uint32)pcanMsg.ID);
		message.setType((CANMsgType)pcanMsg.MSGTYPE);
		message.setData((unsigned char*)pcanMsg.DATA, (int)pcanMsg.LEN);

		/* Log incoming CAN message. */
		_dev->log( "Read:" + message.toString() );
		//LOG_IN(pcanMsg.DATA, pcanMsg.LEN)
	}
	else
	{
		std::cerr << "PCAN: error on reading data (status = " << rdRes << ")" << std::endl;
		throw pcan::Exception("PCAN: error on reading data.");
	}
}

std::string PCanDevImpl::versionInfo() const
{
	char verStr[255];
	if ((CANResult)CAN_VersionInfo(verStr) != CAN_ERR_OK)
		throw pcan::Exception("PCAN: error on requesting driver version.");

	return std::string(verStr);
}

void PCanDevImpl::msgFilter(uint32 fromID, uint32 toID, CANMsgType msgType)
{
	if ((CANResult)CAN_MsgFilter((DWORD)fromID, (DWORD)toID, (int)msgType) != CAN_ERR_OK)
		throw pcan::Exception("PCAN: error on adding message filter.");
}

void PCanDevImpl::resetFilter()
{
	if ((CANResult)CAN_ResetFilter() != CAN_ERR_OK)
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
