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

#include "ControlPanel.h"
#include "Actions.h"
#include "Client.h"

ControlPanel::ControlPanel( QWidget* parent )
: QDialog( parent )
, _vx( 0 )
, _vy( 0 )
, _omega( 0 )
, _speed( 20 )
, _updateTimer( new QTimer( this ) )
{
	_updateTimer->setInterval( 100 );
	_updateTimer->setSingleShot( false );

	bool connections = true;
	connections &= connect( _updateTimer, SIGNAL( timeout() ), SLOT( on_updateTimer_timeout() ) );

	Q_ASSERT( connections );

	QGridLayout* layout = new QGridLayout( this );
	setLayout( layout );

	ccl = new QPushButton( this );
	ccl->setIcon( QPixmap( ":/images/teleop/ccl.png" ) );

	cl = new QPushButton( this );
	cl->setIcon( QPixmap( ":/images/teleop/cl.png" ) );

	n = new QPushButton( this );
	n->setIcon( QPixmap( ":/images/teleop/n.png" ) );

	s = new QPushButton( this );
	s->setIcon( QPixmap( ":/images/teleop/s.png" ) );

	w = new QPushButton( this );
	w->setIcon( QPixmap( ":/images/teleop/w.png" ) );

	o = new QPushButton( this );
	o->setIcon( QPixmap( ":/images/teleop/o.png" ) );

	nw = new QPushButton( this );
	nw->setIcon( QPixmap( ":/images/teleop/nw.png" ) );

	no = new QPushButton( this );
	no->setIcon( QPixmap( ":/images/teleop/no.png" ) );

	sw = new QPushButton( this );
	sw->setIcon( QPixmap( ":/images/teleop/sw.png" ) );

	so = new QPushButton( this );
	so->setIcon( QPixmap( ":/images/teleop/so.png" ) );

	stop = new QPushButton( this );
	stop->setIcon( QPixmap( ":/images/teleop/teleop_stop.png" ) );

	QList< QPushButton* > buttonList;
	buttonList << ccl << cl << n << s << o << w << nw << no << sw << so << stop;
	Q_FOREACH( QPushButton* p, buttonList )
	{
		p->setIconSize( QSize( 40, 40 ) );
		p->setFixedSize( QSize( 44, 44 ) );
		p->setFocusPolicy( Qt::NoFocus );
	}

	speed = new QSlider( Qt::Horizontal, this );
	speed->setRange( 1, 100 );
	speed->setValue( _speed );
	speed->setFocusPolicy( Qt::NoFocus );

	layout->addWidget( ccl, 1, 0 );
	layout->addWidget( cl, 1, 4 );

	layout->addWidget( n, 0, 2 );
	layout->addWidget( stop, 1, 2 );
	layout->addWidget( s, 2, 2 );

	layout->addWidget( w, 1, 1 );
	layout->addWidget( o, 1, 3 );

	layout->addWidget( nw, 0, 1 );
	layout->addWidget( no, 0, 3 );

	layout->addWidget( sw, 2, 1 );
	layout->addWidget( so, 2, 3 );

	layout->addWidget( speed, 3, 0, 1, 4 );

	layout->setColumnStretch( 5, 1 );

	connect( ccl, SIGNAL( clicked() ), this, SLOT( on_ccl_clicked() ) );
	connect( cl, SIGNAL( clicked() ), this, SLOT( on_cl_clicked() ) );
	connect( n, SIGNAL( clicked() ), this, SLOT( on_n_clicked() ) );
	connect( s, SIGNAL( clicked() ), this, SLOT( on_s_clicked() ) );
	connect( w, SIGNAL( clicked() ), this, SLOT( on_w_clicked() ) );
	connect( o, SIGNAL( clicked() ), this, SLOT( on_o_clicked() ) );
	connect( nw, SIGNAL( clicked() ), this, SLOT( on_nw_clicked() ) );
	connect( no, SIGNAL( clicked() ), this, SLOT( on_no_clicked() ) );
	connect( sw, SIGNAL( clicked() ), this, SLOT( on_sw_clicked() ) );
	connect( so, SIGNAL( clicked() ), this, SLOT( on_so_clicked() ) );
	connect( stop, SIGNAL( clicked() ), this, SLOT( on_stop_clicked() ) );
	connect( speed, SIGNAL( valueChanged(int) ), this, SLOT( on_speed_valueChanged(int) ) );
}

void ControlPanel::updateVelocity()
{
	float x = 0.001 * _vx * _speed;
	float y = 0.001 * _vy * _speed;
	float omega = 3.1f / 180.0f * _omega * _speed;

	if( false == Client::singleton()->setCmdVel( x, y, omega ) )
	{
		hide();
	}

	//_omniDrive.setVelocity( _vx * _speed, _vy * _speed, _omega * _speed );
}

void ControlPanel::showEvent( QShowEvent* )
{
	_updateTimer->start();
}

void ControlPanel::hideEvent( QHideEvent* )
{
	_vx = 0;
	_vy = 0;
	_omega = 0;

	_updateTimer->stop();
	Actions::singleton().showTeleop.setChecked( false );
}

void ControlPanel::on_ccl_clicked()
{
	++_omega;
	updateVelocity();
}

void ControlPanel::on_cl_clicked()
{
	--_omega;
	updateVelocity();
}

void ControlPanel::on_n_clicked()
{
	++_vx;
	updateVelocity();
}

void ControlPanel::on_s_clicked()
{
	--_vx;
	updateVelocity();
}

void ControlPanel::on_w_clicked()
{
	++_vy;
	updateVelocity();
}

void ControlPanel::on_o_clicked()
{
	--_vy;
	updateVelocity();
}

void ControlPanel::on_nw_clicked()
{
	on_n_clicked();
	on_w_clicked();
}

void ControlPanel::on_no_clicked()
{
	on_n_clicked();
	on_o_clicked();
}

void ControlPanel::on_sw_clicked()
{
	on_s_clicked();
	on_w_clicked();
}

void ControlPanel::on_so_clicked()
{
	on_s_clicked();
	on_o_clicked();
}

void ControlPanel::on_stop_clicked()
{
	_vx = 0;
	_vy = 0;
	_omega = 0;
	updateVelocity();
}

void ControlPanel::on_speed_valueChanged( int value )
{
	_speed = value;
	updateVelocity();
}

void ControlPanel::on_updateTimer_timeout()
{
	updateVelocity();
}
