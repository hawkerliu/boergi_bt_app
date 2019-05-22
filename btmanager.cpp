#include "btmanager.h"
#include "threadqmlinterface.h"

/* 00001101-0000-1000-8000-00805f9b34fb:
    QBluetoothUuid::SerialPort (0x1101)
    Serial Port Profile UUID (service & profile)
*/
/* 00001200-0000-1000-8000-00805f9b34fb:
    QBluetoothUuid::PnPInformation (0x1200)
    Device Identification (DID) UUID (service & profile)
*/

BtManager::BtManager(QObject *parent, ThreadQmlInterface* tqiRef) : QObject(parent), m_refToInterface(tqiRef)
{
    qDebug()<<"BtManager started";
    Q_UNUSED(parent);

    qDebug("Saying Hello through to Interface to QML");
    m_refToInterface->setRefToBtManager(this); // now the interface also hast a pointer to this instance!
    m_refToInterface->setSomeString("Hello!"); // set a variable protected by a mutex. I have prepared for you an example stringlist buffer also with a mutex.
    m_refToInterface->someStringChanged(); // emit a signal of the interface directly
    m_refToInterface->addOneToSomeBuffer("Diese"); // just adding some to someBuffer to pick it out in Qml
    m_refToInterface->addOneToSomeBuffer("Und Diese"); // just adding some to someBuffer to pick it out in Qml
    m_refToInterface->addOneToSomeBuffer("Und jene"); // just adding some to someBuffer to pick it out in Qml



    QList<QBluetoothHostInfo> localAdapters = QBluetoothLocalDevice::allDevices();

    //    m_buffer.setBuffer(&m_byteArrayBehindBuffer);
    //    m_buffer.open(QIODevice::ReadWrite);
    //buffer.seek(x);
    //buffer.write("def", 3);
    //m_buffer.
    //buffer.close();
    // byteArray == "abcdef"

    QString localName;
    QBluetoothAddress localAddress;
    int i = localAdapters.count();
    if (i > 0)
    {
        switch (i) {
        case 0:
            qDebug()<<"Error: no bluetooth adapters found";
            break;
        case 1:
            localName = localAdapters[0].name();
            localAddress = localAdapters[0].address();
            qDebug()<<"Local bluetooth adapter:"<<localName<<localAddress.toString();
            break;
        default:
            qDebug()<<"More than 1 local adapter. Implementation needed.";
            break;
            //        //we ignore more than two adapters
            //        ui->localAdapterBox->setVisible(true);
            //        ui->firstAdapter->setText(tr("Default (%1)", "%1 = Bluetooth address").
            //                                  arg(localAdapters.at(0).address().toString()));
            //        ui->secondAdapter->setText(localAdapters.at(1).address().toString());
            //        ui->firstAdapter->setChecked(true);
            //        connect(ui->firstAdapter, &QRadioButton::clicked, this, &Chat::newAdapterSelected);
            //        connect(ui->secondAdapter, &QRadioButton::clicked, this, &Chat::newAdapterSelected);

            //        QBluetoothLocalDevice adapter(localAdapters.at(0).address());
            //        adapter.setHostMode(QBluetoothLocalDevice::HostDiscoverable);
        }
    }
    else qDebug()<<"Error: invalid bluetooth adapter";



    mDeviceDiscoveryAgent = new QBluetoothDeviceDiscoveryAgent(localAddress, this);
    mLocalDevice = new QBluetoothLocalDevice(localAddress, this);
    mServiceInfo = new QBluetoothServiceInfo();
    mSocket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol, this); // L2capProtocol
    //mServiceDiscoveryAgent = new QBluetoothServiceDiscoveryAgent(localAddress, this);
    modeTimer = new QTimer(this);

    // Connect DeviceDiscoveryAgent

    connect(mDeviceDiscoveryAgent, SIGNAL(deviceDiscovered(const QBluetoothDeviceInfo&)),
            this, SLOT(onDeviceDiscovered(const QBluetoothDeviceInfo&)));

    connect(mDeviceDiscoveryAgent, SIGNAL(error(QBluetoothDeviceDiscoveryAgent::Error)),
            this, SLOT(onDeviceScanError(QBluetoothDeviceDiscoveryAgent::Error)));
    connect(mDeviceDiscoveryAgent, SIGNAL(finished()), this, SLOT(onDeviceScanFinished()));
    // Connect LocalDevice
    connect(mLocalDevice, SIGNAL(hostModeStateChanged(QBluetoothLocalDevice::HostMode)),
            this, SLOT(onHostModeStateChanged(QBluetoothLocalDevice::HostMode)));
    connect(mLocalDevice, SIGNAL(error(QBluetoothLocalDevice::Error)),
            this, SLOT(onLocalDeviceError(QBluetoothLocalDevice::Error)));
    connect(mLocalDevice, SIGNAL(pairingFinished(QBluetoothAddress,QBluetoothLocalDevice::Pairing)),
            this, SLOT(onPairingFinished(QBluetoothAddress,QBluetoothLocalDevice::Pairing)));
    connect(mLocalDevice, SIGNAL(pairingDisplayConfirmation(QBluetoothAddress,QString)),
            this, SLOT(onPairingDisplayConfirmation(QBluetoothAddress,QString)));
    connect(mLocalDevice, SIGNAL(pairingDisplayPinCode(QBluetoothAddress,QString)),
            this, SLOT(onPairingDisplayPinCode(QBluetoothAddress,QString)));
    // Connect Socket
    connect(mSocket, SIGNAL(readyRead()), this, SLOT(socketReadLine()));
    connect(mSocket, SIGNAL(connected()), this, SLOT(onSocketConnected()));
    connect(mSocket, SIGNAL(disconnected()), this, SLOT(onSocketDisconnected()));
    connect(mSocket, SIGNAL(error(QBluetoothSocket::SocketError)),
            this, SLOT(onSocketError(QBluetoothSocket::SocketError)));
    connect(mSocket, SIGNAL(stateChanged(QBluetoothSocket::SocketState)),
            this, SLOT(onSocketStateChanged(QBluetoothSocket::SocketState)));
    connect(mSocket, SIGNAL(bytesWritten(qint64)), this, SLOT(socketWritten(qint64)));
    // Connect timer
    connect(modeTimer, SIGNAL(timeout()), this, SLOT(onModeTimeout()));

    // Start discovery
    mDeviceDiscoveryAgent->stop();
    qDebug()<<"Device discovery started";
    //onHostModeStateChanged(mLocalDevice->hostMode());
    mDeviceDiscoveryAgent->start(QBluetoothDeviceDiscoveryAgent::ClassicMethod); //ClassicMethod LowEnergyMethod NoMethod

}



