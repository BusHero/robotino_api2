#include "DisplayScene.h"
#include "DisplayDriver.h"

#define SCALE 10.0
#define WHITE qRgb( 255, 255, 255 )
#define DARKBLUE qRgb(  0, 0, 150 )
#define LIGHTBLUE qRgb( 50, 50, 255 )

DisplayScene::DisplayScene()
	: _driver( new DisplayDriver( this ) )
{
	_visibleRect.setWidth( _driver->width() * SCALE );
	_visibleRect.setHeight( _driver->height() * SCALE );

	for( int row = 0; row <= _driver->height(); ++row )
	{
		addLine( 0, row*SCALE, _driver->width()*SCALE, row*SCALE, QPen( Qt::white ) );
	}
	for( int col = 0; col <= _driver->width(); ++col )
	{
		addLine( col*SCALE, 0, col*SCALE, _driver->height()*SCALE, QPen( Qt::white ) );
	}

	_items.resize( _driver->height() );

	for( int row = 0; row < _driver->height(); ++row )
	{
		QVector< QGraphicsItem* >& vec = _items[row];
		vec.resize( _driver->width() );

		for( int col = 0; col < _driver->width(); ++col )
		{
			QGraphicsSimpleTextItem* item = new QGraphicsSimpleTextItem( NULL, this );
			vec[col] = item;

			QFont font( "Courier new", 8 );
			font.setBold( true );

			item->setFont( font );
			item->setPen( QPen( Qt::white ) );
			item->setBrush( QBrush( Qt::white ) );

			item->setPos( col*SCALE + SCALE/9.0 , row*SCALE - SCALE/3.0 );
		}
	}

	bool connected = true;
	connected &= connect( _driver, SIGNAL( changed() ), SLOT( on_changed() ) );
	Q_ASSERT( connected );

	connected &= connect( _driver, SIGNAL( backlightChanged( bool ) ), SLOT( on_backlightChanged( bool ) ) );
	Q_ASSERT( connected );

	setBackgroundBrush( QBrush( DARKBLUE ) );
}

void DisplayScene::on_changed()
{
	for( unsigned int row=0; row<_driver->height(); ++row )
	{
		for( unsigned int col=0; col<_driver->width(); ++col )
		{
			unsigned char ch = _driver->at( col, row );

			if( ch > 7 )
			{
				QVector< QGraphicsItem* >& vec = _items[row];
				QGraphicsSimpleTextItem* item = dynamic_cast<QGraphicsSimpleTextItem*>( vec[col] );
				if( item )
				{
					item->setText( QString( (char)ch ) );
				}
			}
			else
			{
				//switch( ch )
				//{
				//case 0:
				//	p.drawRect( x*_cellWidth+1, (y)*_cellHeight - 1, 2, _cellHeight - 2 );
				//	break;

				//case 1:
				//	p.drawRect( x*_cellWidth+1, (y)*_cellHeight - 1, _cellWidth / 2, _cellHeight - 2 );
				//	break;

				//case 2:
				//	p.drawRect( x*_cellWidth+1, (y)*_cellHeight - 1, _cellWidth - 2, _cellHeight - 2 );
				//	break;

				//case 3:
				//	break;
				//	p.drawRect( x*_cellWidth+1, (y+1)*_cellHeight - 1, _cellWidth - 2, -2 );

				//case 4:
				//	p.drawRect( x*_cellWidth+1, (y+1)*_cellHeight - 1, _cellWidth - 2, -4 );
				//	break;

				//case 5:
				//	p.drawRect( x*_cellWidth+1, (y+1)*_cellHeight - 1, _cellWidth - 2, -_cellHeight / 2 );
				//	break;

				//case 6:
				//	p.drawRect( x*_cellWidth+1, (y+1)*_cellHeight - 1, _cellWidth - 2, -(_cellHeight / 2 + 2) );
				//	break;

				//case 7:
				//	p.drawRect( x*_cellWidth+1, (y+1)*_cellHeight - 1, _cellWidth - 2, - (_cellHeight - 2) );
				//	break;
				//}
			}
		}
	}
}

void DisplayScene::on_backlightChanged( bool on )
{
	if( on )
	{
		setBackgroundBrush( QBrush( LIGHTBLUE ) );
	}
	else
	{
		setBackgroundBrush( QBrush( DARKBLUE ) );
	}
}
