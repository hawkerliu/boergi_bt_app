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
    sendTimer = new QTimer();
    sendTimer->setInterval(500);
    sendTimer->setSingleShot(true);

    qDebug("Saying Hello through to Interface to QML");
    m_refToInterface->setRefToBtManager(this); // now the interface also hast a pointer to this instance!
    m_refToInterface->setSomeString("Hello!"); // set a variable protected by a mutex. I have prepared for you an example stringlist buffer also with a mutex.
    m_refToInterface->someStringChanged(); // emit a signal of the interface directly
    m_refToInterface->addOneToSomeBuffer("Diese"); // just adding some to someBuffer to pick it out in Qml
    m_refToInterface->addOneToSomeBuffer("Und Diese"); // just adding some to someBuffer to pick it out in Qml
    m_refToInterface->addOneToSomeBuffer("Und jene"); // just adding some to someBuffer to pick it out in Qml

    // **********************************************************
    // m_refToInterface->getAllFromSomeBuffer();
    // **********************************************************



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
    else qDebug()<<"Error: no bluetooth adapters found";



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
    // qml
    connect(m_refToInterface, SIGNAL(scanDevicesInterface()), this, SLOT(startDiscovery()));

    connect(this, SIGNAL(deviceFound(QString,QString)),m_refToInterface, SLOT(addDeviceToList(QString,QString)));
            //connect(this, SIGNAL(deviceFound(QBluetoothDeviceInfo)), m_refToInterface, SLOT(addDeviceToList(QBluetoothDeviceInfo)) );

            //connect(this, SIGNAL(deviceFound(QBluetoothDeviceInfo&)), m_refToInterface, SLOT(addDeviceToList(QBluetoothDeviceInfo&)));
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
    // qml
    connect(m_refToInterface, SIGNAL(modeStateChanged()), this, SLOT(mainStateChanged()));

    // Connect send machine: ack, nack, timer
    connect(this, SIGNAL(acknowledged()),this, SLOT(modeACK()));
    connect(this, SIGNAL(notAcknowledged()),this, SLOT(modeNACK()));
    connect(sendTimer, SIGNAL(timeout()), this, SLOT(sendTimerExpired()));


    //this->startDiscovery();
}



BtManager::~BtManager()
{
    this->sendMessage(STOP);
    mSocket->waitForBytesWritten(-1);
    mSocket->waitForReadyRead(-1);
    //    this->setMode(sID, "mode", "bg");
    //    this->setMode(sID, "prs_mr", "8");
    //    this->setMode(sID, "prs_osr", "32");
    //    this->setMode(sID, "temp_mr", "8");
    //    this->setMode(sID, "temp_osr", "32");

    mSocket->disconnectFromService();
    //QThread::sleep(1);
    qDebug()<<"Disconnected.";


    //deleteLater();

    //    delete mDeviceDiscoveryAgent;
    //    delete mLocalDevice;
    //    delete mServiceInfo;
    //    delete mSocket;
    qDebug()<<"You have been terminated.";
}