BtManager::~BtManager()
{
    QString sID = "1"; // sensor id
    qDebug()<<"EXITING: changing to Standard Precision.";
    this->sendMessage(STOP);
    mSocket->waitForBytesWritten(-1);
    this->setMode(sID, "mode", "bg");
    this->setMode(sID, "prs_mr", "8");
    this->setMode(sID, "prs_osr", "32");
    this->setMode(sID, "temp_mr", "8");
    this->setMode(sID, "temp_osr", "32");

    //mSocket->connectToService(mDeviceInfo.address(),QBluetoothUuid::SerialPort, QIODevice::ReadWrite);
    mSocket->disconnectFromService();
    qDebug()<<"Disconnected.";
    QThread::sleep(2);


//    this->sendMessage(STOP);
//    mSocket->waitForBytesWritten(-1);
//    qDebug()<<"stop message sent";
    delete mDeviceDiscoveryAgent;
    delete mLocalDevice;
    delete mServiceInfo;
    delete mSocket;
    qDebug()<<"You have been terminated.";
}

QString BtManager::sayHello() const
{
    return "Hello!";
}

void BtManager::onModeTimeout()
{
    QString sID = "1"; // sensor id
    if (modeCounter > 3) modeCounter = 0;
    switch (modeCounter) {
    case 0:     // High Data Rate
        qDebug()<<"Changing to High Data Rate.";
        this->sendMessage(STOP);
        mSocket->waitForBytesWritten(-1);
        this->setMode(sID, "mode", "bg");
        this->setMode(sID, "prs_mr", "32");
        this->setMode(sID, "prs_osr", "1");
        this->setMode(sID, "temp_mr", "32");
        this->setMode(sID, "temp_osr", "1");
        this->sendMessage(START);
        mSocket->waitForBytesWritten(-1);
        modeCounter++;//=1; // to std rate
        break;
    case 1:     // Standard Precision
        qDebug()<<"Changing to Standard Precision:";
        this->sendMessage(STOP);
        mSocket->waitForBytesWritten(-1);
        this->setMode(sID, "mode", "bg");
        this->setMode(sID, "prs_mr", "8");
        this->setMode(sID, "prs_osr", "32");
        this->setMode(sID, "temp_mr", "8");
        this->setMode(sID, "temp_osr", "32");
        this->sendMessage(START);
        mSocket->waitForBytesWritten(-1);
        modeCounter++;
        break;
    case 2:     // High Precision
        qDebug()<<"Changed to High Precision";
        this->sendMessage(STOP);
        mSocket->waitForBytesWritten(-1);
        this->setMode(sID, "mode", "bg");
        this->setMode(sID, "prs_mr", "1");
        this->setMode(sID, "prs_osr", "128");
        this->setMode(sID, "temp_mr", "1");
        this->setMode(sID, "temp_osr", "128");
        this->sendMessage(START);
        mSocket->waitForBytesWritten(-1);
        modeCounter++;
        break;
    case 3:     // Standard Precision
        qDebug()<<"Changing to Standard Precision:";
        this->sendMessage(STOP);
        mSocket->waitForBytesWritten(-1);
        this->setMode(sID, "mode", "bg");
        this->setMode(sID, "prs_mr", "8");
        this->setMode(sID, "prs_osr", "32");
        this->setMode(sID, "temp_mr", "8");
        this->setMode(sID, "temp_osr", "32");
        this->sendMessage(START);
        mSocket->waitForBytesWritten(-1);
        modeCounter++;
        break;
    default:
        qDebug()<<"onModeTimeout error";
        break;
    }

    //    Mode message sent: "$set_mode sid=1;md=mode;val=bg\n"
    //    Mode message sent: "$set_mode sid=1;md=prs_mr;val=32\n"
    //    Mode message sent: "$set_mode sid=1;md=prs_osr;val=1\n"
    //    Mode message sent: "$set_mode sid=1;md=temp_mr;val=32\n"
    //    Mode message sent: "$set_mode sid=1;md=temp_osr;val=1\n"
    //      Changed to High Data Rate
    //    Mode message sent: "$set_mode sid=1;md=mode;val=bg\n"
    //    Mode message sent: "$set_mode sid=1;md=prs_mr;val=8\n"
    //    Mode message sent: "$set_mode sid=1;md=prs_osr;val=32\n"
    //    Mode message sent: "$set_mode sid=1;md=temp_mr;val=8\n"
    //    Mode message sent: "$set_mode sid=1;md=temp_osr;val=32\n"
    //      Changed to Standard Precision
    //    Mode message sent: "$set_mode sid=1;md=mode;val=bg\n"
    //    Mode message sent: "$set_mode sid=1;md=prs_mr;val=1\n"
    //    Mode message sent: "$set_mode sid=1;md=prs_osr;val=128\n"
    //    Mode message sent: "$set_mode sid=1;md=temp_mr;val=1\n"
    //    Mode message sent: "$set_mode sid=1;md=temp_osr;val=128\n"
    //      Changed to High Precision
}

