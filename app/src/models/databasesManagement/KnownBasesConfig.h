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

    //! Получение версии формата конфигфайла
    int getConfigVersion(void);

    //! Установка версии формата конфигфайла
    void setConfigVersion(int i);

    // int get_crypt_mode(void);
    // void set_crypt_mode(int mode);

    //! Получение количества известных баз
    int getDbCount();

    //! Получение заданного параметра для БД с указанным номером
    QString getDbParameter(const int &num, const QString &name);

    //! Установка параметра для БД с указанным номером
    void setDbParameter(const int &num, const QString &name, const QString &value);

    //! Проверка, существует ли для какой-либо БД параметр с заданным значением
    bool isDbParameterExists(const QString &name, const QString &value);

    //! Получение номера БД, для которой существует параметр с заданным значением
    //! Возвращается номер первой найденной БД
    int getDbParameterExistsNum(const QString &name, const QString &value);

    void removeDb(const int &num);

private:

    //! Получение параметра по полному имени в виде строки (с путем к параметру)
    //! с проверкой существования параметра
    QString getParameter(const QString &name);

    //! Удаление секции INI-файла по имени секции
    void removeSection(const QString &sectionName);

    void renameSection(const QString &oldSection,
                       const QString &newSection);

    //! Переномерация секций (используется после удаления)
    void renumSections();


    //! Префикс названия раздела в INI-файле
    static const QString m_sectionPrefix;

    //! Допустимые имена переменных в разделе INI-файла
    static const QStringList m_availableFields;

    //! Объект работы с конфиг-файлом в формате INI
    QSettings *m_conf;

    //! Флаг, показывающий что инициализация данного объекта была успешно произведена
    bool isInitFlag;

};

#endif // KNOWNBASESCONFIG_H
