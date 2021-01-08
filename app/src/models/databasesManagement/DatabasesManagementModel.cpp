#include <QDebug>
#include <QDateTime>
#include <QCommonStyle>

#include "main.h"
#include "DatabasesManagementModel.h"
#include "models/appConfig/AppConfig.h"

extern AppConfig mytetraConfig;


DatabasesManagementModel::DatabasesManagementModel(QObject *parent) : QAbstractTableModel(parent)
{

}


DatabasesManagementModel::~DatabasesManagementModel()
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

