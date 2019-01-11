#ifndef _ROBOT_H_
#define _ROBOT_H_

#include <QtCore>
#include "rec/robotino/com/all.h"

class Robot : public QObject
{
Q_OBJECT
public:
  Robot( const QString& address );
  bool init();

public Q_SLOTS:
  void on_omnidrv( int x, int y, int phi );
  void on_stop();

Q_SIGNALS:
  void log( const QString& );
  void log( const QString&, int );

private:
  rec::robotino::com::Com _com;
  rec::robotino::com::OmniDrive _omniDrive;
  rec::robotino::com::Bumper _bumper;
  
  const QString _address;
};

#endif //_ROBOT_H_
