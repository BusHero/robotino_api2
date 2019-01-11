#ifndef _POSITIONSTORAGE_H_
#define _POSITIONSTORAGE_H_

#include <QtGui>
#include <QtWidgets>

class PositionStorage : public QListWidget
{
	Q_OBJECT
public:
	PositionStorage( QWidget* parent );

Q_SIGNALS:
	void load( const QString& pos );

private Q_SLOTS:
	void on_load();
	void on_remove();
	void on_edit();
	void on_saveToFile();
	void on_loadFromFile();

private:
	void mousePressEvent( QMouseEvent* e );

	void hideEvent( QHideEvent* e );
};

#endif //_POSITIONSTORAGE_H_
