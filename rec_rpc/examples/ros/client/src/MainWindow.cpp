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

#include "MainWindow.h"
#include "MapView.h"
#include "ToolBar.h"
#include "MenuBar.h"
#include "Actions.h"
#include "Client.h"
#include "AboutDialog.h"

#include "LogView.h"

MainWindow::MainWindow()
: _mapView( NULL )
, _controlPanel( new ControlPanel( this ) )
, _logView( new LogView( this ) )
{
	_controlPanel->hide();

	setWindowTitle( "REC RPC to ROS Client Example" );
	
	_mapView = new MapView( this );

	setCentralWidget( _mapView );

	addToolBar( new ToolBar( this ) );

	this->setMenuBar( new MenuBar( this ) );

	bool connected = true;

	connected &= QObject::connect( &Actions::singleton().showTeleop, SIGNAL( triggered( bool ) ), this, SLOT( on_showTeleop_triggered( bool ) ) );
	connected &= QObject::connect( &Actions::singleton().about, SIGNAL( triggered() ), this, SLOT( on_about_triggered() ) );

	Q_ASSERT( connected );

	addDockWidget( Qt::BottomDockWidgetArea, _logView );
}

void MainWindow::on_showTeleop_triggered( bool show )
{
	if( show )
	{
		_controlPanel->show();
	}
	else
	{
		_controlPanel->hide();
	}
}

void MainWindow::on_about_triggered()
{
	AboutDialog dlg( this );
	dlg.exec();
}
