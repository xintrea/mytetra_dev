#include <QDebug>
#include <QDateTime>
#include <QCommonStyle>

#include "main.h"
#include "DatabasesManagementModel.h"
#include "models/appConfig/AppConfig.h"
#include "libraries/GlobalParameters.h"

extern AppConfig mytetraConfig;
extern GlobalParameters globalParameters;


DatabasesManagementModel::DatabasesManagementModel(QObject *parent) : QAbstractTableModel(parent)
{
    this->initData();
}


DatabasesManagementModel::~DatabasesManagementModel()
{

}


void DatabasesManagementModel::initData()
{
    mTableData.clear();

    this->scanDirectoriesDirect();
    this->scanDirectoriesFromConfig();
    this->scanDirectoriesFromKnownbasesConfig();
}


// Поиск возможных каталогов баз данных напрямую в известных местах
void DatabasesManagementModel::scanDirectoriesDirect()
{
    QList< QPair<QString, QString> > dbDirs;

    QString dbDirName;
    QString dbTrashName;

    // Каталог БД относительно бинарника программы
    QFileInfo mainProgramFileInfo( globalParameters.getMainProgramFile() );
    dbDirName  =mainProgramFileInfo.absolutePath()+"/data";
    dbTrashName=mainProgramFileInfo.absolutePath()+"/trash";
    dbDirs << QPair<QString, QString>(dbDirName, dbTrashName);

    // Каталог БД в пользовательском каталоге в директории ~/.имяПрограммы
    dbDirName  =QDir::homePath()+"/."+globalParameters.getApplicationName()+"/data";
    dbTrashName=QDir::homePath()+"/."+globalParameters.getApplicationName()+"/trash";
    dbDirs << QPair<QString, QString>(dbDirName, dbTrashName);

    // Каталог БД в пользовательском каталоге в директории ~/.config/имяПрограммы
    dbDirName  =QDir::homePath()+"/.config/"+globalParameters.getApplicationName()+"/data";
    dbTrashName=QDir::homePath()+"/.config/"+globalParameters.getApplicationName()+"/trash";
    dbDirs << QPair<QString, QString>(dbDirName, dbTrashName);

    this->scanDirectories(dbDirs);
}


void DatabasesManagementModel::scanDirectoriesFromConfig()
{
    // Получить пары директорияБД/директорияКорзины из возможных файлов conf.ini

    QList< QPair<QString, QString> > dbDirs;
    QString configFileName;

    // Данные из текущего конфига
    dbDirs << QPair<QString, QString>(mytetraConfig.get_tetradir(), mytetraConfig.get_trashdir());

    // Данные из возможного конфига в директории ~/.имяПрограммы
    configFileName=QDir::homePath()+"/."+globalParameters.getApplicationName()+"/conf.ini";
    dbDirs << this->getDirectoriesFromConfigFile( configFileName );

    // Данные из возможного конфига в директории ~/.config/имяПрограммы
    configFileName=QDir::homePath()+"/.config/"+globalParameters.getApplicationName()+"/conf.ini";
    dbDirs << this->getDirectoriesFromConfigFile( configFileName );

    this->scanDirectories(dbDirs);
}


QPair<QString, QString> DatabasesManagementModel::getDirectoriesFromConfigFile(const QString &path)
{
    QFile confFile(path);

    // Если файл конфига существует
    if(confFile.exists())
    {
        QSettings conf(path, QSettings::IniFormat);

        if(conf.contains("tetradir") and
           conf.contains("trashdir"))
        {
            return QPair<QString, QString>( conf.value("tetradir").toString(),
                                            conf.value("trashdir").toString() );
        }
    }

    return QPair<QString, QString>("", "");
}


void DatabasesManagementModel::scanDirectoriesFromKnownbasesConfig()
{
    // Получить пары директорияБД/директорияКорзины из файла knownbases.ini в рабочей директории

}


