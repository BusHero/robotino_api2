#ifndef QSERIALPORT_WIN_H
#define QSERIALPORT_WIN_H

#include "qserialport_win_p.h"

#include <QtCore/qcoreevent.h>

#ifndef Q_OS_WINCE
#include <QtCore/qelapsedtimer.h>
#include <QtCore/qvector.h>
#endif

#include <QtCore/qwineventnotifier.h>
#include <algorithm>

QT_BEGIN_NAMESPACE

#ifndef Q_OS_WINCE

class AbstractOverlappedEventNotifier : public QWinEventNotifier
{
    Q_OBJECT
public:
    enum Type { CommEvent, ReadCompletionEvent, WriteCompletionEvent };

    AbstractOverlappedEventNotifier(QSerialPortPrivate *d, Type type, bool manual, QObject *parent)
        : QWinEventNotifier(parent), dptr(d), t(type) {
        ::ZeroMemory(&o, sizeof(o));
        o.hEvent = ::CreateEvent(NULL, manual, FALSE, NULL);
        if (!o.hEvent) {
            dptr->setError(dptr->decodeSystemError());
        } else {
            setHandle(o.hEvent);
            dptr->notifiers[o.hEvent] = this;
        }
    }

    virtual bool processCompletionRoutine() = 0;

    virtual ~AbstractOverlappedEventNotifier() {
        setEnabled(false);
        if (!::CloseHandle(o.hEvent))
            dptr->setError(dptr->decodeSystemError());
    }

    Type type() const { return t; }
    OVERLAPPED *overlappedPointer() { return &o; }

protected:
    bool event(QEvent *e) Q_DECL_OVERRIDE {
        const bool ret = QWinEventNotifier::event(e);
        if (e->type() == QEvent::WinEventAct)
            processCompletionRoutine();
        return ret;
    }

    QSerialPortPrivate *dptr;
    Type t;
    OVERLAPPED o;
};

class CommOverlappedEventNotifier : public AbstractOverlappedEventNotifier
{
    Q_OBJECT
public:
    CommOverlappedEventNotifier(QSerialPortPrivate *d, DWORD eventMask, QObject *parent)
        : AbstractOverlappedEventNotifier(d, CommEvent, false, parent)
        , originalEventMask(eventMask), triggeredEventMask(0) {
        if (!::SetCommMask(dptr->descriptor, originalEventMask))
            dptr->setError(dptr->decodeSystemError());
        else
            startWaitCommEvent();
    }

    void startWaitCommEvent() {
        if (!::WaitCommEvent(dptr->descriptor, &triggeredEventMask, &o)) {
            const QSerialPort::SerialPortError error = dptr->decodeSystemError();
            if (error != QSerialPort::NoError) {
                dptr->setError(dptr->decodeSystemError());
                return;
            }
        }
    }

    bool processCompletionRoutine() Q_DECL_OVERRIDE {
        DWORD numberOfBytesTransferred = 0;

        if (!::GetOverlappedResult(dptr->descriptor, &o, &numberOfBytesTransferred, FALSE))
            dptr->setError(dptr->decodeSystemError());

        bool error = false;

        // Check for unexpected event. This event triggered when pulled previously
        // opened device from the system, when opened as for not to read and not to
        // write options and so forth.
        if (triggeredEventMask == 0)
            error = true;

        // Workaround for standard CDC ACM serial ports, for which triggered an
        // unexpected event EV_TXEMPTY at data transmission.
        if ((originalEventMask & triggeredEventMask) == 0) {
            if ((triggeredEventMask & EV_TXEMPTY) == 0)
                error = true;
        }

        // Start processing a caught error.
        if (error || (EV_ERR & triggeredEventMask))
            dptr->processIoErrors(error);

        if (!error)
            dptr->startAsyncRead();

        return !error;
    }

private:
    DWORD originalEventMask;
    DWORD triggeredEventMask;
};

class ReadOverlappedCompletionNotifier : public AbstractOverlappedEventNotifier
{
    Q_OBJECT
public:
    ReadOverlappedCompletionNotifier(QSerialPortPrivate *d, QObject *parent)
        : AbstractOverlappedEventNotifier(d, ReadCompletionEvent, false, parent) {}

    bool processCompletionRoutine() Q_DECL_OVERRIDE {
        DWORD numberOfBytesTransferred = 0;
        if (!::GetOverlappedResult(dptr->descriptor, &o, &numberOfBytesTransferred, FALSE))
            dptr->setError(dptr->decodeSystemError());

        dptr->completeAsyncRead(numberOfBytesTransferred);

        // start async read for possible remainder into driver queue
        if ((numberOfBytesTransferred > 0) && (dptr->policy == QSerialPort::IgnorePolicy)) {
            dptr->startAsyncRead();
        } else { // driver queue is emplty, so startup wait comm event
            CommOverlappedEventNotifier *n =
                    qobject_cast<CommOverlappedEventNotifier *>(dptr->lookupCommEventNotifier());
            if (n)
                n->startWaitCommEvent();
        }

        return true;
    }
};

class WriteOverlappedCompletionNotifier : public AbstractOverlappedEventNotifier
{
    Q_OBJECT
public:
    WriteOverlappedCompletionNotifier(QSerialPortPrivate *d, QObject *parent)
        : AbstractOverlappedEventNotifier(d, WriteCompletionEvent, false, parent) {}

    bool processCompletionRoutine() Q_DECL_OVERRIDE {
        setEnabled(false);
        DWORD numberOfBytesTransferred = 0;
        if (!::GetOverlappedResult(dptr->descriptor, &o, &numberOfBytesTransferred, FALSE)) {
            numberOfBytesTransferred = 0;
            dptr->setError(dptr->decodeSystemError());
        }

        dptr->completeAsyncWrite(numberOfBytesTransferred);
        return true;
    }
};

#endif

QT_END_NAMESPACE

#endif //QSERIALPORT_WIN_H
