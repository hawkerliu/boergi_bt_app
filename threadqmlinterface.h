#ifndef THREADQMLINTERFACE_H
#define THREADQMLINTERFACE_H

#include <QObject>
#include <btmanager.h>
#include <QThread>
#include <QMutex>
#include <QVector>

// Mode states
#define STATE_UNCONNECTED   0
#define STATE_INIT          1
#define STATE_UPDATED       2
#define STATE_REQUESTED     3
#define STATE_FAILED        4
//#define STATE_GET_FAILED    5

// Send Machine
// enum
#define PRS_MR 0
#define PRS_OSR 1
#define TEMP_MR 2
#define TEMP_OSR 3
// states
//#define SENDING 5
//#define WAITING 6
// flags
#define DIRTY 0x01
#define SENT 0x02
#define UPDATED 0x03
#define TIMEOUTED 0x04
#define WAITING_FOR_ACK 0x05
#define ACKED 0x06
#define NACKED 0x07

class BtManager;

class ThreadQmlInterface : public QObject
{
    Q_OBJECT
    // The Q_PROPERTY only makes MOC to generate the QML bindings.. we still have to make our setters and getters.
    Q_PROPERTY(bool isActive MEMBER m_isActive NOTIFY isActiveChanged)
    Q_PROPERTY(bool someBufferHasFreshData MEMBER m_someBufferHasFreshData NOTIFY someBufferHasFreshDataChanged)
    Q_PROPERTY(QString someString MEMBER m_someString NOTIFY someStringChanged)

    // Chart
    Q_PROPERTY(double maxOfTemp MEMBER maxT NOTIFY maxOfTempChanged)
    Q_PROPERTY(double minOfTemp MEMBER minT NOTIFY minOfTempChanged)
    Q_PROPERTY(double maxOfPres MEMBER maxP NOTIFY maxOfPresChanged)
    Q_PROPERTY(double minOfPres MEMBER minP NOTIFY minOfPresChanged)
    Q_PROPERTY(double maxOfAlt MEMBER maxA NOTIFY maxOfAltChanged)
    Q_PROPERTY(double minOfAlt MEMBER minA NOTIFY minOfAltChanged)
    Q_PROPERTY(double maxOfVol MEMBER maxV NOTIFY maxOfVolChanged)
    Q_PROPERTY(double minOfVol MEMBER minV NOTIFY minOfVolChanged)

    // Config
    Q_PROPERTY(int prs_mr READ getPrsMr WRITE setPrsMr NOTIFY presMrChanged)
    Q_PROPERTY(int prs_osr READ getPrsOsr WRITE setPrsOsr NOTIFY presOsrChanged)
    Q_PROPERTY(int temp_mr READ getTempMr WRITE setTempMr NOTIFY tempMrChanged)
    Q_PROPERTY(int temp_osr READ getTempOsr WRITE setTempOsr NOTIFY tempOsrChanged)
    Q_PROPERTY(int modeState MEMBER m_modeState NOTIFY modeStateChanged)
    Q_PROPERTY(int prev_prs_mr READ getPrevPrsMr WRITE setPrevPrsMr)
    Q_PROPERTY(int prev_prs_osr READ getPrevPrsOsr WRITE setPrevPrsOsr)
    Q_PROPERTY(int prev_temp_mr READ getPrevTempMr WRITE setPrevTempMr)
    Q_PROPERTY(int prev_temp_osr READ getPrevTempOsr WRITE setPrevTempOsr)


    // Discovery
    Q_PROPERTY(QVariant devices READ devices NOTIFY devicesUpdated)

private:
    bool m_isActive;
    BtManager* m_refToBtManager;
    bool m_someBufferHasFreshData;
    QStringList m_someStringBuffer;
    QString m_someString;
    QList<QObject*> m_devices;


    // Discovery

//    Q_INVOKABLE void exitApplication(void) {qDebug()<<"trying to exit";}


public:
    explicit ThreadQmlInterface(QObject *parent = nullptr);

    int m_prs_mr;
    int m_prs_osr;
    int m_temp_mr;
    int m_temp_osr;

    int m_prev_prs_mr;
    int m_prev_prs_osr;
    int m_prev_temp_mr;
    int m_prev_temp_osr;

    int getPrevPrsMr(){return m_prev_prs_mr;}
    int getPrevPrsOsr(){return m_prev_prs_osr;}
    int getPrevTempMr(){return m_prev_temp_mr;}
    int getPrevTempOsr(){return m_prev_temp_osr;}
    void setPrevPrsMr(int mr){m_prev_prs_mr = mr;}
    void setPrevPrsOsr(int osr){m_prev_prs_osr = osr;}
    void setPrevTempMr(int mr){m_prev_temp_mr = mr;}
    void setPrevTempOsr(int osr){m_prev_temp_osr = osr;}

