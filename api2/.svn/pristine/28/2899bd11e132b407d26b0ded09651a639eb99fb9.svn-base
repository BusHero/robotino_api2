//  Copyright (C) 2004-2008, Robotics Equipment Corporation GmbH

#ifndef _REC_ROBOTINO_API2_C_LASERRANGEFINDER_HH_
#define _REC_ROBOTINO_API2_C_LASERRANGEFINDER_HH_

#include "rec/robotino/api2/c/globals.h"

#include "rec/robotino/api2/LaserRangeFinder.h"

#include <QSharedPointer>
#include <QMutex>

#include <string.h>

class MyLaserRangeFinder : public rec::robotino::api2::LaserRangeFinder
{
public:
	MyLaserRangeFinder()
	{
	}

	~MyLaserRangeFinder()
	{
	}

	BOOL grab()
	{
		rec::robotino::api2::LaserRangeFinderReadings r = readings();
		if( r.seq == _readings.seq )
		{
			return FALSE;
		}

		_readings = r;
		return TRUE;
	}

	BOOL numScans( unsigned int* numRangeMeasurements, unsigned int* numIntensityMeasurements )
	{
		*numRangeMeasurements = _readings.numRanges();
		*numIntensityMeasurements = _readings.numIntensities();
		return TRUE;
	}

	BOOL getReadings(
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
		*seq = _readings.seq;
		*stamp = _readings.stamp;
		*angle_min = _readings.angle_min;
		*angle_max = _readings.angle_max;
		*angle_increment = _readings.angle_increment;
		*time_increment = _readings.time_increment;
		*scan_time = _readings.scan_time;
		*range_min = _readings.range_min;
		*range_max = _readings.range_max;
		
		unsigned int rangesSize;
		const float* rangesp;
		_readings.ranges( &rangesp, &rangesSize );

		memcpy( (void*)ranges, (const void*)rangesp, rangesSize * sizeof( float ) );
		*numRanges = rangesSize;

		unsigned int intensitiesSize;
		const float* intensitiesp;
		_readings.intensities( &intensitiesp, &intensitiesSize );

		memcpy( (void*)intensities, (const void*)intensitiesp, intensitiesSize * sizeof( float ) );
		*numIntensities = intensitiesSize;
			
		return TRUE;
	}

private:
	rec::robotino::api2::LaserRangeFinderReadings _readings;
};

typedef QSharedPointer<MyLaserRangeFinder> LaserRangeFinderPtr;

LaserRangeFinderPtr getLaserRangeFinder( LaserRangeFinderId id );

#endif //_REC_ROBOTINO_API2_C_LASERRANGEFINDER_HH_