void BtManager::socketReadLine()
{    
    double time;
    double val;
    double f;
    QString element;
    int pos = 3; // position of measurement id
    // Read data from buffer as long as there are packages
    while (mSocket->canReadLine()) {
        QByteArray byteBuffer = mSocket->readLine();
        mBufferList.append(QString::fromStdString(byteBuffer.toStdString()));
    }
    // Sort out data and send signal to QML
    if (!mBufferList.empty())
    {
        for (int i = 0; i < mBufferList.length(); i++)
        {
            element = mBufferList.at(i);
            if (element.at(pos)==id_temp)
            {
                val = element.section(",",2,2).toDouble();
                time = element.section(",",3,3).toDouble();
                time = time/10000; // convert to seconds
                emit updateTemperature(QPointF(time,val));

                f = 1/(static_cast<double>(time-prevTime)); // time val: 1250 = 125ms period time
                if (f < 0) f = 0;
                prevTime = time;
                emit updateFrequency(f);
            }
            else if (element.at(pos)==id_pres)
            {
                val = element.section(",",2,2).toDouble();
                time = element.section(",",3,3).toDouble();
                time = time/10000; // convert to seconds
                // MODIFIED to conform with the interface
                m_refToInterface->updatePressure(QPointF(time,val));
            }
            else if (element.at(pos)==id_altitude)
            {
                val = element.section(",",2,2).toDouble();
                time = element.section(",",3,3).toDouble();
                time = time/10000; // convert to seconds
                emit updateAltitude(QPointF(time,val));
            }
            else if (element.at(pos)==id_volume)
            {
                val = element.section(",",2,2).toDouble();
                time = element.section(",",3,3).toDouble();
                time = time/10000; // convert to seconds
                emit updateVolume(QPointF(time,val));
            }
            else qDebug()<<"Unknown element in data:"<<element;
        }
    }
    else {
        qDebug()<<"No data to read";
    }
    mBufferList.clear(); // make list empty
}

