#include <QDir>
#include <QFile>
#include <QDebug>

#include "main.h"
#include "KnownBasesConfig.h"

#include "models/appConfig/AppConfig.h"
#include "models/appConfig/AppConfigUpdater.h"
#include "libraries/GlobalParameters.h"
#include "libraries/helpers/DebugHelper.h"

extern AppConfig mytetraConfig;
extern GlobalParameters globalParameters;

const QString KnownBasesConfig::m_sectionPrefix="num";
const QStringList KnownBasesConfig::m_availableFields={"dbPath",
                                                     "trashPath",
                                                     "descript"};


// Конструктор объекта настройки БД
KnownBasesConfig::KnownBasesConfig(QObject *pobj)
{
    Q_UNUSED(pobj)

    isInitFlag=false;
}


// Деструктор объекта настройки БД
KnownBasesConfig::~KnownBasesConfig()
{
    if (isInitFlag)
    {
        qDebug() << "Save known bases config file";
        m_conf->sync();
    }
}


void KnownBasesConfig::init(void)
{
    // Создается имя файла конфигурации
    // QString configFileName=globalParameters.getWorkDirectory()+"/"+mytetraConfig.get_tetradir()+"/database.ini";
    QString configFileName=globalParameters.getWorkDirectory()+"/knownbases.ini";

    // Проверяется, есть ли файл конфигурации
    QFile confFile(configFileName);
    if (!confFile.exists())
    {
        // Если файла нет, создается конфигфайл с начальным содержимым
        QSettings tempConf(configFileName, QSettings::IniFormat);

        tempConf.setValue("version", 1);

        tempConf.sync();
    }

    // Создается указатель на объект хранилища конфигурации
    m_conf=new QSettings(configFileName, QSettings::IniFormat, this);

    m_conf->sync();

    isInitFlag=true;
}


bool KnownBasesConfig::isInit(void)
{
    return isInitFlag;
}


QString KnownBasesConfig::getParameter(const QString &name)
{
    QString t=m_conf->value(name).toString();

    if (t.length()==0)
        criticalError("In known bases config not found parameter " + name);

    return t;
}


void KnownBasesConfig::removeSection(const QString &sectionName)
{
    // Переход в нужный раздел
    m_conf->beginGroup(sectionName);

    // Получение списка всех ключей в разделе
    QStringList keys = m_conf->allKeys();

    // Удаление всех ключей
    for (const QString &key : keys) {
        m_conf->remove(key);
    }

    // Выход из раздела
    m_conf->endGroup();
}


void KnownBasesConfig::renumSections()
{
    // Количество секций
    int count = this->getDbCount();


    // Перечень номеров секций, в котором могут быть "дырки" в нумерации
    QList<int> currentSectionNums;

    // Из имен секций извлекаются их номера
    QStringList sectionNames = m_conf->childGroups();
    for (const auto &sectionName : sectionNames)
    {
        if (sectionName.startsWith( m_sectionPrefix ) )
        {
            QString numLine = sectionName.mid( m_sectionPrefix.length() );

            currentSectionNums << numLine.toInt();
        }
    }


    // Перечень номеров секций без дырок в нумерации
    QList<int> correctSectionNums;
    for (int i=0; i<count; ++i)
    {
        correctSectionNums << i;
    }


    // Проверка что в получившихся списках одинаковое кол-во элементов
    if (currentSectionNums.size() != correctSectionNums.size() )
    {
        qWarning() << "Error: Not equal currentSectionNums and correctSectionNums";
        return;
    }


    // Переименование секций
    for (int i=0; i<count; ++i)
    {
        if (currentSectionNums[i] != correctSectionNums[i])
        {
            this->renameSection( m_sectionPrefix+QString::number( currentSectionNums[i] ),
                                 m_sectionPrefix+QString::number( correctSectionNums[i] ) );
        }
    }
}


