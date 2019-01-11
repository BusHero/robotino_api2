#ifndef _ACTORVIEW_H_
#define _ACTORVIEW_H_

#include <QtCore>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "rec/robotino3/iocom/IOCom.h"

class ActorView : public QTableWidget
{
	Q_OBJECT
public:
	ActorView( QWidget* parent, rec::robotino3::iocom::IOCom* com );

private Q_SLOTS:
	void on_itemClicked( QTableWidgetItem* item );
	void on_itemChanged( QTableWidgetItem * item );
	void on_setMotorSpeedTimer_timeout();

private:
	rec::robotino3::iocom::IOCom* _com;

	QVector< QTimer* > _setMotorSpeedTimer;
};

#endif //_ACTORVIEW_H_