void BtManager::socketWritten(qint64 val)
{
    Q_UNUSED(val);
    //qDebug()<<"Number of Bytes written:"<<val;
}

void BtManager::setMode(const QString &sid, const QString &md, const QString &val)
{
    QByteArray modetext = START_SIGN + "set_mode sid=" + sid.toUtf8() + ";md=" + md.toUtf8() + ";val=" + val.toUtf8() + STOP_SIGN;
    mSocket->write(modetext);
    mSocket->waitForBytesWritten(-1);
    //qDebug()<<"Mode message sent:"<<modetext;
    //    mSocket->waitForBytesWritten(5000);

    /* CHECK IF MODES ARE SET CORRECTLY / SENDING THE RIGHT MESSAGES */

}

void BtManager::sendMessage(const QString &message)
{
    QByteArray text = START_SIGN + message.toUtf8() + STOP_SIGN;
    mSocket->write(text);
    //qDebug()<<"Message sent:"<<text;
}

void BtManager::connectSocketDevice()
{
    if (mDeviceInfo.isValid())
    {
        qDebug()<<"Socket connecting to"<<mDeviceInfo.name()<<mDeviceInfo.address();
        mSocket->connectToService(mDeviceInfo.address(),QBluetoothUuid::SerialPort, QIODevice::ReadWrite);
    }
    else qDebug()<<"Connecting to socket not possible: device info is not valid";
}

void BtManager::connectSocketService()
{
    if(!mServiceInfoList.isEmpty())
    {
        int idx = 0;
        QList<QBluetoothUuid> UUIDs = mServiceInfoList[idx].serviceClassUuids();

        qDebug()<<"Socket connecting to"<<mServiceInfoList[idx].serviceName()<<"service"<<UUIDs[0];

        mSocket->connectToService(mServiceInfoList[idx].device().address(), UUIDs[0], QIODevice::ReadWrite);
    }
    else qDebug()<<"mServiceInfoList is empty";
}

void BtManager::onSocketConnected()
{
    qDebug()<<"Socket connected";
    qDebug() << "Local:" << mSocket->localName()
             << "Address:" << mSocket->localAddress().toString()
             << "Port:" << mSocket->localPort();
    qDebug() << "Peer:" << mSocket->peerName()
             << "Address:" << mSocket->peerAddress().toString()
             << "Port:" << mSocket->peerPort();
    if (mSocket->writeChannelCount() == 1)
    {
        this->sendMessage(STOP);
        mSocket->waitForBytesWritten(-1);
        qDebug()<<"Stop message sent";
        //        this->sendMessage(INFO);
        //        mSocket->waitForBytesWritten(-1);
        //        this->sendMessage(HELLO);
        //        mSocket->waitForBytesWritten(-1);
        //        this->sendMessage(HELLO);
        //        mSocket->waitForBytesWritten(-1);
        this->sendMessage(START);
        mSocket->waitForBytesWritten(-1);
        qDebug()<<"Start message sent";

        // ****************************************************
        //                  TEST for changing modes
        bool testModes = true;
        if (testModes)
        {
            modeTimeout = 10000;
            modeTimer->setInterval(modeTimeout);
            modeTimer->start();
            qDebug()<<"timer started";
        }
        // ****************************************************

        //mSocket->waitForBytesWritten(5000);
    }
    else {
        qDebug()<<"Multiple number of channels. Implementation needed.";
    }

}

