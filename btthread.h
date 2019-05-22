#ifndef BTTHREAD_H
#define BTTHREAD_H

#include <QObject>
#include "threadqmlinterface.h"
#include <QDebug>
#include "btmanager.h"
#include <QThread>

class BtThread : public QThread
{
    Q_OBJECT

private:
    QString m_threadName;
    ThreadQmlInterface* m_interfaceRef;
    BtManager* m_btm;

protected:
    // since qt x.x it automatically calls exec() the "event machine" what makes signals and slots glue together..
    void run ()
    {
        qDebug()<<"Thread: "<<m_threadName<<"started!";
        m_btm = new BtManager(nullptr, m_interfaceRef);

    }

public:
    BtThread(QString threadName, ThreadQmlInterface* refToInterface)
    {
        m_threadName = threadName;
        m_interfaceRef = refToInterface;
    }
};

#endif // BTTHREAD_H
