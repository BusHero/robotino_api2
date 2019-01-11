#ifndef _CAMERAPARAMETERS_H_
#define _CAMERAPARAMETERS_H_

#include <QtGui>

class CameraParameters : public QListWidget
{
	Q_OBJECT
public:
	CameraParameters();

public Q_SLOTS:
	void setSettings( unsigned int width, unsigned int height, const QString& format );

	void setCapablitities( const QMap<QString, QVector<QSize> >& capabilities );

Q_SIGNALS:
	void settingsChanged( unsigned int width,
							unsigned int height, const QString& format );

private Q_SLOTS:
	void on_itemClicked ( QListWidgetItem* item );

private:
};

#endif //_CAMERAPARAMETERS_H_
