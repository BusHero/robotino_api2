#ifndef _POSITIONTABLE_H_
#define _POSITIONTABLE_H_

#include <QtGui>
#include <QtWidgets>
#include "rec/grappler/HardwareInfo.h"

class PositionTable : public QTableWidget
{
	Q_OBJECT
public:
	PositionTable( QWidget* parent );

	QVector<quint16> positions() const;
	QVector<quint16> currentPositions() const;

	QVector<quint16> speeds() const;

public Q_SLOTS:
	void setServosDetected( const rec::grappler::HardwareInfo& /*info*/ );
	void setAllServoPositions( const rec::grappler::HardwareInfo& info, bool useAsSetPoint );
	void setAxesLimits( const rec::grappler::HardwareInfo& info );
};

#endif //_POSITIONTABLE_H_