void BtManager::onSocketDisconnected()
{
    qDebug()<<"Socket disconnected";




}

void BtManager::onSocketError(QBluetoothSocket::SocketError error)
{
    qDebug()<<"Socket error:"<<error;
}

void BtManager::onSocketStateChanged(QBluetoothSocket::SocketState state)
{
    qDebug()<<"Socket state changed to"<<state;
}

void BtManager::onPairingDisplayPinCode(QBluetoothAddress address,QString pin)
{   // only called if there are issues with requestPairing()
    // display the pin to the user
    Q_UNUSED(address);
    qDebug()<<"onPairingDisplayPinCode, pin:"<<pin;
}

void BtManager::onPairingDisplayConfirmation(QBluetoothAddress address ,QString pin)
{   // only called if there are issues with requestPairing()
    // display a pairing confirmation dialog and call pairingConfirmation()
    Q_UNUSED(address);
    mLocalDevice->pairingConfirmation(true);
    qDebug()<<"onPairingDisplayConfirmation, pin:"<<pin;
}

void BtManager::pairDevice()
{
    if (mDeviceInfo.isValid())
    {
        qDebug()<<"Pairing request to device"<<mDeviceInfo.name();
        //mLocalDevice->pairingConfirmation(true); // should not be used here, maybe in error handler
        mLocalDevice->requestPairing(mDeviceInfo.address(),QBluetoothLocalDevice::Paired);
    }
    else qDebug()<<"Pairing not possible: device info not valid";
}

void BtManager::onPairingFinished(QBluetoothAddress address,QBluetoothLocalDevice::Pairing pairing)
{
    switch (pairing) {
    case QBluetoothLocalDevice::Paired:
        qDebug()<<"Device pairing finished. Address:"<<address;
        // Connect to socket
        connectSocketDevice();
        break;
    case QBluetoothLocalDevice::Unpaired:
        qDebug()<<"Device unpaired, pairing it again";
        this->pairDevice();
        break;
    case QBluetoothLocalDevice::AuthorizedPaired:
        qDebug()<<"QBluetoothLocalDevice::AuthorizedPaired";
        break;
    }
}

void BtManager::onHostModeStateChanged(QBluetoothLocalDevice::HostMode mode)
{
    qDebug()<<"Host mode changed to:"<<mode;
}

void BtManager::onLocalDeviceError(QBluetoothLocalDevice::Error error)
{
    qDebug()<<"Local Device error:"<<error;
}

void BtManager::onDeviceScanFinished()
{
    qDebug()<<"Device scan finished";
}

void BtManager::onDeviceScanError(QBluetoothDeviceDiscoveryAgent::Error error)
{
    if (error == QBluetoothDeviceDiscoveryAgent::PoweredOffError)
        qDebug()<<"The Bluetooth adaptor is powered off, power it on before doing discovery.";
    else if (error == QBluetoothDeviceDiscoveryAgent::InputOutputError)
        qDebug()<<"Writing or reading from the device resulted in an error.";
    else
        qDebug()<<"An unknown error has occurred.";
}


void BtManager::onDeviceDiscovered(const QBluetoothDeviceInfo &dInfo)
{
    qDebug() << "Found new device:" << dInfo.name() << "Address:" << dInfo.address().toString();
    if (dInfo.name().contains("IFX_NANO-25BB"))
    {
        // Stop device discovery (remove later)
        mDeviceDiscoveryAgent->stop();
        // Assign received QBluetoothDeviceInfo
        mDeviceInfo = dInfo;

        // Pair NanoHub
        QBluetoothLocalDevice::Pairing pstatus = mLocalDevice->pairingStatus(mDeviceInfo.address());
        if (pstatus == QBluetoothLocalDevice::Unpaired)
        {
            this->pairDevice();
        }
        else {
            mLocalDevice->requestPairing(mDeviceInfo.address(),QBluetoothLocalDevice::Unpaired);
        }
    }
}
