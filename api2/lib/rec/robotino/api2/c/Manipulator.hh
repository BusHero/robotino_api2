//  Copyright (C) 2004-2008, Robotics Equipment Corporation GmbH

#ifndef _REC_ROBOTINO_API2_C_MANIPULATOR_HH_
#define _REC_ROBOTINO_API2_C_MANIPULATOR_HH_

#include "rec/robotino/api2/c/globals.h"

#include "rec/robotino/api2/Grappler.h"

#include <QSharedPointer>
#include <QMutex>

#include <string.h>

class MyManipulator : public rec::robotino::api2::Grappler
{
public:
	MyManipulator()
	{
	}

	~MyManipulator()
	{
	}

	BOOL grab()
	{
		_store_position.clear();

		processEvents();

		if( false == _store_position.isEmpty() )
		{
			return TRUE;
		}

		rec::robotino::api2::GrapplerReadings r = readings();
		if( r.sequenceNumber == _readings.sequenceNumber )
		{
			return FALSE;
		}

		_readings = r;
		return TRUE;
	}

	BOOL getReadings(
		unsigned int* seq,
		float* angles,
		unsigned int* numAngles,
		float* speeds,
		unsigned int* numSpeeds,
		int* errors,
		unsigned int* numErrors,
		int* motors_enabled,
		int* store_current_position,
		float* cwAxesLimits,
		unsigned int* numCwAxesLimits,
		float* ccwAxesLimits,
		unsigned int* numCcwAxesLimits
		)
	{
		if( false == _store_position.isEmpty() )
		{
			_readings = _store_position;
			_store_position.clear();
		
			*store_current_position = 1;
		}

		*seq = _readings.sequenceNumber;
		
		memcpy( (void*)angles, (const void*)_readings.angles, _readings.numServos * sizeof( float ) );
		*numAngles = _readings.numServos;

		memcpy( (void*)speeds, (const void*)_readings.speeds, _readings.numServos * sizeof( float ) );
		*numSpeeds = _readings.numServos;

		memcpy( (void*)errors, (const void*)_readings.errors, _readings.numServos * sizeof( float ) );
		*numErrors = _readings.numServos;

		*motors_enabled = _readings.isTorqueEnabled;

		memcpy( (void*)cwAxesLimits, (const void*)_readings.cwAxesLimits, _readings.numServos * sizeof( float ) );
		*numCwAxesLimits = _readings.numServos;

		memcpy( (void*)ccwAxesLimits, (const void*)_readings.ccwAxesLimits, _readings.numServos * sizeof( float ) );
		*numCcwAxesLimits = _readings.numServos;
			
		return TRUE;
	}

	BOOL setAxis( unsigned int axis, float angle, float speed )
	{
		rec::robotino::api2::Grappler::setAxis( axis, angle, speed );
		return TRUE;
	}

	BOOL setAxes( const float* angles, unsigned int numAngles, const float* speeds, unsigned int numSpeeds )
	{
		if( numAngles != numSpeeds )
		{
			return FALSE;
		}

		rec::robotino::api2::Grappler::setAxes( angles, numAngles, speeds, numSpeeds );
		return TRUE;
	}

	BOOL setPowerEnabled( unsigned int channel, bool enable )
	{
		rec::robotino::api2::Grappler::setPowerEnabled( channel, enable );
		return TRUE;
	}

	BOOL toggleTorque()
	{
		rec::robotino::api2::Grappler::toggleTorque();
		return TRUE;
	}

private:
	void storePositionsEvent( const rec::robotino::api2::GrapplerReadings& readings )
	{
		_store_position = readings;
	}

	rec::robotino::api2::GrapplerReadings _readings;
	rec::robotino::api2::GrapplerReadings _store_position;
};

typedef QSharedPointer<MyManipulator> ManipulatorPtr;

ManipulatorPtr getManipulator( ManipulatorId id );

#endif //_REC_ROBOTINO_API2_C_MANIPULATOR_HH_