void BtManager::mainStateChanged()
{
    switch (m_refToInterface->m_modeState) {
    case STATE_INIT:
        qDebug()<<"MAIN_STATE: INIT";
        this->start(); // connect, read current config
        break;
    case STATE_UPDATED:
        qDebug()<<"MAIN_STATE: UPDATED";

        // prev values really neccessary? maybe restore values on error
        m_refToInterface->m_prev_prs_mr = m_refToInterface->m_prs_mr;
        m_refToInterface->m_prev_prs_osr = m_refToInterface->m_prs_osr;
        m_refToInterface->m_prev_temp_mr = m_refToInterface->m_temp_mr;
        m_refToInterface->m_prev_temp_osr = m_refToInterface->m_temp_osr;

        // set mode states
        m_refToInterface->modeStates[PRS_MR] = UPDATED;
        m_refToInterface->modeStates[PRS_OSR] = UPDATED;
        m_refToInterface->modeStates[TEMP_MR] = UPDATED;
        m_refToInterface->modeStates[TEMP_OSR] = UPDATED;

        // reset state list
        for (int i=0;i<m_refToInterface->m_stateList.length();i++)
        {
            m_refToInterface->m_stateList[i] = false;
        }

        m_refToInterface->resetBorders();

        //        m_refToInterface->maxP = 0;
        //        m_refToInterface->minP = 0;
        //        m_refToInterface->maxT = 0;
        //        m_refToInterface->minT = 0;
        //        m_refToInterface->maxA = 0;
        //        m_refToInterface->minA = 0;
        //        m_refToInterface->maxV = 0;
        //        m_refToInterface->minV = 0;

        //        m_refToInterface->firstRunP = true;
        //        m_refToInterface->firstRunT = true;
        //        m_refToInterface->firstRunA = true;
        //        m_refToInterface->firstRunV = true;


        break;
    case STATE_REQUESTED:
        qDebug()<<"MAIN_STATE: REQUESTED";
        sendMachine();
        // passt -

        break;

    case STATE_FAILED:
        qDebug()<<"MAIN_STATE: FAILED";
        // ***********************************
        // read modes from device instead of setting previous values
        qDebug()<<"Reading modes failed or timeout occured. Reading current sensor config...";
        for (int i=0; i<m_refToInterface->m_stateList.length();i++)
        {
            m_refToInterface->m_stateList[i] = false;
        }
        getMode(PRSMR);
        getMode(PRSOSR);
        getMode(TEMPMR);
        getMode(TEMPOSR);
        break;

    default:
        qDebug()<<"MAIN_STATE: default state";
        break;

    }
}


