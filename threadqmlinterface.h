#ifndef THREADQMLINTERFACE_H
#define THREADQMLINTERFACE_H

#include <QObject>
#include <btmanager.h>
#include <QThread>
#include <QMutex>
#include <QVector>

#include <QtCharts>
#include <QAbstractSeries>
#include <QXYSeries>
#include <QtMath>

#include "datamodel.h"
#include "modeconfig.h"

// Main states
#define STATE_UNCONNECTED   0
#define STATE_INIT          1
#define STATE_RUN           2
#define STATE_UPDATED       3
#define STATE_REQUESTED     4
#define STATE_STOP          5
#define STATE_FAILED        6
// Sub states (init)
#define STATE_EMPTY 10
#define STATE_INFO_REQ 11
#define STATE_MODE_REQ 12
#define STATE_MODES_REQ 13

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



class BtManager;

class ThreadQmlInterface : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int mainState READ getMainState WRITE setMainState NOTIFY mainStateChanged)
    // Timer
    Q_PROPERTY(bool seriesTimerActive READ getTimerActive WRITE setTimerActive NOTIFY timerActiveChanged)
    // Config
    Q_PROPERTY(QVariant configData READ config NOTIFY configUpdated())
    Q_PROPERTY(bool sendingActive READ getSendingActive NOTIFY sendingActiveChanged)
    // Chart
    Q_PROPERTY(double maxData MEMBER maxData NOTIFY maxDataChanged)
    Q_PROPERTY(QStringList graphNames READ getNames NOTIFY namesChanged)
    Q_PROPERTY(QVariant graphData READ data NOTIFY dataUpdated())
    Q_PROPERTY(QVariant graphDataToHide READ dataToHide NOTIFY dataToHideUpdated())
    // Discovery
    Q_PROPERTY(QVariant devices READ devices NOTIFY devicesUpdated)

    //Q_PROPERTY(QVariant modelList READ modelList NOTIFY modelListChanged)
    Q_PROPERTY(bool scanning READ scanning NOTIFY scanningChanged)
    Q_PROPERTY(int socketState READ socketState NOTIFY socketStateChanged)

private:
    bool m_isActive;
    BtManager* m_refToBtManager;
    bool m_someBufferHasFreshData;
    QStringList m_someStringBuffer;
    QString m_someString;
    QList<QObject*> m_devices;

    QString m_lastDevice;

    bool m_seriesTimerActive = false;
    QElapsedTimer fps_timer;

