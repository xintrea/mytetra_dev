#include <QSettings>
#include <QTextCodec>
#include <QFile>

#include "AppConfigUpdater.h"

#include "libraries/helpers/DebugHelper.h"

#define APPCONFIGUPDATER_VERSION "APPCONFIGUPDATER v.1.0 Build 29.10.2010"


AppConfigUpdater::AppConfigUpdater(QObject *pobj) : maxParameterCount(100)
{
    Q_UNUSED(pobj);

    conf=NULL;
}


AppConfigUpdater::~AppConfigUpdater()
{
    delete conf;
}


void AppConfigUpdater::setConfigFile(QString fileName)
{
    // Проверяется, есть ли файл конфигурации
    QFile conffile(fileName);
    if(!conffile.exists())
        criticalError("appconfigupdater::set_config_file() - File "+fileName+" not found.");

    // Создается рабочий объект для работы с конфигурацией
    conf=new QSettings(fileName, QSettings::IniFormat, this);
    conf->setIniCodec( QTextCodec::codecForName("UTF-8") );
    conf->setPath(QSettings::IniFormat, QSettings::UserScope,"./");
    conf->setPath(QSettings::IniFormat, QSettings::SystemScope,"./");
}


// Обновление значения в случае,
// если значение есть в конфиге, но его представление нужно изменить
QString AppConfigUpdater::updateValueRepresentation(int versionFrom,
                                                    int versionTo,
                                                    QString name,
                                                    QString value)
{
    if(versionFrom==37 and versionTo==38 and name=="dockableWindowsState")
    {
        // В параметре dockableWindowsState теперь хранится
        // и позиция вертикальной прокрутки для каждого отделяемого окна

        if(value.trimmed().size()>1)
        {
            QStringList chunks=value.split(';');

            for(int i=0; i<chunks.size(); ++i)
            {
                chunks[i]+=",0";
            }

            value=chunks.join(';');

            return value;
        }
    }

    if(versionFrom==39 and versionTo==40 and name=="theme")
    {
        // В параметре theme теперь допустимы значения default и dark, а
        // темы light теперь нет

        if(value.trimmed()=="light")
        {
            return "default";
        }
    }

    return value;
}


// Метод разрешения конфликтов
// Должен включать в себя логику обработки только тех параметров
// и только для тех версий конфигов, которые действительно
// должны поменять тип. Если для принятых параметров не будет
// обработки, это значит что что-то сделано программистом не так
// и нужно дорабатывать код
QString AppConfigUpdater::updateVersionAllowCollision(int versionFrom,
                                                      int versionTo,
                                                      QString name,
                                                      QString fromType,
                                                      QString fromValue,
                                                      QString toType,
                                                      QString toValue)
{

    // todo: Прописать сюда правила перевода int в bool и обратно

    criticalError("Error while update config version \nFrom: "+(QString::number(versionFrom))+
                  "\nTo: "+(QString::number(versionTo))+
                  "\nName: "+name+
                  "\nFrom type: "+fromType+
                  "\nFrom value: "+fromValue+
                  "\nTo type: "+toType+
                  "\nTo value: "+toValue);

    return QString();
}


