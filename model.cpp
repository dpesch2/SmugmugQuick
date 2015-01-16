#include <qdebug.h>
#include <QtCore/QFile>
#include <QtCore/QDir>

#include "model.h"
#include "communicator.h"
#include "category.h"
#include "album.h"
#include "image.h"

// where to save albums/images
const QString SAVE_LOCATION = "/../shared/photos/smugmugdemo/";

Model::Model(QObject* parent) :  QObject(parent) {

    // populate categories with empty category representing non-assigned categories/subcategories
    const QString emptyKey("<empty>");
    m_emptyCategory = QSharedPointer<Category>(
            new Category(emptyKey, /*name*/""));
    m_categoryMap.insert(emptyKey, m_emptyCategory);
    m_subcategoryMap.insert(emptyKey, m_emptyCategory);

    new Communicator(this);
}

// cleanup temporary files
Model::~Model() {
    deleteLocalFiles();
}

// clean old user first
void Model::clear() {

    deleteLocalFiles();
    m_username = "";

    foreach(Album* album,  m_albumsMap.values() ) {
        delete album;
    }
    m_albumsMap.clear();
    m_categoryMap.clear();
    m_subcategoryMap.clear();
}

// delete temporary files
void Model::deleteLocalFiles() {

    QStringListIterator iter(m_localFilenames);
    while (iter.hasNext()) {

        // delete the file
        QString filename = iter.next();
        bool result = QFile(filename).remove();
        if (!result) {
            qWarning() << "Cannot delete temporary file: " << filename;
        } else {
            qDebug() << "Deleted temporary file: " << filename;
        }
    }

    m_localFilenames.clear();
}

QString Model::getUsername() const {
    return m_username;
}

void Model::setUsername(QString username) {
       clear();
       m_username = username;
       emit usernameChanged();
}

QList<QObject*> Model::getAlbums() const {

    QList<QObject*> infoList;
    foreach(Album* album, m_albumsMap.values() ) {
        infoList.append(album);
    }
    return infoList;
}

QList<QObject*> Model::searchAlbums(QString re) const {

    QList<QObject*> infoList;
    QRegExp regexp(re);
    foreach(Album* album, m_albumsMap.values() ) {
        if (album->getTitle().contains(re)) {
            infoList.append(album);
        }
    }
    return infoList;
}

QObject* Model::getAlbum(QString albumID) const {
    return m_albumsMap[albumID];
}

QList<QObject*> Model::getImages(QString albumID) const {
    QList<QObject*> infoList;
    Album* album = m_albumsMap[albumID];
    if (album != NULL) {
        foreach (Image* image, album->getImages()) {
            infoList.append(image);
        }
    }
    return infoList;
}

QObject* Model::getImage(QString albumID, QString imageID) const {
    Image* image = findImage(albumID, imageID);
    if (image == NULL)
        return NULL;

    return image;
}

QList<QObject*> Model::searchImages( QString re) const {

    QList<QObject*> infoList;
    QRegExp regexp(re);

    for (AlbumMap_t::const_iterator it = m_albumsMap.begin();
            it != m_albumsMap.end(); it++) {
        Album* album = m_albumsMap[it.key()];

        foreach (Image* image, album->getImages() ) {

            if (image->getKeywords().contains(re)
                    || image->getCaption().contains(re)) {
                infoList.append(image);
            }
        }
    }
    return infoList;
}

Album* Model::findAlbum(QString albumID) const {
    return m_albumsMap[albumID];
}

Image* Model::findImage(QString albumId, QString imageId) const {
    Album* album = m_albumsMap[albumId];
    if (album == NULL) {
        qWarning() << "Unknown album '" << albumId << "' at " << __FUNCTION__;
        return NULL;
    }

    Image* image = album->getImage(imageId);
    if (image == NULL) {
        qWarning() << "Unknown image '" << imageId << "' at " << __FUNCTION__;
        return NULL;
    }

    return image;
}

void Model::dumpShort() const {
    qDebug() << "Albums count = " << m_albumsMap.size();
    qDebug() << "Category count = " << m_categoryMap.size();
    qDebug() << "Subcategory count = " << m_subcategoryMap.size();
}

void Model::dump() const {
    qDebug() << "Albums:";
    for (AlbumMap_t::const_iterator it = m_albumsMap.begin();
            it != m_albumsMap.end(); it++) {
        Album* album = m_albumsMap[it.key()];
        album->dump();
    }
}

void Model::updateImage(QString albumId, QString imageId,
        QString assetPath, bool isThumb, QString filename) {
    Image* image = findImage(albumId, imageId);
    if (image == NULL) {
        return;
    }

    image->setImage(assetPath, isThumb);

    if (isThumb) {
        Album* album = findAlbum(albumId);
        if (album->getThumbImage().isEmpty()) {
            album->setThumbImage(assetPath);
            emit albumThumbImageLoaded(albumId, filename);
        }
    }

    m_localFilenames << filename;

    emit imageLoaded(albumId, imageId,isThumb);
}