public:
    explicit ThreadQmlInterface(QObject *parent = nullptr);

    // States
    int m_mainState = STATE_UNCONNECTED;
    int m_initState = STATE_EMPTY;
    // Device info
    QStringList measNames;
    QStringList measIDs;
    QStringList measUnits;
    // Device mode config
    QSettings configSettings;
    QString settingsPath = QApplication::applicationDirPath()+"/config/";//inf_sv_config.ini"
    int modeCount;
    QStringList modeNames;
    QStringList modeCommands;
    QList<QStringList> valueList;
    QList<QObject*> modeConfigList;
    bool sendingIsActive = false;
    // Graph variables
    QList<QObject*> dataList;
    QList<QObject*> dataToHideList;
    int maxData = 500;
    double axisOffs;
    bool reset = false;
    QList<bool> resetArray;
    // Variables for saving data
    QStringList dataNamesFile;
    QList <int> dataCountsFile;


    // Ref function
    void setRefToBtManager(BtManager* ref)
    {
        m_isActive = true;
        if ( ref != nullptr)
            m_refToBtManager = ref;
        else
            qCritical("We got pointed to nowhere..");
    }

    // Clear functions
    void clearDevices() {
        m_devices.clear();
    }

    void clearInit() {
        dataList.clear();
        measNames.clear();
        measIDs.clear();
        measUnits.clear();
        modeCount = 0;
        modeNames.clear();
        modeCommands.clear();
        valueList.clear();
    }

    // Getters & Setters
    int getMainState() {
        return m_mainState;
    }
    void setMainState(int state) {
        m_mainState = state;
        emit mainStateChanged();
    }
    bool getTimerActive() {
        return m_seriesTimerActive;
    }
    void setTimerActive (bool active) {
        m_seriesTimerActive = active;
    }
    bool scanning() {
        return m_refToBtManager->isScanning();
    }
    int socketState() {
        return m_refToBtManager->getSocketState();
    }
    QVariant devices() {
        return QVariant::fromValue(m_devices);
    }
    QVariant data() {
        return QVariant::fromValue(dataList);
    }
    QVariant dataToHide() {
        return QVariant::fromValue(dataToHideList);
    }
    QVariant config() {
        return QVariant::fromValue(modeConfigList);
    }
    //    QStringList currentModes() {
    //        return currentModeConfig;
    //    }
    QStringList getNames() {
        return measNames;
    }
    bool getSendingActive () {
        return sendingIsActive;
    }




    // Invokables
    Q_INVOKABLE void measureFPS()
    {
        double fps;
        double elapsed;

        if (!fps_timer.isValid())
        {
            fps_timer.start();
        }
        else {
            elapsed = fps_timer.elapsed();
            fps = 1/(elapsed/1000);
            qDebug()<<"FPS:"<<fps;
            fps_timer.restart();
        }
    }

    Q_INVOKABLE void clearSeries(QAbstractSeries *p_series)
    {
        qDebug()<<"clearSeries";
        if (p_series) {
            QXYSeries *xySeries = static_cast<QXYSeries *>(p_series);
            xySeries->clear();
        }
    }

    Q_INVOKABLE void resetData()
    {
        for (int i=0; i < dataToHideList.size(); i++) {
            ( (DataModel*) dataToHideList.at(i) )->clearTotalData();
        }
        for (int i=0; i < dataList.size(); i++) {
            ( (DataModel*) dataList.at(i) )->clearTotalData();
        }
    }





    Q_INVOKABLE void saveDataToFile(QUrl folderPath, QString valueName, int count)
    {

        // get current timestamp
        QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd_HH:mm:ss");
        // file name
        QString fileName = timestamp+"_SensorData_"+m_lastDevice+"_"+valueName+".csv";
        // full path name
        QString fullPath = folderPath.toLocalFile()+QDir::separator()+fileName;
        // remove whitespace
        fullPath = fullPath.remove(QRegExp(" "));

        QFile file(fullPath);
        if (file.open(QIODevice::ReadWrite|QIODevice::Append)) { // |QFile::Truncate
            QTextStream output(&file);

            for (int i = 0; i < dataList.size(); i++) {
                if (valueName == ( (DataModel*) dataList.at(i) )->getDataName())
                {
                    output << "time/s" << "," << valueName+"/"+( (DataModel*) dataList.at(i) )->getDataUnit() << '\n';
                    //        qDebug()<<"Indexes to save:"<<dataCountsFile;
                    QVector<QPointF> tmpVec = ( (DataModel*) dataList.at(i) )->getTotalData();
                    tmpVec = tmpVec.mid(0,count);
                    for (int j=0;j<tmpVec.size();j++) {
                        qreal t = tmpVec.at(j).x();
                        qreal val = tmpVec.at(j).y();
                        // Fill a line
                        output << t << "," << val << '\n';
                    }
                    ( (DataModel*) dataList.at(i) )->clearSelectedData(count);
                }
            }

            //            for (int i = 0; i < dataToHideList.size(); i++) {
            //                if (valueName == ( (DataModel*) dataToHideList.at(i) )->getDataName())
            //                {
            //                    output << "time/s" << "," << valueName+"/"+( (DataModel*) dataToHideList.at(i) )->getDataUnit() << '\n';
            //                    //        qDebug()<<"Indexes to save:"<<dataCountsFile;
            //                    QVector<QPointF> tmpVec = ( (DataModel*) dataToHideList.at(i) )->getTotalData();
            //                    tmpVec = tmpVec.mid(0,count);
            //                    for (int j=0;j<tmpVec.size();j++) {
            //                        qreal t = tmpVec.at(j).x();
            //                        qreal val = tmpVec.at(j).y();
            //                        // Fill a line
            //                        output << t << "," << val << '\n';
            //                    }
            //                    ( (DataModel*) dataToHideList.at(i) )->clearSelectedData(count);
            //                }
            //            }

        }
        else qDebug()<<"Error: could not create .csv file for"<<fileName;

        file.close();
    }



    Q_INVOKABLE void scanDevices() {
        emit scanDevicesInterface();
    }

    Q_INVOKABLE void stopScanDevices() {
        emit stopScanDevicesInterface();
    }

    Q_INVOKABLE void connectDeviceQML(QString address, QString name) {
        qDebug()<<"last device:"<<m_lastDevice;
        m_lastDevice = name;
        emit connectDeviceInterface(address);
    }

    Q_INVOKABLE void disconnectDeviceQML() {
        emit disconnectDeviceInterface();
    }

    Q_INVOKABLE void exitApplication(void) {
        emit closeBTMthread();
    }

    Q_INVOKABLE void updateData(QAbstractSeries *p_series, QString seriesName)
    {
        int index = -1;
        QVector<QPointF> tmpVec;
        QXYSeries *xySeries = static_cast<QXYSeries *>(p_series);

        // selecting the right data
        for (int i = 0; i < dataList.size(); i++) {
            if (seriesName == ( (DataModel*) dataList.at(i) )->getDataName())
            {
                tmpVec = ( (DataModel*) dataList.at(i) )->getTotalData();
                index = i;
                break;
            }
        }

        // cut shown data if maximum is exceeded
        if (xySeries->count() >= maxData)
        {
            //qDebug()<<"Max size reached, Count (series):"<<xySeries->count()<<"tmp size:"<<tmpVec.size();
            tmpVec = tmpVec.mid(tmpVec.size()-maxData,maxData);
            if (index >= 0)
            {
                ( (DataModel*) dataList.at(index) )->setMinX(tmpVec.at(0).x());
            }
            else qDebug()<<"Error setting min x in updateData()";
        }

        double val;
        double time;
        int size = tmpVec.size();
        double tmpminx = 0;
        double tmpmaxx = 0;
        double tmpminy = std::numeric_limits<double>::max();
        double tmpmaxy = 0;
        double diff = 0;
        double axisOffset = 0.05;

        // Set minima/maxima
        if (size > 0)
        {
            if (size == 1) {
                val = tmpVec.at(0).y();
                time = tmpVec.at(0).x();
                ( (DataModel*) dataList.at(index) )->setMinY(val-(axisOffs*val));
                ( (DataModel*) dataList.at(index) )->setMaxY(val+(axisOffs*val));
                ( (DataModel*) dataList.at(index) )->setMinX(time);
                ( (DataModel*) dataList.at(index) )->setMaxX(time+1);
            }
            else {
                tmpminx = tmpVec.at(0).x();
                tmpmaxx = tmpVec.at(size-1).x();
                for (int i=0;i<size;i++) {
                    val = tmpVec.at(i).y();
                    if (val > tmpmaxy) {
                        tmpmaxy = val;
                    }
                    else if (val < tmpminy) {
                        tmpminy = val;
                    }
                }
                diff = tmpmaxy-tmpminy;
                tmpminy = tmpminy-(axisOffset*diff);
                tmpmaxy = tmpmaxy+(axisOffset*diff);
                ( (DataModel*) dataList.at(index) )->setMinY(tmpminy);
                ( (DataModel*) dataList.at(index) )->setMaxY(tmpmaxy);
                ( (DataModel*) dataList.at(index) )->setMinX(tmpminx);
                ( (DataModel*) dataList.at(index) )->setMaxX(tmpmaxx);
            }
        }
        else qDebug()<<"Graph update: invalid data";

        // Update graph
        xySeries->replace(tmpVec);
    }

    Q_INVOKABLE void hideData(QString id)
    {
        for (int i=0; i < dataList.size(); i++)
        {
            if ( ( (DataModel*) dataList.at(i) )->getDataName() == id)
            {
                dataToHideList.append((DataModel*) dataList.at(i));
                dataList.removeAt(i);
            }
        }
        emit dataUpdated();
        emit dataToHideUpdated();
    }

    Q_INVOKABLE void showData(QString id)
    {
        for (int i=0; i < dataToHideList.size(); i++)
        {
            if ( ( (DataModel*) dataToHideList.at(i) )->getDataName() == id)
            {
                dataList.append((DataModel*) dataToHideList.at(i));
                dataToHideList.removeAt(i);
            }
        }
        emit dataUpdated();
        emit dataToHideUpdated();
    }

    Q_INVOKABLE void appendConfigCommand(QString name, QString cmd)
    {
        for (int i=0;i<modeConfigList.size();i++) {
            if (( (ModeConfig*) modeConfigList.at(i) )->getModeCommand() == name)
            {
                ( (ModeConfig*) modeConfigList.at(i) )->setConfig(cmd);
            }
        }
    }

    Q_INVOKABLE void saveConfigSetting(QString name, QString value)
    {
        static qint8 statecounter = 0;
        name.remove(QRegExp("\\s")); // remove whitespace
        QString fullPath = settingsPath+"conf_"+m_lastDevice+".ini";
        QSettings settingsToSave(fullPath, QSettings::NativeFormat);
        settingsToSave.setValue("configdata/"+name,value);
        qDebug()<<"saved config:"<<name<<value<<settingsPath;
        if (statecounter+1 < modeConfigList.size())
            statecounter++;
        else {
            statecounter = 0;
            emit configSaved(fullPath);
        }
    }

    Q_INVOKABLE void loadConfigSetting(QString name)
    {
        QString cleanname = name;
        cleanname.remove(QRegExp("\\s")); // remove whitespace
        QString fullPath = settingsPath+"conf_"+m_lastDevice+".ini";
        QSettings loadedSettings(fullPath, QSettings::NativeFormat);
        QString value = loadedSettings.value("configdata/"+cleanname).value<QString>();
        static qint8 statecounter = 0;
        qDebug()<<"loaded config:"<<cleanname<<value;
        if (value != "")
            appendConfigCommand(name,value);
        else {
            emit configNotLoaded(cleanname);
        }
        if (statecounter+1 < modeConfigList.size())
            statecounter++;
        else {
            statecounter = 0;
            m_mainState = STATE_REQUESTED;
            emit mainStateChanged();
        }
    }

