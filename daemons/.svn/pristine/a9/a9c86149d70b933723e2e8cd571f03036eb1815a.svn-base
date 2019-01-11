#ifndef _HANDLER_H_
#define _HANDLER_H_

#include <QtCore>

namespace rec
{
	namespace vema
	{
		class VEMA;
	}
}

class Handler : public QObject
{
	Q_OBJECT
public:
	Handler( const QString& port );
	~Handler();

public Q_SLOTS:
	void start();
	void set_pressure( const QVector<float>& );

private Q_SLOTS:
	void step();

private:
	void initVEMA();
	void timerEvent( QTimerEvent* e );

	int _timerId;

	rec::vema::VEMA* _vema;

	int _max_pressure;

	int _intern2Extern[8];
	int _extern2Intern[8];

	static const QVector< float > _stringPotsZero;
};

#endif //_HANDLER_H_
