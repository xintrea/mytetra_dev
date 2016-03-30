#ifndef __RECORDTABLECONTROLLER_H__
#define __RECORDTABLECONTROLLER_H__

#include <QObject>
#include <QModelIndexList>

class Record;
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

  bool isTableNotExists(void);
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
  int         convertSourcePosToProxyPos(int sourcePos);
  int         convertProxyPosToSourcePos(int proxyPos);
  QModelIndex convertIdToSourceIndex(QString id);
  QModelIndex convertIdToProxyIndex(QString id);

  int     getFirstSelectionPos(void);
  QString getFirstSelectionId(void);
  void    setSelectionToPos(int pos);
  void    setSelectionToId(QString id);

  // Методы удаления записей перенесены в открытый доступ, так как через них удаляются даннные из KnowTreeView при DragAndDrop
  void removeRowById(QString delId);
  void removeRowsByIdList(QVector<QString> delIds);

  void switchMetaEditorToEditorOrAttach(QModelIndex index);

signals:

public slots:

  // Вызов действий для копирования записей в буфер обмена с удалением
  void cut(void);

  // Вызов действий для копирования записей в буфер обмена
  void copy(void);

  // Вызов действий для вставки записей из буфера
  void paste(void);

  void onEditFieldContext(void);
  void onBlockContext(void);

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

  // Слот для вызов настроек
  void settings(void);

  // Слот, обновляющий вид если изменились настройки таблицы конечных записей в конфиге программы
  void onRecordTableConfigChange(void);

  // Печать таблицы конечных записей
  void onPrintClick(void);

  void onSwitchSelectionMode(void);

protected:

  void initMetaEditorAtClickToRecord(const int pos);
  void initAttachTableAtClickToRecord(const int pos);

  RecordTableView *view;
  RecordTableModel *recordSourceModel; // Класс, расширенный от QAbstractTableModel
  RecordTableProxyModel *recordProxyModel;

  void addNewRecord(int mode);

  void addNew(int mode, Record record);

  void editField(int pos,
                 QString name,
                 QString author,
                 QString url,
                 QString tags);

};

#endif // __RECORDTABLECONTROLLER_H__
