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

    m_refToInterface->setRefToBtManager(this); // the interface also has a pointer to this instance
    //    m_refToInterface->setSomeString("Hello!"); // set a variable protected by a mutex. I have prepared for you an example stringlist buffer also with a mutex.
    //    m_refToInterface->someStringChanged(); // emit a signal of the interface directly
    //    m_refToInterface->addOneToSomeBuffer("Diese"); // just adding some to someBuffer to pick it out in Qml
    //    m_refToInterface->addOneToSomeBuffer("Und Diese"); // just adding some to someBuffer to pick it out in Qml
    //    m_refToInterface->addOneToSomeBuffer("Und jene"); // just adding some to someBuffer to pick it out in Qml
    // m_refToInterface->getAllFromSomeBuffer();

    m_DeviceDiscoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
    m_LocalDevice = new QBluetoothLocalDevice(this);

    // Connect Socket
    clearSocket();

    // init timers
    m_sendTimer = new QTimer(this);
    m_sendTimer->setInterval(2000);
    m_sendTimer->setSingleShot(true);
    m_startTimer = new QTimer(this);
    m_startTimer->setInterval(5000);
    m_startTimer->setSingleShot(true);

    // Connect DeviceDiscoveryAgent
    connect(m_DeviceDiscoveryAgent, SIGNAL(deviceDiscovered(const QBluetoothDeviceInfo&)),
            this, SLOT(onDeviceDiscovered(const QBluetoothDeviceInfo&)));
    connect(m_DeviceDiscoveryAgent, SIGNAL(error(QBluetoothDeviceDiscoveryAgent::Error)),
            this, SLOT(onDeviceScanError(QBluetoothDeviceDiscoveryAgent::Error)));
    connect(m_DeviceDiscoveryAgent, SIGNAL(finished()), this, SLOT(onDeviceScanFinished()));

    // Interface
    connect(m_refToInterface, SIGNAL(mainStateChanged()), this, SLOT(onMainStateChangedCpp()));
    connect(m_refToInterface, SIGNAL(scanDevicesInterface()), this, SLOT(startDiscovery()));
    connect(m_refToInterface, SIGNAL(stopScanDevicesInterface()), this, SLOT(stopDiscovery()));
    connect(m_refToInterface, SIGNAL(connectDeviceInterface(QString)), this, SLOT(connectToDevice(QString)));
    connect(m_refToInterface, SIGNAL(disconnectDeviceInterface()),this,SLOT(disconnectDevice()));
    connect(this, SIGNAL(deviceFound(QString,QString)),m_refToInterface, SLOT(addDeviceToList(QString,QString)));

    // Connect LocalDevice
    connect(m_LocalDevice, SIGNAL(hostModeStateChanged(QBluetoothLocalDevice::HostMode)),
            this, SLOT(onHostModeStateChanged(QBluetoothLocalDevice::HostMode)));
    connect(m_LocalDevice, SIGNAL(error(QBluetoothLocalDevice::Error)), m_refToInterface,
            SLOT(deviceError(QBluetoothLocalDevice::Error)));
    connect(m_LocalDevice, SIGNAL(pairingFinished(QBluetoothAddress,QBluetoothLocalDevice::Pairing)),
            this, SLOT(onPairingFinished(QBluetoothAddress,QBluetoothLocalDevice::Pairing)));
    connect(m_LocalDevice, SIGNAL(pairingDisplayConfirmation(QBluetoothAddress,QString)),
            this, SLOT(onPairingDisplayConfirmation(QBluetoothAddress,QString)));
    connect(m_LocalDevice, SIGNAL(pairingDisplayPinCode(QBluetoothAddress,QString)),
            this, SLOT(onPairingDisplayPinCode(QBluetoothAddress,QString)));

    // Connect send machine: ack, nack, timer
    connect(this, SIGNAL(acknowledged()),this, SLOT(modeACK()));
    connect(this, SIGNAL(notAcknowledged()),this, SLOT(modeNACK()));
    connect(m_sendTimer, SIGNAL(timeout()), this, SLOT(sendTimerExpired()));
    //  connect(m_refToInterface, SIGNAL(configCommandsReceived()), this, SLOT(sendMachine()));
    // Connecct start timer
    connect(m_startTimer, SIGNAL(timeout()), this, SLOT(startTimerExpired()));

    connect(this, SIGNAL(addDataToModel(QString,QString,QString)),m_refToInterface,SLOT(addDataModel(QString,QString,QString)));
    connect(this, SIGNAL(addDataToModeConfig(QString,QString,QStringList)),m_refToInterface,SLOT(addModeConfig(QString,QString,QStringList)));

}

