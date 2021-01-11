#ifndef DATABASESMANAGEMENTMODEL_H
#define DATABASESMANAGEMENTMODEL_H

#include <QObject>
#include <QAbstractTableModel>
#include <QVariant>
#include <QPair>

#define DBMANAGEMENT_COLUMNS 4
#define DBMANAGEMENT_COLUMN_SELECT 0
#define DBMANAGEMENT_COLUMN_DBPATH 1
#define DBMANAGEMENT_COLUMN_TRASHPATH 2
#define DBMANAGEMENT_COLUMN_DESCRIPT 3

#define DBMANAGEMENT_LINE_SELECT_FLAG "1"


class DatabasesManagementModel : public QAbstractTableModel
{
  Q_OBJECT

  struct DatabasesDirsInfo
  {
      QString dbPath;
      QString trashPath;
      QString descript;
  };

public:
  DatabasesManagementModel(QObject *parent);
  virtual ~DatabasesManagementModel();

  int columnCount(const QModelIndex & parent = QModelIndex()) const;
  int rowCount(const QModelIndex & parent = QModelIndex()) const;
  QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
  QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

private:

  QVariant getCell(int row, int column, int role) const;

  void initData();

  void scanDirectoriesDirect();
  void scanDirectoriesFromConfig();
  void scanDirectoriesFromKnownbasesConfig();

  void scanDirectories(const QList< DatabasesDirsInfo > &dbDirs);

  bool isDbDirectory(const QString &path);
  bool isTrashDirectory(const QString &path);

  QPair<QString, QString> getDirectoriesFromConfigFile(const QString &path);

  // На каждой строке хранится строковый набор данных
  // Столбцы содержат информацию согласно определениям DBMANAGEMENT_COLUMN_*
  // Столбец 0 - признак выбора строки, если содержит DBMANAGEMENT_LINE_SELECT_FLAG
  //             значит надо рисовать флажок выбора
  // Столбец 1 - путь к БД
  // Столбец 2 - путь к корзине
  QList< QStringList > mTableData;

};

#endif // DATABASESMANAGEMENTMODEL_H