// Основной метод обновления версий конфига
// baseTable - Структура таблицы, которая есть в данный момент
// finalTable - Структура таблицы, на которую надо обновиться
void AppConfigUpdater::updateVersion(int versionFrom,
                                     int versionTo,
                                     QStringList baseTable,
                                     QStringList finalTable)
{
    // Структура таблица исходных параметров преобразуется к более удобному для работы виду
    QMap< QString, QMap< QString, QString > > fromTable;
    for(int i=0; i<maxParameterCount; i++)
    {
        QString name=    baseTable.at(i*3+0);
        QString type=    baseTable.at(i*3+1);
        QString defValue=baseTable.at(i*3+2);

        // Если достигнут конец массива
        if(name=="0" && type=="0" && defValue=="0")
            break;

        // Подготовка массива для текущего параметра
        QMap< QString, QString > line;
        line.clear();
        line["type"]=type;
        line["defValue"]=defValue;

        // Для текущего имени параметра запоминается массив его свойств
        fromTable[name]=line;
    }


    // Таблица конечных параметров преобразуется к более удобному для работы виду
    // Параллельно создаётся список контролирующий обработанные параметры
    // Далее из контролирующего списка будут убираться обработанные параметры
    // Контролирующий список нужен для того, чтобы не удалять записи
    // из конечного массива во время обхода через итератор
    QMap< QString, QMap< QString, QString > > toTable;
    QList<QString> controlList;
    for (int i=0; i<MYTETRA_CONFIG_PARAM_NUM; i++)
    {
        QString name=    finalTable.at(i*MYTETRA_CONFIG_PARAM_FIELDS_AT_RECORD+0);
        QString type=    finalTable.at(i*MYTETRA_CONFIG_PARAM_FIELDS_AT_RECORD+1);
        QString defValue=finalTable.at(i*MYTETRA_CONFIG_PARAM_FIELDS_AT_RECORD+2);

        // Если достигнут конец массива
        if (name=="0" && type=="0" && defValue=="0")
        {
            break;
        }

        // Подготовка массива для текущего параметра
        QMap< QString, QString > line;
        line.clear();
        line["type"]=type;
        line["defValue"]=defValue; // Дефолтное значение

        // Для текущего имени параметра запоминается массив его свойств
        toTable[name]=line;

        // Имя заносится в контролирующий список
        controlList << name;
    }

    qDebug() << "From table";
    qDebug() << fromTable;
    qDebug() << "To table";
    qDebug() << toTable;

    // Перебирается конечный массив toTable
    QMapIterator< QString, QMap< QString, QString > > i(toTable);
    while(i.hasNext())
    {
        i.next();

        // Данные для новой версии конфига
        QString toName=i.key();
        QMap< QString, QString > line=i.value();
        QString toType=line["type"];
        QString toDefValue=line["defValue"];

        qDebug() << "To name: " << toName;
        qDebug() << "To type: " << toType;
        qDebug() << "To default value: " << toDefValue;


        // Определяется, есть ли полный аналог параметра в предыдущей версии конфига
        BeforeParamFlag beforeParamFlag = BeforeParamFlag::BEFORE_PARAM_NON_EXISTS;

        QMap< QString, QString > line2;
        QString fromType;
        QString fromDefValue;

        if (fromTable.contains(toName))
        {
            line2=fromTable[toName];
            fromType=line2["type"];
            fromDefValue=line2["defValue"];

            qDebug() << "Line2: " << line2;
            qDebug() << "From type: " << fromType;
            qDebug() << "From default value: " << fromDefValue;

            if (toType==fromType)
                beforeParamFlag=BeforeParamFlag::BEFORE_PARAM_EXISTS_AND_SAME_TYPE; // Параметр есть, и типы совпадают
            else
                beforeParamFlag=BeforeParamFlag::BEFORE_PARAM_EXISTS_AND_OTHER_TYPE; // Параметр есть, но типы не совпадают
        }


        // Параметра в предыдущей версии конфига не было
        if (beforeParamFlag==BeforeParamFlag::BEFORE_PARAM_NON_EXISTS)
        {
            // Будет просто сохранено новое дефолтное значение
            toTable[toName]["value"] = toTable[toName]["defValue"];

            // Параметр из контролирующего массива исключается
            controlList.removeOne(toName);
        }


        // Параметр в предыдущей версии конфига есть, и типы совпадают
        if (beforeParamFlag==BeforeParamFlag::BEFORE_PARAM_EXISTS_AND_SAME_TYPE)
        {
            // В новой верии конфига будет сохранено предыдущее значение с учетом репрезентации
            // Репрезентация - это когда _тип значения не меняется_, но содержимое значения
            // трактуется как-то по-другому. Например, значение хранилась как строка из двух координат,
            // разделенных запятой. А стало храниться как строка из трех координат, разделенных запятой

            QString existsValue = this->updateValueRepresentation(versionFrom,
                                                                  versionTo,
                                                                  toName,
                                                                  conf->value(toName).toString()); // Значение из конфига

            toTable[toName]["value"] = existsValue;

            // Параметр из контролирующего массива исключается
            controlList.removeOne(toName);
        }


        // Параметр в предыдущей версии конфига есть, но типы не совпадают
        // Пока этот функционал не реализован и внутри updateVersionAllowCollision()
        // будет возникать ошибка
        if (beforeParamFlag==BeforeParamFlag::BEFORE_PARAM_EXISTS_AND_OTHER_TYPE)
        {
            // Будет возвращено высчитанное значение
            toTable[toName]["value"]=updateVersionAllowCollision(versionFrom, versionTo, toName,
                                                                 fromType,
                                                                 conf->value(toName).toString(),
                                                                 toType,
                                                                 toTable[toName]["defValue"]);

            // Параметр из контролирующего массива исключается
            controlList.removeOne(toName);
        }

    } // Закрылся перебор конечного массива


    // Если в контролирующем массиве остались необработанные параметры
    if(controlList.size()>0)
    {
        // Программа завершается
        qDebug() << "Can not compute parameter " << controlList;
        criticalError("Error while update config from "+(QString::number(versionFrom))+" to "+(QString::number(versionTo)) );
    }

    // Конфиг обнуляется
    conf->clear();

    // Конечный массив toTable записывается в конфиг
    QMapIterator< QString, QMap< QString, QString > > j(toTable);
    while(j.hasNext())
    {
        j.next();

        QString toName=j.key();

        QMap< QString, QString > line=j.value();
        QString toType=line["type"];
        QString toValue=line["value"];

        if(toType=="QString")
            conf->setValue(toName,toValue);
        else if(toType=="int")
            conf->setValue(toName,toValue.toInt());
        else if(toType=="bool")
        {
            if(toValue=="true") conf->setValue(toName, true);
            else conf->setValue(toName, false);
        }
    }

    // Устанавливается новый номер версии
    conf->setValue("version", versionTo);

    // Конфигурация записывается на диск
    conf->sync();
}

