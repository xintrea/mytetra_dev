#ifndef KNOWNBASESCONFIG_H
#define KNOWNBASESCONFIG_H

#include <QObject>
#include <QSettings>
#include <QString>

// Объект для работы со списком известных баз данных
// Список хранится в файле knownbases.ini

#define KNOWN_BASES_MAX_COUNT 256

class KnownBasesConfig : public QObject
{
    Q_OBJECT

public:
    KnownBasesConfig(QObject *pobj=nullptr);
    ~KnownBasesConfig();

    void init(void);
    bool isInit(void);

    // Версия формата конфигфайла
    int getConfigVersion(void);
    void setConfigVersion(int i);

    // int get_crypt_mode(void);
    // void set_crypt_mode(int mode);

    int getDbCount();
    QString getDbParameter(const int &num, const QString &name);
    void setDbParameter(const int &num, const QString &name, const QString &value);
    bool isDbParameterExists(const QString &name, const QString &value);
    int getExistsParameterNum(const QString &name, const QString &value);

private:

    static const QString sectionPrefix;
    static const QStringList fieldList;

    QSettings *conf;
    QString getParameter(QString name);

    bool isInitFlag;
};

#endif // KNOWNBASESCONFIG_H
