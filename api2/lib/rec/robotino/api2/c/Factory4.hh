//  Copyright (C) 2004-2008, Robotics Equipment Corporation GmbH

#ifndef _REC_ROBOTINO_API2_C_FACTORY4_HH_
#define _REC_ROBOTINO_API2_C_FACTORY4_HH_

#include "rec/robotino/api2/Factory4.h"
#include "rec/robotino/api2/c/Factory4.h"

#include <QtCore>

class Factory4Extended : public rec::robotino::api2::Factory4
{
public:
	Factory4Extended()
		: _mclayoutEventCb(0)
		, _mcstatusEventCb(0)
		, _pathnetworkEventCb(0)
		, _localizationModeEventCb(0)
		, _smartlogEventCb(0)
		, _smartnavigationplanEventCb(0)
		, _smartlocationsEventCb(0)
		, _smartstationsEventCb(0)
		, _smartrobotinfoEventCb(0)
		, _smartmyrobotidEventCb(0)
		, _smartjoblistEventCb(0)
		, _mapDirEventCb(0)
		, _mapDirWithDataEventCb(0)
		, _fleetcom_responseEventCb(0)
		, _mapDir_responseEventCb(0)
		, _smartlicenseEventCb(0)
		, _mapEventCb(0)
		, _mapPlannerEventCb(0)
	{

	}

	void setMclayoutEventCb(Factory4_data1Cb_t func)
	{
		_mclayoutEventCb = func;
	}

	void setMapEventCb(Factory4_mapEventCb_t func)
	{
		_mapEventCb = func;
	}

	Factory4_data1Cb_t _mclayoutEventCb;
	Factory4_data1Cb_t _mcstatusEventCb;
	Factory4_data1Cb_t _pathnetworkEventCb;
	Factory4_data1Cb_t _localizationModeEventCb;
	Factory4_data1Cb_t _smartlogEventCb;
	Factory4_data1Cb_t _smartnavigationplanEventCb;
	Factory4_data1Cb_t _smartlocationsEventCb;
	Factory4_data1Cb_t _smartstationsEventCb;
	Factory4_data1Cb_t _smartrobotinfoEventCb;
	Factory4_data1Cb_t _smartmyrobotidEventCb;
	Factory4_data1Cb_t _smartjoblistEventCb;
	Factory4_data1Cb_t _mapDirEventCb;
	Factory4_data2Cb_t _mapDirWithDataEventCb;
	Factory4_data1Cb_t _fleetcom_responseEventCb;
	Factory4_data2Cb_t _mapDir_responseEventCb;
	Factory4_data1Cb_t _smartlicenseEventCb;

	Factory4_mapEventCb_t _mapEventCb;
	Factory4_mapEventCb_t _mapPlannerEventCb;

private:
	

	void mclayoutEvent(const char* data, const unsigned int dataSize)
	{
		if (_mclayoutEventCb)
		{
			_mclayoutEventCb(data, (int)dataSize);
		}
	}

	void mcstatusEvent(const char* data, const unsigned int dataSize)
	{
		if (_mcstatusEventCb)
		{
			_mcstatusEventCb(data, (int)dataSize);
		}
	}

	void pathnetworkEvent(const char* data, const unsigned int dataSize)
	{
		if (_pathnetworkEventCb)
		{
			_pathnetworkEventCb(data, (int)dataSize);
		}
	}

	void localizationModeEvent(const char* mode, const unsigned int dataSize)
	{
		if (_localizationModeEventCb)
		{
			_localizationModeEventCb(mode, (int)dataSize);
		}
	}

	void smartlogEvent(const char* data, const unsigned int dataSize)
	{
		if (_smartlogEventCb)
		{
			_smartlogEventCb(data, (int)dataSize);
		}
	}

	void smartnavigationplanEvent(const char* data, const unsigned int dataSize)
	{
		if (_smartnavigationplanEventCb)
		{
			_smartnavigationplanEventCb(data, (int)dataSize);
		}
	}

	void smartlocationsEvent(const char* data, const unsigned int dataSize)
	{
		if (_smartlocationsEventCb)
		{
			_smartlocationsEventCb(data, (int)dataSize);
		}
	}

	void smartstationsEvent(const char* data, const unsigned int dataSize)
	{
		if (_smartstationsEventCb)
		{
			_smartstationsEventCb(data, (int)dataSize);
		}
	}

	void smartrobotinfoEvent(const char* data, const unsigned int dataSize)
	{
		if (_smartrobotinfoEventCb)
		{
			_smartrobotinfoEventCb(data, (int)dataSize);
		}
	}

	void smartmyrobotidEvent(const char* data, const unsigned int dataSize)
	{
		if (_smartmyrobotidEventCb)
		{
			_smartmyrobotidEventCb(data, (int)dataSize);
		}
	}

	void smartjoblistEvent(const char* data, const unsigned int dataSize)
	{
		if (_smartjoblistEventCb)
		{
			_smartjoblistEventCb(data, (int)dataSize);
		}
	}

	void mapDirEvent(const char* data, const unsigned int dataSize)
	{
		if (_mapDirEventCb)
		{
			_mapDirEventCb(data, (int)dataSize);
		}
	}

	void mapDirWithDataEvent(const char* jsonData, const unsigned int jsonDataSize, const char* data, const unsigned int dataSize)
	{
		if (_mapDirWithDataEventCb)
		{
			_mapDirWithDataEventCb(jsonData, (int)jsonDataSize, data, (int)dataSize);
		}
	}

	void fleetcom_responseEvent(const char* data, const unsigned int dataSize)
	{
		if (_fleetcom_responseEventCb)
		{
			_fleetcom_responseEventCb(data, (int)dataSize);
		}
	}

	void mapDir_responseEvent(const char* infoData, const unsigned int infoDataSize, const char* data, const unsigned int dataSize)
	{
		if (_mapDir_responseEventCb)
		{
			_mapDir_responseEventCb(infoData, (int)infoDataSize, data, (int)dataSize);
		}
	}

	void smartlicenseEvent(const char* data, const unsigned int dataSize)
	{
		if (_smartlicenseEventCb)
		{
			_smartlicenseEventCb(data, (int)dataSize);
		}
	}


	void mapEvent(const char* data, unsigned int dataSize, const rec::robotino::api2::Factory4MapInfo& info)
	{
		if (_mapEventCb)
		{
			_mapEventCb(data, (int)dataSize, info.width, info.height, info.resolution, info.offsetx, info.offsety, info.sequence);
		}
	}

	void mapPlannerEvent(const char* data, unsigned int dataSize, const rec::robotino::api2::Factory4MapInfo& info)
	{
		if (_mapPlannerEventCb)
		{
			_mapPlannerEventCb(data, (int)dataSize, info.width, info.height, info.resolution, info.offsetx, info.offsety, info.sequence);
		}
	}
};

typedef QSharedPointer<Factory4Extended> Factory4Ptr;

Factory4Ptr getFactory4( Factory4Id id );

#endif //_REC_ROBOTINO_API2_C_FACTORY4_HH_
