#ifndef __APPCONFIGUPDATER_H__
#define	__APPCONFIGUPDATER_H__

#include <QObject>
#include <QSettings>
#include <QDebug>
#include <QString>


#define MYTETRA_CONFIG_PARAM_NUM 100
#define MYTETRA_CONFIG_PARAM_FIELDS_AT_RECORD 3


class AppConfigUpdater : public QObject
{
    Q_OBJECT

public:
    AppConfigUpdater(QObject *pobj=nullptr);
    virtual ~AppConfigUpdater();

    void setConfigFile(QString fileName);

    void updateVersion(int versionFrom,
                        int versionTo,
                        QStringList baseTable,
                        QStringList finalTable);

private:

    const int maxParameterCount;

    QString updateValueRepresentation(int versionFrom,
                                      int versionTo,
                                      QString name,
                                      QString value);

    QString updateVersionAllowCollision(int versionFrom,
                                          int versionTo,
                                          QString name,
                                          QString fromType,
                                          QString fromValue,
                                          QString toType,
                                          QString toValue);

    QSettings *conf;

};

#endif	/* __APPCONFIGUPDATER_H__ */

