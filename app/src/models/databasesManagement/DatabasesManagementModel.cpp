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
    mKnownBasesConfig.init();
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

    this->selectDirectories(mytetraConfig.get_tetradir(), mytetraConfig.get_trashdir());
}


// Поиск возможных каталогов баз данных напрямую в известных местах
void DatabasesManagementModel::scanDirectoriesDirect()
{
    QList< DatabasesDirsInfo > dbDirs;
    DatabasesDirsInfo dbDirsInfo;
    QString workingPath;

    // Каталог БД относительно бинарника программы
    QFileInfo mainProgramFileInfo( globalParameters.getMainProgramFile() );
    workingPath=mainProgramFileInfo.absolutePath();
    dbDirsInfo.dbPath   =workingPath+"/data";
    dbDirsInfo.trashPath=workingPath+"/trash";
    dbDirsInfo.descript =tr("Knowledge base in executable binary file directory %1").arg(workingPath);
    dbDirsInfo.isCurrentConfigPath=false;
    dbDirs << dbDirsInfo;

    // Каталог БД в пользовательском каталоге в директории ~/.имяПрограммы
    workingPath=QDir::homePath()+"/."+globalParameters.getApplicationName();
    dbDirsInfo.dbPath   =workingPath+"/data";
    dbDirsInfo.trashPath=workingPath+"/trash";
    dbDirsInfo.descript =tr("Knowledge base in user directory %1").arg(workingPath);
    dbDirsInfo.isCurrentConfigPath=false;
    dbDirs << dbDirsInfo;

    // Каталог БД в пользовательском каталоге в директории ~/.config/имяПрограммы
    workingPath=QDir::homePath()+"/.config/"+globalParameters.getApplicationName();
    dbDirsInfo.dbPath   =workingPath+"/data";
    dbDirsInfo.trashPath=workingPath+"/trash";
    dbDirsInfo.descript =tr("Knowledge base in user directory %1").arg(workingPath);
    dbDirsInfo.isCurrentConfigPath=false;
    dbDirs << dbDirsInfo;

    this->scanDirectories(dbDirs);
}


