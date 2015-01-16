TEMPLATE = app

QT += qml quick widgets script

SOURCES += main.cpp \
    album.cpp \
    category.cpp \
    communicator.cpp \
    image.cpp \
    settings.cpp \
    model.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    album.h \
    category.h \
    communicator.h \
    image.h \
    settings.h \
    model.h \
    imageinfo.h

