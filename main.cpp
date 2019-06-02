#include <QApplication>
#include <QQmlApplicationEngine>

#include <btmanager.h>
#include "threadqmlinterface.h"
#include "btthread.h"

#include <QQmlContext>

/* to do:
 *
 * evtl daten mit qml timer abholen statt signale
 * zuvor neuen buffer mit daten füllen
 *
 * differenz und offset für chart:
 * werte in cpp berechnen und innerhalb der klasse updaten,
 * dann mit qproperty übergeben bei änderung (cppInterface.myParam)
 *
 * Destructor aus Interface Klasse aufrufen (pointer zum btmanager)
 *
 *
 *
 */

int main(int argc, char *argv[])
{
    // *****************************
    // to do:
    // - calcYborders()
    //
    // *****************************


    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    //QGuiApplication app(argc, argv);
    QApplication app(argc,argv);

    ThreadQmlInterface tqi;
    BtThread* btt;
    btt = new BtThread("Bluetooth Thread", &tqi);
    btt->start(); // pass ref

    //qmlRegisterType<BtManager>("btmanager", 1, 0, "BtManager");

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("cppInterface", &tqi);


    //BtManager manager;

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    //
    //    QObject* modeDialog = engine.rootObjects().first()->findChild<QObject*>("CustomModeDialog");
    //    QObject::connect(modeDialog, SIGNAL(testSignal()), &manager, SLOT(onTestSignal()));

    // https://stackoverflow.com/questions/22281291/qt-bluetooth-unable-to-connect-socket-to-device
    // https://www.maketecheasier.com/setup-bluetooth-in-linux/

    return app.exec();


}
