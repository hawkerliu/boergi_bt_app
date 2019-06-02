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

private slots:
    void onBTMdestroyed()
    {
        //sleep(1);
        this->exit();

        deleteLater();
    }

    void closeThread()
    {
        this->exit();
        deleteLater();
        //sleep(1);
        m_btm->~BtManager();
    }

protected:
    // since qt x.x it automatically calls exec() the "event machine" what makes signals and slots glue together..
    void run ()
    {
        qDebug()<<"Thread: "<<m_threadName<<"started!";
        m_btm = new BtManager(nullptr, m_interfaceRef);
        //connect(m_btm, SIGNAL(destroyed()), this, SLOT(onBTMdestroyed()));
        connect(m_interfaceRef, SIGNAL(closeBTMthread()),this,SLOT(closeThread()));

        exec();


    }

public:
    BtThread(QString threadName, ThreadQmlInterface* refToInterface)
    {
        m_threadName = threadName;
        m_interfaceRef = refToInterface;
    }
    ~BtThread()
    {
        qDebug()<<"Thread killed";
    }
};




#endif // BTTHREAD_H
