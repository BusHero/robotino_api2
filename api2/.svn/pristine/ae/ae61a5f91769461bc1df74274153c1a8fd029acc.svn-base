//  Copyright (C) 2004-2009, Robotics Equipment Corporation GmbH

#ifndef _CAMERAVIEW_H_
#define _CAMERAVIEW_H_

#include <QtGui>

#include "rec/robotino/com/Camera.h"

class CameraView : public QWidget
{
	Q_OBJECT
public:
	CameraView();

public Q_SLOTS:
	void setImage( const QByteArray& data,
									unsigned int width,
									unsigned int height,
									unsigned int step,
									const QString& format );

private:
	void setMJPGImage( const QByteArray& data );
	void paintEvent( QPaintEvent* e );

	QLabel* _frequencyLabel;
	QLabel* _numBytesLabel;
	QImage _image;
	QTime _timer;
	QVector<float> _timerBuffer;
	int _timerBufferIndex;
};

#endif
