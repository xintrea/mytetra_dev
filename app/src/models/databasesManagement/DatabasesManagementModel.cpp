#include <QDebug>
#include <QDateTime>
#include <QCommonStyle>
#include <QTextCodec>

#include "main.h"
#include "DatabasesManagementModel.h"
#include "models/appConfig/AppConfig.h"
#include "libraries/GlobalParameters.h"

extern AppConfig mytetraConfig;
extern GlobalParameters globalParameters;


DatabasesManagementModel::DatabasesManagementModel(QObject *parent) : QAbstractTableModel(parent)
{
    m_KnownBasesConfig.init();
    this->initData();
}


DatabasesManagementModel::~DatabasesManagementModel()
{

}


void DatabasesManagementModel::initData()
{
    m_TableData.clear();

    this->scanDirectoriesDirect();
    this->scanDirectoriesFromConfig();
    this->scanDirectoriesFromKnownbasesConfig();

    this->selectDirectories(mytetraConfig.get_tetradir(),
                            mytetraConfig.get_trashdir());
}


// Поиск возможных каталогов баз данных напрямую в известных местах
void DatabasesManagementModel::scanDirectoriesDirect()
{
    QList< DatabaseDirInfo > dbDirs;
    DatabaseDirInfo dbDirInfo;
    QString workingPath;

    // Каталог БД относительно бинарника программы
    QFileInfo mainProgramFileInfo( globalParameters.getMainProgramFile() );
    workingPath=mainProgramFileInfo.absolutePath();
    dbDirInfo.dbPath   =workingPath+"/data";
    dbDirInfo.trashPath=workingPath+"/trash";
    dbDirInfo.descript =tr("Knowledge base in application executable binary file directory %1").arg(workingPath);
    dbDirInfo.isInConfigData=false;
    dbDirs << dbDirInfo;

    // Каталог БД в пользовательском каталоге в директории ~/.имяПрограммы
    workingPath=QDir::homePath()+"/."+globalParameters.getApplicationName();
    dbDirInfo.dbPath   =workingPath+"/data";
    dbDirInfo.trashPath=workingPath+"/trash";
    dbDirInfo.descript =tr("Knowledge base in user directory %1").arg(workingPath);
    dbDirInfo.isInConfigData=false;
    dbDirs << dbDirInfo;

    // Каталог БД в пользовательском каталоге в директории ~/.config/имяПрограммы
    workingPath=QDir::homePath()+"/.config/"+globalParameters.getApplicationName();
    dbDirInfo.dbPath   =workingPath+"/data";
    dbDirInfo.trashPath=workingPath+"/trash";
    dbDirInfo.descript =tr("Knowledge base in user directory %1").arg(workingPath);
    dbDirInfo.isInConfigData=false;
    dbDirs << dbDirInfo;

    this->scanDirectories(dbDirs);
}


// Поиск возможных каталогов БД по путям, прописанных в возможных файлах conf.ini
void DatabasesManagementModel::scanDirectoriesFromConfig()
{
    // Получить пары директорияБД/директорияКорзины из возможных файлов conf.ini

    QList< DatabaseDirInfo > dbDirs;
    DatabaseDirInfo dbDirsInfo;
    QString configFileName;

    // Данные из текущего конфига
    dbDirsInfo.dbPath   =mytetraConfig.get_tetradir();
    dbDirsInfo.trashPath=mytetraConfig.get_trashdir();
    dbDirsInfo.descript =tr("Knowledge base from current config file %1").arg(mytetraConfig.getConfigFileName());
    dbDirsInfo.isInConfigData=true;
    dbDirs << dbDirsInfo;

    // Данные из возможного конфига в директории ~/.имяПрограммы
    configFileName=QDir::homePath()+"/."+globalParameters.getApplicationName()+"/conf.ini";
    dbDirsInfo.dbPath   =this->getDirectoriesFromConfigFile( configFileName ).first;
    dbDirsInfo.trashPath=this->getDirectoriesFromConfigFile( configFileName ).second;
    dbDirsInfo.descript =tr("Knowledge base from config file in user directory %1").arg(dbDirsInfo.dbPath);
    dbDirsInfo.isInConfigData=false;
    dbDirs << dbDirsInfo;

    // Данные из возможного конфига в директории ~/.config/имяПрограммы
    configFileName=QDir::homePath()+"/.config/"+globalParameters.getApplicationName()+"/conf.ini";
    dbDirsInfo.dbPath   =this->getDirectoriesFromConfigFile( configFileName ).first;
    dbDirsInfo.trashPath=this->getDirectoriesFromConfigFile( configFileName ).second;
    dbDirsInfo.descript =tr("Knowledge base from config file in user directory %1").arg(dbDirsInfo.dbPath);
    dbDirsInfo.isInConfigData=false;
    dbDirs << dbDirsInfo;

    this->scanDirectories(dbDirs);
}


