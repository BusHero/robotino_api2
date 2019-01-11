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

#ifndef _CONTROLPANEL_H_
#define _CONTROLPANEL_H_

#include <QtGui>

class ControlPanel : public QDialog
{
	Q_OBJECT
public:
	ControlPanel( QWidget* parent );

private Q_SLOTS:
		void on_ccl_clicked();
		void on_cl_clicked();
		void on_n_clicked();
		void on_s_clicked();
		void on_o_clicked();
		void on_w_clicked();
		void on_nw_clicked();
		void on_no_clicked();
		void on_sw_clicked();
		void on_so_clicked();
		void on_stop_clicked();
		void on_speed_valueChanged(int);

		void on_updateTimer_timeout();

private:
	void showEvent( QShowEvent* );
	void hideEvent( QHideEvent* );

	void updateVelocity();

	QTimer* _updateTimer;

	QSlider* speed;
	QPushButton* ccl;
	QPushButton* cl;
	QPushButton* n;
	QPushButton* s;
	QPushButton* o;
	QPushButton* w;
	QPushButton* nw;
	QPushButton* no;
	QPushButton* sw;
	QPushButton* so;
	QPushButton* stop;

	int _vx;
	int _vy;
	int _omega;
	int _speed;
};

#endif
