#include "plotdata.h"

PlotData::PlotData(QObject *parent,double y, double t) : QObject(parent), m_t(t), m_y(y)
{
    qDebug()<<"new data:"<<m_y,m_t;
}

