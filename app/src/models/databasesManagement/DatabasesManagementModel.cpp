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
    // Получить пары директоряБД/директорияКорзины из возможных файлов conf.ini

}


void DatabasesManagementModel::scanDirectoriesFromKnownbasesConfig()
{
    // Получить пары директоряБД/директорияКорзины из файла knownbases.ini в рабочей директории

}


void DatabasesManagementModel::scanDirectories(const QList< QPair<QString, QString> > &dbDirs)
{

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

  return 0;
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
            return QVariant( QString() );
        }

        break;

    case DBMANAGEMENT_COLUMN_DBPATH:
        if(role==Qt::DisplayRole)
            return QVariant( QString() );

        // Вывод иконок
        if(role==Qt::DecorationRole)
        {
            return QCommonStyle().standardIcon(QStyle::SP_MessageBoxWarning);
        }

        break;

    case DBMANAGEMENT_COLUMN_TRASHPATH:

        if(role==Qt::DisplayRole)
        {
            return QVariant( QString() );
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

