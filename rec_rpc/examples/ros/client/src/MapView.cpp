/*
Copyright (c) 2011, REC Robotics Equipment Corporation GmbH, Planegg, Germany
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

- Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation and/or
  other materials provided with the distribution.
- Neither the name of the REC Robotics Equipment Corporation GmbH nor the names of
  its contributors may be used to endorse or promote products derived from this software
  without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "MapView.h"
#include "Client.h"
#include "Actions.h"
#include "RobotItem.h"
#include "PolylineItem.h"
#include "ArrowItem.h"

#include "rec/rpc/utils.h"

#include <QGraphicsSvgItem>
#include <cassert>

#define MAP_Z     -1
#define BUTTON_Z		10

#define MAP_PLANNER_Z	0
#define PLAN_Z	1
#define POSE_Z	2
#define SCAN_AREA_Z		8
#define MARKERS_Z		9
#define STATIONS_Z		10
#define CURRENT_GOAL_Z  11
#define F5_Z		12
#define F5_TMP_Z		13

MapView::MapView( QWidget* parent )
: QGraphicsView( parent )
, _scene( NULL )
, _mapItem( NULL )
, _robotItem( NULL )
, _robotItem_tmp( NULL )
, _current_goal( NULL )
{
	setWindowTitle( tr( "Map View" ) );

	setBackgroundBrush( Qt::white );

	QPixmapCache::setCacheLimit( 20480 );	// set cache size to 20MB (default on desktop usually 10MB).
	_scene = new QGraphicsScene();
	_scene->setSceneRect( 1,1,1,1 );
	_scene->setBackgroundBrush( QBrush( QColor::fromRgb(0, 0, 8) ) );
	setScene( _scene );
	//setRenderHints( QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform );
	setAlignment( Qt::AlignLeft | Qt::AlignTop );
	setTransformationAnchor( QGraphicsView::NoAnchor );
	setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
	setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );

	_mapItem = new QGraphicsPixmapItem;
	_mapItem->setZValue( MAP_Z );
	_mapItem->setTransformationMode( Qt::FastTransformation );
	_mapItem->setCacheMode( QGraphicsItem::ItemCoordinateCache );
	_scene->addItem( _mapItem );

	_robotItem = new RobotItem( _mapItem );
	_robotItem->setZValue( F5_Z );
	_robotItem->setCacheMode( QGraphicsItem::ItemCoordinateCache );

	_robotItem_tmp = new RobotItem( _mapItem );
	_robotItem_tmp->setZValue( F5_TMP_Z );
	_robotItem_tmp->setCacheMode( QGraphicsItem::ItemCoordinateCache );
	_robotItem_tmp->setVisible( false );

	_global_plan = new PolylineItem( _mapItem );
	_global_plan->setZValue( PLAN_Z );
	_global_plan->setPen( QPen( QColor::fromRgb(40, 255, 40) ) );
	_global_plan->setBrush( Qt::NoBrush );

	_current_goal = new ArrowItem( _mapItem );
	_current_goal->setZValue( CURRENT_GOAL_Z );
	_current_goal->setVisible( false );

	resizeEvent( NULL );

	bool connected = true;

	connected &= connect(
		Client::singleton(),
		SIGNAL( mapCb( const QImage&, float, float, float ) ),
		SLOT( on_client_mapCb( const QImage&, float, float, float ) ),
		Qt::QueuedConnection
		);

	connected &= connect(
		Client::singleton(),
		SIGNAL( scanCb( const QVector< QPointF >& ) ),
		SLOT( on_client_scanCb( const QVector< QPointF >& ) ),
		Qt::QueuedConnection
		);

	connected &= connect(
		Client::singleton(),
		SIGNAL( odomCb( const QPointF&, float ) ),
		SLOT( on_client_odomCb( const QPointF&, float ) ),
		Qt::QueuedConnection
		);

	connected &= connect(
		Client::singleton(),
		SIGNAL( planCb( const QPolygonF& ) ),
		SLOT( on_client_planCb( const QPolygonF& ) ),
		Qt::QueuedConnection
		);

	connected &= connect(
		Client::singleton(),
		SIGNAL( currentGoalCb( const QPointF&, double ) ),
		SLOT( on_client_currentGoalCb( const QPointF&, double ) ),
		Qt::QueuedConnection
		);

	connected &= connect(
		Client::singleton(),
		SIGNAL( moveBaseDoneCb( bool ) ),
		SLOT( on_client_moveBaseDoneCb( bool ) ),
		Qt::QueuedConnection
		);

	connected &= connect( &Actions::singleton().tools, SIGNAL( triggered( QAction* ) ), SLOT( on_tools_triggered( QAction* ) ) );

	assert( connected );
}

void MapView::on_client_mapCb( const QImage& image, float resolution, float offsetx, float offsety )
{
	_mapItem->setPixmap( QPixmap::fromImage( image.mirrored( true, false ) ) );

	if( _mapItem->pixmap().isNull() )
	{
		_mapItem->scale( 0.5, 0.5 );
	}

	double k = 1.0 / ( resolution * 1000.0 );

	_robotItem->resetTransform();
	_robotItem->scale( k, k );

	_robotItem_tmp->resetTransform();
	_robotItem_tmp->scale( k, k );
}

void MapView::on_client_scanCb( const QVector< QPointF >& points )
{
	for( int i=points.size(); i<_scanPoints.size(); ++i )
	{
		delete _scanPoints[i];
	}

	_scanPoints.resize( points.size() );

	for( int i=0; i<points.size(); ++i )
	{
		QGraphicsEllipseItem* item = _scanPoints[i];

		if( NULL == item )
		{
			item = new QGraphicsEllipseItem( _mapItem );
			item->setRect( -0.3, -0.3, 0.6, 0.6 );
			item->setPen( Qt::NoPen );
			item->setBrush( Qt::yellow );

			_scanPoints[i] = item;
		}

		item->setPos( points[i] );
	}
}

void MapView::on_client_odomCb( const QPointF& point, float yaw )
{
	_robotItem->setPos( point );
	_robotItem->setRotation( yaw );
}

void MapView::on_client_planCb( const QPolygonF& p )
{
	_global_plan->setPolygon( p );
}

void MapView::on_client_currentGoalCb( const QPointF& pos, double rotDeg )
{
	_current_goal->resetTransform();
	_current_goal->setPos( pos );
	_current_goal->rotate( rotDeg );
	_current_goal->setVisible( true );
}

void MapView::on_client_moveBaseDoneCb( bool )
{
	_current_goal->setVisible( false );
}

void MapView::on_tools_triggered( QAction* action )
{
	setMouseTracking( false );
	_robotItem_tmp->setVisible( false );
	setCursor( Qt::ArrowCursor );

	if( &Actions::singleton().setPosTool == action )
	{
		setMouseTracking( action->isChecked() );
		_robotItem_tmp->setVisible( action->isChecked() );
		_robotItem_tmp->setState( RobotItem::Position );
	}
}

void MapView::on_setPoseEstimate()
{
	const QPointF p = _mapItem->mapFromScene( mapToScene( _setPosPoint ) );
	Client::singleton()->setPoseEstimate( p, _robotItem_tmp->rotation() );
}

void MapView::on_setGoal()
{
	const QPointF p = _mapItem->mapFromScene( mapToScene( _setPosPoint ) );
	Client::singleton()->setGoal( p, _robotItem_tmp->rotation() );
}

void MapView::mousePressEvent( QMouseEvent* e )
{
	_mouseStartPoint = e->pos();

	if( e->buttons().testFlag( Qt::LeftButton ) )
	{
		if( Actions::singleton().setPosTool.isChecked() )
		{
			if( RobotItem::Orientation == _robotItem_tmp->state() )
			{
				QMenu menu;
				menu.addAction( tr("Set goal"), this, SLOT( on_setGoal() ) );
				menu.addAction( tr("Set pose estimate"), this, SLOT( on_setPoseEstimate() ) );

				menu.exec( e->globalPos() );

				Actions::singleton().pickTool.trigger();
			}
			else
			{
				_robotItem_tmp->setState( RobotItem::Orientation );
				_setPosPoint = e->pos();
			}

			return;
		}
	}
}

void MapView::mouseMoveEvent( QMouseEvent * e )
{
	Qt::MouseButton panButton = Qt::LeftButton;

	if( Actions::singleton().setPosTool.isChecked() )
	{
		panButton = Qt::RightButton;
		
		_robotItem_tmp->setVisible( true );

		if( RobotItem::Orientation == _robotItem_tmp->state() )
		{
			QPoint delta = e->pos() - _mouseStartPoint;
			double alpha = atan2( (double)delta.y(), (double)delta.x() );

			//_setPosEstimateItem->resetTransform();
			_robotItem_tmp->setRotation( rec::rpc::rad2deg( alpha ) );
		}
		else
		{
			_robotItem_tmp->setPos( _mapItem->mapFromScene( mapToScene( e->pos() ) ) );
		}
	}
	
	if( ( e->buttons() & panButton ) == panButton )
	{
		// pan the _mapPixmap
		int diffX = e->x() - _mouseStartPoint.x();
		int diffY = e->y() - _mouseStartPoint.y();
		_mouseStartPoint = e->pos();

		pan( diffX, diffY );
	}
}

void MapView::wheelEvent( QWheelEvent * e )
{
	if( e->delta() > 0 )
	{
		zoom( 1.25 );
	}
	else
	{
		zoom( 0.8 );
	}

	e->accept();
}

void MapView::zoom( double factor )
{
	if( _mapItem != NULL )
	{
		QPointF sceneCenter = mapToScene( QPoint( size().width() / 2, size().height() / 2 ) );
		QPointF local = _mapItem->mapFromScene( sceneCenter );

		_mapItem->scale( factor, factor );
		//if( _robotFootprint != NULL )
		//{
		//	_robotFootprint->scale( factor, factor );
		//}

		QPointF now = _mapItem->mapToScene( local );
		double x = sceneCenter.x() - now.x();
		double y = sceneCenter.y() - now.y();
		pan( x, y );
	}
}

void MapView::pan( int x, int y )
{
	if( _mapItem != NULL )
	{
		_mapItem->moveBy( x, y );
	}
}
