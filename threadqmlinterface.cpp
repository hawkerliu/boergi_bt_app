#include "threadqmlinterface.h"

ThreadQmlInterface::ThreadQmlInterface(QObject *parent) : QObject(parent)
{
    dataList = QList<QObject*>();
    //configMtx = new QMutex();
}