void KnownBasesConfig::renameSection(const QString &oldSection,
                                     const QString &newSection)
{
    // Открывается старая секция
    m_conf->beginGroup(oldSection);
    QStringList keys = m_conf->childKeys();

    // Читаются все ключи и значения
    QMap<QString, QVariant> keyValueMap;
    for (const QString &key : keys) {
        keyValueMap.insert(key, m_conf->value(key));
    }

    m_conf->endGroup();


    // Данные записываются в новую секцию
    m_conf->beginGroup(newSection);

    for (auto it = keyValueMap.cbegin(); it != keyValueMap.cend(); ++it) {
        m_conf->setValue(it.key(), it.value());
    }

    m_conf->endGroup();


    // Удаляется старая секция
    /*
    m_conf->beginGroup(oldSection);
    for (const QString &key : keys) {
        m_conf->remove(key);
    }
    m_conf->endGroup();
    */
    m_conf->remove(oldSection);
}

/*
int KnownBasesConfig::get_crypt_mode(void)
{
 return conf->value("crypt_mode", 0).toInt();
}


void KnownBasesConfig::set_crypt_mode(int mode)
{
 conf->setValue("crypt_mode", mode);
}
*/


// --------------------
// Номер версии конфига
// --------------------

int KnownBasesConfig::getConfigVersion(void)
{
    if (m_conf->contains("version"))
        return m_conf->value("version").toInt();
    else
        return 0;
}


void KnownBasesConfig::setConfigVersion(int i)
{
    m_conf->setValue("version", i);
}


int KnownBasesConfig::getDbCount()
{
    /*
    for (int i=0; i<KNOWN_BASES_MAX_COUNT; ++i)
    {
        // Если в перебраемом разделе нет обязательных данных
        // Значит либо самого раздела нет, либо данных нет
        // Значит на этом разделе надо закончить обработку
        if( !m_conf->contains(m_sectionPrefix+QString::number(i)+"/"+m_availableFields[0]) )
        {
            return i;
        }
    }
    */

    // Находятся все нумерованные секции
    int i = 0;
    QStringList sectionNames = m_conf->childGroups();
    for (const auto &sectionName : sectionNames)
    {
        if (sectionName.startsWith( m_sectionPrefix ) )
        {
            ++i;
        }
    }

    return i;
}


QString KnownBasesConfig::getDbParameter(const int &num, const QString &name)
{
    if ( !m_availableFields.contains(name) )
    {
        qWarning() << Q_FUNC_INFO << tr("Incorrect parameter name:") << name;
        return "";
    }

    QVariant value=m_conf->value(m_sectionPrefix+QString::number(num)+"/"+name);

    if (value.isValid())
    {
        return value.toString();
    }
    else
    {
        return "";
    }
}


void KnownBasesConfig::setDbParameter(const int &num, const QString &name, const QString &value)
{
    m_conf->setValue(m_sectionPrefix+QString::number(num)+"/"+name, value);
}


bool KnownBasesConfig::isDbParameterExists(const QString &name, const QString &value)
{
    for (int i=0; i<KNOWN_BASES_MAX_COUNT; ++i)
    {
        QVariant currentValue=m_conf->value( m_sectionPrefix+QString::number(i)+"/"+name );

        if ( currentValue.isValid() && currentValue.toString()==value )
        {
            return true;
        }
    }

    return false;
}


int KnownBasesConfig::getDbParameterExistsNum(const QString &name, const QString &value)
{
    for (int i=0; i<KNOWN_BASES_MAX_COUNT; ++i)
    {
        QVariant currentValue=m_conf->value( m_sectionPrefix+QString::number(i)+"/"+name );

        if ( currentValue.isValid() && currentValue.toString()==value )
        {
            return i;
        }
    }

    return -1;
}


void KnownBasesConfig::removeDb(const int &num)
{
    this->removeSection( m_sectionPrefix+QString::number(num) );

    this->renumSections();

    m_conf->sync();
}
