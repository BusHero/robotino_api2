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

#ifndef MAP_VIEW_H
#define MAP_VIEW_H

#include <QtGui>

#include "rec/rpc/serialization/Pose2D.h"

class RobotItem;
class PoseItem;
class ArrowItem;

class MapView : public QGraphicsView
{
	Q_OBJECT;

public:
	MapView( QWidget* parent );

private Q_SLOTS:
	void on_client_mapCb( const QImage&, float, float, float );
	void on_client_scanCb( const QVector< QPointF >& );
	void on_client_odomCb( const QPointF&, float );
	void on_client_planCb( const QPolygonF& );
	void on_client_currentGoalCb( const QPointF&, double );
	void on_client_moveBaseDoneCb( bool );

	void on_tools_triggered( QAction* );

	void on_setPoseEstimate();
	void on_setGoal();

private:
	void mousePressEvent( QMouseEvent* e );
	void mouseMoveEvent( QMouseEvent * e );
	void wheelEvent( QWheelEvent * e );

	void zoom(double factor);
	void pan(int x, int y);

	QGraphicsScene* _scene;
	QVector< QGraphicsEllipseItem* > _scanPoints;
	QGraphicsPixmapItem* _mapItem;
	RobotItem* _robotItem;
	RobotItem* _robotItem_tmp;
	QGraphicsPolygonItem* _global_plan;
	ArrowItem* _current_goal;

	QPoint _mouseStartPoint;
	QPoint _setPosPoint;
};

#endif
