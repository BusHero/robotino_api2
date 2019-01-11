#ifndef _DisplayView_H_
#define _DisplayView_H_

#include <QtGui>

class DisplayView : public QGraphicsView
{
public:
	DisplayView();

private:
	void resizeEvent( QResizeEvent* e );
	void mousePressEvent( QMouseEvent* );
};

#endif //_DisplayView_H_
