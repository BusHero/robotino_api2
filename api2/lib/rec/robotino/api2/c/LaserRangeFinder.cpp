//  Copyright (C) 2004-2008, Robotics Equipment Corporation GmbH

#include "rec/robotino/api2/c/LaserRangeFinder.h"
#include "rec/robotino/api2/c/LaserRangeFinder.hh"
#include "rec/robotino/api2/c/Com.hh"

#include <QMap>
#include <QMutex>

typedef QMap<LaserRangeFinderId,LaserRangeFinderPtr> LaserRangeFinderContainer;

LaserRangeFinderContainer rangefinders;

QMutex _rangefindersMutex;

LaserRangeFinderId nextLaserRangeFinderId= 0;

LaserRangeFinderId LaserRangeFinder_construct()
{
	QMutexLocker lk( &_rangefindersMutex );
	
	LaserRangeFinderId currentLaserRangeFinderId = nextLaserRangeFinderId;
	++nextLaserRangeFinderId;

	rangefinders[ currentLaserRangeFinderId ] = LaserRangeFinderPtr( new MyLaserRangeFinder );
	return currentLaserRangeFinderId;
}

BOOL LaserRangeFinder_destroy( LaserRangeFinderId id )
{
	QMutexLocker lk( &_rangefindersMutex );
	
	LaserRangeFinderContainer::iterator iter = rangefinders.find( id );
	if( rangefinders.end() != iter )
	{
		rangefinders.erase( iter );
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL LaserRangeFinder_setComId( LaserRangeFinderId id, ComId comId )
{
	LaserRangeFinderPtr rangefinder = getLaserRangeFinder( id );
	ComPtr com = getCom( comId );

	if( rangefinder && com )
	{
		try
		{
			rangefinder->setComId( com->id() );
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

BOOL LaserRangeFinder_grab( LaserRangeFinderId id )
{
	LaserRangeFinderPtr rangefinder = getLaserRangeFinder( id );

	if( rangefinder )
	{
		return rangefinder->grab();
	}
	else
	{
		return FALSE;
	}
}

BOOL LaserRangeFinder_numMeasurements( LaserRangeFinderId id, unsigned int* numRangeMeasurements, unsigned int* numIntensityMeasurements )
{
	LaserRangeFinderPtr rangefinder = getLaserRangeFinder( id );

	if( rangefinder )
	{
		return rangefinder->numScans( numRangeMeasurements, numIntensityMeasurements );
	}
	else
	{
		return FALSE;
	}
}


BOOL LaserRangeFinder_getReadings(
								  LaserRangeFinderId id,
								  unsigned int* seq,
								  unsigned int* stamp,
								  float* angle_min,
								  float* angle_max,
								  float* angle_increment,
								  float* time_increment,
								  float* scan_time,
								  float* range_min,
								  float* range_max,
								  float* ranges,
								  unsigned int* numRanges,
								  float* intensities,
								  unsigned int* numIntensities
								  )
{
	LaserRangeFinderPtr rangefinder = getLaserRangeFinder( id );
	if( rangefinder )
	{
		return rangefinder->getReadings(
			seq,
			stamp,
			angle_min,
			angle_max,
			angle_increment,
			time_increment,
			scan_time,
			range_min,
			range_max,
			ranges,
			numRanges,
			intensities,
			numIntensities
			);
	}
	else
	{
		return FALSE;
	}
}

LaserRangeFinderPtr getLaserRangeFinder( LaserRangeFinderId id )
{
	QMutexLocker lk( &_rangefindersMutex );
	
	LaserRangeFinderContainer::const_iterator iter = rangefinders.find( id );
	if( rangefinders.end() != iter )
	{
		return iter.value();
	}
	else
	{
		return LaserRangeFinderPtr();
	}
}

