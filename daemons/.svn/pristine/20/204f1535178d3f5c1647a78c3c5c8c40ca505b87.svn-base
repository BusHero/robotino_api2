#ifndef MCHANDLER_H
#define MCHANDLER_H

#include <QtCore>

class McHandler: public QObject
{
    Q_OBJECT
public:
    struct layoutData {
        int id;
        double x,y;
        int pose;
        int path;
    };
    struct statusData {
        int id;
        int status;
        int rId;
    };

    McHandler( QObject* parent );

    std::vector<layoutData> getLayout();
    void getStatus(std::vector<statusData>& sts, std::map< int, std::vector<int> >& path);

private Q_SLOTS:
    void on_mclayout_changed( const QByteArray& data );
    void on_mcstatus_changed( const QByteArray& data );

private:
    mutable QMutex _mutex;

    std::vector<layoutData> _layouts;
    std::vector<statusData> _status;
    std::map< int, std::vector<int> > _paths;
};

#endif // MCHANDLER_H