QPair<QString, QString> DatabasesManagementModel::getDirectoriesFromConfigFile(const QString &path)
{
    QFile confFile(path);

    // Если файл конфига существует
    if (confFile.exists())
    {
        QSettings conf(path, QSettings::IniFormat);
        conf.setIniCodec( QTextCodec::codecForName("UTF-8") );

        if (conf.contains("tetradir") and
            conf.contains("trashdir"))
        {
            return QPair<QString, QString>( conf.value("tetradir").toString(),
                                            conf.value("trashdir").toString() );
        }
    }

    return QPair<QString, QString>("", "");
}


// Поиск возможных каталогов БД по путям, прописанных в файле knownbases.ini
void DatabasesManagementModel::scanDirectoriesFromKnownbasesConfig()
{
    // Получить пары директорияБД/директорияКорзины из файла knownbases.ini в рабочей директории
    int n=m_KnownBasesConfig.getDbCount();

    if (n==0)
    {
        return;
    }

    // Напрямую заполняется mTableData _без_ проверки, является ли
    // перечисленные в knownbases.ini действительно директориями с БД.
    // Это нужно из-за того, что все записи, попадающие в knownbases.ini
    // проходят множественные проверки в момент добавления и не могут
    // содержать некорректные пути.
    // А так же даже если пути некорректны, это могут быть пути
    // к сетевым ресурсам в примонтированном каталоге, которые в данный
    // момент недоступны, но которые надо иметь возможность выбрать
    // когда сетевой ресурс подключен
    for (int i=0; i<n; ++i)
    {
        QString dbPath    = m_KnownBasesConfig.getDbParameter(i, "dbPath");
        QString trashPath = m_KnownBasesConfig.getDbParameter(i, "trashPath");
        QString descript  = m_KnownBasesConfig.getDbParameter(i, "descript");
        if (descript.isEmpty())
        {
            descript=tr(DBMANAGEMENT_DEFAULT_DESCRIPT);
        }

        // Если в списке баз уже есть база с таким же путем к БД и корзине
        // то ее описание заменяется на значение из knownbases.ini
        // а сама строка не добавляется, так как строка с этой базой
        // в списке для вывода на экран уже есть
        /*
        bool isDouble = false;
        for (auto &tableData : mTableData)
        {
            if (tableData[DBMANAGEMENT_COLUMN_DBPATH] == dbPath and
                tableData[DBMANAGEMENT_COLUMN_TRASHPATH] == trashPath )
            {
                tableData[DBMANAGEMENT_COLUMN_DESCRIPT] = descript;
                isDouble = true;
                break;
            }
        }

        if ( !isDouble )
        {
            QStringList tableLine;
            tableLine << "" << dbPath << trashPath << descript;

            mTableData << tableLine;
        }
        */


        // Если в списке баз уже есть база с таким же путем к БД и корзине
        // то такая запись удаляется, а новая запись из knownbases.ini добавляется
        // Это нужно чтобы возле путей были осмысленные названия, заданные пользователем
        // и чтобы последовательность баз, перечисленных в knownbases.ini не менялась
        for (auto it = m_TableData.begin(); it != m_TableData.end();)
        {
            auto tableData = *it;

            if (tableData[DBMANAGEMENT_COLUMN_DBPATH] == dbPath and
                tableData[DBMANAGEMENT_COLUMN_TRASHPATH] == trashPath)
            {
                it = m_TableData.erase(it); // Удаление элемента из перечня баз и сдвиг итератора
            }
            else
                ++it; // Переход к следующему элементу
        }

        // Безусловное добавление считанной строки в перечень баз
        QStringList tableLine;
        tableLine << "" << dbPath << trashPath << descript;
        m_TableData << tableLine;
    }
}


