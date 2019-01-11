//  Copyright (C) 2004-2009, Robotics Equipment Corporation GmbH

#include "ControlPanel.h"

ControlPanel::ControlPanel()
: _vx( 0 )
, _vy( 0)
, _omega( 0 )
, _speed( 20 )
{
	QGridLayout* layout = new QGridLayout( this );
	setLayout( layout );

	ccl = new QPushButton( this );
	ccl->setIcon( QPixmap( ":/icons/ccl.png" ) );

	cl = new QPushButton( this );
	cl->setIcon( QPixmap( ":/icons/cl.png" ) );

	n = new QPushButton( this );
	n->setIcon( QPixmap( ":/icons/n.png" ) );

	s = new QPushButton( this );
	s->setIcon( QPixmap( ":/icons/s.png" ) );

	w = new QPushButton( this );
	w->setIcon( QPixmap( ":/icons/w.png" ) );

	o = new QPushButton( this );
	o->setIcon( QPixmap( ":/icons/o.png" ) );

	nw = new QPushButton( this );
	nw->setIcon( QPixmap( ":/icons/nw.png" ) );

	no = new QPushButton( this );
	no->setIcon( QPixmap( ":/icons/no.png" ) );

	sw = new QPushButton( this );
	sw->setIcon( QPixmap( ":/icons/sw.png" ) );

	so = new QPushButton( this );
	so->setIcon( QPixmap( ":/icons/so.png" ) );

	stop = new QPushButton( this );
	stop->setIcon( QPixmap( ":/icons/stop.png" ) );

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

	layout->addWidget( cl, 1, 0 );
	layout->addWidget( ccl, 1, 4 );

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

	QTimer* updateTimer = new QTimer( this );
	updateTimer->setSingleShot( false );
	updateTimer->setInterval( 100 );
	updateTimer->start();

	bool connected = true;

	connected &= connect( ccl, SIGNAL( clicked() ), this, SLOT( on_ccl_clicked() ) );
	connected &= connect( cl, SIGNAL( clicked() ), this, SLOT( on_cl_clicked() ) );
	connected &= connect( n, SIGNAL( clicked() ), this, SLOT( on_n_clicked() ) );
	connected &= connect( s, SIGNAL( clicked() ), this, SLOT( on_s_clicked() ) );
	connected &= connect( w, SIGNAL( clicked() ), this, SLOT( on_w_clicked() ) );
	connected &= connect( o, SIGNAL( clicked() ), this, SLOT( on_o_clicked() ) );
	connected &= connect( nw, SIGNAL( clicked() ), this, SLOT( on_nw_clicked() ) );
	connected &= connect( no, SIGNAL( clicked() ), this, SLOT( on_no_clicked() ) );
	connected &= connect( sw, SIGNAL( clicked() ), this, SLOT( on_sw_clicked() ) );
	connected &= connect( so, SIGNAL( clicked() ), this, SLOT( on_so_clicked() ) );
	connected &= connect( stop, SIGNAL( clicked() ), this, SLOT( on_stop_clicked() ) );
	connected &= connect( speed, SIGNAL( valueChanged(int) ), this, SLOT( on_speed_valueChanged(int) ) );
	connected &= connect( updateTimer, SIGNAL( timeout() ), this, SLOT( updateVelocity() ) );

	Q_ASSERT( connected );
}

void ControlPanel::updateVelocity()
{
	Q_EMIT speedChanged( 0.005f * _speed * _vx, 0.005f * _speed * _vy, 0.02 * _speed * _omega );
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