signals:
    void closeBTMthread();


    // Discovery
    void devicesUpdated();
    void scanningChanged();
    void socketStateChanged();
    void scanDevicesInterface();
    void stopScanDevicesInterface();
    void connectDeviceInterface(QString);
    void disconnectDeviceInterface();
    // Config
    void mainStateChanged();
    void configUpdated();
    void configCommandsReceived();
    void sendingActiveChanged();
    void sendingFailed();
    void configNotLoaded(QString name);
    void configSaved(QString path);
    // Charts
    void timerActiveChanged();
    void updateFrequency(const double& f);
    void updateQmlSeries(QString);
    // Graphs & communication
    void newPointCollected(QString modelName);
    void initComplete();
    void dataUpdated();
    void dataToHideUpdated();
    void namesChanged();
    // maxima/minima members
    void maxDataChanged();


    void errorToQml(int error);
    // File
    // void openFileDialog(QString name,QString path);



public slots:

    void addDeviceToList( QString name, QString address )
    {
        m_devices.append(new DeviceInfo(name, address));
        emit devicesUpdated();
    }

    void deviceError(QBluetoothLocalDevice::Error error)
    {
        emit errorToQml(error);
    }

    void addDataModel(QString name,QString id,QString unit)
    {
        dataList.append(new DataModel(name,id,unit));
        qDebug()<<"List of DataModels: size is"<<dataList.size();
        emit dataUpdated();
    }

    void addModeConfig(QString name, QString command, QStringList values)
    {
        modeConfigList.append(new ModeConfig(name,command,values));
        emit configUpdated();
    }



};

