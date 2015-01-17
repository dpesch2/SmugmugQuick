#ifndef MODEL_H
#define MODEL_H

#include <QtCore/QObject>
#include <QtCore/QMap>
#include <QtCore/QStringList>
#include <QtCore/QSharedPointer>

#include "image.h"

class Album;
class Category;

class Model : public QObject
{
    Q_OBJECT

public:
    Model(QObject* parent = NULL);
    ~Model();

    Q_PROPERTY(QList<QObject*> albums READ getAlbums NOTIFY albumsLoaded);
    Q_PROPERTY(QString username READ getUsername WRITE setUsername NOTIFY usernameChanged);

// script interface
    Q_INVOKABLE QList<QObject*> getAlbums() const;
    Q_INVOKABLE QObject* getAlbum(QString albumID) const;
    Q_INVOKABLE QList<QObject*> getImages(QString albumID) const;
    Q_INVOKABLE QObject* getImage(QString albumID, QString imageID ) const;
    Q_INVOKABLE QList<QObject*> searchAlbums( QString re) const;
    Q_INVOKABLE QList<QObject*> searchImages( QString re) const;
    Q_INVOKABLE void dump() const;
    Q_INVOKABLE void loadImage(QString albumID, QString imageID, bool isThumb );
    Q_INVOKABLE void loadExtraImageInfo(QString albumID, QString imageID );
    Q_INVOKABLE void loadAlbum( QString albumID, QString dirName);
    Q_INVOKABLE void downloadImage( QString albumId, QString imageId, QString fileName = "");
    Q_INVOKABLE void viewImage( QString albumId, QString imageId);

    QString getUsername() const;
    void setUsername(QString username);

    Album* findAlbum(QString albumID) const;
    Image* findImage(QString albumId, QString imageId ) const;
    void dumpShort() const;

    Q_SIGNAL void usernameChanged();
    Q_SIGNAL void albumsLoaded();
    Q_SIGNAL void albumLoaded(QString id);
    Q_SIGNAL void imageLoaded( QString albumId, QString imageId, bool isThumb);
    Q_SIGNAL void albumThumbImageLoaded(QString albumId, QString filename);
    Q_SIGNAL void albumImageCountLoaded(QString albumId, int imageCount);
    Q_SIGNAL void extraImageInfoLoaded( QString albumId, QString imageId);
    Q_SIGNAL void imageSavedToShared(QString fullPath);
    Q_SIGNAL void error(QString errorMsg);
    Q_SIGNAL void requestImage(QString jpegUrl, QString albumID, QString imageID, bool isThumb, QString imageFullPath);
    Q_SIGNAL void requestExtraImageInfo(QString albumId, QString imageId, QString imageKey);

    Q_SLOT void updateImage(QString albumId, QString imageId, QString assetPath, bool isThumb, QString filename);
    Q_SLOT void updateCategory(QString categoryId, QString name );
    Q_SLOT void updateSubcategory(QString categoryId, QString name );
    Q_SLOT void updateAlbum(QString id, QString key, QString title, QString categoryId, QString subcategoryId);
    Q_SLOT void updateImageInfo(QString albumId, QString imageId, ImageInfo info);
    Q_SLOT void updateImageExtraInfo(QString albumId, QString imageId, ExtraImageInfo info);
    Q_SLOT void updateAlbumImageCount(QString albumId, int imageCount);

private:

    //session
    QString m_username;

    //caches
    typedef QMap<QString, Album* >  AlbumMap_t;
    AlbumMap_t m_albumsMap;

    typedef QMap<QString, QSharedPointer<Category> >  CategoryMap_t;
    CategoryMap_t m_categoryMap;
    CategoryMap_t m_subcategoryMap;

    QSharedPointer<Category> m_emptyCategory; // empty category with name ""

    // downloaded files
    QStringList m_localFilenames;

    void clear();
    void deleteLocalFiles();

};

#endif // MODEL_H

