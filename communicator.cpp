/*
 * Communicator.cpp
 *
 *  Created on: Apr 5, 2012
 *      Author: dpesch
 */

#include <qdebug.h>
#include <QDir>
#include <QUrlQuery>
#include <limits.h>

#include "communicator.h"
#include "model.h"

const QString API_KEY = "D7U8tZqdFT9XI6UvuPoyqpNAE2WwwBpP"; //per client
const QString LOGIN = "smugmug.login.anonymously";
const QString ALBUMS = "smugmug.albums.get";
const QString IMAGES = "smugmug.images.get";
const QString EXIF = "smugmug.images.getEXIF";

// Dynamic properties for Request object
const char* IS_EXTIF_REQEST = "isExifRequest";
const char* IS_IMAGE_REQEST = "isImageRequest";
const char* ALBUM_ID = "albumID";
const char* IMAGE_ID = "imageID";
const char* IS_THUMB = "isThumb";
const char* IMAGE_FULL_PATH = "imageFullPath";

// for debug purpose only
//const uint MAX_ALBUMS =  3;
//const uint MAX_IMAGES =  4;

const uint MAX_ALBUMS = UINT_MAX / 2; // divided to be compatible with int indexes!!!
const uint MAX_IMAGES = UINT_MAX / 2;

Communicator::Communicator(Model* model) :  QObject(model), m_model(model) {

	m_nwam = new QNetworkAccessManager(this);

    connect(m_nwam, &QNetworkAccessManager::finished, this, &Communicator::finishedSlot);
    connect(this, &Communicator::jsonDataReceived, this, &Communicator::processJSONResponse);
    connect(this, &Communicator::loginResponseReceived, this, &Communicator::processLoginResponse);
    connect(this, &Communicator::albumsResponseReceived, this, &Communicator::processAlbumsResponse);
    connect(this, &Communicator::imagesResponseReceived, this, &Communicator::processImagesResponse);
    connect(this, &Communicator::exifResponseReceived, this, &Communicator::processExifResponse);

	qDebug() << "Qt homedir " << QDir::homePath();
	qDebug() << "Qt currentdir " << QDir::currentPath();
    QDir tmpPath(QDir::currentPath());
    tmpPath.mkdir("tmp");

    connect(this, &Communicator::imageLoaded, m_model, &Model::updateImage );
    connect(this, &Communicator::categoryLoaded, m_model, &Model::updateCategory );
    connect(this, &Communicator::subcategoryLoaded, m_model, &Model::updateSubcategory );
    connect(this, &Communicator::albumLoaded, m_model, &Model::updateAlbum );
    connect(this, &Communicator::imageInfoLoaded, m_model, &Model::updateImageInfo );
    connect(this, &Communicator::extraImageInfoLoaded, m_model, &Model::updateImageExtraInfo );
    connect(this, &Communicator::albumImageCountLoaded, m_model, &Model::updateAlbumImageCount);

    connect(this, &Communicator::albumsLoaded, m_model, &Model::albumsLoaded);
    connect(this, &Communicator::albumImageCountLoaded, m_model, &Model::albumImageCountLoaded);

    connect(m_model, &Model::usernameChanged, this, &Communicator::login );
    connect(m_model, &Model::requestImage, this, &Communicator::requestImage );
    connect(m_model, &Model::requestExtraImageInfo, this, &Communicator::requestExtraImageInfo );


}

Communicator::~Communicator() {
}

void Communicator::login() {
    m_session = "";
	requestData(LOGIN, API_KEY, NULL);
}

void Communicator::requestData( QString method, QString APIKey,
		const QMap<QString, QString> *queryItems,
		const QMap<QString, QVariant> * responseProps) {

	QNetworkRequest request(
			QUrl("http://api.smugmug.com/services/api/json/1.2.2/"));
	request.setHeader(QNetworkRequest::ContentTypeHeader,
			"application/x-www-form-urlencoded");

    QUrlQuery queryParams;

    queryParams.addQueryItem("method", method); //always here
    queryParams.addQueryItem("APIKey", APIKey);
	if (queryItems != NULL) {
		QMapIterator<QString, QString> iter(*queryItems);
		while (iter.hasNext()) {
			iter.next();
            queryParams.addQueryItem(iter.key(), iter.value());
		}
	}

    QByteArray data = queryParams.toString().toUtf8();

	qDebug("Requesting");

	QNetworkReply * response = m_nwam->post(request, data);
	if (responseProps != NULL) {
		QMapIterator<QString, QVariant> iter(*responseProps);
		while (iter.hasNext()) {
			iter.next();
			response->setProperty(iter.key().toStdString().c_str(),
					iter.value());
		}
	}
}

