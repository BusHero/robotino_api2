//  Copyright (C) 2004-2008, Robotics Equipment Corporation GmbH

#include "rec/robotino/api2/c/Factory4.hh"
#include "rec/robotino/api2/c/Com.hh"

#include <QMap>
#include <QMutex>

typedef QMap<Factory4Id,Factory4Ptr> Factory4Container;

Factory4Container Factory4s;

QMutex _Factory4sMutex;

Factory4Id nextFactory4Id = 0;

Factory4Id Factory4_construct()
{
	QMutexLocker lk( &_Factory4sMutex );
	
	Factory4Id currentFactory4Id = nextFactory4Id;
	++nextFactory4Id;

	Factory4Extended* a = new Factory4Extended;

	Factory4s[ currentFactory4Id ] = Factory4Ptr( a );

	return currentFactory4Id;
}

BOOL Factory4_destroy( Factory4Id id )
{
	QMutexLocker lk( &_Factory4sMutex );
	
	Factory4Container::iterator iter = Factory4s.find( id );
	if( Factory4s.end() != iter )
	{
		Factory4s.erase( iter );
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL Factory4_setComId( Factory4Id id, ComId comId )
{
	Factory4Ptr Factory4 = getFactory4( id );
	ComPtr com = getCom( comId );

	if( Factory4 && com )
	{
		try
		{
			Factory4->setComId( com->id() );
		}
		catch( const rec::robotino::api2::RobotinoException& )
		{
			return FALSE;
		}
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

Factory4Ptr getFactory4( Factory4Id id )
{
	QMutexLocker lk( &_Factory4sMutex );
	
	Factory4Container::const_iterator iter = Factory4s.find( id );
	if( Factory4s.end() != iter )
	{
		return iter.value();
	}
	else
	{
		return Factory4Ptr();
	}
}

BOOL Factory4_processEvents(Factory4Id id)
{
	Factory4Ptr p = getFactory4(id);
	if (p)
	{
		p->processEvents();
		return TRUE;
	}

	return FALSE;
}

BOOL Factory4_sendMessage(Factory4Id id, const char* message)
{
	Factory4Ptr p = getFactory4(id);
	if (p)
	{
		p->sendMessage(message);
		return TRUE;
	}
	return FALSE;
}

BOOL Factory4_setMapPlanner(Factory4Id id, const char* data, unsigned int dataSize)
{
	Factory4Ptr p = getFactory4(id);
	if (p)
	{
		p->setMapPlanner(data, dataSize);
		return TRUE;
	}
	return FALSE;
}

BOOL Factory4_mapDirRequest(Factory4Id id, const char* infoData, const unsigned int infoDataSize, const char* data, const unsigned int dataSize)
{
	Factory4Ptr p = getFactory4(id);
	if (p)
	{
		p->mapDirRequest(infoData, infoDataSize, data, dataSize);
		return TRUE;
	}
	return FALSE;
}

BOOL Factory4_savePathNetwork(Factory4Id id, const char* data, const unsigned int dataSize)
{
	Factory4Ptr p = getFactory4(id);
	if (p)
	{
		p->savePathNetwork(data, dataSize);
		return TRUE;
	}
	return FALSE;
}

BOOL Factory4_setSmartLocations(Factory4Id id, const char* data, const unsigned int dataSize)
{
	Factory4Ptr p = getFactory4(id);
	if (p)
	{
		p->setSmartLocations(data, dataSize);
		return TRUE;
	}
	return FALSE;
}

BOOL Factory4_setSmartStations(Factory4Id id, const char* data, const unsigned int dataSize)
{
	Factory4Ptr p = getFactory4(id);
	if (p)
	{
		p->setSmartStations(data, dataSize);
		return TRUE;
	}
	return FALSE;
}

BOOL Factory4_setMclayoutEventCb(Factory4Id id, Factory4_data1Cb_t func)
{
	Factory4Ptr p = getFactory4(id);
	if (p)
	{
		p->setMclayoutEventCb(func);
		return TRUE;
	}

	return FALSE;
}

BOOL Factory4_setMcstatusEventCb(Factory4Id id, Factory4_data1Cb_t func)
{
	Factory4Ptr p = getFactory4(id);
	if (p)
	{
		p->_mcstatusEventCb = func;
		return TRUE;
	}

	return FALSE;
}

BOOL Factory4_setPathnetworkEventCb(Factory4Id id, Factory4_data1Cb_t func)
{
	Factory4Ptr p = getFactory4(id);
	if (p)
	{
		p->_pathnetworkEventCb = func;
		return TRUE;
	}

	return FALSE;
}

BOOL Factory4_setLocalizationModeEventCb(Factory4Id id, Factory4_data1Cb_t func)
{
	Factory4Ptr p = getFactory4(id);
	if (p)
	{
		p->_localizationModeEventCb = func;
		return TRUE;
	}

	return FALSE;
}

BOOL Factory4_setSmartlogEventCb(Factory4Id id, Factory4_data1Cb_t func)
{
	Factory4Ptr p = getFactory4(id);
	if (p)
	{
		p->_smartlogEventCb = func;
		return TRUE;
	}

	return FALSE;
}

BOOL Factory4_setSmartnavigationplanEventCb(Factory4Id id, Factory4_data1Cb_t func)
{
	Factory4Ptr p = getFactory4(id);
	if (p)
	{
		p->_smartnavigationplanEventCb = func;
		return TRUE;
	}

	return FALSE;
}

BOOL Factory4_setSmartlocationsEventCb(Factory4Id id, Factory4_data1Cb_t func)
{
	Factory4Ptr p = getFactory4(id);
	if (p)
	{
		p->_smartlocationsEventCb = func;
		return TRUE;
	}

	return FALSE;
}

BOOL Factory4_setSmartstationsEventCb(Factory4Id id, Factory4_data1Cb_t func)
{
	Factory4Ptr p = getFactory4(id);
	if (p)
	{
		p->_smartstationsEventCb = func;
		return TRUE;
	}

	return FALSE;
}

BOOL Factory4_setSmartrobotinfoEventCb(Factory4Id id, Factory4_data1Cb_t func)
{
	Factory4Ptr p = getFactory4(id);
	if (p)
	{
		p->_smartrobotinfoEventCb = func;
		return TRUE;
	}

	return FALSE;
}

BOOL Factory4_setSmartmyrobotidEventCb(Factory4Id id, Factory4_data1Cb_t func)
{
	Factory4Ptr p = getFactory4(id);
	if (p)
	{
		p->_smartmyrobotidEventCb = func;
		return TRUE;
	}

	return FALSE;
}

BOOL Factory4_setSmartjoblistEventCb(Factory4Id id, Factory4_data1Cb_t func)
{
	Factory4Ptr p = getFactory4(id);
	if (p)
	{
		p->_smartjoblistEventCb = func;
		return TRUE;
	}

	return FALSE;
}

BOOL Factory4_setMapDirEventCb(Factory4Id id, Factory4_data1Cb_t func)
{
	Factory4Ptr p = getFactory4(id);
	if (p)
	{
		p->_mapDirEventCb = func;
		return TRUE;
	}

	return FALSE;
}

BOOL Factory4_setMapDirWithDataEventCb(Factory4Id id, Factory4_data2Cb_t func)
{
	Factory4Ptr p = getFactory4(id);
	if (p)
	{
		p->_mapDirWithDataEventCb = func;
		return TRUE;
	}

	return FALSE;
}

BOOL Factory4_setFleetcom_responseEventCb(Factory4Id id, Factory4_data1Cb_t func)
{
	Factory4Ptr p = getFactory4(id);
	if (p)
	{
		p->_fleetcom_responseEventCb = func;
		return TRUE;
	}

	return FALSE;
}

BOOL Factory4_setMapDir_responseEventCb(Factory4Id id, Factory4_data2Cb_t func)
{
	Factory4Ptr p = getFactory4(id);
	if (p)
	{
		p->_mapDir_responseEventCb = func;
		return TRUE;
	}

	return FALSE;
}

BOOL Factory4_setSmartlicenseEventCb(Factory4Id id, Factory4_data1Cb_t func)
{
	Factory4Ptr p = getFactory4(id);
	if (p)
	{
		p->_smartlicenseEventCb = func;
		return TRUE;
	}

	return FALSE;
}


BOOL Factory4_setMapEventCb(Factory4Id id, Factory4_mapEventCb_t func)
{
	Factory4Ptr p = getFactory4(id);
	if (p)
	{
		p->setMapEventCb(func);
		return TRUE;
	}

	return FALSE;
}

BOOL Factory4_setMapPlannerEventCb(Factory4Id id, Factory4_mapEventCb_t func)
{
	Factory4Ptr p = getFactory4(id);
	if (p)
	{
		p->_mapPlannerEventCb = func;
		return TRUE;
	}

	return FALSE;
}