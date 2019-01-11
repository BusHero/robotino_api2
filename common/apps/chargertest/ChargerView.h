#ifndef _CHARGERVIEW_H_
#define _CHARGERVIEW_H_

#include <QtCore>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets>
#else
#include <QtGui>
#endif

class StateView;
class SensorView;

class ChargerView : public QMainWindow
{
	Q_OBJECT
public:
	ChargerView( QWidget* parent, int id );

	const int id;

Q_SIGNALS:
	void closed();

public Q_SLOTS:
	void setReadings( int chargerId, const QVariantMap& readings );

private Q_SLOTS:

private:
	StateView* _stateView;
	SensorView* _sensorView;

	void closeEvent( QCloseEvent* );
	void showEvent( QShowEvent* );
};

#endif //_CHARGERVIEW_H_
