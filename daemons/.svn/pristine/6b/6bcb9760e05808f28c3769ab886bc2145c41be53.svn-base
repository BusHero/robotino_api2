#ifndef MAPEDITOR_H
#define MAPEDITOR_H

#include <QObject>
#include "ShapeData.h"

class MapEditor : public QObject
{
    Q_OBJECT
public:
    explicit MapEditor(QObject *parent = 0);

    void editMap(const QString& srcImageFileName, const QString& destImageFileName, const QList<ShapeData>& shapes);

signals:

public slots:

};

#endif // MAPEDITOR_H