void BtManager::calcYborders(double &minY, double &maxY, double &value, bool &first, const QString &id)
{
    double axisDiff;
    double axisOffs = 0.1;
    axisDiff = maxY - minY;
    if (first) {
        maxY = value + (axisDiff*axisOffs);
        minY = value - (axisDiff*axisOffs);
        maxY += 0.001;
        minY -= 0.001;
        //qDebug()<<id<<"Initial MAX:"<<maxY<<"MIN:"<<minY;
        first = false; // if reset is implemented, this has to be changed to true again
        if (id == id_temp) {
            emit m_refToInterface->maxOfTempChanged();
            emit m_refToInterface->minOfTempChanged();
        }
        else if (id == id_pres) {
            emit m_refToInterface->maxOfPresChanged();
            emit m_refToInterface->minOfPresChanged();
        }
        else if (id == id_altitude) {

            emit m_refToInterface->maxOfAltChanged();
            emit m_refToInterface->minOfAltChanged();
        }
        else if (id == id_volume) {
            emit m_refToInterface->maxOfVolChanged();
            emit m_refToInterface->minOfVolChanged();
        }
    }
    else {
        if (value > maxY)
        {
            //qDebug()<<id<<"NEW MAX:"<<value;
            maxY = value + (axisDiff*axisOffs);
            if (id == id_temp) emit m_refToInterface->maxOfTempChanged();
            else if (id == id_pres)  emit m_refToInterface->maxOfPresChanged();
            else if (id == id_altitude) emit m_refToInterface->maxOfAltChanged();
            else if (id == id_volume) emit m_refToInterface->maxOfVolChanged();
        }
        if (value <= minY)
        {
            //qDebug()<<id<<"NEW MIN:"<<value;
            minY = value - (axisDiff*axisOffs);
            if (id == id_temp) emit m_refToInterface->minOfTempChanged();
            else if (id == id_pres)  emit m_refToInterface->minOfPresChanged();
            else if (id == id_altitude) emit m_refToInterface->minOfAltChanged();
            else if (id == id_volume) emit m_refToInterface->minOfVolChanged();
        }
    }
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
    //qDebug()<<"READ_BUFFER:"<<mBufferList;
    // Sort out data and send signal to QML
    if (!mBufferList.empty())
    {
        for (int i = 0; i < mBufferList.length(); i++)
        {
            element = mBufferList.at(i);
            if (element.at(pos)==id_temp)
            {
                val = element.section(',',2,2).toDouble();
                time = element.section(',',3,3).toDouble();
                time = time/10000; // convert to seconds
                m_refToInterface->updateTemperature(QPointF(time,val));
                calcYborders(m_refToInterface->minT,m_refToInterface->maxT,val,m_refToInterface->firstRunT,id_temp);
                f = 1/(static_cast<double>(time-prevTime)); // time val: 1250 = 125ms period time
                if (f < 0) f = 0;
                prevTime = time;
                m_refToInterface->updateFrequency(f);
            }
            else if (element.at(pos)==id_pres)
            {
                val = element.section(',',2,2).toDouble();
                time = element.section(',',3,3).toDouble();
                time = time/10000; // convert to seconds
                m_refToInterface->updatePressure(QPointF(time,val));
                calcYborders(m_refToInterface->minP,m_refToInterface->maxP,val,m_refToInterface->firstRunP,id_pres);
            }
            else if (element.at(pos)==id_altitude)
            {
                val = element.section(',',2,2).toDouble();
                time = element.section(',',3,3).toDouble();
                time = time/10000; // convert to seconds
                m_refToInterface->updateAltitude(QPointF(time,val));
                calcYborders(m_refToInterface->minA,m_refToInterface->maxA,val,m_refToInterface->firstRunA,id_altitude);
            }
            else if (element.at(pos)==id_volume)
            {
                val = element.section(',',2,2).toDouble();
                time = element.section(',',3,3).toDouble();
                time = time/10000; // convert to seconds
                m_refToInterface->updateVolume(QPointF(time,val));
                calcYborders(m_refToInterface->minV,m_refToInterface->maxV,val,m_refToInterface->firstRunV,id_volume);
                //qDebug()<<"V max is"<<maxV<<"V min is"<<minV;
            }
            else if (element.contains("nack"))
            {
                qDebug()<<"SOCKET_READ: Not acknowledged from device";
                emit notAcknowledged();
            }
            else if (element.contains("ack"))
            {
                qDebug()<<"SOCKET_READ: Acknowledged from device";
                emit acknowledged();
            }

            //else if (m_refToInterface->m_modeState == (STATE_INIT | STATE_FAILED) && element.contains("prs_mr"))
            else if (element.contains("prs_mr"))
            {
                //qDebug()<<"debug: element qstringlist"<<element.split('');
                m_refToInterface->m_prs_mr = static_cast<int>(element.section('\"',11,11).toDouble());

                qDebug()<<"SOCKET_READ: prs_mr:"<<m_refToInterface->m_prs_mr;
                m_refToInterface->m_stateList[PRS_MR] = true;
                if (m_refToInterface->m_stateList.indexOf(false) == -1)
                {
                    m_refToInterface->m_modeState = STATE_UPDATED;
                    emit m_refToInterface->modeStateChanged();
                }

            }
            //else if (m_refToInterface->m_modeState == (STATE_INIT | STATE_FAILED) && element.contains("prs_osr"))
            else if (element.contains("prs_osr"))
            {
                m_refToInterface->m_prs_osr = static_cast<int>(element.section('\"',11,11).toDouble());
                qDebug()<<"SOCKET_READ: prs_osr:"<<m_refToInterface->m_prs_osr;

                m_refToInterface->m_stateList[PRS_OSR] = true;
                if (m_refToInterface->m_stateList.indexOf(false) == -1)
                {
                    m_refToInterface->m_modeState = STATE_UPDATED;
                    emit m_refToInterface->modeStateChanged();
                }
            }
            //            else if (m_refToInterface->m_modeState == (STATE_INIT | STATE_FAILED) && element.contains("temp_mr"))
            else if (element.contains("temp_mr"))
            {
                m_refToInterface->m_temp_mr = static_cast<int>(element.section('\"',11,11).toDouble());
                qDebug()<<"SOCKET_READ: temp_mr:"<<m_refToInterface->m_temp_mr;

                m_refToInterface->m_stateList[TEMP_MR] = true;
                if (m_refToInterface->m_stateList.indexOf(false) == -1)
                {
                    m_refToInterface->m_modeState = STATE_UPDATED;
                    emit m_refToInterface->modeStateChanged();
                }
            }
            //else if (m_refToInterface->m_modeState == (STATE_INIT | STATE_FAILED) && element.contains("temp_osr"))
            else if (element.contains("temp_osr"))
            {
                m_refToInterface->m_temp_osr = static_cast<int>(element.section('\"',11,11).toDouble());
                qDebug()<<"SOCKET_READ: temp_osr:"<<m_refToInterface->m_temp_osr;

                m_refToInterface->m_stateList[TEMP_OSR] = true;
                if (m_refToInterface->m_stateList.indexOf(false) == -1)
                {
                    m_refToInterface->m_modeState = STATE_UPDATED;
                    emit m_refToInterface->modeStateChanged();
                }
            }
            else if (element.contains("error"))
            {

                //SOCKET_READ: Rejected element in data: "${\"sid\":\"1\",\"md\":\"prs_mr\",\"val\":\"8.00\"}\n"
                //SOCKET_READ: Rejected element in data: "${\"sid\":\"1\",\"md\":\"prs_osr\",\"val\":\"8\"}\n"
                //SOCKET_READ: Rejected element in data: "${\"sid\":\"1\",\"md\":\"temp_mr\",\"val\":\"8.00\"}\n"
                //SOCKET_READ: Rejected element in data: "${\"sid\":\"1\",\"md\":\"temp_osr\",\"val\":\"8\"}\n"

                //SOCKET_READ: Rejected element in data: "${\"sid\":\"1\",\"md\":\"prs_mr\",\"val\":\"32.00\"}\n"
                //SOCKET_READ: Rejected element in data: "${\"sid\":\"1\",\"md\":\"prs_osr\",\"val\":\"1\"}\n"
                //SOCKET_READ: Rejected element in data: "${\"sid\":\"1\",\"md\":\"temp_mr\",\"val\":\"32.00\"}\n"
                //SOCKET_READ: Rejected element in data: "${\"sid\":\"1\",\"md\":\"temp_osr\",\"val\":\"1\"}\n"

                // SOCKET_READ: Rejected element in data: "$d,error,message could not be parsed\n"
                // SOCKET_READ: Rejected element in data: "$d,error,get mode failed\n"
                qDebug()<<"GET_FAILED debug element:"<<element;
                m_refToInterface->m_modeState = STATE_FAILED;
                emit m_refToInterface->modeStateChanged();
            }


            else qDebug()<<"SOCKET_READ: Rejected element in data:"<<element;
        }
    }
    else {
        //qDebug()<<"SOCKET_READ: No data to read";
    }
    mBufferList.clear(); // make list empty
}


