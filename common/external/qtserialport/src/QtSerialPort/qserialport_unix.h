#ifndef QTSERIALPORTUNIX_H
#define QTSERIALPORTUNIX_H

#include "qserialport_unix_p.h"

#include <errno.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#ifdef Q_OS_MAC
#if defined (MAC_OS_X_VERSION_10_4) && (MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_4)
#include <IOKit/serial/ioss.h>
#endif
#endif

#include <QtCore/qelapsedtimer.h>
#include <QtCore/qsocketnotifier.h>
#include <QtCore/qmap.h>

QT_BEGIN_NAMESPACE

class ReadNotifier : public QSocketNotifier
{
    Q_OBJECT
public:
    ReadNotifier(QSerialPortPrivate *d, QObject *parent)
        : QSocketNotifier(d->descriptor, QSocketNotifier::Read, parent)
        , dptr(d)
    {}

protected:
    bool event(QEvent *e) Q_DECL_OVERRIDE {
        bool ret = QSocketNotifier::event(e);
        if (ret)
            dptr->readNotification();
        return ret;
    }

private:
    QSerialPortPrivate *dptr;
};

class WriteNotifier : public QSocketNotifier
{
    Q_OBJECT
public:
    WriteNotifier(QSerialPortPrivate *d, QObject *parent)
        : QSocketNotifier(d->descriptor, QSocketNotifier::Write, parent)
        , dptr(d)
    {}

protected:
    bool event(QEvent *e) Q_DECL_OVERRIDE {
        bool ret = QSocketNotifier::event(e);
        if (ret)
            dptr->writeNotification(QSerialPortPrivateData::WriteChunkSize);
        return ret;
    }

private:
    QSerialPortPrivate *dptr;
};

class ExceptionNotifier : public QSocketNotifier
{
    Q_OBJECT
public:
    ExceptionNotifier(QSerialPortPrivate *d, QObject *parent)
        : QSocketNotifier(d->descriptor, QSocketNotifier::Exception, parent)
        , dptr(d)
    {}

protected:
    bool event(QEvent *e) Q_DECL_OVERRIDE {
        bool ret = QSocketNotifier::event(e);
        if (ret)
            dptr->exceptionNotification();
        return ret;
    }

private:
    QSerialPortPrivate *dptr;
};

QT_END_NAMESPACE

#endif //QTSERIALPORTUNIX_H