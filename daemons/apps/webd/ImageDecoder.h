#ifndef _IMAGEDECODER_H_
#define _IMAGEDECODER_H_

#include <QtGui>

class ImageDecoder : public QObject
{
	Q_OBJECT
public:
	ImageDecoder();

	void setImage( const QByteArray& data,
									unsigned int width,
									unsigned int height,
									unsigned int step,
									const QString& format );

	QImage getImage() const;

private Q_SLOTS:
	void decode();

private:
	mutable QMutex _imageMutex;
	QImage _image;

	QMutex _dataMutex;
	QByteArray _data;
	unsigned int _width;
	unsigned int _height;
	unsigned int _step;
	unsigned int _sequenceNumber;
	QString _format;

	QTimer* _decodeTimer;
	unsigned int _lastDecodedImage;
};

#endif //_IMAGEDECODER_H_