void Model::updateCategory(QString id, QString name ) {
    if ( ! m_categoryMap.contains(id) ) {
        QSharedPointer<Category> category = QSharedPointer<Category>(new Category(id,name));
        m_categoryMap.insert(id, category);
    }
}

void Model::updateSubcategory(QString id, QString name ) {
    if ( ! m_subcategoryMap.contains(id) ) {
        QSharedPointer<Category> category = QSharedPointer<Category>(new Category(id,name));
        m_subcategoryMap.insert(id, category);
    }
}

void Model::updateAlbum(QString id, QString key, QString title, QString categoryId, QString subcategoryId) {
    QSharedPointer<Category> category = categoryId.isEmpty() ?  m_emptyCategory : m_categoryMap[categoryId];
    QSharedPointer<Category> subcategory = subcategoryId.isEmpty() ? m_emptyCategory : m_subcategoryMap[subcategoryId];
    Album*  album =	new Album(id, key, title, category, subcategory, this);
    m_albumsMap.insert(id, album);
}

void Model::updateImageInfo(QString albumId, QString imageId, ImageInfo info) {

    Album* album = findAlbum(albumId);
    if (! album ) {
        return;
    }

    Image* image = new Image(imageId, info.key, info.caption, info.keywords,
                             info.thumbURL, info.largeURL, info.filename, info.date, album);

    album->appendImage(image);

    // load album thumbs
    if (album->getLoadedImageCount() < 4) {
        loadImage(albumId, imageId, true);
    }
}

void Model::updateImageExtraInfo(QString albumId, QString imageId, ExtraImageInfo info) {
    Image* image = findImage(albumId, imageId);
    if (image != NULL)
        return;

    image->setAperture(info.aperture);
    image->setDateTime(info.dateTime);
    image->setExposureTime(info.exposureTime);
    image->setFocalLength(info.focalLength);
    image->setIso(info.iso);

    emit extraImageInfoLoaded(albumId, imageId);
}

void Model::updateAlbumImageCount(QString albumId, int imageCount) {
    Album* album = findAlbum(albumId);
    if (! album ) {
        return;
    }
    album->setImageCount(imageCount);
}

void Model::loadImage(QString albumID, QString imageID, bool isThumb) {
    Image* image = findImage(albumID, imageID);
    if (image == NULL) {
        qWarning("Image is null");
        return;
    }
    QString url = (isThumb ? image->getThumbURL() : image->getLargeURL());
    emit requestImage(url, albumID, imageID, isThumb, "");
}

void Model::loadAlbum( QString albumId, QString downloadDir) {
    Album* album = findAlbum(albumId);
    if (album == NULL) {
        qWarning() << "Unknown album '" << albumId << "' at " << __FUNCTION__;
        return;
    }

    QDir dir(downloadDir);
    QString dirPath = downloadDir + QDir::separator() + album->getTitle();
    dirPath.replace(" ", "_");
    dir.mkpath(dirPath);

    // check if all images are downloaded
    foreach (Image* image, album->getImages() ) {
        QString fullPath = dirPath + QDir::separator() + image->getFilename();
        fullPath.replace(" ", "_");
        emit requestImage(image->getLargeURL(), albumId, image->getId(), false,
                fullPath);
    }
}

void Model::loadExtraImageInfo(QString albumId, QString imageId) {

    Image* image = findImage(albumId, imageId);
    if (image == NULL)
        return;

    emit requestExtraImageInfo(albumId, image->getId(), image->getKey());
}

void Model::downloadImage(QString albumId,
         QString imageId, QString fileName) {
    Album* album = findAlbum(albumId);

    if (album == NULL) {
        qWarning() << "Unknown album '" << albumId << "' at " << __FUNCTION__;
        return;
    }

    Image* image = findImage(albumId, imageId);
    if (image == NULL) {
        qWarning() << "Unknown image '" << imageId << "' at " << __FUNCTION__;
        return;
    }

    if (fileName.isEmpty()) {
        QDir home(QDir::homePath());
        QString dirPath = QDir::homePath() + SAVE_LOCATION + album->getTitle();
        dirPath.replace(" ", "_");
        home.mkpath(dirPath);

        QString fullPath = dirPath + QString("/") + image->getFilename();
        fullPath.replace(" ", "_");
        fileName = fullPath;
    }


    emit requestImage(image->getLargeURL(), albumId, image->getId(), false,
            fileName);
}

void Model::viewImage(QString albumId, QString imageId) {

    Album* album = findAlbum(albumId);

    if (album == NULL) {
        qWarning() << "Unknown album '" << albumId << "' at " << __FUNCTION__;
        return;
    }

    Image* image = findImage(albumId, imageId);
    if (image == NULL) {
        qWarning() << "Unknown image '" << imageId << "' at " << __FUNCTION__;
        return;
    }

    QDir home(QDir::homePath());
    QString dirPath = QDir::homePath() + SAVE_LOCATION + album->getTitle();
    dirPath.replace(" ", "_");
    home.mkpath(dirPath);

    QString fullPath = dirPath + QString("/") + image->getFilename();
    fullPath.replace(" ", "_");
    QFile file(fullPath);

    if (! file.exists()) {
        downloadImage(albumId, imageId);
    }
}



