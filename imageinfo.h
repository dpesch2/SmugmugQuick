#ifndef IMAGEINFO_H
#define IMAGEINFO_H

#include <QtCore/QObject>
#include <QtCore/QString>

struct ImageInfo {
    QString key;
    QString caption;
    QString keywords;
    QString thumbURL;
    QString largeURL;
    QString date;
    QString filename;
};
Q_DECLARE_METATYPE(ImageInfo)

struct ExtraImageInfo {
    QString aperture;
    QString dateTime;
    QString exposureTime;
    QString focalLength;
    QString iso;
};
Q_DECLARE_METATYPE(ExtraImageInfo)

#endif // IMAGEINFO_H

