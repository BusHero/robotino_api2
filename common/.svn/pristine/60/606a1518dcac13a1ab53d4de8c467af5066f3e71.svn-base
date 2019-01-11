#ifndef _STATEVIEW_
#define _STATEVIEW_

#include <QtCore>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include <QtSvg>

class StateView : public QWidget
{
	Q_OBJECT
public:
	StateView( QWidget* parent );

	QSize sizeHint() const;

public Q_SLOTS:
	void setState( int state );

private:
	QSvgWidget* _svgWidget;
	int _state;

	void resizeEvent( QResizeEvent* e );
	void paintEvent( QPaintEvent* e );
};

#endif //_STATEVIEW_
