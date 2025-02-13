#ifndef DATABASESMANAGEMENTMODEL_H
#define DATABASESMANAGEMENTMODEL_H

#include <QObject>
#include <QAbstractTableModel>
#include <QVariant>
#include <QPair>

#include "KnownBasesConfig.h"

// Модель для работы с возможными расположениями каталогов баз данных и каталогов корзин

// Примечание: если в конфиге прописаны относительные пути,
// то они высчитываются относительно рабочего каталога,
// а не каталога бинарника, хотя эти каталоги могут совпадать.
// Другими словами: файл conf.ini всегда лежит в рабочем каталоге,
// и таким образом, если в conf.ini прописаны относительные пути, то
// эти пути и будут высчитываться относительно каталога, в котором
// лежит этот conf.ini


#define DBMANAGEMENT_COLUMNS 4
#define DBMANAGEMENT_COLUMN_ISSELECT  0 // Признак выбора базы
#define DBMANAGEMENT_COLUMN_DBPATH    1 // Путь к БД
#define DBMANAGEMENT_COLUMN_TRASHPATH 2 // Путь к корзине
#define DBMANAGEMENT_COLUMN_DESCRIPT  3 // Человекочитаемое описание базы

#define DBMANAGEMENT_LINE_SELECT_FLAG "1" // Значение, свидетельствующее о выборе базы

#define DBMANAGEMENT_DEFAULT_DESCRIPT "Custom database directory"


class DatabasesManagementModel : public QAbstractTableModel
{
  Q_OBJECT

  // Информация о директории с базой данных
  struct DatabaseDirInfo
  {
      QString dbPath;
      QString trashPath;
      QString descript;

      // Флаг, что данные прописаны в текущем рабочем конфиге conf.ini,
      bool isInConfigData=false;
  };

public:
  DatabasesManagementModel(QObject *parent);
  virtual ~DatabasesManagementModel();

  int columnCount(const QModelIndex & parent = QModelIndex()) const;
  int rowCount(const QModelIndex & parent = QModelIndex()) const;
  QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
  QVariant headerData ( int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole ) const;

  //! Получение данных из ячейки в виде строки
  QString getCellValue(const int &row,
                       const int &column,
                       const int &role = Qt::DisplayRole) const;

  bool isDbPathExists(const QString &path);
  void addDatabaseByUser(const QString &dbPath, const QString &trashPath);
  void deleteDatabaseByUser(const QString &dbPath, const QString &trashPath);
  void selectDatabase(const int &row);
  bool editDatabaseDescript(const int &row, const QString &descript);

  bool isDbDirectory(const QString &path);
  bool isTrashDirectory(const QString &path);

  bool isDbInKnownBasesConfig(const QString &dbPath, const QString &trashPath);

private:

  QVariant getCell(int row, int column, int role) const;

  void initData();


  //! Запоминание директорий с базами из известных возможных мест на диске
  void scanDirectoriesDirect();

  //! Запоминание директории с базой из conf.ini
  void scanDirectoriesFromConfig();

  //! Запоминание директорий с базами из knownbases.ini
  void scanDirectoriesFromKnownbasesConfig();

  //! Универсальный метод, проверяющий переданный список директорий
  //! и сохраняющий в mTableData действительно существующие директории
  void scanDirectories(const QList< DatabaseDirInfo > &dbDirs);


  //! Снятие пометки выбора со всех баз
  void clearSelection();

  //! Выбор директории БД, с которой происходит работа пользователя
  void selectDirectories(const QString &dbPath, const QString &trashPath);

  QPair<QString, QString> getDirectoriesFromConfigFile(const QString &path);

  // На каждой строке хранится строковый набор данных
  // Столбцы содержат информацию согласно определениям DBMANAGEMENT_COLUMN_*
  QList< QStringList > m_TableData;

  KnownBasesConfig m_KnownBasesConfig;
};

#endif // DATABASESMANAGEMENTMODEL_H