    QVariant devices()
    {
        return QVariant::fromValue(m_devices);
    }

    // Config modes
    int m_modeState = STATE_UNCONNECTED;
    QVector<bool> m_stateList = QVector<bool> (4, false);
    uint8_t modeStates[4]; // = QVector<uint8_t> (4, false);


    void setPrsMr(int mr) { m_prs_mr = mr;
                           modeStates[PRS_MR] = DIRTY; }
    void setPrsOsr(int osr) { m_prs_osr = osr;
                            modeStates[PRS_OSR] = DIRTY; }
    void setTempMr(int mr) {m_temp_mr = mr;
                            modeStates[TEMP_MR] = DIRTY; }
    void setTempOsr(int osr) {m_temp_osr = osr;
                              modeStates[TEMP_OSR] = DIRTY; }
    int getPrsMr() {return m_prs_mr;}
    int getPrsOsr() {return m_prs_osr;}
    int getTempMr() {return m_temp_mr;}
    int getTempOsr() {return m_temp_osr;}

    // P axis calculation
    bool firstRunP = true; // if reset is implemented, this has to be changed to true again
    double maxP = 0;
    double minP = 0;
    // T axis calculation
    bool firstRunT = true; // if reset is implemented, this has to be changed to true again
    double maxT = 0;
    double minT = 0;
    // A axis calculation
    bool firstRunA = true; // if reset is implemented, this has to be changed to true again
    double maxA = 0;
    double minA = 0;
    // V axis calculation
    bool firstRunV = true; // if reset is implemented, this has to be changed to true again
    double maxV = 0;
    double minV = 0;

    void setSomeString(QString str)
    {
        m_someString = str;
        emit someStringChanged();
    }

    Q_INVOKABLE void resetBorders()
    {
        this->maxP = 0;
        this->minP = 0;
        this->maxT = 0;
        this->minT = 0;
        this->maxA = 0;
        this->minA = 0;
        this->maxV = 0;
        this->minV = 0;
        this->firstRunP = true;
        this->firstRunT = true;
        this->firstRunA = true;
        this->firstRunV = true;
    }


    Q_INVOKABLE void exitApplication(void)
    {
        //qDebug()<<"trying to exit";
        //m_refToBtManager->~BtManager();
        emit closeBTMthread();
    }


    Q_INVOKABLE void messageFromQml(const QString &str)
    {
        Q_UNUSED(str)
        //qDebug()<<"Somebody spit in the pipe: "<<str;
    }

    Q_INVOKABLE void modeMessageQml(const QString &mode, const QString &param)
    {
        qDebug()<<"Set mode QML:"<<mode<<param;
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

    Q_INVOKABLE void scanDevices()
    {
        emit scanDevicesInterface();
        //m_refToBtManager->startDiscovery();
    }



signals:
    void closeBTMthread();
    // Discovery
    //void devicesChanged();
   void devicesUpdated();
    void somefkingsignal();
    void scanDevicesInterface(); // qml to interface to btmanager
    // Config
    void modeStateChanged();
    void presMrChanged();
    void presOsrChanged();
    void tempMrChanged();
    void tempOsrChanged();
    //void requestModes();
    // Charts
    //void newDataAvailable();
    void updateTemperature(const QPointF& pt); // I added them just to make the app run, but it will still lag if we just dispatch them ungracefully through // An other idea would be to start a timer in qml and fetch the data periodically through the interface, but controlled by the qml timer.
    void updatePressure(const QPointF& pp);
    void updateAltitude(const QPointF& pa);
    void updateVolume(const QPointF& pv);
    void updateFrequency(const double& f);
    void maxOfTempChanged();
    void minOfTempChanged();
    void maxOfPresChanged();
    void minOfPresChanged();
    void maxOfAltChanged();
    void minOfAltChanged();
    void maxOfVolChanged();
    void minOfVolChanged();


    void isActiveChanged();
    void someStringChanged();
    void someBufferHasFreshDataChanged();
    void someStringBufferPushesData(QString &str);


public slots:

    void addDeviceToList( QString name, QString address )
    {
        m_devices.append(new DeviceInfo(name, address));
        emit devicesUpdated();
        qDebug()<<"added one";
    }


//    void updateModes() {
//        qDebug()<<"MODE STATE CHANGED CPP";
//        if (this->m_modeState == STATE_REQUESTED)
//        {
//            emit requestModes();
//        }
//    }

};

#endif // THREADQMLINTERFACE_H