void BtManager::sendMachine()
{
    if (m_refToInterface->m_modeState == STATE_REQUESTED)
    {
        qDebug()<<"SEND_MACHINE: entry";
        for (int i=0; i<4; i++)
        {
            //qDebug()<<"print states:"<<i<<m_refToInterface->modeStates[i];
            if ( m_refToInterface->modeStates[i] == DIRTY )
            {
                qDebug()<<"SEND_MACHINE: Data dirty"<<"Index:"<<i;
                m_refToInterface->m_stateList[i] = false;


                QByteArray tmp;
                tmp.clear();

                // send mode and value to nanohub
                if (i == PRS_MR)
                {
                    tmp.setNum(m_refToInterface->m_prs_mr);
                    sendMode(PRSMR,tmp);
                    sentCommands.append(PRS_MR);
                }
                if (i == PRS_OSR)
                {
                    tmp.setNum(m_refToInterface->m_prs_osr);
                    sendMode(PRSOSR,tmp);
                    sentCommands.append(PRS_OSR);
                }
                if (i == TEMP_MR)
                {
                    tmp.setNum(m_refToInterface->m_temp_mr);
                    sendMode(TEMPMR,tmp);
                    sentCommands.append(TEMP_MR);
                }
                if (i == TEMP_OSR)
                {
                    tmp.setNum(m_refToInterface->m_temp_osr);
                    sendMode(TEMPOSR,tmp);
                    sentCommands.append(TEMP_OSR);
                }
                m_refToInterface->modeStates[i] = SENT;
                sendTimer->start();
            }

            else if (m_refToInterface->modeStates[i] == (NACKED | TIMEOUTED)) {
                qDebug()<<"SEND_MACHINE: Mode NACKED or TIMEOUTED"<<"Index:"<<i;
                // send again
                m_refToInterface->modeStates[i] = DIRTY;
            }

            else if ( m_refToInterface->modeStates[i] == UPDATED ) {
                //you did it no more to send
                //updated!
                qDebug()<<"SEND_MACHINE: Updated"<<"Index:"<<i;
                m_refToInterface->m_stateList[i] = true;

                if (m_refToInterface->m_stateList.indexOf(false) == -1)
                {
                    sendTimer->stop();
                    sentCommands.clear();
                    errorCnt = 0;
                    m_refToInterface->m_modeState = STATE_UPDATED;
                    emit m_refToInterface->modeStateChanged();
                }
            }
        }
    }
    else qDebug()<<"SEND_MACHINE: Called but incorrect state";
}

