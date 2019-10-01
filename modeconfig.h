#ifndef MODECONFIG_H
#define MODECONFIG_H

#include <QObject>
#include <QDebug>

// flags
#define DIRTY 0x01
#define SENT 0x02
#define UPDATED 0x03
#define TIMEOUTED 0x04
#define WAITING_FOR_ACK 0x05
#define ACKED 0x06
#define NACKED 0x07


class ModeConfig : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString modeName READ getModeName NOTIFY modeNameChanged)
    Q_PROPERTY(QString modeCommand READ getModeCommand NOTIFY modeCommandChanged)
    Q_PROPERTY(QStringList modeValues READ getModeValues NOTIFY modeValuesChanged)
    Q_PROPERTY(QString currentConfig READ getConfig WRITE setConfig NOTIFY currentConfigChanged())
public:
    ModeConfig(QString modeName,QString modeCommand,QStringList modeValues)
    {
        m_modeName = modeName;
        emit modeNameChanged();
        m_modeCommand = modeCommand;
        m_modeValues = modeValues;
        emit modeValuesChanged();
        qDebug()<<"New ModeConfig:"<<m_modeName<<m_modeCommand<<m_modeValues;


    }
    QString getModeName() {
        return m_modeName;
    }
    QStringList getModeValues() {
        return m_modeValues;
    }
    QString getConfig() {
        return m_currentConfig;
    }
    void setConfig(QString str) {
        if (m_currentConfig != str)
        {
            m_currentConfig = str;
            m_transmitState = DIRTY;
            qDebug()<<"Config for"<<m_modeName<<"changed to"<<m_currentConfig;
            emit currentConfigChanged();
        }
    }

    uint8_t getTransmitState() {
        return m_transmitState;
    }

    void setTransmitState(uint8_t state) {
        m_transmitState = state;
    }

    QString getModeCommand() {
        return m_modeCommand;
    }

signals:
    void modeNameChanged();
    void modeValuesChanged();
    void currentConfigChanged();
    void modeCommandChanged();

private:
    QString m_modeName;
    QString m_modeCommand;
    QStringList m_modeValues;
    QString m_currentConfig = "";
    uint8_t m_transmitState = UPDATED; // 0x00;
};

#endif // MODECONFIG_H
