#ifndef THREADQMLINTERFACE_H
#define THREADQMLINTERFACE_H

#include <QObject>
#include <btmanager.h>
#include <QThread>
#include <QMutex>

class BtManager;

class ThreadQmlInterface : public QObject
{
    Q_OBJECT
    // The Q_PROPERTY only makes MOC to generate the QML bindings.. we still have to make our setters and getters.
    Q_PROPERTY(bool isActive MEMBER m_isActive NOTIFY isActiveChanged)
    Q_PROPERTY(bool someBufferHasFreshData MEMBER m_someBufferHasFreshData NOTIFY someBufferHasFreshDataChanged)
    Q_PROPERTY(QString someString MEMBER m_someString NOTIFY someStringChanged)

private:
    bool m_isActive;
    BtManager* m_refToBtManager;
    bool m_someBufferHasFreshData;
    QStringList m_someStringBuffer;
    QString m_someString;


public:
    explicit ThreadQmlInterface(QObject *parent = nullptr);

    void setSomeString(QString str)
    {
        m_someString = str;
        emit someStringChanged();
    }


    Q_INVOKABLE void messageFromQml(const QString &str)
    {
        qDebug()<<"Somebody spit in the pipe: "<<str;
    }

    QMutex someStringBufferMutex;

    void addOneToSomeBuffer(QString tstr)
    {
        someStringBufferMutex.lock();

        m_someStringBuffer.append(tstr);
        m_someBufferHasFreshData = true;
        emit someBufferHasFreshDataChanged(); // Please test if it gets emitted automatically to QML, this call might be not necessary..

        someStringBufferMutex.unlock();

    }
    Q_INVOKABLE void getAllFromSomeBuffer(void)
    {
        someStringBufferMutex.lock();
        while (m_someStringBuffer.size())
        {
            emit someStringBufferPushesData(m_someStringBuffer.last());
            m_someStringBuffer.removeLast();
        }
        m_someBufferHasFreshData = false;
        someStringBufferMutex.unlock();
    }
    Q_INVOKABLE QString getLastFromSomeBuffer(void)
    {
        if (!m_someStringBuffer.size())
            return "No new String..";

        QString tmpString;
        someStringBufferMutex.lock();

        tmpString = m_someStringBuffer.last();
        m_someStringBuffer.removeLast();

        someStringBufferMutex.unlock();
        return tmpString;
    }
    void setRefToBtManager(BtManager* ref)
    {
        m_isActive = true;
        if ( ref != nullptr)
            m_refToBtManager = ref;
        else
            qCritical("We got pointed to nowhere..");
    }



signals:
    void isActiveChanged();
    void someStringChanged();
    void someBufferHasFreshDataChanged();
    void someStringBufferPushesData(QString &str);

    // I added them just to make the app run, but it will still lag if we just dispatch them ungracefully through
    // An other idea would be to start a timer in qml and fetch the data periodically through the interface, but controlled by the qml timer.
    void newDataAvailable();
    void updateTemperature(const QPointF& pt);
    void updatePressure(const QPointF& pp);
    void updateAltitude(const QPointF& pa);
    void updateVolume(const QPointF& pv);
    void updateFrequency(const double& f);

public slots:
};

#endif // THREADQMLINTERFACE_H