void DatabasesManagementModel::scanDirectories(const QList< QPair<QString, QString> > &dbDirs)
{
    for(auto currentDbDirs : dbDirs)
    {
        QString dbDirName=currentDbDirs.first;
        QString dbTrashName=currentDbDirs.second;

        if(dbDirName=="" or dbTrashName=="")
        {
            continue; // Если встречены пустые пути, такие данные добавлять и обрабатывать нельзя
        }

        // Пути директорий преобразуются в абсолютные
        dbDirName=QDir(dbDirName).absolutePath();
        dbTrashName=QDir(dbTrashName).absolutePath();

        // Если директории БД и корзины действительно являются таковыми директориями
        if( this->isDbDirectory(dbDirName) and this->isTrashDirectory(dbTrashName) )
        {
            // Проверка что таких директорий еще нет в списке возможных директорий
            bool isExists=false;
            for(auto tableDataLine: mTableData)
            {
                if(tableDataLine[DBMANAGEMENT_COLUMN_DBPATH]==dbDirName and
                   tableDataLine[DBMANAGEMENT_COLUMN_TRASHPATH]==dbTrashName )
                    isExists=true;
            }

            if( !isExists)
            {
                QStringList tableLine;
                tableLine << "" << dbDirName << dbTrashName;

                mTableData << tableLine; // Директории добавляются в список
            }
        }
    }
}


// Проверка, что каталог является каталогом с базой данных
bool DatabasesManagementModel::isDbDirectory(const QString &path)
{
    // Формальными признаками каталога с БД являются:
    // - Существование в нем файла mytetra.xml
    // - Существование в нем файла настроек database.ini
    // - Существование в нем подкаталога /base

    if( !QFileInfo(path+"/mytetra.xml").isFile() )
    {
        return false;
    }

    if( !QFileInfo(path+"/database.ini").isFile() )
    {
        return false;
    }

    if( !QFileInfo(path+"/base").isDir() )
    {
        return false;
    }

    return true;
}


// Проверка, что каталог является каталогом корзины
bool DatabasesManagementModel::isTrashDirectory(const QString &path)
{
    // Формальным признаком каталога с корзиной является то, что он существует
    return QFileInfo(path).isDir();
}


// Число столбцов
int DatabasesManagementModel::columnCount(const QModelIndex & parent) const
{
  Q_UNUSED(parent)

  return DBMANAGEMENT_COLUMNS;
}


// Число строк
int DatabasesManagementModel::rowCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent)

  return mTableData.size();
}


// Получение данных
QVariant DatabasesManagementModel::data(const QModelIndex& index, int role) const
{
  if (index.isValid() && (role == Qt::DisplayRole || role == Qt::DecorationRole))
    return getCell(index.row(), index.column(), role);

  return QVariant();
}


// Получение значения ячейки, защищенный метод
QVariant DatabasesManagementModel::getCell(int row, int column, int role) const
{
    Q_UNUSED(row)

    switch (column) {
    case DBMANAGEMENT_COLUMN_SELECT:

        if(role==Qt::DisplayRole)
        {
            if(mTableData[row][DBMANAGEMENT_COLUMN_SELECT]==DBMANAGEMENT_LINE_SELECT_FLAG)
            {
                return QVariant( tr("Selected") );
            }
            else
            {
                return QVariant( QString() );
            }
        }

        // Вывод иконок
        if(role==Qt::DecorationRole)
        {
            if(mTableData[row][DBMANAGEMENT_COLUMN_SELECT]==DBMANAGEMENT_LINE_SELECT_FLAG)
            {
                return QCommonStyle().standardIcon(QStyle::SP_DialogApplyButton);
            }
        }

        break;

    case DBMANAGEMENT_COLUMN_DBPATH:

        if(role==Qt::DisplayRole)
        {
            return QVariant( mTableData[row][DBMANAGEMENT_COLUMN_DBPATH] );
        }

        break;

    case DBMANAGEMENT_COLUMN_TRASHPATH:

        if(role==Qt::DisplayRole)
        {
            return QVariant( mTableData[row][DBMANAGEMENT_COLUMN_TRASHPATH] );
        }

        break;

    }

    return QVariant();
}


QVariant DatabasesManagementModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Vertical)
        return QVariant(section + 1);
    else
        switch (section) {
        case DBMANAGEMENT_COLUMN_SELECT:
            return QVariant(tr("Select"));

        case DBMANAGEMENT_COLUMN_DBPATH:
            return QVariant(tr("Detabase path"));

        case DBMANAGEMENT_COLUMN_TRASHPATH:
            return QVariant(tr("Trash path"));

        default:
            return QVariant();
        }
}

