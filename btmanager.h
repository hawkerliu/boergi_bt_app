#ifndef BTMANAGER_H
#define BTMANAGER_H

#include <QObject>
#include <QDebug>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothServiceDiscoveryAgent>
#include <QBluetoothDeviceInfo>
#include <QBluetoothLocalDevice>
#include <QBluetoothServer>
#include <QTimer>
#include <QQueue>
#include <QElapsedTimer>
#include <QBuffer>
#include <QPoint>
#include <QThread>
#include <QMutex>
#include <QVector>
#include <QQueue>

#include <QAbstractListModel>
//#include "threadqmlinterface.h"
#include "deviceinfo.h"
#include "datamodel.h"
#include "modeconfig.h"


class ThreadQmlInterface;

class BtManager : public QObject
{
    Q_OBJECT
public:
    BtManager(QObject *parent = nullptr, ThreadQmlInterface* tqiRef= nullptr);
    ~BtManager();

    QBluetoothSocket::SocketState getSocketState() {return m_Socket->state();}
    QBluetoothLocalDevice::Pairing pairStatus;
    //QList<QVector<QPointF>*> pointList;
    int corruptedCounter = 0;
    // public functions
    bool isScanning() {return m_DeviceDiscoveryAgent->isActive();}
    void initError();
signals:
    void notAcknowledged();
    void acknowledged();
    void deviceFound( QString name, QString address);
    void addDataToModel(QString, QString, QString);
    void addDataToModeConfig(QString, QString, QStringList);

public slots:
    // Main state
    void onMainStateChangedCpp();
    // Timer slots
    void startTimerExpired();
    void sendTimerExpired();
    // Device Discovery slots
    void onDeviceDiscovered(const QBluetoothDeviceInfo&);
    void onDeviceScanError(QBluetoothDeviceDiscoveryAgent::Error);
    void onDeviceScanFinished();
    void onHostModeStateChanged(QBluetoothLocalDevice::HostMode);
    void startDiscovery();
    void stopDiscovery();
    // Local Device slots
    void onPairingFinished(QBluetoothAddress,QBluetoothLocalDevice::Pairing);
    void onPairingDisplayConfirmation(QBluetoothAddress,QString);
    void onPairingDisplayPinCode(QBluetoothAddress,QString);
    // Socket slots
    void connectToDevice(QString);
    void disconnectDevice();
    void onSocketConnected();
    void onSocketDisconnected();
    void onSocketError(QBluetoothSocket::SocketError);
    void onSocketStateChanged(QBluetoothSocket::SocketState);
    void socketReadData();
    // Communication slots
    void sendMode(const QByteArray &md, const QByteArray &val);
    void getMode(const QByteArray &md);
    void getSensorInfo(const QByteArray &sid);
    void sendMessage(const QByteArray &msg);
    void getHello();
    void getAvailableModes();
    void getAvailableMode(const QByteArray &sid);

    // Send machine slots
    void sendMachine();
    void modeACK();
    void modeNACK();
private:
    ThreadQmlInterface* m_refToInterface;
    QBluetoothDeviceDiscoveryAgent *m_DeviceDiscoveryAgent;
    QBluetoothLocalDevice *m_LocalDevice;
    QBluetoothDeviceInfo m_DeviceInfo;
    QBluetoothSocket *m_Socket;
    QBluetoothAddress m_connectAddress;
    uint8_t m_sendMachineState;
    uint8_t m_errorCnt;
    bool m_unpair = false;
  //  QVector<uint8_t> m_sentCommands;
    QQueue<int> m_sentCommands;
    QVector<bool> m_checkList;
    QList<double> m_prevTimes;
    double m_prevTime = 0;
    // Messages
    QByteArray SENSORID = "1";
    const QByteArray HELLO = "hello";
    const QByteArray INFO = "info";
    const QByteArray START = "start";
    const QByteArray START_SIGN = "$";
    const QByteArray STOP = "stop";
    const QByteArray STOP_SIGN = "\n";
    const QByteArray PRSMR = "prs_mr";
    const QByteArray PRSOSR = "prs_osr";
    const QByteArray TEMPMR = "temp_mr";
    const QByteArray TEMPOSR = "temp_osr";
    const QByteArray SENSOR_INFO = "sinfo";
    const QByteArray MODE = "mode";
    const QByteArray MODES = "modes";
    // timers
    QTimer *m_startTimer;
    QTimer *m_sendTimer;
    // private functions
    void init();
    void connectSocketService();
    void connectSocketDevice();
    void clearSocket();
    void pairDevice();
    //void calcYBorders(double&,double&,double&,int);
    void calcYBorders(double&,int);

    void sendMachineOLD();
    bool containsNonASCII(QString);
    void cleanString(QString*);
};

#endif // BTMANAGER_H
