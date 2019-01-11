#ifndef _Client_H_
#define _Client_H_

#include <QtCore>
#include <QtNetwork>

#include "rec/robotino/rpc/Client.h"

class Camera;

class Client : public rec::robotino::rpc::Client
{
	Q_OBJECT
public:
	Client( const QString& device, int channel, const QString& name, bool isTest );
	~Client();

private Q_SLOTS:
	void on_timer_timeout();
	void on_restartTimer_timeout();
	void on_checkDeviceTimer_timeout();

	void on_is_connected();
	void on_is_disconnected();

	void on_set_camera_settings_changed( unsigned int width, unsigned int height, QString format );
	void on_set_camera_control_changed( const QString& name, int value );
	void on_image_info_changed( const rec::rpc::ClientInfoSet& );

private:
	QVector<QSize> parseResolutions( const QString& resStr ) const;
	void parseCalibrations( const QString& capabilitiesSectionName );

	void start();
	void restart();
	void stop();

	QTimer* _timer;
	QTimer* _restartTimer;
	QTimer* _checkDeviceTimer;
	Camera* _camera;
	int _image_width;
	int _image_height;
	QString _image_format;

	QString _device;
	int _channel;
	QString _cameraName;

	bool _isTest;

	QMap<QString, QVector<QSize> > _capabilities;
	QMap< QSize, QVector<double> > _calibrations;
	
	QSize _fixedResolution;
	QString _fixedFormat;
};

inline bool operator<(const QSize &s1, const QSize &s2 )
 {
	 if( s1.width() < s2.width() )
         return true;
	 if( s1.width() > s2.width() )
         return false;
     return s1.height() < s2.height();
 }

#endif //_Client_H_
