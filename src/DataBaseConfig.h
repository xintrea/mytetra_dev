#ifndef __DATABASECONFIG_H__
#define __DATABASECONFIG_H__

#include <QObject>
#include <QSettings>
#include <QString>


#define CRYPT_CHECK_ROUNDS 1000
#define CRYPT_CHECK_HASH_LEN 160


class DataBaseConfig : public QObject
{
    Q_OBJECT

public:
    DataBaseConfig(QObject *pobj=0);
    ~DataBaseConfig();

    void init(void);
    bool is_init(void);

    // Версия формата конфигфайла
    int get_config_version(void);
    void set_config_version(int i);


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
    QString get_parameter(QString name);

    bool is_init_flag;
};

#endif // __DATABASECONFIG_H__
