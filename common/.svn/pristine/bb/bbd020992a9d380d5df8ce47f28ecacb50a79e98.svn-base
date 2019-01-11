#ifndef _SENSORVIEW_H_
#define _SENSORVIEW_H_

#include <QtCore>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets>
#else
#include <QtGui>
#endif

class SensorView : public QDockWidget
{
	Q_OBJECT
public:
	SensorView( QWidget* parent );

public Q_SLOTS:
	void setReadings( const QVariantMap& readings );

private:
	QTableWidget* _table;
};

#endif //_SENSORVIEW_H_