void DatabasesManagementModel::scanDirectoriesFromConfig()
{
    // Получить пары директорияБД/директорияКорзины из возможных файлов conf.ini

    QList< DatabasesDirsInfo > dbDirs;
    DatabasesDirsInfo dbDirsInfo;
    QString configFileName;

    // Данные из текущего конфига
    dbDirsInfo.dbPath   =mytetraConfig.get_tetradir();
    dbDirsInfo.trashPath=mytetraConfig.get_trashdir();
    dbDirsInfo.descript =tr("Knowledge base from current config file %1").arg(mytetraConfig.getConfigFileName());
    dbDirsInfo.isCurrentConfigPath=true;
    dbDirs << dbDirsInfo;

    // Данные из возможного конфига в директории ~/.имяПрограммы
    configFileName=QDir::homePath()+"/."+globalParameters.getApplicationName()+"/conf.ini";
    dbDirsInfo.dbPath   =this->getDirectoriesFromConfigFile( configFileName ).first;
    dbDirsInfo.trashPath=this->getDirectoriesFromConfigFile( configFileName ).second;
    dbDirsInfo.descript =tr("Knowledge base from config file in user directory %1").arg(dbDirsInfo.dbPath);
    dbDirsInfo.isCurrentConfigPath=false;
    dbDirs << dbDirsInfo;

    // Данные из возможного конфига в директории ~/.config/имяПрограммы
    configFileName=QDir::homePath()+"/.config/"+globalParameters.getApplicationName()+"/conf.ini";
    dbDirsInfo.dbPath   =this->getDirectoriesFromConfigFile( configFileName ).first;
    dbDirsInfo.trashPath=this->getDirectoriesFromConfigFile( configFileName ).second;
    dbDirsInfo.descript =tr("Knowledge base from config file in user directory %1").arg(dbDirsInfo.dbPath);
    dbDirsInfo.isCurrentConfigPath=false;
    dbDirs << dbDirsInfo;

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


void DatabasesManagementModel::scanDirectories(const QList< DatabasesDirsInfo > &dbDirs)
{
    for(auto currentDbDirs : dbDirs)
    {
        QString dbPath=currentDbDirs.dbPath;
        QString trashPath=currentDbDirs.trashPath;
        QString descript=currentDbDirs.descript;
        bool isCurrentConfigPath=currentDbDirs.isCurrentConfigPath;

        if(dbPath=="" or trashPath=="")
        {
            continue; // Если встречены пустые пути, такие данные добавлять и обрабатывать нельзя
        }

        // Получение абсолютных путей
        QString absoluteDbPath=QDir(dbPath).absolutePath();
        QString absoluteTrashPath=QDir(trashPath).absolutePath();

        // Если директории БД и корзины действительно являются таковыми директориями
        if( this->isDbDirectory(dbPath) and this->isTrashDirectory(trashPath) )
        {
            // Проверка что таких директорий еще нет в списке возможных директорий
            bool isExists=false;
            for(auto& tableDataLine : mTableData) // Выводимый auto-тип в виде ссылки, чтобы элементы можно было изменять
            {
                // В списке возможных директорий могут быть как относительные, так и абсолютные пути
                // И для корректного сравнения нужны абсолютные
                QString lineAbsoluteDbPath   =QDir( tableDataLine[DBMANAGEMENT_COLUMN_DBPATH] ).absolutePath();
                QString lineAbsoluteTrashPath=QDir( tableDataLine[DBMANAGEMENT_COLUMN_TRASHPATH] ).absolutePath();

                if(lineAbsoluteDbPath==absoluteDbPath and
                   lineAbsoluteTrashPath==absoluteTrashPath )
                {
                    isExists=true;

                    // Пути текущего проверяемого элемента исправляются на пути в явном виде,
                    // если они совпадают с прописанными в конфиге путями, так как форма записи путей
                    // в конфиге приоритетнее, чем просто абсолютные пути
                    if(isCurrentConfigPath)
                    {
                        tableDataLine[DBMANAGEMENT_COLUMN_DBPATH]=dbPath;
                        tableDataLine[DBMANAGEMENT_COLUMN_TRASHPATH]=trashPath;
                    }
                }
            }

            if( !isExists)
            {
                QStringList tableLine;

                if(isCurrentConfigPath)
                {
                    // Путь из текущего конфига добавляется как есть, так как он может быть относительным
                    tableLine << "" << dbPath << trashPath << descript;
                }
                else
                {
                    tableLine << "" << absoluteDbPath << absoluteTrashPath << descript;
                }

                mTableData << tableLine; // Директории добавляются в список
            }
        }
    }
}


// Выставление пометки что директории базы и корзины выбраны в качестве рабочих
void DatabasesManagementModel::selectDirectories(const QString &dbPath, const QString &trashPath)
{
    for(auto& tableDataLine : mTableData)
    {
        if(tableDataLine[DBMANAGEMENT_COLUMN_DBPATH]==dbPath and
           tableDataLine[DBMANAGEMENT_COLUMN_TRASHPATH]==trashPath )
        {
            tableDataLine[DBMANAGEMENT_COLUMN_SELECT]=DBMANAGEMENT_LINE_SELECT_FLAG;
            return;
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

    case DBMANAGEMENT_COLUMN_DESCRIPT:

        if(role==Qt::DisplayRole)
        {
            return QVariant( mTableData[row][DBMANAGEMENT_COLUMN_DESCRIPT] );
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

        case DBMANAGEMENT_COLUMN_DESCRIPT:
            return QVariant(tr("Description"));

        default:
            return QVariant();
        }
}


bool DatabasesManagementModel::isDbPathExists(const QString &path)
{
    for(auto& tableDataLine : mTableData)
    {
        if( QDir( tableDataLine[DBMANAGEMENT_COLUMN_DBPATH] ).absolutePath() == QDir( path ).absolutePath() )
        {
            return true;
        }
    }

    return false;
}

void DatabasesManagementModel::addDatabaseByUser(const QString &dbPath, const QString &trashPath)
{
    QStringList line;
    line << "" << dbPath << trashPath << tr("Custom database directory");

    this->beginResetModel();
    mTableData << line;
    this->endResetModel();

    // todo: Добавить сюда запоминание в mKnownBasesConfig
}

