#ifndef DATABASESMANAGEMENTMODEL_H
#define DATABASESMANAGEMENTMODEL_H

#include <QObject>
#include <QAbstractTableModel>
#include <QVariant>
#include <QPair>

#define DBMANAGEMENT_COLUMNS 4
#define DBMANAGEMENT_COLUMN_SELECT    0 // Признак выбора базы
#define DBMANAGEMENT_COLUMN_DBPATH    1 // Путь к БД
#define DBMANAGEMENT_COLUMN_TRASHPATH 2 // Путь к корзине
#define DBMANAGEMENT_COLUMN_DESCRIPT  3 // Человекочитаемое описание базы

#define DBMANAGEMENT_LINE_SELECT_FLAG "1" // Значение, свидетельствующее о выборе базы


class DatabasesManagementModel : public QAbstractTableModel
{
  Q_OBJECT

  struct DatabasesDirsInfo
  {
      QString dbPath;
      QString trashPath;
      QString descript;
      bool isCurrentConfigPath=false;
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

  void selectDirectories(const QString &dbPath, const QString &trashPath);

  bool isDbDirectory(const QString &path);
  bool isTrashDirectory(const QString &path);

  QPair<QString, QString> getDirectoriesFromConfigFile(const QString &path);

  // На каждой строке хранится строковый набор данных
  // Столбцы содержат информацию согласно определениям DBMANAGEMENT_COLUMN_*
  QList< QStringList > mTableData;

};

#endif // DATABASESMANAGEMENTMODEL_H
