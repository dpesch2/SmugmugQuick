/*
 * Communicator.h
 *
 *  Created on: Apr 5, 2012
 *
 */

#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H

#include <QtCore/QObject>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtScript/QScriptEngine>
#include <QtScript/QScriptValueIterator>

#include "imageinfo.h"

class Settings;
class Model;

class Communicator : public QObject
{
    Q_OBJECT

public:
    Communicator(Model* model);
    ~Communicator();

    Q_SIGNAL void albumsLoaded();
    Q_SIGNAL void imageLoaded( QString albumId, QString imageId, QString localPath, bool isThumb, QString fullPath );
    Q_SIGNAL void imageInfoLoaded(QString albumId, QString imageId, ImageInfo info);
    Q_SIGNAL void categoryLoaded(QString categoryId, QString name);
    Q_SIGNAL void subcategoryLoaded(QString categoryId, QString name);
    Q_SIGNAL void albumLoaded(QString id, QString key, QString title, QString categoryId, QString subcategoryId);
    Q_SIGNAL void extraImageInfoLoaded( QString albumId, QString imageId, ExtraImageInfo info);
    Q_SIGNAL void imageSavedToShared( QString fullPath);
    Q_SIGNAL void albumImageCountLoaded(QString albumId, int imageCount);
    Q_SIGNAL void error( QString errorMsg);

private:
    Q_SLOT void login();
    Q_SLOT void finishedSlot(QNetworkReply*);
    Q_SLOT void processJSONResponse(QScriptValue*, QNetworkReply*);
    Q_SLOT void processLoginResponse(QScriptValue*);
    Q_SLOT void processAlbumsResponse(QScriptValue*);
    Q_SLOT void processImagesResponse(QScriptValue*);
    Q_SLOT void processExifResponse(QScriptValue*, QNetworkReply* reply);

// private signals
    Q_SIGNAL void jsonDataReceived(QScriptValue*,QNetworkReply* reply);
    Q_SIGNAL void loginResponseReceived(QScriptValue*);
    Q_SIGNAL void albumsResponseReceived(QScriptValue*);
    Q_SIGNAL void imagesResponseReceived(QScriptValue*);
    Q_SIGNAL void exifResponseReceived(QScriptValue*,QNetworkReply* reply);

private:
    void requestData( QString,  QString, const QMap<QString, QString>* param = NULL, const QMap<QString, QVariant> *responseProps = NULL);
    static QScriptValue queryProperty(QScriptValue&, QString);
    void dumpJSONData(QScriptValue);
    void requestImage( QString jpegUrl, QString albumId, QString imageId, bool isThumb, QString imageFullPath = QString() );
    void requestExtraImageInfo( QString albumId,  QString imageId, QString imageKey );
    static bool isImageResponse(QNetworkReply* reply);
    void processImageResponse(QNetworkReply* reply);
    void processJsonResponse(QNetworkReply* reply);

    bool storeImage( QString fullPath, QNetworkReply* reply );
    bool processResponseStat(QScriptValue *sc);

    QNetworkAccessManager* m_nwam;
    Model* m_model;
    QString m_session;
};

#endif /* COMMUNICATOR_H */
