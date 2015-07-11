#ifndef __RECORDTABLECONTROLLER_H__
#define __RECORDTABLECONTROLLER_H__

#include <QObject>
#include <QModelIndexList>

class RecordTableView;
class RecordTableData;
class RecordTableModel;
class RecordTableProxyModel;
class ClipboardRecords;


class RecordTableController : public QObject
{
  Q_OBJECT
public:
  RecordTableController(QObject *parent = 0);
  virtual ~RecordTableController();

  void init(void);

  RecordTableView *getView(void);

  void clickToRecord(const QModelIndex &index);

  bool isTableEmpty(void);
  void setTableData(RecordTableData *rtData);

  int getRowCount(void);

  void addRecordsToClipboard(ClipboardRecords *clipboardRecords, QModelIndexList itemsForCopy);

  // Действия при редактировании инфополей записи из контекстного меню
  void editFieldContext(QModelIndex proxyIndex);

  QModelIndex convertPosToProxyIndex(int pos);
  QModelIndex convertPosToSourceIndex(int pos);
  int         convertProxyIndexToPos(QModelIndex index);
  int         convertSourceIndexToPos(QModelIndex index);
  QModelIndex convertProxyIndexToSourceIndex(QModelIndex proxyIndex);
  QModelIndex convertSourceIndexToProxyIndex(QModelIndex sourceIndex);

  int getFirstSelectionPos(void);
  void setSelectionToPos(int pos);

signals:

public slots:

  // Вызов действий для копирования записей в буфер обмена с удалением
  void cut(void);

  // Вызов действий для копирования записей в буфер обмена
  void copy(void);

  // Вызов действий для вставки записей из буфера
  void paste(void);

  void onEditFieldContext(void);

  void deleteRecordByPos(int pos);
  void deleteRecordsByPos(QVector<int> vectorPos);
  void deleteRecords(void);

  // Вызов действий из контекстного меню для открытия окна с вводом новой записи
  void addNewToEndContext(void);
  void addNewBeforeContext(void);
  void addNewAfterContext(void);

  // Вызов действий из контекстного меню для удаления конечной записи
  void deleteContext(void);

  void moveUp(void);
  void moveDn(void);

  // Клик по пункту "Сортировка" в контекстном меню
  void onSortClick(void);

protected:
  RecordTableView *view;
  RecordTableModel *recordSourceModel; // Класс, расширенный от QAbstractTableModel
  RecordTableProxyModel *recordProxyModel;

  void addNewRecord(int mode);

  void addNew(int mode,
              QMap<QString, QString> fields,
              QString text,
              QMap<QString, QByteArray> files=(QMap<QString, QByteArray>()) );

  void editField(int pos,
                 QString name,
                 QString author,
                 QString url,
                 QString tags);
};

#endif // __RECORDTABLECONTROLLER_H__
