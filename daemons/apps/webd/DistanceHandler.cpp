#include "DistanceHandler.h"
#include "ControlHandler.h"

DistanceHandler::DistanceHandler( QObject* parent )
: QObject( parent )
, _odoX( 0.0 )
, _odoY( 0.0 )
, _odoPhi( 0.0 )
, _max_voltage( 24.0 )
, _sleep_voltage( 22.5 )
, _distances( 9, 0.0f )
, _bumper( false )
, _battery_voltage( 0.0f )
, _system_current( 0.0f )
{
	_template = QImage( ":/web/images/sensors_template.png" );

	_noimageavailable = QImage( ":/web/images/noimageavailable.png" );
}

QByteArray DistanceHandler::page( const QImage& liveimage, ControlHandler* controlHandler, bool isWebControl ) const
{
	QImage fullImage;
	if( liveimage.isNull() )
	{
		fullImage =  _noimageavailable;
	}
	else
	{
		fullImage = liveimage;
	}

	//QImage img( 115, 115, QImage::Format_ARGB32 );

	QVector<int> normdist( 9 );

	for( unsigned int i=0; i<9; i++ )
	{
		normdist[i] = static_cast<int>( 30.0f * ( 1.0f - _distances[i] / 2.55f ) );
	}

	{
		QPainter p( &fullImage );
		p.fillRect( 0, 0, 115, 115, QBrush( QColor( 255, 255, 255, 100 ) ) );

		if( _bumper )
		{
			p.setPen( QPen( Qt::blue, 3 ) );
			p.drawEllipse( 33, 33, 50, 50 );
		}

		p.setPen( QPen( Qt::blue, 3 ) );

		drawLine( &p, QPoint( 58, 33 ), -90.0, static_cast< unsigned int >( normdist[0] ) );
		drawLine( &p, QPoint( 42, 39 ), -130, static_cast< unsigned int >( normdist[1] ) );
		drawLine( &p, QPoint( 33, 54 ), -170, static_cast< unsigned int >( normdist[2] ) );
		drawLine( &p, QPoint( 36, 71 ), -210, static_cast< unsigned int >( normdist[3] ) );
		drawLine( &p, QPoint( 49, 81 ), 110, static_cast< unsigned int >( normdist[4] ) );
		drawLine( &p, QPoint( 66, 82 ), 70, static_cast< unsigned int >( normdist[5] ) );
		drawLine( &p, QPoint( 80, 69 ), 30, static_cast< unsigned int >( normdist[6] ) );
		drawLine( &p, QPoint( 83, 54 ), -10, static_cast< unsigned int >( normdist[7] ) );
		drawLine( &p, QPoint( 74, 39 ), -40, static_cast< unsigned int >( normdist[8] ) );

		p.drawImage( 39, 39, _template );

		switch( controlHandler->movement() )
		{
		case ControlHandler::NoMovement:
			p.setPen( Qt::NoPen );
			p.setBrush( QBrush( QColor( 255, 255, 255, 100 ) ) );
			p.drawEllipse( fullImage.width() - 140, 5, 30, 30 );
			//p.drawImage( fullImage.width() - 65, 5, QImage( ":/web/images/stop.png" ) );
			break;

		case ControlHandler::MoveN:
			p.drawImage( fullImage.width() - 140, 5, QImage( ":/web/images/n.png" ) );
			break;

		case ControlHandler::MoveNW:
			p.drawImage( fullImage.width() - 140, 5, QImage( ":/web/images/nw.png" ) );
			break;

		case ControlHandler::MoveNO:
			p.drawImage( fullImage.width() - 140, 5, QImage( ":/web/images/no.png" ) );
			break;

		case ControlHandler::MoveW:
			p.drawImage( fullImage.width() - 140, 5, QImage( ":/web/images/w.png" ) );
			break;

		case ControlHandler::MoveO:
			p.drawImage( fullImage.width() - 140, 5, QImage( ":/web/images/o.png" ) );
			break;

		case ControlHandler::MoveS:
			p.drawImage( fullImage.width() - 140, 5, QImage( ":/web/images/s.png" ) );
			break;

		case ControlHandler::MoveSW:
			p.drawImage( fullImage.width() - 140, 5, QImage( ":/web/images/sw.png" ) );
			break;

		case ControlHandler::MoveSO:
			p.drawImage( fullImage.width() - 140, 5, QImage( ":/web/images/so.png" ) );
			break;
		}

		switch( controlHandler->rotation() )
		{
		case ControlHandler::NoRotation:
			p.setPen( Qt::NoPen );
			p.setBrush( QBrush( QColor( 255, 255, 255, 100 ) ) );
			p.drawEllipse( fullImage.width() - 105, 5, 30, 30 );
			break;

		case ControlHandler::RotateCL:
			p.drawImage( fullImage.width() - 105, 5, QImage( ":/web/images/cl.png" ) );
			break;

		case ControlHandler::RotateCCL:
			p.drawImage( fullImage.width() - 105, 5, QImage( ":/web/images/ccl.png" ) );
			break;
		}

		if( isWebControl )
		{
			p.drawImage( fullImage.width() - 70, 5, QImage( ":/web/images/active.png" ) );
		}
		else
		{
			p.drawImage( fullImage.width() - 70, 5, QImage( ":/web/images/passive.png" ) );
		}

		p.drawImage( fullImage.width() - 35, 5, QImage( ":/web/images/battery.png" ) );

		p.setPen( Qt::NoPen );
		p.setBrush( Qt::green );
		if( _battery_voltage > _max_voltage )
		{
			p.drawRect( fullImage.width() - 35 + 5, 5 + 7, 19, 7 );
		}
		else if( _battery_voltage > _sleep_voltage )
		{
			float capacity = ( _battery_voltage - _sleep_voltage ) / ( _max_voltage - _sleep_voltage );
			p.drawRect( fullImage.width() - 35 + 5, 5 + 7, 19 * capacity, 7 );
		}

		QFont f( "Courier New" );
		f.setPixelSize( 12 );
		p.setFont( f );
		p.setPen( Qt::black );

		p.drawText( fullImage.width() - 35 + 3, 5 + 25, QString("%1").arg( _battery_voltage, 0, 'f', 1 ) );


		p.fillRect( 0, fullImage.height() - 18, fullImage.width(), 18, QBrush( QColor( 255,255,255, 100 ) ) );
		p.drawText( 5, fullImage.height() - 5, QString( "Odometry (x,y,phi) : %1m %2m %3°" ).arg( _odoX, 0, 'f', 2 ).arg( _odoY, 0, 'f', 2 ).arg( 57.29577 * _odoPhi, 0, 'f', 1 ) );
	}

	QByteArray ba;
	QBuffer buffer( &ba );

	fullImage.save( &buffer, "png" );

	return ba;
}

