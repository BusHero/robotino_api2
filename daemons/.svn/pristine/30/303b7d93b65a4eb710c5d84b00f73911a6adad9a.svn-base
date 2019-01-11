#ifndef _IMAGEHANDLER_H_
#define _IMAGEHANDLER_H_

#include <QtGui>

class ImageDecoder;

class ImageHandler : public QThread
{
	Q_OBJECT
public:
	ImageHandler( QObject* parent );

	~ImageHandler();

	QImage getImage() const;

private Q_SLOTS:
	void on_client_image0_changed( const QByteArray& data,
									unsigned int width,
									unsigned int height,
									unsigned int step,
									const QString& format );

private:
	void run();

	mutable QMutex _imageDecoderMutex;
	ImageDecoder* _imageDecoder;
};


#endif //_IMAGEHANDLER_H_
