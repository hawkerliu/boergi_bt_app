#ifndef PLOTDATA_H
#define PLOTDATA_H

#include <QObject>
#include <QDebug>

class PlotData : public QObject
{
    Q_OBJECT

public:
    PlotData(QObject *parent = nullptr, double y=0, double t=0);


    double getT() { return m_t; }
    double getX() { return m_y; }

signals:
    void tChanged();
    void yChanged();

public slots:

private:
    double m_t;
    double m_y;
};

#endif // PLOTDATA_H
