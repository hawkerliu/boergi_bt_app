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
#include "threadqmlinterface.h"

class ThreadQmlInterface;

class BtManager : public QObject
{
    Q_OBJECT
    //    //Q_PROPERTY(QList<double> sensorData READ readSensorData NOTIFY newDataAvailable)
    //    Q_PROPERTY(double pressure READ getPressure NOTIFY newDataAvailable)
    //    Q_PROPERTY(double pres_t READ getTimeP NOTIFY newDataAvailable)
    //    Q_PROPERTY(double pres_s READ getSensP NOTIFY newDataAvailable)
    //    Q_PROPERTY(double temperature READ getTemp NOTIFY newDataAvailable)
    //    Q_PROPERTY(double temp_t READ getTimeT NOTIFY newDataAvailable)
    //    Q_PROPERTY(double temp_s READ getSensT NOTIFY newDataAvailable)
    //    Q_PROPERTY(double freq READ getFreq NOTIFY newDataAvailable)



public:
    BtManager(QObject *parent = nullptr, ThreadQmlInterface* tqiRef= nullptr);
    //BtManager();
    ~BtManager();
    QAbstractListModel *listmodel;
    QAbstractItemModel *itemmodel;


    Q_INVOKABLE QString sayHello() const;

    //    double getPressure() { return value_p; }
    //    double getTimeP() { return time_p; }
    //    double getSensP() { return sensor_p; }
    //    double getTemp() { return value_t; }
    //    double getTimeT() { return time_t; }
    //    double getSensT() { return sensor_t; }
    //    double getFreq() { return f; }

signals:
    void newDataAvailable();

    void updateTemperature(const QPointF& pt);
    void updatePressure(const QPointF& pp);
    void updateAltitude(const QPointF& pa);
    void updateVolume(const QPointF& pv);

    void updateFrequency(const double& f);

public slots:



    // Device Discovery slots

    void onDeviceDiscovered(const QBluetoothDeviceInfo&);

    void onDeviceScanError(QBluetoothDeviceDiscoveryAgent::Error);
    void onDeviceScanFinished();
    void onHostModeStateChanged(QBluetoothLocalDevice::HostMode);
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
    void setMode(const QString &sid, const QString &md, const QString &val);
    void onModeTimeout();

private:
    ThreadQmlInterface* m_refToInterface;

    // Buffer for incoming messages
    QList<QString> mBufferList;
    // Define value identificators
    const QString id_temp = "t";
    const QString id_pres = "p";
    const QString id_altitude = "a";
    const QString id_volume = "v";
    // used for readyRead signal
    double prevTime;
    // Variables which are sent to QML
    //    double sensor_t = 0;
    //    double value_t = 0;
    //    double time_t = 0;
    //    double sensor_p = 0;
    //    double value_p = 0;
    //    double time_p = 0;
    //    double f = 0;
    // Datatype for porting QML
    //QList<QPoint> qmlData(); // 0=t 1=p 2=a 3=v

    //QPoint testpoint();

    // Messages
    const QByteArray GET_MODE = "get_mode sid=%s;md=%s";
    const QByteArray HELLO = "hello";
    const QByteArray INFO = "info";
    const QByteArray MODE = "mode id=%s";
    const QByteArray MODES = "modes";
    const QByteArray SENSOR_INFO = "sinfo id=%s";
    const QByteArray SET_MODE = "set_mode sid=%s;md=%s;val=%s";
    const QByteArray START = "start";
    const QByteArray START_SENSOR = "start_sensor id=%s";
    const QByteArray START_SIGN = "$";
    const QByteArray STOP = "stop";
    const QByteArray STOP_SENSOR = "stop_sensor id=%s";
    const QByteArray STOP_SIGN = "\n";

    //    QByteArray m_byteArrayBehindBuffer;
    //    QBuffer m_buffer;



    QTimer *modeTimer; // alter precision modes
    int modeTimeout = 10000;
    int modeCounter = 0;


    QBluetoothDeviceDiscoveryAgent *mDeviceDiscoveryAgent;
    QBluetoothLocalDevice *mLocalDevice;
    QBluetoothDeviceInfo mDeviceInfo;
    QBluetoothServiceInfo *mServiceInfo;
    QList<QBluetoothServiceInfo> mServiceInfoList;
    QBluetoothSocket *mSocket;
    QVector<QList<QBluetoothUuid>> mBtUUIDs;

    void connectSocketService();
    void connectSocketDevice();
    void pairDevice();

};

#endif // BTMANAGER_H