void BtManager::sendTimerExpired()
{
    qDebug()<<"SEND_MACHINE: Timer expired";
    for (int i=0; i<4; i++)
    {
        if ( m_refToInterface->modeStates[i] == SENT)
        {
            m_refToInterface->modeStates[i] = TIMEOUTED;
        }
    }

    errorCnt++;
    if (errorCnt > 3)
    {
        qDebug()<<"SEND_MACHINE: ERROR sending failed after 3 timeouts";
    }
    else {
        qDebug()<<"SEND_MACHINE: Timeout Error, retrying";
        sendMachine();
    }
}

void BtManager::modeNACK()
{
    if (m_refToInterface->m_modeState == STATE_REQUESTED)
    {
        qDebug()<<"SEND_MACHINE: NACK";
        sendTimer->stop();
        for (int i=0; i<4; i++)
        {
            if ( m_refToInterface->modeStates[i] == SENT)
            {
                m_refToInterface->modeStates[i] = NACKED;
            }

        }
        errorCnt++;
        if (errorCnt > 3)
        {
            // your programs main state should be SEND FAILED
            qDebug()<<"SEND_MACHINE: ERROR sending failed";
        }
        else {
            qDebug()<<"SEND_MACHINE: NACK Error, retrying";
            sendMachine();
        }
    }
}

void BtManager::modeACK()
{
    if (m_refToInterface->m_modeState == STATE_REQUESTED)
    {
        qDebug()<<"SEND_MACHINE: ACK";
        sendTimer->stop();

        if (!sentCommands.empty())
        {
            int sentCmd = sentCommands.takeFirst();
            if ( m_refToInterface->modeStates[sentCmd] == SENT)
            {
                m_refToInterface->modeStates[sentCmd] = UPDATED; //ACKED;
            }
            else qDebug()<<"SEND_MACHINE: ack received without waiting for it";
        }
        else qDebug()<<"SEND_MACHINE: Nothing to acknowledge";

        // *** TEST
        //        for (int i=0; i<sentCommands.length(); i++)
        //        {
        //            int sentCmd = sentCommands.at(i);
        //            if ( m_refToInterface->modeStates[sentCmd] == SENT)
        //            {
        //                m_refToInterface->modeStates[sentCmd] = UPDATED; //ACKED;
        //            }
        //        }

        // *** BALINT
        //        for (int i=0; i<4; i++)
        //        {
        //            if ( m_refToInterface->modeStates[i] == SENT)
        //            {
        //                m_refToInterface->modeStates[i] = UPDATED; //ACKED;
        //            }
        //        }

        sendMachine();
    }

    else if (m_refToInterface->m_modeState == STATE_INIT) {
        qDebug()<<"SEND_MACHINE: ACK received while INIT state";

    }
}

