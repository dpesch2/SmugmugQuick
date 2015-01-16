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

// public signals
signals:
    void albumsLoaded();
    void imageLoaded( QString albumId, QString imageId, QString localPath, bool isThumb, QString fullPath );
    void imageInfoLoaded(QString albumId, QString imageId, ImageInfo info);
    void categoryLoaded(QString categoryId, QString name);
    void subcategoryLoaded(QString categoryId, QString name);
    void albumLoaded(QString id, QString key, QString title, QString categoryId, QString subcategoryId);
    void extraImageInfoLoaded( QString albumId, QString imageId, ExtraImageInfo info);
    void imageSavedToShared( QString fullPath);
    void albumImageCountLoaded(QString albumId, int imageCount);
    void error( QString errorMsg);

private slots:
    void login();
    void finishedSlot(QNetworkReply*);
    void processJSONResponse(QScriptValue*, QNetworkReply*);
    void processLoginResponse(QScriptValue*);
    void processAlbumsResponse(QScriptValue*);
    void processImagesResponse(QScriptValue*);
    void processExifResponse(QScriptValue*, QNetworkReply* reply);

// private signals
signals:
    void jsonDataReceived(QScriptValue*,QNetworkReply* reply);
    void loginResponseReceived(QScriptValue*);
    void albumsResponseReceived(QScriptValue*);
    void imagesResponseReceived(QScriptValue*);
    void exifResponseReceived(QScriptValue*,QNetworkReply* reply);

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