void DatabasesManagementModel::scanDirectories(const QList< DatabaseDirInfo > &dbDirs)
{
    for (auto currentDbDirs : dbDirs)
    {
        QString dbPath=currentDbDirs.dbPath;
        QString trashPath=currentDbDirs.trashPath;
        QString descript=currentDbDirs.descript;
        bool isInConfigData=currentDbDirs.isInConfigData;

        if (dbPath=="" or trashPath=="")
        {
            continue; // Если встречены пустые пути, такие данные добавлять и обрабатывать нельзя
        }

        // Получение абсолютных путей
        QString absoluteDbPath=QDir(dbPath).absolutePath();
        QString absoluteTrashPath=QDir(trashPath).absolutePath();

        // Если директории БД и корзины действительно являются таковыми директориями
        if ( this->isDbDirectory(dbPath) and this->isTrashDirectory(trashPath) )
        {
            // Определение, есть ли текущие директории в списке возможных директорий
            bool isExists=false;
            for (auto& tableDataLine : m_TableData) // Выводимый auto-тип в виде ссылки, чтобы элементы можно было изменять
            {
                // В списке возможных директорий могут быть как относительные, так и абсолютные пути
                // И для корректного сравнения нужны абсолютные
                QString lineAbsoluteDbPath   =QDir( tableDataLine[DBMANAGEMENT_COLUMN_DBPATH] ).absolutePath();
                QString lineAbsoluteTrashPath=QDir( tableDataLine[DBMANAGEMENT_COLUMN_TRASHPATH] ).absolutePath();

                if (lineAbsoluteDbPath==absoluteDbPath and
                   lineAbsoluteTrashPath==absoluteTrashPath )
                {
                    isExists=true;

                    // Пути текущего проверяемого элемента исправляются на пути в явном виде,
                    // если они совпадают с прописанными в рабочем конфиге путями, так как форма записи путей
                    // в конфиге приоритетнее, чем просто абсолютные пути
                    if (isInConfigData)
                    {
                        // Изменяются элементы по ссылке в mTableData
                        tableDataLine[DBMANAGEMENT_COLUMN_DBPATH]=dbPath;
                        tableDataLine[DBMANAGEMENT_COLUMN_TRASHPATH]=trashPath;
                    }
                }
            }

            // Если директорий еще нет в списке возможных директорий
            if ( !isExists )
            {
                QStringList tableLine;

                // Если это путь из текущего рабочего конфига
                if (isInConfigData)
                {
                    // Путь из текущего конфига добавляется как есть, так как он может быть относительным
                    tableLine << "" << dbPath << trashPath << descript;
                }
                else
                {
                    tableLine << "" << absoluteDbPath << absoluteTrashPath << descript;
                }

                m_TableData << tableLine; // Директории добавляются в список
            }
        }
    }
}


void DatabasesManagementModel::clearSelection()
{
    for (auto& tableDataLine : m_TableData)
    {
        tableDataLine[DBMANAGEMENT_COLUMN_ISSELECT]="";
    }
}


// Выставление пометки что директории базы и корзины выбраны в качестве рабочих
void DatabasesManagementModel::selectDirectories(const QString &dbPath,
                                                 const QString &trashPath)
{
    this->clearSelection();

    for (auto& tableDataLine : m_TableData)
    {
        if (tableDataLine[DBMANAGEMENT_COLUMN_DBPATH]==dbPath and
           tableDataLine[DBMANAGEMENT_COLUMN_TRASHPATH]==trashPath )
        {
            tableDataLine[DBMANAGEMENT_COLUMN_ISSELECT]=DBMANAGEMENT_LINE_SELECT_FLAG;
            return;
        }
    }
}


