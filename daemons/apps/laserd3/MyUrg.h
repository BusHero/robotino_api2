#ifndef _MYURG_H_
#define _MYURG_H_

#include <QtCore>

class MyUrg : public QThread
{
	Q_OBJECT
public:
	MyUrg(const QString& device, unsigned int channel);
	~MyUrg();

signals:
	void scan(unsigned int channel, QVector<float> ranges, QVector<float> intensities, QVariantMap parameters);

private:
	void run();

	bool _run;
	const QString _device;
	const unsigned int _channel;
};

#endif //_MYURG_H_
