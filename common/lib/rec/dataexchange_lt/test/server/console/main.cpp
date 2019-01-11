#include "rec/dataexchange/Server.h"
#include "rec/core/utils.h"
#include "rec/core/Thread.h"
#include "rec/core/LaserRangeFinderInfo.h"
#include "rec/core/LocalizedLaserRangeFinderData.h"

#include <iostream>

#define _USE_MATH_DEFINES
#include <cmath>

#include <QtCore>
#include <QtXml>

rec::core::List< rec::core::LocalizedLaserRangeFinderData > rangeDataList;
rec::core::LaserRangeFinderInfo laser0_info;

class MyServer : public rec::dataexchange::Server
{
public:
	MyServer()
		: stop( false )
	{
	}

	virtual void clientConnected( const std::string& address )
	{
		std::cout << "clientConnected: " << address << std::endl;
	}

	virtual void clientDisconnected( const std::string& address )
	{
		std::cout << "clientDisconnected: " << address << std::endl;
	}

	virtual void serverError( const std::string& errorString )
	{
		std::cout << "Server error: " << errorString << std::endl;
	}

	virtual void clientError( const std::string& errorString )
	{
		std::cout << "Client error: " << errorString << std::endl;
	}

	virtual void dataReceived( const std::string& name, rec::dataexchange::DataType type, const rec::core::variant::Variant& value )
	{
		std::cout << "dataReceived: " << name << " " << rec::dataexchange::dataTypeName( type ) << " " << value.toString() << std::endl;

		if( "stop" == name )
		{
			stop = value.toBool();
		}
	}

	virtual void closed()
	{
		std::cout << "closed" << std::endl;
	}

	virtual void listening()
	{
		std::cout << "listening" << std::endl;
	}

	bool stop;
};

MyServer server;

class OdometryThread : public rec::core::Thread
{
public:
	OdometryThread()
		: _odometry( 3 )
	{
	}

private:
	void run()
	{
		while( !server.stop )
		{
			_odometry[0] += 10.f;
			_odometry[1] += 0.1f;
			_odometry[2] += -0.09f;
		
			server.sendData( "odometry", _odometry );

			rec::core::msleep( 10 );
		}

		std::cout << "Odometry thread stopped" << std::endl;
	}

	rec::core::FloatVector _odometry;
};

class LaserScannerThread : public rec::core::Thread
{
public:
	LaserScannerThread()
	{
	}

private:
	void run()
	{
		int scanNumber = 0;
		rec::core::List< rec::core::LocalizedLaserRangeFinderData >::const_iterator iter =  rangeDataList.constBegin();
		while( !server.stop && rangeDataList.constEnd() != iter )
		{
			if( server.data( "laser0_request" ).toBool() )
			{
				server.sendData( "laser0_ldata", *iter );

				std::cout << "Send laser0_data scan " << scanNumber++ << std::endl;
			
				++iter;
			}

			rec::core::msleep( 200 );
		}

		std::cout << "LaserScanner thread stopped" << std::endl;
	}
};

void rotationToEuler( float q[4], float* heading, float* attitude, float* bank )
{
	for( int i = 0; i < 4; i++ )
	{
		if ( std::fabs( q[i] ) < 0.00001f )
			q[i] = 0;
		else if ( std::fabs( q[i] - 0.5f ) < 0.00001f )
			q[i] = 0.5f;
		else if ( std::fabs( q[i] + 0.5f ) < 0.00001f )
			q[i] = -0.5f;
		else if ( std::fabs( q[i] - 1.f ) < 0.00001f )
			q[i] = 1.f;
		else if ( std::fabs( q[i] + 1.f ) < 0.00001f )
			q[i] = -1.f;
		else if ( std::fabs( q[i] - M_SQRT1_2 ) < 0.00001f )
			q[i] = static_cast< float >( M_SQRT1_2 );
		else if ( std::fabs( q[i] + M_SQRT1_2 ) < 0.00001f )
			q[i] = - static_cast< float >( M_SQRT1_2 );
	}

	*attitude = asin( 2 * q[0] * q[1] + 2 * q[2] * q[3] );

	float testValue = q[0] * q[1] + q[2] * q[3];

	if( std::fabs( 0.5 - testValue ) < 0.00001f )
	{
		*heading = 2 * atan2( q[0], q[3] );
		*bank = 0;
	}
	else if( std::fabs( 0.5 + testValue ) < 0.00001f )
	{
		*heading = -2 * atan2( q[0], q[3] );
		*bank = 0;
	}
	else
	{
		*heading = atan2( 2 * q[1] * q[3] - 2 * q[0] * q[2] , 1 - 2 * q[1] * q[1] - 2 * q[2] * q[2] );
		*bank = atan2( 2 * q[0] * q[3] - 2 * q[1] * q[2] , 1 - 2 * q[0] * q[0] - 2 * q[2] * q[2] );
	}

}

