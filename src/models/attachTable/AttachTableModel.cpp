#include "AttachTableModel.h"


AttachTableModel::AttachTableModel(QObject *parent) : QAbstractTableModel(parent)
{
  attachList = new QList<Attach>();
}


AttachTableModel::~AttachTableModel()
{
  delete attachList;
}


int AttachTableModel::columnCount(const QModelIndex & parent) const
{
  Q_UNUSED(parent);

  return 2; // Имя файла и размер
}


int AttachTableModel::rowCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent);

  return attachList->size();
}


QVariant AttachTableModel::data(const QModelIndex& index, int role) const
{
  if (index.isValid() && role == Qt::DisplayRole)
    return getData(index.row(), index.column());

  return QVariant();
}


QVariant AttachTableModel::getData(int num, int position) const
{
  switch (position) {
    case 0:
      return QVariant(attachList->at(num).getFileName());
    case 1:
      return QVariant(attachList->at(num).getFileSize());
    default:
      return QVariant();
  }
}


QVariant AttachTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role != Qt::DisplayRole)
    return QVariant();
  if (orientation == Qt::Vertical)
    return QVariant(section + 1);
  else
    switch (section) {
      case 0:
        return QVariant(tr("File name"));
      case 1:
        return QVariant(tr("File size"));

      default:
        return QVariant();
    }
}


void AttachTableModel::addAttach(Attach& newAttach)
{
   beginInsertRows(QModelIndex(), attachList->size(), attachList->size());
   attachList->append(newAttach);
   endInsertRows();
}


void AttachTableModel::delAttach(const QModelIndex & index) {
   beginRemoveRows(QModelIndex(), index.row(), index.row());
   attachList->removeAt(index.row());
   endRemoveRows();
}


Attach& AttachTableModel::getAttach(const QModelIndex & index) const
{
  return (*attachList)[index.row()];
}