void Communicator::requestImage(QString jpegUrl, QString albumID,
		QString imageID, bool isThumb, QString imageFullPath) {
	QUrl url(jpegUrl);
	if (url.isValid()) {
		qDebug() << "load " << url << " thumb " << isThumb;
		QNetworkRequest request;
		request.setUrl(url);
		QNetworkReply* reply = m_nwam->get(request);

		reply->setProperty(IS_IMAGE_REQEST, true);
		reply->setProperty(ALBUM_ID, albumID);
		reply->setProperty(IMAGE_ID, imageID);
		reply->setProperty(IS_THUMB, isThumb);

        if (!imageFullPath.isEmpty())
			reply->setProperty(IMAGE_FULL_PATH, imageFullPath);

	} else {
		qWarning() << "Invalid URL" << jpegUrl;
	}
}

void Communicator::requestExtraImageInfo(QString albumId, QString imageId, QString imageKey) {

	// request image Exif
	QMap<QString, QString> params;
	params["SessionID"] = m_session;
    params["ImageID"] = imageId;
    params["ImageKey"] = imageKey;

	QMap<QString, QVariant> responseParams;
	responseParams[IS_EXTIF_REQEST] = true;
	responseParams[ALBUM_ID] = albumId;
	responseParams[IMAGE_ID] = imageId;

	requestData(EXIF, API_KEY, &params, &responseParams);
}

void Communicator::finishedSlot(QNetworkReply* reply) {
	qDebug() << "Got reply";
	// Reading attributes of the reply
	// e.g. the HTTP status code
	QVariant statusCodeV = reply->attribute(
			QNetworkRequest::HttpStatusCodeAttribute);
	// Or the target URL if it was a redirect:
	QVariant redirectionTargetUrl = reply->attribute(
			QNetworkRequest::RedirectionTargetAttribute);
	// see CS001432 on how to handle this

	// no error received?
	if (reply->error() == QNetworkReply::NoError) {
		// handle based on response
		if (isImageResponse(reply))
			processImageResponse(reply);
		else
			processJsonResponse(reply);

	}
	// Some http error received
	else {
		qWarning() << "Network Error " << reply->errorString();
		emit error(reply->errorString());
	}

	reply->deleteLater();
}

bool Communicator::isImageResponse(QNetworkReply* reply) {

	// Image request has albumID, imageID and isThumb attached as dynamic properties
	QVariant prop = reply->property(IS_IMAGE_REQEST);
	return prop.isValid();
}

bool Communicator::storeImage(QString fullPath, QNetworkReply* reply) {
	QByteArray bytes = reply->readAll(); // bytes

	QFile file(fullPath);
	if (!file.open(QIODevice::WriteOnly)) {
		qWarning() << "Could not open file " << fullPath << " for writing";
		emit error(tr("Could not open file %1 for writing").arg(fullPath));
		return false;
	}

	file.write(bytes);
	file.close();
	qDebug() << "jpeg downloaded" << fullPath;
	return true;
}

void Communicator::processImageResponse(QNetworkReply* reply) {

	// get dynamic properties associated with request
	QString albumID = reply->property(ALBUM_ID).toString();

	QVariant fullPathProp = reply->property(IMAGE_FULL_PATH);
	if (fullPathProp.isValid()) {
		// if full path is part of response - download only
		QString fullPath = reply->property(IMAGE_FULL_PATH).toString();
		if (!storeImage(fullPath, reply))
			return;
		emit imageSavedToShared(fullPath);

	} else {
		// otherwise download, update model and notify
		QString imageID = reply->property(IMAGE_ID).toString();
		bool isThumb = reply->property(IS_THUMB).toBool();

		QString filename = QString("image_%1_%2_%3.jpg").arg(albumID).arg(
				imageID).arg((isThumb ? "thumb" : "full"));

		QString fullPath = QDir::currentPath() + QString("/tmp/") + filename;
		QString localAsset = QUrl::fromLocalFile(fullPath).toString(QUrl::None);
		qDebug("downloaded image localAsset %s", qPrintable(localAsset));

		if (!storeImage(fullPath, reply))
			return;

		if (isThumb) {
			// try to save the image as galery thumb
			QString galeryFilename = QString("image_%1.jpg").arg(albumID);
			QFile galeryFile(galeryFilename);
			if (!galeryFile.exists()) {

			}
		}

		// finished - we are loaded
        emit imageLoaded(albumID, imageID, localAsset, isThumb, fullPath);
	}
}

