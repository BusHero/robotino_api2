//  Copyright (C) 2004-2009, Robotics Equipment Corporation GmbH

#ifndef _CONTROLPANEL_H_
#define _CONTROLPANEL_H_

#include <QtGui>

class ControlPanel : public QWidget
{
	Q_OBJECT
public:
	ControlPanel();

Q_SIGNALS:
	void speedChanged( float, float, float );

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
	void updateVelocity();

private:
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
