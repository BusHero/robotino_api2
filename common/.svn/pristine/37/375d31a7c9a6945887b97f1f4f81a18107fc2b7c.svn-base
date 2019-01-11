#include "StateView.h"

#define BOOT_STATE_RECT QRectF( 0.243, 3.052, 12.353, 5.648 )
#define IDLE_STATE_RECT QRectF( 16.552, 3.052, 5.677, 5.648 )
#define MONITOR_STATE_RECT QRectF( 51.134, 3.052, 8.031, 5.648 )
#define NTCERROR_STATE_RECT QRectF( 84.972, 3.052, 12.353, 5.648 )
#define START_STATE_RECT QRectF( 2.827, 12.039, 15.22, 5.644 )
#define CHECKBATTERY_STATE_RECT QRectF( 64.718, 12.039, 12.353, 5.644 )
#define NOBATTERY_STATE_RECT QRectF( 84.972, 12.039, 12.353, 5.644 )
#define STAGE1_STATE_RECT QRectF( 4.261, 20.506, 12.348, 5.643 )
#define STAGE2_STATE_RECT QRectF( 47.856, 20.506, 12.856, 5.643 )
#define STAGE3_STATE_RECT QRectF( 76.761, 20.506, 12.353, 5.643 )
#define STAGE4_STATE_RECT QRectF( 4.441, 82.597, 41.005, 5.644 )
#define ERROR_STATE_RECT QRectF( 50.389, 82.597, 46.936, 5.644 )

enum{
	CHARGER_STATE_None=0,
    CHARGER_STATE_Idle,
    CHARGER_STATE_Monitor, //2
	CHARGER_STATE_CheckBattery,
    CHARGER_STATE_NoBattery, //4
	CHARGER_STATE_NTC_Error,
    CHARGER_STATE_Start, //6
	CHARGER_STATE_Stage1,
    CHARGER_STATE_Stage2, //8
	CHARGER_STATE_Stage3,
    CHARGER_STATE_Stage4, //10
	CHARGER_STATE_Error
};

StateView::StateView( QWidget* parent )
	: QWidget( parent )
	, _svgWidget( new QSvgWidget( this ) )
	, _state( CHARGER_STATE_None )
{
	_svgWidget->load( QString(":/images/Zustandsdiagramm.svg") );
}

QSize StateView::sizeHint() const
{
	return QSize( 600, 800 );
}

void StateView::setState( int state )
{
	_state = state;
	update();
}

void StateView::resizeEvent( QResizeEvent* e )
{
	_svgWidget->resize( e->size() );

	QWidget::resizeEvent( e );
}

void StateView::paintEvent( QPaintEvent* )
{
	qreal xScale = 0.01 * size().width();
	qreal yScale = 0.01 * size().height();

	QPainter painter(this);
	painter.setPen( Qt::red );
	painter.setBrush( Qt::white );

	QRectF sourceRect;

	switch( _state )
	{
	case CHARGER_STATE_Idle:
		sourceRect = IDLE_STATE_RECT;
		break;

	case CHARGER_STATE_Monitor:
		sourceRect = MONITOR_STATE_RECT;
		break;

	case CHARGER_STATE_CheckBattery:
		sourceRect = CHECKBATTERY_STATE_RECT;
		break;

	case CHARGER_STATE_NoBattery:
		sourceRect = NOBATTERY_STATE_RECT;
		break;

	case CHARGER_STATE_NTC_Error:
		sourceRect = NTCERROR_STATE_RECT;
		break;

	case CHARGER_STATE_Start:
		sourceRect = START_STATE_RECT;
		break;

	case CHARGER_STATE_Stage1:
		sourceRect = STAGE1_STATE_RECT;
		break;

	case CHARGER_STATE_Stage2:
		sourceRect = STAGE2_STATE_RECT;
		break;

	case CHARGER_STATE_Stage3:
		sourceRect = STAGE3_STATE_RECT;
		break;

	case CHARGER_STATE_Stage4:
		sourceRect = STAGE4_STATE_RECT;
		break;

	case CHARGER_STATE_Error:
		sourceRect = ERROR_STATE_RECT;
		break;
	}

	if( sourceRect.isValid() )
	{
		painter.drawRect( xScale * sourceRect.x(), yScale * sourceRect.y(), xScale * sourceRect.width(), yScale * sourceRect.height() );
	}
}