void readMap()
{
	QDomDocument doc("rangeData");
	QFile file("map.xml");
	if (!file.open(QIODevice::ReadOnly))
		return;
	if (!doc.setContent(&file)) {
		file.close();
		return;
	}
	file.close();

	// print out the element names of all elements that are direct children
	// of the outermost element.
	QDomElement docElem = doc.documentElement();

	QDomElement DeviceList = docElem.firstChildElement( "DeviceList" );
	if( DeviceList.isElement() )
	{
		QDomElement LaserRangeFinder = DeviceList.firstChildElement( "LaserRangeFinder" );
		if( LaserRangeFinder.isElement() )
		{
			laser0_info.name = LaserRangeFinder.firstChildElement( "ID" ).text().toStdString();
			laser0_info.minimumAngle = LaserRangeFinder.firstChildElement( "MinimumAngle" ).text().toDouble();
			laser0_info.maximumAngle = LaserRangeFinder.firstChildElement( "MaximumAngle" ).text().toDouble();
			laser0_info.angularResolution = LaserRangeFinder.firstChildElement( "Resolution" ).text().toDouble();
			laser0_info.minimumRange = LaserRangeFinder.firstChildElement( "MinRange" ).text().toDouble();
			laser0_info.maximumRange = LaserRangeFinder.firstChildElement( "MaxRange" ).text().toDouble();
			laser0_info.offsetPoseX = LaserRangeFinder.firstChildElement( "PoseX" ).text().toDouble();
			laser0_info.offsetPoseY = LaserRangeFinder.firstChildElement( "PoseY" ).text().toDouble();
			laser0_info.offsetPoseOrientation = LaserRangeFinder.firstChildElement( "PosePhi" ).text().toDouble();
		}
	}

	QDomElement DeviceStates = docElem.firstChildElement( "DeviceStates" );
	if( DeviceStates.isElement() )
	{
		QDomNode n = DeviceStates.firstChild();
		while( !n.isNull() )
		{
			QDomElement LocalizedRangeScan = n.toElement(); // try to convert the node to an element.
			if( !LocalizedRangeScan.isNull() && "LocalizedRangeScan" == LocalizedRangeScan.tagName() )
			{
				QDomElement DistanceMeasurements = LocalizedRangeScan.firstChildElement( "DistanceMeasurements" );

				rec::core::LocalizedLaserRangeFinderData rangeData;

				QDomNode m = DistanceMeasurements.firstChild();
				while( !m.isNull() )
				{
					QDomElement FloatElement = m.toElement(); // try to convert the node to an element.
					if( !FloatElement.isNull() )
					{
						rangeData.measuresVector.append( FloatElement.text().toFloat() );
					}

					m = m.nextSibling();
				}

				QDomElement Pose = LocalizedRangeScan.firstChildElement( "Pose" );
				QDomElement Position = Pose.firstChildElement( "Position" );
				rangeData.poseX = Position.firstChildElement( "Z" ).text().toDouble();
				rangeData.poseY = Position.firstChildElement( "X" ).text().toDouble();

				QDomElement Orientation = Pose.firstChildElement( "Orientation" );
				float q[4];
				q[0] = Orientation.firstChildElement( "X" ).text().toFloat();
				q[1] = Orientation.firstChildElement( "Y" ).text().toFloat();
				q[2] = Orientation.firstChildElement( "Z" ).text().toFloat();
				q[3] = Orientation.firstChildElement( "W" ).text().toFloat();

				float heading;
				float attitude;
				float bank;

				rotationToEuler( q, &heading, &attitude, &bank );
				rangeData.poseOrientation = heading;

				std::cout << rangeData.poseX << "  " << rangeData.poseY << "  " << rangeData.poseOrientation << std::endl;

				rangeDataList << rangeData;

				std::cout << "Processed range scan " << rangeDataList.size() << std::endl;
			}
			n = n.nextSibling();
		}

	}

}

int main(int argc, char* argv[])
{
	readMap();

	rec::core::LocalizedLaserRangeFinderData testData = rangeDataList[0];

	QByteArray buffer( testData.encodedDataSize(), 0 );
	testData.encode( buffer.data() );

	rec::core::LocalizedLaserRangeFinderData testData2( buffer.constData(), buffer.size() );

	if( testData != testData2 )
	{
		return 1;
	}

	QByteArray buffer2( laser0_info.encodedDataSize(), 0 );
	laser0_info.encode( buffer2.data() );

	rec::core::LaserRangeFinderInfo testInfo( buffer2.constData(), buffer2.size() );

	if( testInfo != laser0_info )
	{
		return 1;
	}

	OdometryThread odometry;
	LaserScannerThread laser;

	server.addChannel( "drive", rec::dataexchange::FLOATARRAY );
	server.addChannel( "odometry", rec::dataexchange::FLOATARRAY );
	server.addChannel( "devices", rec::dataexchange::LASERRANGEFINDERINFOLIST );
	server.addChannel( "laser0_ldata", rec::dataexchange::LOCALIZEDLASERRANGEFINDERDATA );
	server.addChannel( "laser0_request", rec::dataexchange::U32 );
	server.addChannel( "stop", rec::dataexchange::I32 );

	server.setQueuedSendingInterval( 20 );

	server.listen();

	//odometry.start();
	laser.start();

	rec::core::LaserRangeFinderInfoList devices;

	devices << laser0_info;

	server.sendData( "devices", devices );

	while( !server.stop )
	{
		rec::core::msleep( 200 );
	}

	std::cout << "Stop received" << std::endl;

	odometry.stop();
	laser.stop();

	return 0;
}
