#ifndef RECORDLISTSCREEN_H_
#define RECORDLISTSCREEN_H_

#include <QWidget>
#include <QListView>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolBar>
#include <QStringList>
#include <QItemSelection>
#include <QMenu>

class RecordTableData;
class RecordTableModel;
class ClipboardRecords;


class RecordListScreen : public QListView
{
 Q_OBJECT

public:
 RecordListScreen(QWidget *parent=0);
 virtual ~RecordListScreen();

 void init(void);

 int getFirstSelectionPos(void);
 void setSelectionToPos(int pos);

 bool isSelectedSetToTop(void);
 bool isSelectedSetToBottom(void);

 void setTableData(RecordTableData *rtData);
 RecordTableData *getTableData(void);
 ClipboardRecords *getSelectedRecords(void);

 int getRowCount(void);

 void deleteRecordByPos(int pos);
 void deleteRecordsByPos(QVector<int> vectorPos);

signals:

 void listSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

public slots:

 // Вызов действий для копирования записей в буфер обмена с удалением
 void cut(void);

 // Вызов действий для копирования записей в буфер обмена
 void copy(void);

 // Вызов действий для вставки записей из буфера
 void paste(void);

 // Открытие контекстного меню
 void onCustomContextMenuRequested(const QPoint &pos);

 // Вызов действий из контекстного меню для открытия окна с вводом новой записи
 void addNewToEndContext(void);
 void addNewBeforeContext(void);
 void addNewAfterContext(void);

 // Вызов действий из контекстного меню для редактирования инфополей записи
 void editFieldContext(void);

 // Вызов действий из контекстного меню для удаления конечной записи
 void deleteContext(void);

 void moveUp(void);
 void moveDn(void);

protected slots:

 // Реакия на сдвиг засветки клавишами или мышкой
 void onSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected );

 // Слот, который автоматически срабатыват при изменении selection в списке
 // Этот слот нигде не надо прописывать через connect(), так как он
 // является переопределенным, так как его тип virtual protected slot
 virtual void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);


protected:

 RecordTableModel *recordModel; // Класс, расширенный от QAbstractTableModel

 QMenu *contextMenu;

 void setupSignals(void);

 void assemblyContextMenu(void);

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

 void deleteRecords(void);

 // Реакия на выбор записи мышкой или клавишами
 void onClickToRecord(const QModelIndex &index);

 QPoint mouseStartPos;
 void mousePressEvent(QMouseEvent *event);
 void mouseMoveEvent(QMouseEvent *event);
 void mouseReleaseEvent(QMouseEvent *event);

private:

 void startDrag();

};

#endif /* RECORDLISTSCREEN_H_ */
