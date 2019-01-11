/*
Copyright (c) 2011, REC Robotics Equipment Corporation GmbH, Planegg, Germany
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

- Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation and/or
  other materials provided with the distribution.
- Neither the name of the REC Robotics Equipment Corporation GmbH nor the names of
  its contributors may be used to endorse or promote products derived from this software
  without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_

#include <QtCore>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "Client.h"
#include "TopicInfoView.h"
#include "LogView.h"

class MainWindow : public QWidget
{
	Q_OBJECT
public:
	MainWindow();
	~MainWindow();

private:
	void updateWidgets( bool connected );

	Client _client;

	QComboBox* _address;
	QSpinBox* _port;
	QPushButton* _connectPb;
	QCheckBox* _localIPCEnabled;
	QCheckBox* _blocking;
	QCheckBox* _enableTopics;

	QLabel* _versionLabel;

	QDoubleSpinBox* _a;
	QDoubleSpinBox* _b;
	QDoubleSpinBox* _c;
	QComboBox* _opCb;
	QLabel* _equalLabel;
	QPushButton* _calcPb;
	QPushButton* _textPb;
	QPushButton* _startStopPb;

	QLineEdit* _imgPath;
	QPushButton* _openImgPb;

	QLabel* _imgView;

	LogView* _logView;
	TopicInfoView* _imageInfoView;

	QString _imagePath;

	static const QString _texts[20];
	int _textIndex;

	QTimer _textTimer;

private Q_SLOTS:
	void on_client_connected();
	void on_client_disconnected( rec::rpc::ErrorCode errorCode );
	void on_client_calcResultReceived( float v, rec::rpc::ErrorCode e );
	void on_client_imageReceived( const QImage& img );

	void on_connectPb_clicked();
	void on_enableTopics_toggled( bool enabled );
	void on_calcPb_clicked();
	void on_openImgPb_clicked();

	void on_textPb_clicked();

	void on_textTimer_timeout();
	void on_startStopPb_clicked();
};

#endif
