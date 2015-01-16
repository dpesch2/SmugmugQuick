#ifndef IMAGE_H
#define IMAGE_H

#include <QtCore/QObject>
#include "imageinfo.h"


class Image : public QObject
{
	Q_OBJECT

public:
    Image(QString id,
    	  QString key,
    	  QString caption,
    	  QString keywords,
    	  QString thumbURL,
    	  QString largeURL,
    	  QString filename,
    	  QString date,
    	  QObject* parent = NULL);

    Q_PROPERTY(QString id READ getId);
    Q_PROPERTY(QString caption READ getCaption);
    Q_PROPERTY(QString keywords READ getKeywords);
    Q_PROPERTY(QString dateProp READ getDate);
    Q_PROPERTY(QString filename READ getFilename);
    Q_PROPERTY(QString thumbURL READ getThumbURL);
    Q_PROPERTY(QString largeURL READ getLargeURL);

    Q_PROPERTY(QString thumbImage READ getThumbImage NOTIFY thumbImageChanged);
    Q_PROPERTY(QString largeImage READ getLargeImage NOTIFY largeImageChanged);

    Q_PROPERTY(QString aperture READ getAperture WRITE setAperture NOTIFY apertureChanged );
    Q_PROPERTY(QString exposureTime READ getExposureTime WRITE setExposureTime NOTIFY exposureTimeChanged );
    Q_PROPERTY(QString dateTime READ getDateTime WRITE setDateTime NOTIFY dateTimeChanged );
    Q_PROPERTY(QString focalLength READ getFocalLength WRITE setFocalLength NOTIFY focalLengthChanged );
    Q_PROPERTY(QString iso READ getIso WRITE setIso NOTIFY isoChanged );

    QString getId() const {return m_id;}
    QString getKey() const {return m_key;}
    QString getCaption() const {return m_caption;}
    QString getKeywords() const {return m_keywords;}
    QString getFilename() const {return m_filename;}
    QString getDate() const {return m_date;}
    QString getThumbURL() const {return m_thumbURL; }
    QString getLargeURL() const {return m_largeURL; }
    QString getThumbImage() const {return m_thumbImage; }
    QString getLargeImage() const {return m_largeImage; }
    QString getAperture() const { return m_aperture; }
    QString getExposureTime() const { return m_exposureTime; }
    QString getDateTime() const { return m_dateTime; }
    QString getFocalLength() const { return m_focalLength; }
    QString getIso() const { return m_iso; }

    Q_SIGNAL void thumbImageChanged();
    Q_SIGNAL void largeImageChanged();

    Q_SIGNAL void apertureChanged();
    Q_SIGNAL void exposureTimeChanged();
    Q_SIGNAL void dateTimeChanged();
    Q_SIGNAL void focalLengthChanged();
    Q_SIGNAL void isoChanged();

    void setImage(QString fullPath, bool isThumb );
    void setAperture(QString val) { m_aperture = val; emit apertureChanged(); }
    void setExposureTime(QString val) { m_exposureTime = val; emit exposureTimeChanged(); }
    void setDateTime(QString val) { m_dateTime = val; emit dateTimeChanged(); }
    void setFocalLength(QString val) { m_focalLength = val; emit focalLengthChanged(); }
    void setIso(QString val) { m_iso = val; emit isoChanged(); }

    void dump() const;

private:
    const QString m_id;
    const QString m_key;
    const QString m_caption;
    const QString m_keywords;
    const QString m_thumbURL;
    const QString m_largeURL;
    const QString m_filename;
    const QString m_date;

    QString m_thumbImage;
    QString m_largeImage;

    // exif
    QString m_aperture;
    QString m_dateTime;
    QString m_exposureTime;
    QString m_focalLength;
    QString m_iso;
};

#endif // IMAGE_H
