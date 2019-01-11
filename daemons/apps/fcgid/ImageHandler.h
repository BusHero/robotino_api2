#ifndef _IMAGEHANDLER_H_
#define _IMAGEHANDLER_H_

#include <QtGui>

class ImageDecoder;

class ImageHandler : public QObject
{
	Q_OBJECT
public:
	ImageHandler( QObject* parent );

	~ImageHandler();

	QImage getImage();
	QByteArray getJpeg();

	int timeSinceLastImg();

	private Q_SLOTS:
		void on_client_image0_changed( const QByteArray& data,
			unsigned int width,
			unsigned int height,
			unsigned int step,
			const QString& format );

		private Q_SLOTS:
			void on_imageRequestTimeoutTimer_timeout();

private:
	void customEvent( QEvent* e );

	enum
	{
		SetImageTopicEnableEventId = QEvent::User
	};

	class SetImageTopicEnableEvent : public QEvent
	{
	public:
		SetImageTopicEnableEvent()
			: QEvent( (QEvent::Type)SetImageTopicEnableEventId )
		{
		}
	};

	mutable QMutex _imageDecoderMutex;
	ImageDecoder* _imageDecoder;
	QTime lastImageTime;
	QTimer* _imageRequestTimeoutTimer;
};


#endif //_IMAGEHANDLER_H_