#endif // THREADQMLINTERFACE_H


// ************ Balint Templates ************
// Q_PROPERTY(bool isActive MEMBER m_isActive NOTIFY isActiveChanged)
// Q_PROPERTY(bool someBufferHasFreshData MEMBER m_someBufferHasFreshData NOTIFY someBufferHasFreshDataChanged)
// Q_PROPERTY(QString someString MEMBER m_someString NOTIFY someStringChanged)
//    QMutex someStringBufferMutex;
//    void setSomeString(QString str) {
//        m_someString = str;
//        emit someStringChanged();
//    }
//    void addOneToSomeBuffer(QString tstr)
//    {
//        someStringBufferMutex.lock();
//        m_someStringBuffer.append(tstr);
//        m_someBufferHasFreshData = true;
//        emit someBufferHasFreshDataChanged(); // Please test if it gets emitted automatically to QML, this call might be not necessary..
//        someStringBufferMutex.unlock();
//    }
//    Q_INVOKABLE void messageFromQml(const QString &str)
//    {
//        Q_UNUSED(str)
//        //qDebug()<<"Somebody spit in the pipe: "<<str;
//    }
//    Q_INVOKABLE void getAllFromSomeBuffer(void)
//    {
//        someStringBufferMutex.lock();
//        while (m_someStringBuffer.size())
//        {
//            emit someStringBufferPushesData(m_someStringBuffer.last());
//            m_someStringBuffer.removeLast();
//        }
//        m_someBufferHasFreshData = false;
//        someStringBufferMutex.unlock();
//    }
//    Q_INVOKABLE QString getLastFromSomeBuffer(void)
//    {
//        if (!m_someStringBuffer.size())
//            return "No new String..";

//        QString tmpString;
//        someStringBufferMutex.lock();
//        tmpString = m_someStringBuffer.last();
//        m_someStringBuffer.removeLast();
//        someStringBufferMutex.unlock();
//        return tmpString;
//    }

//    void isActiveChanged();
//    void someStringChanged();
//    void someBufferHasFreshDataChanged();
//    void someStringBufferPushesData(QString &str);
