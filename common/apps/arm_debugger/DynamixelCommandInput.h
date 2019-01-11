#ifndef _DYNAMIXELCOMMANDINPUT_H_
#define _DYNAMIXELCOMMANDINPUT_H_

#include <QtGui>
#include <QtWidgets>

class DynamixelCommandInput : public QWidget
{
	Q_OBJECT
public:
	DynamixelCommandInput( QWidget* parent );

Q_SIGNALS:
	void sendCommand( const QByteArray& command );
	void error( const QString& errorMessage );

private Q_SLOTS:
	void on_sendButton_clicked();

private:
	QComboBox* _channelSelector;
	QSpinBox* _id;
	QSpinBox* _instr;
	QLineEdit* _params;
};

#endif //_DYNAMIXELCOMMANDINPUT_H_