void Communicator::processJsonResponse(QNetworkReply* reply) {

	QByteArray bytes = reply->readAll(); // bytes
	QString urlString = reply->request().url().toString();

	QString string(bytes); // string
	qDebug() << "JsonResponse: " << string;
	QScriptValue sc(QString::null); //default null
	QScriptEngine engine;
	sc = engine.evaluate("( " + string + ")");

#ifdef DEBUG
	dumpJSONData(sc);
	qDebug() << queryProperty(sc, QString("method")).toString();
	QString session = queryProperty(sc, QString("Login/Session/id")).toString();
	qDebug() << session;
#endif
	emit jsonDataReceived(&sc, reply);
}

void Communicator::processJSONResponse(QScriptValue* sc, QNetworkReply* reply) {
	QString method = queryProperty(*sc, QString("method")).toString();
	if (method == LOGIN) {
		emit loginResponseReceived(sc);
	} else if (method == ALBUMS) {
		emit albumsResponseReceived(sc);
	} else if (method == IMAGES) {
		emit imagesResponseReceived(sc);
	} else if (method == EXIF) {
		emit exifResponseReceived(sc, reply);
	} else {
		qWarning() << "Unexpected JSON response for method " << method;
	}
}

bool Communicator::processResponseStat(QScriptValue *sc) {
	QString stat = queryProperty(*sc, QString("stat")).toString();
	if (stat == QString::null || stat != "ok") {
			QString message = queryProperty(*sc, QString("message")).toString();
			if ( message == QString::null ) {
				message = "Unknown error.";
			}
			emit error( message );
			return false;
	}
	return true;
}

void Communicator::processLoginResponse(QScriptValue *sc) {
	if (! processResponseStat(sc)) {
		return;
	}

	m_session = queryProperty(*sc, QString("Login/Session/id")).toString();
	if (m_session != QString::null) {
		QMap<QString, QString> params;
        params["NickName"] = m_model->getUsername();
		params["SessionID"] = m_session;
		requestData(ALBUMS, API_KEY, &params);
	}
}

void Communicator::processAlbumsResponse(QScriptValue *sc) {
	if (! processResponseStat(sc)) {
			return;
	}

	QScriptValue albums = queryProperty(*sc, QString("Albums"));
	QScriptValueIterator it(albums);

	uint i = 0;
	while (it.hasNext()) {
		it.next();
		QScriptValue albumData = it.value();
        QString categoryId;
        QString subcategoryId;

		// skip items for which we could not obtain valid id
		QScriptValue idData = queryProperty(albumData, QString("id"));
		if (!idData.isValid())
			continue;

		QScriptValue categoryData = queryProperty(albumData,
				QString("Category"));
		QScriptValue subCategoryData = queryProperty(albumData,
				QString("SubCategory"));

		if (categoryData.isValid()) {
            categoryId = queryProperty(categoryData, QString("id")).toString();
            emit categoryLoaded(categoryId, queryProperty(categoryData,
                                                  QString("Name")).toString());
        }

		if (subCategoryData.isValid()) {
            subcategoryId = queryProperty(subCategoryData, QString("id")).toString();
            emit categoryLoaded(subcategoryId, queryProperty(categoryData,QString("Name")).toString());
        }

        QString id = idData.toString();
        QString key = queryProperty(albumData, QString("Key")).toString();
        emit albumLoaded(id, key, queryProperty(albumData, QString("Title")).toString(),
                         categoryId, subcategoryId);

        // request images
        QMap<QString, QString> params;
        params["SessionID"] = m_session;
        params["AlbumID"] = id;
        params["AlbumKey"] = key;
        params["Heavy"] = "true";
        requestData(IMAGES, API_KEY, &params);

		if (i++ > MAX_ALBUMS)
			break;
	}

    m_model->dumpShort();
	emit albumsLoaded();
}

