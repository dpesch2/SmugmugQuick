#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include <QMetaType>

#include "applicationui.hpp"
#include "communicator.h"
#include "settings.h"
#include "album.h"
#include "model.h"
#include "image.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // register Settings
    qmlRegisterType<Settings>("Backend", 1, 0,"Settings");

    // register Settings
    qmlRegisterType<Model>("Backend", 1, 0,"Model");

    qmlRegisterType<Album>();
    qmlRegisterType<Model>();

    qRegisterMetaType<ImageInfo>();
    qRegisterMetaType<ExtraImageInfo>();

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
