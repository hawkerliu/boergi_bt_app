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

#include <QAbstractListModel>
//#include "threadqmlinterface.h"
#include "deviceinfo.h"


class ThreadQmlInterface;

class BtManager : public QObject
{
    Q_OBJECT
public:
    BtManager(QObject *parent = nullptr, ThreadQmlInterface* tqiRef= nullptr);
    //BtManager();
    ~BtManager();
    QAbstractListModel *listmodel;
    QAbstractItemModel *itemmodel;


signals:
    void notAcknowledged();
    void acknowledged();
    void deviceFound( QString name, QString address);




public slots:
    void mainStateChanged();

    // Device Discovery slots

    void onDeviceDiscovered(const QBluetoothDeviceInfo&);

    void onDeviceScanError(QBluetoothDeviceDiscoveryAgent::Error);
    void onDeviceScanFinished();
    void onHostModeStateChanged(QBluetoothLocalDevice::HostMode);

    void startDiscovery();
    // Local Device slots
    void onPairingFinished(QBluetoothAddress,QBluetoothLocalDevice::Pairing);
    void onPairingDisplayConfirmation(QBluetoothAddress,QString);
    void onPairingDisplayPinCode(QBluetoothAddress,QString);
    void onLocalDeviceError(QBluetoothLocalDevice::Error);
    // Socket slots
    void onSocketConnected();
    void onSocketDisconnected();
    void onSocketError(QBluetoothSocket::SocketError);
    void onSocketStateChanged(QBluetoothSocket::SocketState);
    //void socketRead();
    void socketReadLine();
    void socketWritten(qint64);
    void sendMessage(const QString &message);
    //void setMode(const QString &sid, const QString &md, const QString &val); // old
    void sendMode(const QByteArray &md, const QByteArray &val);
    //void sendModes();
    void getMode(const QByteArray &md);

    //void onModeTimeout();

//    // new send mode
//    void sendPrsMr(int);
//    void sendPrsOsr(int);
//    void sendTempMr(int);
//    void sendTempOsr(int);

    void sendTimerExpired(void);

    void modeACK();
    void modeNACK();



private:
    ThreadQmlInterface* m_refToInterface;

    void sendMachine();


//    static uint8_t sendMachineState;
//    static uint8_t errorCnt;
    uint8_t sendMachineState;
    uint8_t errorCnt;
    QVector<uint8_t> sentCommands;

    // Buffer for incoming messages
    QList<QString> mBufferList;
    // Define value identificators
    const QString id_temp = "t";
    const QString id_pres = "p";
    const QString id_altitude = "a";
    const QString id_volume = "v";
    // used for readyRead signal
    double prevTime;


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
    //    const QByteArray GET_MODE = "get_mode sid=%s;md=%s";
    //    const QByteArray MODE = "mode id=%s";
    //    const QByteArray MODES = "modes";
    //    const QByteArray SENSOR_INFO = "sinfo id=%s";
    //    const QByteArray SET_MODE = "set_mode sid=%s;md=%s;val=%s";
    //    const QByteArray START_SENSOR = "start_sensor id=%s";
    //    const QByteArray STOP_SENSOR = "stop_sensor id=%s";

    QTimer *modeTimer; // alter precision modes
    int modeTimeout = 10000;
    int modeCounter = 0;

    QTimer *sendTimer;

    QBluetoothDeviceDiscoveryAgent *mDeviceDiscoveryAgent;
    QBluetoothLocalDevice *mLocalDevice;
    QBluetoothDeviceInfo mDeviceInfo;
    QBluetoothServiceInfo *mServiceInfo;
    QList<QBluetoothServiceInfo> mServiceInfoList;
    QBluetoothSocket *mSocket;
    QVector<QList<QBluetoothUuid>> mBtUUIDs;

    void start();
    void connectSocketService();
    void connectSocketDevice();
    void pairDevice();

    void calcYborders(double &minY, double &maxY, double &value, bool &first, const QString &id);

};

#endif // BTMANAGER_H