// Проверка, что каталог является каталогом с базой данных
bool DatabasesManagementModel::isDbDirectory(const QString &path)
{
    if ( !QFileInfo(path).isDir() ) // Путь должен быть именно каталогом
    {
        return false;
    }

    if ( !QDir(path).exists() ) // Каталог должен существовать
    {
        return false;
    }

    // Формальными признаками каталога с БД являются:
    // - Существование в нем файла mytetra.xml
    // - Существование в нем файла настроек database.ini
    // - Существование в нем подкаталога /base

    if ( !QFileInfo(path+"/mytetra.xml").isFile() )
    {
        return false;
    }

    if ( !QFileInfo(path+"/database.ini").isFile() )
    {
        return false;
    }

    if ( !QFileInfo(path+"/base").isDir() )
    {
        return false;
    }

    return true;
}


// Проверка, что каталог является каталогом корзины
bool DatabasesManagementModel::isTrashDirectory(const QString &path)
{
    // Формальным признаком каталога с корзиной является то,
    // что это каталог и он существует

    if ( !QFileInfo(path).isDir() ) // Путь должен быть именно каталогом
    {
        return false;
    }

    if ( !QDir(path).exists() ) // Каталог должен существовать
    {
        return false;
    }

    return true;
}


// Проверка что БД с указанным путем и путем к корзине прописана в knownbases.ini
bool DatabasesManagementModel::isDbInKnownBasesConfig(const QString &dbPath,
                                                      const QString &trashPath)
{
    int numForDbPath = m_KnownBasesConfig.getDbParameterExistsNum("dbPath", dbPath);
    int numForTrashPath = m_KnownBasesConfig.getDbParameterExistsNum("trashPath", trashPath);

    if ( numForDbPath==-1 or numForTrashPath==-1 or numForDbPath != numForTrashPath)
    {
        return false;
    }

    return true;
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

  return m_TableData.size();
}


QVariant DatabasesManagementModel::data(const QModelIndex& index, int role) const
{
  if (index.isValid() && (role == Qt::DisplayRole || role == Qt::DecorationRole))
    return getCell(index.row(), index.column(), role);

  return QVariant();
}


QString DatabasesManagementModel::getCellValue(const int &row,
                                               const int &column,
                                               const int &role) const
{
    return this->getCell(row, column, role).toString();
}


