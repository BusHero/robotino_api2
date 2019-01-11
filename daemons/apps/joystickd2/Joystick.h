#ifndef _JOYSTICK_H_
#define _JOYSTICK_H_

#include <QtCore>

#define JS_EVENT_BUTTON 0x01
#define JS_EVENT_AXIS   0x02
#define JS_EVENT_INIT   0x80

#define JOYSTICK_DEVNAME "/dev/input/js0"

struct js_event
{
  unsigned int time;
  short value;
  unsigned char type;
  unsigned char number;
};

struct xbox_js_event
{
  int button[13];
  int stick_left_x; // axis 0
  int stick_left_y; // axis 1
  int stick_right_x; // axis 3 
  int stick_right_y; // axis 4
  int lt; // axis 2
  int rt; // axis 5 
};

class Joystick : public QThread
{
Q_OBJECT
public:
  Joystick();
  ~Joystick();
  bool init();

Q_SIGNALS:
  void omnidrv( int x, int y, int phi );
  void stop();
  void log( const QString& );
  void log( const QString&, int );

private:
  void run();
  int calAcceleration(int);
  int openJoystick();
  int readJoystickEvent( struct js_event *jse );
  void closeJoystick();
  int getJoystickStatus( struct xbox_js_event *xjse );

  int _joystick_fd;
};
#endif //_JOYSTICK_H_
