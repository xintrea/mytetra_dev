#ifndef KNOWNBASESCONFIG_H
#define KNOWNBASESCONFIG_H

#include <QObject>
#include <QSettings>
#include <QString>

// Объект для работы со списком известных баз данных
// Список хранится в файле knownbases.ini

#define KNOWN_BASES_MAX_COUNT 1000

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


private:

    QSettings *conf;
    QString getParameter(QString name);

    bool isInitFlag;
};

#endif // KNOWNBASESCONFIG_H