void Communicator::processImagesResponse(QScriptValue *sc) {

	QString albumId = queryProperty(*sc, QString("Album/id")).toString();
	QScriptValue images = queryProperty(*sc, QString("Album/Images"));
	int imageCount = queryProperty(*sc, QString("Album/ImageCount")).toInt32();
    emit albumImageCountLoaded(albumId, imageCount);

	QScriptValueIterator it(images);

	int i;
	for (i = 0; it.hasNext() && i < imageCount; i++) {
		it.next();
		QScriptValue val = it.value();

		// skip items without id
		QScriptValue idData = queryProperty(val, QString("id"));
		if (!idData.isValid()) {
			qWarning("Album ID is missing. Skipping");
			continue;
		}

		QScriptValue keyData = queryProperty(val, QString("Key"));
		QScriptValue captionData = queryProperty(val, QString("Caption"));
		QScriptValue keywordsData = queryProperty(val, QString("Keywords"));

		QScriptValue thumbUrlData = queryProperty(val, QString("ThumbURL"));

		// fall back to middle or thumb data
		QScriptValue largeUrlData = queryProperty(val, QString("LargeURL"));
		if (!largeUrlData.isValid()) {
			largeUrlData = queryProperty(val, QString("MediumURL"));
		}
		if (!largeUrlData.isValid()) {
			largeUrlData = thumbUrlData;
		}

		QScriptValue filename = queryProperty(val, QString("FileName"));
		QScriptValue date = queryProperty(val, QString("Date"));

        ImageInfo info;
        info.key = keyData.toString();
        info.caption = captionData.toString();
        info.keywords = keywordsData.toString();
        info.thumbURL = thumbUrlData.toString();
        info.largeURL = largeUrlData.toString();
        info.filename = filename.toString();
        info.date = date.toString();

        emit imageInfoLoaded(albumId, idData.toString(), info);

		qDebug("Image %s (%s) appended", qPrintable(filename.toString()),
				qPrintable(thumbUrlData.toString()));

		if (i > (int) MAX_IMAGES)
			break;
	}

	qDebug() << "album:" << albumId << " has " << i << " images";
}

void Communicator::processExifResponse(QScriptValue *sc, QNetworkReply* reply) {

	// get dynamic properties associated with request
	QString albumId = reply->property(ALBUM_ID).toString();
	QString imageId = reply->property(IMAGE_ID).toString();

    ExtraImageInfo info;
    info.aperture = queryProperty(*sc, QString("Image/Aperture")).toString();
    info.dateTime = queryProperty(*sc, QString("Image/DateTime")).toString();
    info.exposureTime = queryProperty(*sc, QString("Image/ExposureTime")).toString();
    info.focalLength = queryProperty(*sc, QString("Image/FocalLength")).toString();
    info.iso = queryProperty(*sc, QString("Image/ISO")).toString();

    emit extraImageInfoLoaded(albumId, imageId, info);
}

void Communicator::dumpJSONData(QScriptValue value) {
	QScriptValueIterator it(value);
	while (it.hasNext()) {
		it.next();
		if (it.value().isObject()) {
			dumpJSONData(it.value());
			continue;
		}
		qDebug() << it.name() << ": " << it.value().toString();
	}
}


QScriptValue Communicator::queryProperty(QScriptValue& value, QString path) {
	QStringList pathList = path.split(QRegExp("[/]"));
	QScriptValueIterator *it;
	QScriptValue val = value;

	for (int i = 0; i < pathList.length(); i++) {
		QString pathElement = pathList.at(i);
		it = new QScriptValueIterator(val);

		while (it->hasNext()) {
			it->next();
			QScriptValue structObject = it->value();
#ifdef DEBUG
			qDebug() << "Path element " << pathElement << " struct NAME: " << it->name();
#endif
			if (pathElement == it->name()) {
				if (i == pathList.length() - 1) {
					return structObject; //we are at the end of path, return value
				}
				if (structObject.isObject()) { //it has to be Object!
					val = structObject;
					break; //we are not at the end, found another part of path, let's search for next one
				} else {
					return QScriptValue(); //assert, we can not get here under normal conditions??
				}
			}
		}
		delete it;
	}
	qWarning() << "Could not found property at path " << path;
	return QScriptValue(); //not found
}

