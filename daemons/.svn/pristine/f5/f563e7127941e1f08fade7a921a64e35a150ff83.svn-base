#include "MapEditor.h"
#include <QImage>
#include <QStringList>
#include <QPainter>
#include "rec/robotino/daemons/Log.h"

MapEditor::MapEditor(QObject *parent) :
    QObject(parent)
{
}

void MapEditor::editMap( const QString& srcImageFileName, const QString& destImageFileName, const QList<ShapeData>& shapes)
{
    QImage image( srcImageFileName );

    if (image.isNull())
    {
        rec::robotino::daemons::Log::singleton()->log("Failed to load image: " + srcImageFileName,0);
        return;
    }

    image = image.convertToFormat(QImage::Format_RGB32);
    rec::robotino::daemons::Log::singleton()->log(QString("shapes num = %1").arg(shapes.length()),0);

    QList<ShapeData> shapesSorted = shapes;
    qSort( shapesSorted.begin(), shapesSorted.end(), shapeDataLessThan );

    QPainter painter;
    painter.begin(&image);

    Q_FOREACH( const ShapeData& shape, shapesSorted )
    {
        QMap<QString, double> paraMap;
        QStringList paras = shape.params.split(",", QString::SkipEmptyParts);

        Q_FOREACH( const QString& para, paras)
        {
            QStringList parts = para.split("=");
            if(parts.size() != 2){
                rec::robotino::daemons::Log::singleton()->log("Invalid shape params",0);
                return;
            }
            paraMap.insert(parts[0], parts[1].toDouble());

            rec::robotino::daemons::Log::singleton()->log(QString("para %1 = %2").arg(parts[0]).arg(parts[1].toDouble()),0);
        }

        if("circle" == shape.shape)
        {
            rec::robotino::daemons::Log::singleton()->log("draw circle",0);

            if("Free" == shape.passability)
            {
                painter.setBrush(QBrush(Qt::green));
                painter.setPen(Qt::green);
            }
            else
            {
                painter.setBrush(QBrush(Qt::red));
                painter.setPen(Qt::red);
            }
            painter.drawEllipse(QPoint((int)paraMap["cx"],(int)paraMap["cy"]),(int)paraMap["R"],(int)paraMap["R"]);
        }
        else if("rect" == shape.shape)
        {
            rec::robotino::daemons::Log::singleton()->log("rect",0);

            if("Free" == shape.passability)
            {
                painter.setBrush(QBrush(Qt::white));
                painter.setPen(Qt::white);
            }
            else
            {
                painter.setBrush(QBrush(Qt::red));
                painter.setPen(Qt::red);
            }
            painter.drawRect(paraMap["left"],paraMap["top"],paraMap["width"],paraMap["height"]);
        }
    }

    painter.end();

    image.save( destImageFileName,"ppm");
}
