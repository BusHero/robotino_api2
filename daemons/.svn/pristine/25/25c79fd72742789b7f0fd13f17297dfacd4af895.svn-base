#ifndef SHAPEDATA_H
#define SHAPEDATA_H

#include <QString>

class ShapeData
{
public:
    ShapeData();

    ShapeData(QString shape_,int z_value_, QString passability_, QString params_){
        shape = shape_;
        z_value = z_value_;
        passability = passability_;
        params = params_;
    }

    QString shape ;
    int z_value;
    QString passability;
    QString params;
};

static bool shapeDataLessThan( const ShapeData& a, const ShapeData& b )
{
    return a.z_value < b.z_value;
}

#endif // SHAPEDATA_H