void BtManager::socketWritten(qint64 val)
{
    Q_UNUSED(val);
    //qDebug()<<"Number of Bytes written:"<<val;
}



void BtManager::sendMode(const QByteArray &md, const QByteArray &val)
{
    QByteArray cmd = START_SIGN + "set_mode sid=" + SENSORID + ";md=" + md + ";val=" + val + STOP_SIGN;
    mSocket->write(cmd);
    mSocket->waitForBytesWritten(-1);
    //qDebug()<<"Command written:"<<cmd;
}



void BtManager::getMode(const QByteArray &md)
{// "get_mode sid=%s;md=%s"
    QByteArray cmd = START_SIGN + "get_mode sid=" + this->SENSORID + ";md=" + md + STOP_SIGN;
    //qDebug()<<"Command sent:"<<cmd;
    mSocket->write(cmd);
    mSocket->waitForBytesWritten(-1);
    mSocket->waitForReadyRead(-1);
}

void BtManager::sendMessage(const QString &message)
{
    QByteArray cmd = START_SIGN + message.toUtf8() + STOP_SIGN;
    mSocket->write(cmd);
    //qDebug()<<"Message sent:"<<text;
}

void BtManager::start()
{
    // ADD START_TIMER HERE


    //QThread::msleep(500);
    this->sendMessage(START);
    mSocket->waitForBytesWritten(-1);
    mSocket->waitForReadyRead(-1);
    qDebug()<<"Start message sent, asking for current modes";

    getMode(PRSMR);
    getMode(PRSOSR);
    getMode(TEMPMR);
    getMode(TEMPOSR);
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
        m_refToInterface->m_modeState = STATE_INIT;
        emit m_refToInterface->modeStateChanged();
        //this->start();


        // ****************************************************
        //                  TEST for changing modes
        bool testModes = false;
        if (testModes)
        {
            modeTimeout = 10000;
            modeTimer->setInterval(modeTimeout);
            modeTimer->start();
            qDebug()<<"timer started";
        }
        // ****************************************************
    }
    else {
        qDebug()<<"Multiple number of channels. Implementation needed.";
    }

}



void BtManager::onSocketDisconnected()
{
    qDebug()<<"Socket disconnected";
    m_refToInterface->m_modeState = STATE_UNCONNECTED;
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
// Pair NanoHub
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

void BtManager::startDiscovery()
{
    if (!mDeviceDiscoveryAgent->isActive())
    {
        qDebug()<<"Device discovery started";
        //onHostModeStateChanged(mLocalDevice->hostMode());
        mDeviceDiscoveryAgent->start(QBluetoothDeviceDiscoveryAgent::ClassicMethod);
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
    bool ifxOnly = true;

    qDebug() << "Found new device:" << dInfo.name() << "Address:" << dInfo.address().toString();
    if (dInfo.name().contains("IFX_NANO-25BB") && ifxOnly)
    {

        // Stop device discovery (remove later)
        mDeviceDiscoveryAgent->stop();
        // Assign received QBluetoothDeviceInfo
        mDeviceInfo = dInfo;

        // Pair NanoHub
        QBluetoothLocalDevice::Pairing pstatus = mLocalDevice->pairingStatus(mDeviceInfo.address());
        qDebug()<<"Initial pairing status:"<<pstatus;
        if (pstatus == QBluetoothLocalDevice::Unpaired)
        {
            this->pairDevice();
        }
        else {
            mLocalDevice->requestPairing(mDeviceInfo.address(),QBluetoothLocalDevice::Unpaired);
        }
    }
    else {
        //m_devices.append(new QBluetoothDeviceInfo(dInfo));

        //m_refToInterface->addDeviceToList(dInfo);
        mDeviceInfo = dInfo;

        emit deviceFound(mDeviceInfo.name(), mDeviceInfo.address().toString());
        //m_devices.append(new DeviceInfo(dInfo));
        //emit m_refToInterface->devicesChanged();
    }
}
