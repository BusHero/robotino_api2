#include "ChargerView.h"
#include "StateView.h"
#include "SensorView.h"
#include "Actions.h"

ChargerView::ChargerView( QWidget* parent, int id_ )
	: QMainWindow( parent )
	, id( id_ )
	, _stateView( new StateView( this ) )
	, _sensorView( new SensorView( this ) )
{
	setWindowTitle( tr("Charger %1").arg( id ) );

	setCentralWidget( _stateView );

	_sensorView->setAllowedAreas( Qt::AllDockWidgetAreas );
	addDockWidget( Qt::LeftDockWidgetArea, _sensorView );
}

void ChargerView::setReadings( int chargerId, const QVariantMap& readings )
{
	if( id == chargerId )
	{
		_stateView->setState( readings.value( "state" ).toInt() );
		_sensorView->setReadings( readings );
	}
}

void ChargerView::showEvent( QShowEvent* )
{
	Actions::singleton().chargerView[id]->setChecked( true );
}

void ChargerView::closeEvent( QCloseEvent* e )
{
	qDebug() << "ChargerView::closeEvent";
	Q_EMIT closed();
}
