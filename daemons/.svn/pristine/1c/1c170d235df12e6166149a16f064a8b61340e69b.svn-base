#include "MainWindow.h"
#include "ButtonWidget.h"
#include "DisplayScene.h"
#include "DisplayView.h"

#define WHITE qRgb( 255, 255, 255 )
#define DARKBLUE qRgb(  0, 38, 255 )
#define LIGHTBLUE qRgb( 0, 181, 181 )

MainWindow::MainWindow()
{
	QDockWidget* buttonDock = new QDockWidget( "Buttons", this );
	buttonDock->setWidget( new ButtonWidget );
	buttonDock->setFeatures( QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
	addDockWidget(Qt::BottomDockWidgetArea, buttonDock );
	buttonDock->setFloating( false );
	buttonDock->setVisible( true );

	DisplayScene* scene = new DisplayScene;
	DisplayView* view = new DisplayView;

	view->setScene( scene );

	setCentralWidget( view );
}

//void MainWindow::updateImage()
//{
//	if( _backlight )
//	{
//		_pixmap.fill( LIGHTBLUE );
//	}
//	else
//	{
//		_pixmap.fill( DARKBLUE );
//	}
//
//	QPainter p( &_pixmap );
//	p.setFont( QFont( "Courier new", 12 ) );
//	p.setBrush( Qt::black );
//	for( unsigned int y=0; y<_displayDriver.height(); ++y )
//	{
//		for( unsigned int x=0; x<_displayDriver.width(); ++x )
//		{
//			unsigned char ch = _displayDriver.at( x, y );
//
//			if( ch > 7 )
//			{
//				p.drawText( x*_cellWidth, (y+1)*_cellHeight - 1, QString( (char)ch ) );
//			}
//			else
//			{
//				switch( ch )
//				{
//				case 0:
//					p.drawRect( x*_cellWidth+1, (y)*_cellHeight - 1, 2, _cellHeight - 2 );
//					break;
//
//				case 1:
//					p.drawRect( x*_cellWidth+1, (y)*_cellHeight - 1, _cellWidth / 2, _cellHeight - 2 );
//					break;
//
//				case 2:
//					p.drawRect( x*_cellWidth+1, (y)*_cellHeight - 1, _cellWidth - 2, _cellHeight - 2 );
//					break;
//
//				case 3:
//					break;
//					p.drawRect( x*_cellWidth+1, (y+1)*_cellHeight - 1, _cellWidth - 2, -2 );
//
//				case 4:
//					p.drawRect( x*_cellWidth+1, (y+1)*_cellHeight - 1, _cellWidth - 2, -4 );
//					break;
//
//				case 5:
//					p.drawRect( x*_cellWidth+1, (y+1)*_cellHeight - 1, _cellWidth - 2, -_cellHeight / 2 );
//					break;
//
//				case 6:
//					p.drawRect( x*_cellWidth+1, (y+1)*_cellHeight - 1, _cellWidth - 2, -(_cellHeight / 2 + 2) );
//					break;
//
//				case 7:
//					p.drawRect( x*_cellWidth+1, (y+1)*_cellHeight - 1, _cellWidth - 2, - (_cellHeight - 2) );
//					break;
//				}
//			}
//		}
//	}
//
//	update();
//}
