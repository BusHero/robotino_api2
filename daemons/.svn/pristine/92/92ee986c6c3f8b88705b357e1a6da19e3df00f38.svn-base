#include "DisplayView.h"
#include "DisplayScene.h"

DisplayView::DisplayView()
{
	setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
	setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
}

void DisplayView::resizeEvent( QResizeEvent* e )
{
	fitInView( scene()->sceneRect() );
}

void DisplayView::mousePressEvent( QMouseEvent* )
{
	QRectF rect = static_cast<DisplayScene*>( scene() )->visibleRect();

	QPixmap pix( rect.width() * 3 + 10, rect.height() * 3 + 10);
	QRectF rect2( 4, 4, pix.rect().width()-10, pix.rect().height()-10 );

	QPainter p( &pix );
	p.fillRect( pix.rect(), QBrush( Qt::white ) );
	scene()->render( &p, rect2, rect );
	p.end();

	QClipboard* clipboard = QApplication::clipboard();
	clipboard->setPixmap( pix );
}
