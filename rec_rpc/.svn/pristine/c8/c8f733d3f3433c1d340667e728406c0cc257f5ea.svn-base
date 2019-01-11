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

#include "RECLogo.h"

class HyperLabel : public QLabel
{
public:
	HyperLabel( QWidget* parent )
		: QLabel( parent )
	{
		setCursor( Qt::PointingHandCursor );
		setText( "<a href=\"http://www.servicerobotics.eu\">servicerobotics.eu</a>" );
		setToolTip( "servicerobotics.eu" );
		setPixmap( QPixmap( ":/images/rec.png" ) );
	}

private:
	void mousePressEvent( QMouseEvent* ev )
	{
		if( ev->button() == Qt::LeftButton )
		{
			QDesktopServices::openUrl( QString( "http://www.servicerobotics.eu" ) );
		}
	}

};

RECLogo::RECLogo( QWidget* parent )
: QWidget( parent )
{
	setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

	QHBoxLayout* layout = new QHBoxLayout;
	layout->setMargin( 0 );
	layout->setSpacing( 0 );

	setLayout( layout );

	layout->addStretch( 10000 );
	//layout->addSpacing( 35 );

	HyperLabel* label = new HyperLabel( this );

	layout->addWidget( label );

	layout->addSpacing( 35 );
}
