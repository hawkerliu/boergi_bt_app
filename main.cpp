#include <QApplication>
#include <QQmlApplicationEngine>
#include <btmanager.h>
#include "threadqmlinterface.h"
#include "btthread.h"
#include <QQmlContext>
#include <QQuickStyle>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc,argv);
    app.setOrganizationName("MCI - Management Center Innsbruck");
    app.setOrganizationDomain("mci.edu");
    app.setApplicationName("Infineon Sensor Vision");
    app.setWindowIcon(QIcon(":/pics/Infineon-Logo_white.png"));
    QQuickStyle::setStyle("Material");
    ThreadQmlInterface tqi;
    BtThread* btt;
    btt = new BtThread("Bluetooth Thread", &tqi);
    btt->start(); // pass ref
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("cppInterface", &tqi);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;
    return app.exec();
}
