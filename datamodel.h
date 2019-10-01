#ifndef DATAMODEL_H
#define DATAMODEL_H

#include <QObject>
#include <QDebug>
#include <QPointF>
#include <QAbstractSeries>
#include <QXYSeries>

using namespace QtCharts;


/*
 *
 * Model List -> Name, ID, UNIT und Data
 * Gridview ->
 * */

class DataModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString dataName READ getDataName NOTIFY nameChanged)
    Q_PROPERTY(QString dataUnit READ getDataUnit NOTIFY unitChanged)
    Q_PROPERTY(double dataFrequency READ getFrequency NOTIFY frequencyChanged)
    Q_PROPERTY(double maximumY READ getMaxY NOTIFY maxChangedY)
    Q_PROPERTY(double minimumY READ getMinY NOTIFY minChangedY)
    Q_PROPERTY(double maximumX READ getMaxX NOTIFY maxChangedX)
    Q_PROPERTY(double minimumX READ getMinX NOTIFY minChangedX)
    Q_PROPERTY(int count READ getDataCount NOTIFY dataCountChanged)
//    Q_PROPERTY(QString lineColor READ getColor WRITE setColor NOTIFY lineColorChanged)
//    Q_PROPERTY(Qt::PenStyle lineStyle READ getStyle WRITE setStyle NOTIFY lineStyleChanged)

public:
    DataModel(QString name,QString id,QString unit)
    {
        m_name = name;
        emit nameChanged();
        m_id = id;
        m_unit = unit;
        emit unitChanged();
        qDebug()<<"New DataModel:"<<name<<id<<unit;
        m_dataPoints = new QVector<QPointF>();
        m_dataPoints->reserve(10000); // reserving some space
    }

    void setFrequency (double freq) {
        m_freq = freq;
        emit frequencyChanged();
    }

    void setMaxY (double max) {
        m_maximumY = max;
        //qDebug()<<"MaxY of"<<m_name<<"changed to:"<<m_maximumY;
        emit maxChangedY();

    }
    void setMinY (double min) {
        m_minimumY = min;
        //qDebug()<<"MinY of"<<m_name<<"changed to:"<<m_minimumY;
        emit minChangedY();
    }
    void setMaxX (double max) {
        m_maximumX = max;
        //qDebug()<<"MaxX of"<<m_name<<"changed to:"<<m_maximumX;
        emit maxChangedX();
    }
    void setMinX (double min) {
        m_minimumX = min;
        //qDebug()<<"MinX of"<<m_name<<"changed to:"<<m_minimumX;
        emit minChangedX();
    }

//    void setColor(QString color) {
//        m_color = color;
//        qDebug()<<m_name<<"Color changed to"<<m_color;
//        emit lineColorChanged();
//        emit lineStyleChanged();
//    }

//    void setStyle(Qt::PenStyle style) {
//        m_style = style;
//        emit lineStyleChanged();
//        qDebug()<<m_name<<"Style changed to"<<m_style;
//    }

    double getFrequency() {
        return m_freq;
    }

    double getMaxY () {
        return m_maximumY;
    }

    double getMinY () {
        return m_minimumY;
    }

    double getMaxX () {
        return m_maximumX;

    }

    double getMinX () {
        return m_minimumX;
    }

    QString getID () {
        return m_id;
    }

    QString getDataName () {
        return m_name;
    }

    QString getDataUnit () {
        return m_unit;
    }

    void addPoint (QPointF point) {
        m_dataPoints->append(point);
        emit dataCountChanged();
    }

    QVector<QPointF> getTotalData () {
        return *m_dataPoints;
    }

    QVector<QPointF>* getTotalDataPtr () {
        return m_dataPoints;
    }

    void clearTotalData () {
        m_dataPoints->clear();
        emit dataCountChanged();
    }

    void clearSelectedData (int pos) {
        int size = m_dataPoints->size();
        m_dataPoints = new QVector<QPointF> (m_dataPoints->mid(pos,size));
        emit dataCountChanged();
    }

    int getDataCount() {
        return m_dataPoints->size();
    }

//    QString getColor() {
//        return m_color;
//    }

//    Qt::PenStyle getStyle() {
//        return m_style;
//    }

signals:
    void nameChanged();
    void unitChanged();
    void frequencyChanged();
    void maxChangedY();
    void minChangedY();
    void maxChangedX();
    void minChangedX();
    void dataCountChanged();
//    void lineColorChanged();
//    void lineStyleChanged();

private:
    QString m_name;
    QString m_id;
    QString m_unit;
    double m_freq;
    double m_maximumY = 0;
    double m_minimumY = 0;
    double m_maximumX = 0;
    double m_minimumX = 0;
    QVector<QPointF>* m_dataPoints;
//    QString m_color;
//    Qt::PenStyle m_style;
};

#endif // DATAMODEL_H
