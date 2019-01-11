#ifndef _IMAGEDECODER_H_
#define _IMAGEDECODER_H_

#include <QtGui>
#include <QTime>
class ImageDecoder : public QObject
{
	Q_OBJECT
public:
	ImageDecoder( QObject* parent );

	void setImage( const QByteArray& data,
									unsigned int width,
									unsigned int height,
									unsigned int step,
									const QString& format );
	QImage getImage();
	QByteArray getJpeg();

private:
	void decode();

	mutable QMutex _imageMutex;
	QImage _image;

	QMutex _dataMutex;
	QByteArray _data;
	unsigned int _width;
	unsigned int _height;
	unsigned int _step;
	unsigned int _sequenceNumber;
	QString _format;

	//QTimer* _decodeTimer;
	unsigned int _lastDecodedImage;
};

#endif //_IMAGEDECODER_H_
