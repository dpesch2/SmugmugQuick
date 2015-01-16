#ifndef ALBUM_H
#define ALBUM_H

#include <QMap>
#include <QSharedPointer>
#include <QVariantMap>

#include "category.h"
#include "image.h"


class Album : public QObject
{
	Q_OBJECT

public:
    Album(QString, QString, QString, QSharedPointer<Category>, QSharedPointer<Category>, QObject* parent);
    ~Album();

    Q_PROPERTY(QString id READ getId);
    Q_PROPERTY(QString title READ getTitle);
    Q_PROPERTY(QString category READ getCategoryName);
    Q_PROPERTY(QString subcategory READ getSubcategoryName);

    Q_PROPERTY(QString thumbImage READ getThumbImage NOTIFY thumbImageChanged);
    Q_PROPERTY(int imageCount READ getImageCount NOTIFY imageCountChanged);

    QString getId() const { return m_id; }
    QString getKey() const { return m_key; }
    QString getTitle() const { return m_title; }
    QString getThumbImage() const { return m_thumbImage; }
    QSharedPointer<Category> getCategory() const { return m_category; }
    QString getCategoryName() const { return m_category->getName(); }
    QSharedPointer<Category> getSubcategory() const { return m_subcategory; }
    QString getSubcategoryName() const { return m_subcategory->getName(); }
    int getImageCount() const { return m_imageCount; }
    uint getLoadedImageCount() const { return m_images.size(); }

    Q_SIGNAL void imageCountChanged();
    Q_SIGNAL void thumbImageChanged();

    QList<Image*> getImages() const {
        return m_images;
    }

    void appendImage(Image* image);
    Image* getImage(QString imageID ) const;
    void setImage(QString imageID, const Image& image);
    void setImageCount(uint count) { m_imageCount = count; emit imageCountChanged(); }
    void setThumbImage(QString path) { m_thumbImage = path; emit thumbImageChanged(); }

    void dump() const;

private:
    const QString m_id;
    const QString m_key;
    const QString m_title;
    //these two should be probably shared pointers as long as one album can share same Category and Subcategory as well
    const QSharedPointer<Category> m_category;
    const QSharedPointer<Category> m_subcategory;

    uint m_imageCount;
    QString m_thumbImage;
    QList<Image*> m_images;

    // for quick access by id
    typedef QMap<QString,Image*> ImageMap_t;
    ImageMap_t m_imagesMap;

};
//Q_DECLARE_METATYPE(Album)
#endif // ALBUM_H
