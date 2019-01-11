#ifndef _DISPLAYSCENE_H_
#define _DISPLAYSCENE_H_

#include <QtGui>

class DisplayDriver;

class DisplayScene : public QGraphicsScene
{
	Q_OBJECT
public:
	DisplayScene();

	QRectF visibleRect() const { return _visibleRect; }

private Q_SLOTS:
	void on_changed();
	void on_backlightChanged( bool );

private:
	DisplayDriver* _driver;

	QRectF _visibleRect;
	QVector< QVector< QGraphicsItem* > > _items;
};

#endif //_DISPLAYSCENE_H_