// Получение значения ячейки, защищенный метод
QVariant DatabasesManagementModel::getCell(int row, int column, int role) const
{
    Q_UNUSED(row)

    switch (column) {
    case DBMANAGEMENT_COLUMN_ISSELECT:

        // Вывод текста
        // Теперь вывода текста нет, только иконка
        /*
        if(role==Qt::DisplayRole)
        {
            if(mTableData[row][DBMANAGEMENT_COLUMN_ISSELECT]==DBMANAGEMENT_LINE_SELECT_FLAG)
            {
                return QVariant( tr("Selected") );
            }
            else
            {
                return QVariant( QString() );
            }
        }
        */

        if (role == Qt::UserRole)
        {
            if (m_TableData[row][DBMANAGEMENT_COLUMN_ISSELECT]==DBMANAGEMENT_LINE_SELECT_FLAG)
            {
                return QVariant( true );
            }
            else
            {
                return QVariant( false );
            }
        }

        // Вывод иконок
        if (role==Qt::DecorationRole)
        {
            if (m_TableData[row][DBMANAGEMENT_COLUMN_ISSELECT]==DBMANAGEMENT_LINE_SELECT_FLAG)
            {
                return QCommonStyle().standardIcon(QStyle::SP_DialogApplyButton);
            }
        }

        break;

    case DBMANAGEMENT_COLUMN_DBPATH:

        if (role == Qt::DisplayRole or
            role == Qt::UserRole)
        {
            return QVariant( m_TableData[row][DBMANAGEMENT_COLUMN_DBPATH] );
        }

        break;

    case DBMANAGEMENT_COLUMN_TRASHPATH:

        if (role == Qt::DisplayRole or
            role == Qt::UserRole )
        {
            return QVariant( m_TableData[row][DBMANAGEMENT_COLUMN_TRASHPATH] );
        }

        break;

    case DBMANAGEMENT_COLUMN_DESCRIPT:

        if (role == Qt::DisplayRole or
            role == Qt::UserRole)
        {
            return QVariant( m_TableData[row][DBMANAGEMENT_COLUMN_DESCRIPT] );
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
        case DBMANAGEMENT_COLUMN_ISSELECT:
            return QVariant(tr("Sel."));

        case DBMANAGEMENT_COLUMN_DBPATH:
            return QVariant(tr("Database path"));

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
    for (auto& tableDataLine : m_TableData)
    {
        if ( QDir( tableDataLine[DBMANAGEMENT_COLUMN_DBPATH] ).absolutePath() == QDir( path ).absolutePath() )
        {
            return true;
        }
    }

    return false;
}

void DatabasesManagementModel::addDatabaseByUser(const QString &dbPath, const QString &trashPath)
{
    QStringList line;
    line << "" << dbPath << trashPath << tr(DBMANAGEMENT_DEFAULT_DESCRIPT);

    this->beginResetModel();
    m_TableData << line;
    this->endResetModel();

    // Если такая база уже есть в конфиге баз данных
    if ( m_KnownBasesConfig.isDbParameterExists("dbPath", dbPath) )
    {
        int n=m_KnownBasesConfig.getDbParameterExistsNum("dbPath", dbPath);

        // Параметр dbPath стоит такой, какой надо,
        // поэтому устанавливается только trashPath
        m_KnownBasesConfig.setDbParameter(n, "trashPath", trashPath);
    }
    else // Иначе такой базы данных нет и она добавляется
    {
        int n=m_KnownBasesConfig.getDbCount();

        m_KnownBasesConfig.setDbParameter(n, "dbPath", dbPath);
        m_KnownBasesConfig.setDbParameter(n, "trashPath", trashPath);
    }
}

void DatabasesManagementModel::deleteDatabaseByUser(const QString &dbPath, const QString &trashPath)
{
    // Номер БД в модели
    int modelDbNum = 0;
    bool isFound = false;
    for (const auto &rowData : m_TableData)
    {
        if (rowData[DBMANAGEMENT_COLUMN_DBPATH] == dbPath and
            rowData[DBMANAGEMENT_COLUMN_TRASHPATH] == trashPath )
        {
            isFound = true;
            break;
        }

        ++modelDbNum;
    }

    if ( !isFound )
    {
        return;
    }


    // Номер БД в конфиг-файле известных баз данных
    int dbNum   = m_KnownBasesConfig.getDbParameterExistsNum("dbPath", dbPath);
    int pathNum = m_KnownBasesConfig.getDbParameterExistsNum("trashPath", trashPath);

    if (dbNum == -1 or pathNum == -1 or dbNum!=pathNum)
    {
        return;
    }


    // Удаление в модели
    this->beginResetModel();
    m_TableData.removeAt(modelDbNum);
    this->endResetModel();

    // Удаление в файле
    m_KnownBasesConfig.removeDb(dbNum);
}


void DatabasesManagementModel::selectDatabase(const int &row)
{
    this->clearSelection();

    QStringList line = m_TableData[row];
    line[DBMANAGEMENT_COLUMN_ISSELECT] = DBMANAGEMENT_LINE_SELECT_FLAG;

    this->beginResetModel();
    m_TableData[row] = line;
    this->endResetModel();
}


// Редактирование описания
bool DatabasesManagementModel::editDatabaseDescript(const int &row,
                                                    const QString &descript)
{
    QString dbPath = this->getCellValue(row, DBMANAGEMENT_COLUMN_DBPATH);
    QString trashPath = this->getCellValue(row, DBMANAGEMENT_COLUMN_TRASHPATH);

    if ( !this->isDbInKnownBasesConfig(dbPath, trashPath) )
    {
        return false;
    }

    // Изменение на экране
    QStringList line = m_TableData[row];
    line[DBMANAGEMENT_COLUMN_DESCRIPT] = descript;

    this->beginResetModel();
    m_TableData[row] = line;
    this->endResetModel();


    // Изменение в файле knownbases.ini
    int numForDbPath = m_KnownBasesConfig.getDbParameterExistsNum("dbPath", dbPath);
    int numForTrashPath = m_KnownBasesConfig.getDbParameterExistsNum("trashPath", trashPath);

    if ( numForDbPath==-1 or numForTrashPath==-1 or numForDbPath != numForTrashPath )
    {
        return false;
    }

    m_KnownBasesConfig.setDbParameter(numForDbPath, "descript", descript);

    return true;
}



