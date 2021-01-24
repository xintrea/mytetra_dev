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

    // Флаг, установлен пароль или нет
    // Возможные значения:
    // 0 - в базе нет зашифрованных данных и нет пароля
    // 1 - в базе есть зашифрованные данные и установлен пароль (разобраться с вариантом нет зашифрованных данных но запомнен пароль)
    int get_crypt_mode(void);
    void set_crypt_mode(int mode);

    QString get_crypt_check_salt(void);
    void set_crypt_check_salt(QString salt);

    QString get_crypt_check_hash(void);
    void set_crypt_check_hash(QString hash);

    QString get_middle_hash_check_data(void);
    void set_middle_hash_check_data(QString hash);

private:

    QSettings *conf;
    QString getParameter(QString name);

    bool isInitFlag;
};

#endif // KNOWNBASESCONFIG_H