BtManager::~BtManager()
{
    this->sendMessage(STOP);
    m_Socket->waitForBytesWritten(-1);
    m_Socket->waitForReadyRead(-1);

    m_Socket->disconnectFromService();
    //QThread::sleep(1);
    qDebug()<<"Disconnected.";

    //deleteLater();

    //    delete mDeviceDiscoveryAgent;
    //    delete mLocalDevice;
    //    delete mServiceInfo;
    //    delete mSocket;
    qDebug()<<"You have been terminated.";
}

void BtManager::onMainStateChangedCpp()
{
    switch (m_refToInterface->m_mainState) {
    case STATE_INIT:
        qDebug()<<"MAIN_STATE: INIT";
        this->init();
        break;
    case STATE_RUN:
        qDebug()<<"MAIN_STATE: RUN";
        // sendMessage(START);
        break;

    case STATE_REQUESTED:
        qDebug()<<"MAIN_STATE: REQUESTED";
        sendMachine();
        break;

    case STATE_UPDATED:
        qDebug()<<"MAIN_STATE: UPDATED";
        m_refToInterface->m_mainState = STATE_RUN;
        // prev values really neccessary? maybe restore values on error
        //        m_refToInterface->m_prev_prs_mr = m_refToInterface->m_prs_mr;
        //        m_refToInterface->m_prev_prs_osr = m_refToInterface->m_prs_osr;
        //        m_refToInterface->m_prev_temp_mr = m_refToInterface->m_temp_mr;
        //        m_refToInterface->m_prev_temp_osr = m_refToInterface->m_temp_osr;

        // set mode states
        //        m_refToInterface->modeStatesOLD[PRS_MR] = UPDATED;
        //        m_refToInterface->modeStatesOLD[PRS_OSR] = UPDATED;
        //        m_refToInterface->modeStatesOLD[TEMP_MR] = UPDATED;
        //        m_refToInterface->modeStatesOLD[TEMP_OSR] = UPDATED;

        // reset state list
        //        for (int i=0;i<m_refToInterface->m_stateList.length();i++)
        //        {
        //            m_refToInterface->m_stateList[i] = false;
        //        }

        // m_refToInterface->resetBorders();
        break;



    case STATE_UNCONNECTED:
        qDebug()<<"TIMER STOPPED";
        m_startTimer->stop();
        break;

    case STATE_FAILED:
        qDebug()<<"MAIN_STATE: FAILED";
        // ***********************************************************
        // read modes from device instead of setting previous values
        qDebug()<<"Reading modes failed or timeout occured. Reading current sensor config...";
        //        for (int i=0; i<m_refToInterface->m_stateList.length();i++)
        //        {
        //            m_refToInterface->m_stateList[i] = false;
        //        }
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

//void BtManager::calcYBorders(double &value, int id)
//{
//    double maxY =( (DataModel*) m_refToInterface->dataList.at(id) )->getMaxY();
//    double minY =( (DataModel*) m_refToInterface->dataList.at(id) )->getMinY();
//    if (value < 50000) m_refToInterface->axisOffs = 0.1; // 10%
//    else m_refToInterface->axisOffs = 0.01; // 1%
//    double axisDiff = maxY - minY;
//    if (value < minY)
//    {
//        ( (DataModel*) m_refToInterface->dataList.at(id) )->setMinY(value - (axisDiff*m_refToInterface->axisOffs));
//    }
//    if (value > maxY)
//    {
//        ( (DataModel*) m_refToInterface->dataList.at(id) )->setMaxY(value + (axisDiff*m_refToInterface->axisOffs));
//    }
//}

void BtManager::init()
{
    qDebug()<<"waiting 1sec";
    QThread::msleep(1000);
    m_startTimer->start();
    m_startTimer->setSingleShot(true);
    m_refToInterface->m_initState = STATE_INFO_REQ;
    getSensorInfo(this->SENSORID);
}

bool BtManager::containsNonASCII(QString str)
{
    return str.contains(QRegularExpression(QStringLiteral("[^\\x{0000}-\\x{007F}]")));
}

void BtManager::cleanString(QString *str)
{
    // doesnt remove ',' or ':', because it is needed for separation,
    // '/' needed for units, '_' for ifx commands, '.' for values
    str->remove(QRegExp(QString::fromUtf8("[-`~!@#$%^&*()â€”+=|;<>Â«Â»?{}\'\"\\\[\\\]\\\\]")));
}

void BtManager::socketReadData()
{
    QByteArray byteBuffer;
    QStringList bufferList;
    QStringList elementList;
    QStringList infoList;
    QString element;
    QString valID;
    //bool invalidString = false;
    double time;
    double val;
    double f = 0;

    // Read data from buffer as long as there are packages
    while (m_Socket->canReadLine()) {
        byteBuffer = m_Socket->readLine();
        if (byteBuffer.isEmpty()) qDebug()<<"no data to read";
        bufferList.append(QString::fromStdString(byteBuffer.toStdString()));
    }
    // qDebug()<<"WHOLE MESSAGE:"<<bufferList<<"size:"<<bufferList.size();
    if (!bufferList.empty())
    {
        if (m_refToInterface->m_mainState == STATE_RUN ||
                m_refToInterface->m_mainState == STATE_REQUESTED)
        {
            for (int i = 0; i < bufferList.length(); i++)
            {
                element = bufferList.at(i);
                if (!containsNonASCII(element))
                {
                    val = element.section(',',2,2).toDouble();
                    time = element.section(',',3,3).toDouble();
                    time = time/10000; // convert to seconds
                    valID = element.section(',',1,1);

                    // update visible data
                    for (int j=0;j<m_refToInterface->dataList.size();j++) {
                        if (valID == ( (DataModel*) m_refToInterface->dataList.at(j) )->getID())
                        {
                            if (time < m_prevTimes.at(j)) {
                                qDebug()<<"TIME ERROR: value is"<<time;
                                return; // nicht optimal... rest von bufferlist wird nicht gelesen
                            }
                            // Calculate frequency
                            f = 1/(static_cast<double>(time-m_prevTimes.at(j)));
                            m_prevTimes[j] = time;
                            ( (DataModel*) m_refToInterface->dataList.at(j) )->setFrequency(f);
                            // Add point
                            ( (DataModel*) m_refToInterface->dataList.at(j) )->addPoint(QPointF(time,val));
                            emit m_refToInterface->newPointCollected(( (DataModel*) m_refToInterface->dataList.at(j) )->getDataName());
                            //                            if (false)
                            //                            {
                            //                                if (( (DataModel*) m_refToInterface->dataList.at(j) )->getTotalData().size()==1)
                            //                                {

                            //                                    if (val < 50000) m_refToInterface->axisOffs = 0.1; // 10%
                            //                                    else m_refToInterface->axisOffs = 0.01; // 1%
                            //                                    //qDebug()<<"Using axis offset:"<<m_refToInterface->axisOffs;
                            //                                    ( (DataModel*) m_refToInterface->dataList.at(j) )->setMinY(val-(m_refToInterface->axisOffs*val));
                            //                                    ( (DataModel*) m_refToInterface->dataList.at(j) )->setMaxY(val+(m_refToInterface->axisOffs*val));
                            //                                    ( (DataModel*) m_refToInterface->dataList.at(j) )->setMinX(time);
                            //                                    ( (DataModel*) m_refToInterface->dataList.at(j) )->setMaxX(time+1);
                            //                                }
                            //                                else {
                            //                                    calcYBorders(val, j);
                            //                                    ( (DataModel*) m_refToInterface->dataList.at(j) )->setMaxX(time);
                            //                                }
                            //                            }
                            break;
                        }
                    }
                    // update hidden data
                    for (int j=0;j<m_refToInterface->dataToHideList.size();j++) {
                        if (valID == ( (DataModel*) m_refToInterface->dataToHideList.at(j) )->getID())
                        {
                            // Add point, but don't emit newPointCollected signal
                            ( (DataModel*) m_refToInterface->dataToHideList.at(j) )->addPoint(QPointF(time,val));
                            break;
                        }
                    }
                    // read ack and nack
                    if (element.contains("nack")) emit notAcknowledged();
                    else if (element.contains("ack")) emit acknowledged();
                    else if (element.contains("error")) qDebug()<<"Error received";
                }
                else {
                    corruptedCounter++;
                    //  qDebug()<<"Invalid string:"<<element<<"\t Skipping. Corrupted count:"<<corruptedCounter;
                }
            }
        }
        else if (m_refToInterface->m_mainState == STATE_INIT)
        {
            if (m_refToInterface->m_initState == STATE_INFO_REQ)
            {
                infoList.clear();
                m_refToInterface->measNames.clear();
                emit m_refToInterface->namesChanged();
                m_refToInterface->measIDs.clear();
                m_refToInterface->measUnits.clear();
                qDebug()<<"READ IN INFO REQ STATE";
                for (int i = 0; i < bufferList.length(); i++)
                {
                    element = bufferList.at(i);
                    // simple fix for passing only infolists
                    if (element.contains("manufacturer"))
                    {
                        //qDebug()<<"infolist raw:"<<element;
                        if (containsNonASCII(element))
                        {
                            qDebug()<<"NON ASCII IN INFO LIST. RETRYING.";
                            startTimerExpired();
                            break;
                        }
                        element = element.simplified();
                        cleanString(&element);
                        elementList = element.split(',');
                        // further seperation
                        for (int j=0;j<elementList.size();j++) {
                            QStringList tmplist = elementList.at(j).split(':');
                            infoList.append(tmplist);
                        }
                    }
                }
                if (!infoList.empty())
                {
                    int firstMeasurand = 12;
                    int measurandStep = 8;
                    int idStep = 2;
                    int unitStep = 6;
                    for (int i=firstMeasurand;i<infoList.size();i+=measurandStep) {
                        m_refToInterface->measNames.append( infoList.at(i).toUtf8() );
                        emit m_refToInterface->namesChanged();
                        m_refToInterface->measIDs.append( infoList.at(i+idStep).toUtf8() );
                        m_refToInterface->measUnits.append( infoList.at(i+unitStep).toUtf8() );
                        qDebug()<<infoList.at(i)<<infoList.at(i+idStep)<<infoList.at(i+unitStep);
                    }
                    qDebug()<<"Names:"<<m_refToInterface->measNames;
                    qDebug()<<"IDs:"<<m_refToInterface->measIDs;
                    qDebug()<<"Units:"<<m_refToInterface->measUnits;

                    m_refToInterface->m_initState = STATE_MODES_REQ;
                    getAvailableModes();
                }
                else {
                    qDebug()<<"info list empty";
                    //initError();
                }
            }
            else if (m_refToInterface->m_initState == STATE_MODES_REQ)
            {
                qDebug()<<"READ IN MODES REQ STATE";
                if (bufferList.length() > 1) qDebug()<<"Message ambiguity: MODES";
                if (bufferList.empty()) initError();

                for (int i = 0; i < bufferList.length(); i++)
                {
                    element = bufferList.at(i);
                    if (element.contains("modes"))
                    {
                        int count;
                        bool reverseOrder = false;
                        QByteArray modeNum;
                        element = element.simplified();
                        cleanString(&element);
                        count = element.section(':',1,1).toInt();
                        qDebug()<<"Number of MODES:"<<count;
                        m_refToInterface->modeCount = count;
                        m_refToInterface->m_initState = STATE_MODE_REQ;
                        // trying reverse order, as asking for index 0 causes problems

                        if (reverseOrder) {
                            for (int j=count-1;j>=0;j--) {
                                modeNum.setNum(j);
                                getAvailableMode(modeNum);
                                qDebug()<<"waiting 10ms";
                                QThread::msleep(10);
                            }
                        }
                        else {
                            for (int j=0;j<count;j++) {
                                modeNum.setNum(j);
                                getAvailableMode(modeNum);
                                qDebug()<<"waiting 10ms";
                                QThread::msleep(10);
                            }
                        }
                    }
                    else qDebug()<<"Wrong message in MODES REQ:"<<element;
                }
            }
            else if (m_refToInterface->m_initState == STATE_MODE_REQ)
            {
                qDebug()<<"READ IN MODE REQ STATE";
                qDebug()<<bufferList;
                QStringList tmplist;
                QStringList modeList;
                if (bufferList.empty()) initError();
                for (int i = 0; i < bufferList.length(); i++)
                {
                    element = bufferList.at(i);
                    element = element.simplified();
                    cleanString(&element);
                    tmplist = element.split(',');
                    modeList.clear();
                    for (int j=0;j<tmplist.size();j++) {
                        modeList.append(tmplist.at(j).split(':'));
                    }
                    //qDebug()<<"MODE LIST:"<<modeList;
                    m_refToInterface->modeNames.append(modeList.at(modeList.indexOf("name")+1));
                    m_refToInterface->modeCommands.append(modeList.at(modeList.indexOf("cmd")+1));

                    tmplist.clear();
                    for (int j=modeList.indexOf("val")+1;j<modeList.size()-3;j++) {
                        //qDebug()<<"iteration is"<<i;
                        tmplist.append(modeList.at(j));
                    }
                    //qDebug()<<"index of val:"<<modeList.indexOf("val")<<"size:"<<modeList.size();
                    m_refToInterface->valueList.append(tmplist);
                }

                qDebug()<<"MODE NAMES:"<<m_refToInterface->modeNames;
                qDebug()<<"MODE COMMANDS:"<<m_refToInterface->modeCommands;
                qDebug()<<"MODE VALUES:"<<m_refToInterface->valueList;

                if (m_refToInterface->modeNames.size()==m_refToInterface->modeCount
                        && m_refToInterface->modeCommands.size()==m_refToInterface->modeCount
                        && m_refToInterface->valueList.size()==m_refToInterface->modeCount
                        && m_refToInterface->measNames.size() == m_refToInterface->measIDs.size()
                        && m_refToInterface->measNames.size() == m_refToInterface->measUnits.size())
                {
                    qDebug()<<"*** INIT COMPLETE ***";
                    m_startTimer->stop();
                    m_refToInterface->dataList.clear();
                    m_refToInterface->dataToHideList.clear();
                    m_refToInterface->resetArray.clear();
                    m_refToInterface->resetArray.reserve(m_refToInterface->measNames.size());
                    m_prevTimes.clear();
                    m_prevTimes.reserve(m_refToInterface->measNames.size());
                    m_checkList.reserve(m_refToInterface->modeNames.size());

                    for (int i=0;i<m_refToInterface->measNames.size();i++) {
                        emit addDataToModel(m_refToInterface->measNames.at(i),
                                            m_refToInterface->measIDs.at(i),
                                            m_refToInterface->measUnits.at(i));
                        m_prevTimes.append(0);
                        m_refToInterface->resetArray.append(false);
                    }
                    for (int i=0;i<m_refToInterface->modeNames.size();i++) {
                        emit addDataToModeConfig(m_refToInterface->modeNames.at(i),
                                                 m_refToInterface->modeCommands.at(i),
                                                 m_refToInterface->valueList.at(i));
                        m_checkList.append(false);
                    }
                    // wait until all DataModels are created
                    while (m_refToInterface->dataList.size() != m_refToInterface->measNames.size()) {
                        QThread::msleep(10);
                    }
                    emit m_refToInterface->initComplete();
                    sendMessage(START);
                    m_refToInterface->m_mainState = STATE_RUN;
                    emit m_refToInterface->mainStateChanged();
                }
                else qDebug()<<"Init not complete.";
            }
        }
        else qDebug()<<"undefined main state while reading";
    }
    else {
        // qDebug()<<"no data in buffer";
    }
    bufferList.clear();
}

void BtManager::sendMachine()
{
    if (!m_refToInterface->sendingIsActive) {
        m_refToInterface->sendingIsActive = true;
        qDebug()<<"Starting SEND_MACHINE";
    }
    uint8_t state;

    for (int i=0;i<m_refToInterface->modeConfigList.size();i++) {
        state = ( (ModeConfig*) m_refToInterface->modeConfigList.at(i) )->getTransmitState();
        if ( state == DIRTY )
        {
            qDebug()<<"needs to be cleaned:"<<( (ModeConfig*) m_refToInterface->modeConfigList.at(i) )->getModeCommand();

            qDebug()<<"waiting 10ms";
            QThread::msleep(10);

            sendMode(( (ModeConfig*) m_refToInterface->modeConfigList.at(i) )->getModeCommand().toLatin1(),
                     ( (ModeConfig*) m_refToInterface->modeConfigList.at(i) )->getConfig().toLatin1());
            ( (ModeConfig*) m_refToInterface->modeConfigList.at(i) )->setTransmitState(SENT);
            m_sentCommands.enqueue(i);
            m_sendTimer->start();
        }
        else if ( state == (NACKED | TIMEOUTED))
        {
            qDebug()<<"SEND_MACHINE: Mode NACKED or TIMEOUTED, Index:"<<i;
            // send again
            ( (ModeConfig*) m_refToInterface->modeConfigList.at(i) )->setTransmitState(DIRTY);
            qDebug()<<"another call of sendMachine() neccessary?";
            sendMachine();
        }
        else if ( state == UPDATED )
        {
            m_checkList[i] = true;
            if (m_checkList.indexOf(false) == -1)
            {
                qDebug()<<"SEND_MACHINE: CONFIG UPDATE COMPLETE";
                m_sendTimer->stop();
                m_refToInterface->sendingIsActive = false;
                m_sentCommands.clear();
                m_errorCnt = 0;
                m_refToInterface->m_mainState = STATE_UPDATED;
                emit m_refToInterface->mainStateChanged();
                for (int j=0;j<m_checkList.size();j++) {
                    m_checkList[j] = false;
                }
                return;
            }
        }
    }
}

void BtManager::sendTimerExpired()
{
    qDebug()<<"SEND_MACHINE: Timer expired";
    for (int i=0;i<m_refToInterface->modeConfigList.size();i++)
    {
        if  (SENT == ( (ModeConfig*) m_refToInterface->modeConfigList.at(i) )
             ->getTransmitState())
        {
            ( (ModeConfig*) m_refToInterface->modeConfigList.at(i) )->setTransmitState(TIMEOUTED);
        }
    }

    m_errorCnt++;
    if (m_errorCnt > 3)
    {
        qDebug()<<"SEND_MACHINE: ERROR sending failed after 3 timeouts";
        m_refToInterface->sendingIsActive = false;
        emit m_refToInterface->sendingFailed();
    }
    else {
        qDebug()<<"SEND_MACHINE: Timeout Error, retrying";
        sendMachine();
    }
}

void BtManager::modeNACK()
{
    qDebug()<<"NOT ACKNOWLEDGED";
    if (m_refToInterface->m_mainState == STATE_REQUESTED)
    {
        qDebug()<<"SEND_MACHINE: NACK";
        m_sendTimer->stop();
        for (int i=0;i<m_refToInterface->modeConfigList.size();i++)
        {
            if  (SENT == ( (ModeConfig*) m_refToInterface->modeConfigList.at(i) )
                 ->getTransmitState())
            {
                ( (ModeConfig*) m_refToInterface->modeConfigList.at(i) )->setTransmitState(NACKED);
            }

        }

        m_errorCnt++;
        if (m_errorCnt > 3)
        {
            // your programs main state should be SEND FAILED
            m_refToInterface->sendingIsActive = false;
            qDebug()<<"SEND_MACHINE: ERROR sending failed";
            emit m_refToInterface->sendingFailed();
            return;

        }
        else {
            qDebug()<<"SEND_MACHINE: NACK Error, retrying";
            sendMachine();
        }
    }
    else {
        qDebug()<<"SEND_MACHINE: Nack received";
    }
}

void BtManager::modeACK()
{
    int index;
    if (m_refToInterface->m_mainState == STATE_REQUESTED)
    {
        m_sendTimer->stop();
        if (!m_sentCommands.empty()) {
            index = m_sentCommands.dequeue();
            if  (SENT == ( (ModeConfig*) m_refToInterface->modeConfigList.at(index) )
                 ->getTransmitState())
            {
                qDebug()<<"SEND_MACHINE: Acknowledging sent command";
                ( (ModeConfig*) m_refToInterface->modeConfigList.at(index) )
                        ->setTransmitState(UPDATED);
                //m_checkList[index] = true;
                qDebug()<<"state updated";
            }
            else qDebug()<<"SEND_MACHINE: ack received without waiting for it";
        }
        else qDebug()<<"SEND_MACHINE: Nothing to acknowledge";
        sendMachine();
    }

    else {
        qDebug()<<"SEND_MACHINE: Ack received";
    }
}

void BtManager::sendMode(const QByteArray &md, const QByteArray &val)
{
    QByteArray cmd = START_SIGN + "set_mode sid=" + SENSORID + ";md=" + md + ";val=" + val + STOP_SIGN;
    m_Socket->write(cmd);
    //    mSocket->waitForBytesWritten(-1);
    qDebug()<<"SOCKET_WRITE: Mode written:"<<cmd;
}

void BtManager::getMode(const QByteArray &md)
{// "get_mode sid=%s;md=%s"
    QByteArray cmd = START_SIGN + "get_mode sid=" + this->SENSORID + ";md=" + md + STOP_SIGN;
    //qDebug()<<"Command sent:"<<cmd;
    m_Socket->write(cmd);
    //   mSocket->waitForBytesWritten(-1);
    qDebug()<<"SOCKET_WRITE: Asked for mode:"<<cmd;
}

void BtManager::getSensorInfo(const QByteArray &sid)
{
    QByteArray cmd = START_SIGN + SENSOR_INFO + " id=" + sid + STOP_SIGN;
    m_Socket->write(cmd);
    //    mSocket->waitForBytesWritten(-1);
    qDebug()<<"SOCKET_WRITE: Asked for info:"<<cmd;
}

void BtManager::getHello()
{
    QByteArray cmd = START_SIGN + HELLO + STOP_SIGN;
    m_Socket->write(cmd);
    //    mSocket->waitForBytesWritten(-1);
    qDebug()<<"SOCKET_WRITE: Said Hello to sensor:"<<cmd;
}

void BtManager::getAvailableModes()
{
    QByteArray cmd = START_SIGN + MODES + STOP_SIGN;
    m_Socket->write(cmd);
    //    mSocket->waitForBytesWritten(-1);
    qDebug()<<"SOCKET_WRITE: Asked for modes:"<<cmd;
}

void BtManager::getAvailableMode(const QByteArray &id)
{
    QByteArray cmd = START_SIGN + MODE + " id=" + id + STOP_SIGN;
    m_Socket->write(cmd);
    //    mSocket->waitForBytesWritten(-1);
    qDebug()<<"SOCKET_WRITE: Asked for modes:"<<cmd;
}

void BtManager::sendMessage(const QByteArray &msg)
{
    QByteArray cmd = START_SIGN + msg + STOP_SIGN;
    m_Socket->write(cmd);
    //    mSocket->waitForBytesWritten(-1);
    qDebug()<<"SOCKET_WRITE: Message sent:"<<cmd;
}

void BtManager::initError()
{
    qDebug()<<"Error! Restarting initialization...";
    m_startTimer->stop();
    m_refToInterface->clearInit();
    m_refToInterface->m_mainState = STATE_INIT;
    this->init();
}

void BtManager::startTimerExpired()
{
    m_startTimer->stop();
    emit m_refToInterface->errorToQml(2);
    qDebug()<<"*** START TIMEOUT -> RESET ALL AND START AGAIN ***";
    m_refToInterface->clearInit();
    m_refToInterface->m_mainState = STATE_INIT;
    this->init();
}

void BtManager::connectSocketDevice()
{
    if (m_DeviceInfo.isValid())
    {
        qDebug()<<"Socket connecting to"<<m_DeviceInfo.name()<<m_DeviceInfo.address();
        m_Socket->connectToService(m_DeviceInfo.address(),QBluetoothUuid::SerialPort, QIODevice::ReadWrite);
    }
    else qDebug()<<"Connecting to socket not possible: device info is not valid";
}

void BtManager::clearSocket()
{
    // Initialize socket. Moved to function for use in Windows.
    m_Socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol, this);
    connect(m_Socket, SIGNAL(readyRead()), this, SLOT(socketReadData()));
    connect(m_Socket, SIGNAL(connected()), this, SLOT(onSocketConnected()));
    connect(m_Socket, SIGNAL(disconnected()), this, SLOT(onSocketDisconnected()));
    connect(m_Socket, SIGNAL(error(QBluetoothSocket::SocketError)),
            this, SLOT(onSocketError(QBluetoothSocket::SocketError)));
    connect(m_Socket, SIGNAL(stateChanged(QBluetoothSocket::SocketState)),
            this, SLOT(onSocketStateChanged(QBluetoothSocket::SocketState)));
}

void BtManager::connectToDevice(QString address)
{
    m_connectAddress = QBluetoothAddress(address);
    if (!m_connectAddress.isNull())
    {
        m_unpair = false;
        qDebug()<<"Connect to device:"<<m_connectAddress;
        if (m_DeviceDiscoveryAgent->isActive() && m_DeviceDiscoveryAgent != nullptr)
        {
            m_DeviceDiscoveryAgent->stop();
            emit m_refToInterface->scanningChanged();
        }
#if defined(Q_OS_WIN)
        clearSocket();
#endif
        pairStatus = m_LocalDevice->pairingStatus(m_connectAddress);
        if (pairStatus == QBluetoothLocalDevice::Unpaired)
        {




#ifndef Q_OS_WIN
            qDebug()<<"OS: Linux -> pairing before connecting)";
            m_LocalDevice->requestPairing(m_connectAddress,QBluetoothLocalDevice::Paired);
#elif defined(Q_OS_WIN)
            qDebug()<<"OS: Windows -> connecting without pairing)";
            m_Socket->connectToService(m_connectAddress,QBluetoothUuid::SerialPort, QIODevice::ReadWrite);
#endif
        }
        else {
#ifndef Q_OS_WIN
            qDebug()<<"OS: Linux -> unpairing and pairing again)";
            m_LocalDevice->requestPairing(m_connectAddress,QBluetoothLocalDevice::Unpaired);
#elif defined(Q_OS_WIN)
            qDebug()<<"OS: Windows -> already paired, trying to connect)";
            m_Socket->connectToService(m_connectAddress,QBluetoothUuid::SerialPort, QIODevice::ReadWrite);
#endif
        }
        // mSocket->connectToService(connectAddress,QBluetoothUuid::SerialPort, QIODevice::ReadWrite);
    }
    else qDebug()<<"Address is NULL";
    //    // Get pairing status -> useless in windows
    //    QBluetoothLocalDevice::Pairing pstatus = mLocalDevice->pairingStatus(mDeviceInfo.address());
    //    qDebug()<<"Initial pairing status:"<<pstatus;
    //    if (pstatus == QBluetoothLocalDevice::Unpaired)
    //    {
    //        mLocalDevice->requestPairing(mDeviceInfo.address(),QBluetoothLocalDevice::Paired);
    //    }
    //    else {
    //        mLocalDevice->requestPairing(mDeviceInfo.address(),QBluetoothLocalDevice::Unpaired);
    //    }
}

void BtManager::disconnectDevice()
{
    m_unpair = true;
    this->sendMessage(STOP);
    m_Socket->waitForBytesWritten(1000);
    qDebug()<<"DISCONNECT SOCKET FROM QML";
    m_startTimer->stop();
    m_Socket->disconnectFromService();
    m_Socket->close();
#ifndef Q_OS_WIN
    m_LocalDevice->requestPairing(m_connectAddress,QBluetoothLocalDevice::Unpaired);
#endif
}

void BtManager::onSocketConnected()
{
    qDebug()<<"Socket connected";
    qDebug() << "Local:" << m_Socket->localName()
             << "Address:" << m_Socket->localAddress().toString()
             << "Port:" << m_Socket->localPort();
    qDebug() << "Peer:" << m_Socket->peerName()
             << "Address:" << m_Socket->peerAddress().toString()
             << "Port:" << m_Socket->peerPort();
    if (m_Socket->writeChannelCount() == 1)
    {
        m_refToInterface->m_mainState = STATE_INIT;
        emit m_refToInterface->mainStateChanged();
    }
    else {
        qDebug()<<"Multiple number of channels. Implementation needed.";
    }
}

void BtManager::onSocketDisconnected()
{
    qDebug()<<"Socket disconnected";
    m_refToInterface->m_mainState = STATE_UNCONNECTED;
}

void BtManager::onSocketError(QBluetoothSocket::SocketError error)
{
    qDebug()<<"Socket error:"<<error;
    if (pairStatus != QBluetoothLocalDevice::Unpaired)
    {
        qDebug()<<"SOCKET ERROR OCCURED BECAUSE DEVICE WAS ALREADY PAIRED. INFORM USER ABOUT IT.";
    }
}

void BtManager::onSocketStateChanged(QBluetoothSocket::SocketState state)
{
    qDebug()<<"Socket state changed to"<<state;
    emit m_refToInterface->socketStateChanged();
    if (state == QBluetoothSocket::UnconnectedState || state == QBluetoothSocket::ClosingState)
    {
        m_startTimer->stop();
    }

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
    m_LocalDevice->pairingConfirmation(true);
    qDebug()<<"onPairingDisplayConfirmation, pin:"<<pin;
}

//void BtManager::pairDevice()
//{
//    if (mDeviceInfo.isValid())
//    {
//        qDebug()<<"Pairing request to device"<<mDeviceInfo.name();
//        //mLocalDevice->pairingConfirmation(true); // should not be used here, maybe in error handler
//        mLocalDevice->requestPairing(mDeviceInfo.address(),QBluetoothLocalDevice::Paired);
//    }
//    else qDebug()<<"Pairing not possible: device info not valid";
//}

void BtManager::onPairingFinished(QBluetoothAddress address,QBluetoothLocalDevice::Pairing pairing)
{
    qDebug()<<"Pairing finished:"<<address<<pairing;
#ifndef Q_OS_WIN
    switch (pairing) {
    case QBluetoothLocalDevice::Paired:
        // Connect to socket
        m_Socket->connectToService(address,QBluetoothUuid::SerialPort, QIODevice::ReadWrite);
        break;
    case QBluetoothLocalDevice::Unpaired:
        if (!m_unpair) {
            qDebug()<<"Device unpaired, pairing it again";
            m_LocalDevice->requestPairing(address,QBluetoothLocalDevice::Paired);
        }
        break;
    case QBluetoothLocalDevice::AuthorizedPaired:
        qDebug()<<"QBluetoothLocalDevice::AuthorizedPaired";
        break;
    }
#endif
}

void BtManager::startDiscovery()
{
    if (!m_DeviceDiscoveryAgent->isActive())
    {
        m_refToInterface->clearDevices();
        qDebug()<<"Device discovery started";
        m_DeviceDiscoveryAgent->start();
        emit m_refToInterface->scanningChanged();
    }
}

void BtManager::stopDiscovery()
{
    m_DeviceDiscoveryAgent->stop();
    emit m_refToInterface->scanningChanged();
}

void BtManager::onHostModeStateChanged(QBluetoothLocalDevice::HostMode mode)
{
    qDebug()<<"Host mode changed to:"<<mode;
}

void BtManager::onDeviceScanFinished()
{
    qDebug()<<"Device scan finished";
    emit m_refToInterface->scanningChanged();
}

void BtManager::onDeviceScanError(QBluetoothDeviceDiscoveryAgent::Error error)
{
    if (error == QBluetoothDeviceDiscoveryAgent::PoweredOffError)
        qDebug()<<"The Bluetooth adapter is powered off, power it on before doing discovery.";
    else if (error == QBluetoothDeviceDiscoveryAgent::InputOutputError)
        qDebug()<<"Writing or reading from the device resulted in an error.";
    else
        qDebug()<<"An unknown error has occurred.";
}

void BtManager::onDeviceDiscovered(const QBluetoothDeviceInfo &dInfo)
{
    qDebug() << "Found new device:" << dInfo.name() << "Address:" << dInfo.address().toString();
    m_DeviceInfo = dInfo;
    emit deviceFound(m_DeviceInfo.name(), m_DeviceInfo.address().toString());
}