void DistanceHandler::on_distance_sensors_changed( const QVector< float >& dist )
{
	if( dist.size() != 9 )
	{
		return;
	}

	_distances = dist;
}

void DistanceHandler::on_bumper_changed( bool bumper )
{
	_bumper = bumper;
}

void DistanceHandler::on_battery_changed( float battery_voltage, float system_current )
{
	_battery_voltage = battery_voltage;
	_system_current = system_current;
}

void DistanceHandler::on_odometry_changed( double x, double y, double phi, float vx, float vy, float omega, unsigned int sequence )
{
	_odoX = x;
	_odoY = y;
	_odoPhi = phi;
}

void DistanceHandler::drawLine( QPainter* p, const QPoint& p0, float phi_deg, unsigned int length ) const
{
	int x2 = p0.x() + static_cast<int>( cos( 0.0175 * phi_deg ) * length );
	int y2 = p0.y() + static_cast<int>( sin( 0.0175 * phi_deg ) * length );

	p->drawLine( p0.x(), p0.y(), x2, y2 );
}

void DistanceHandler::on_parameters_changed( const QMap< QString, QVariant >& parameters )
{
	QMap< QString, QVariant >::const_iterator iter = parameters.constBegin();
	while( parameters.constEnd() != iter )
	{
		const QString& key = iter.key();

		if( key.endsWith( "max_voltage" ) )
		{
			_max_voltage = iter.value().toDouble();
		}
		else if( key.endsWith( "sleep_voltage" ) )
		{
			_sleep_voltage = iter.value().toDouble();
		}

		++iter;
	}
}
