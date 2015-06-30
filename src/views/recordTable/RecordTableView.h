#ifndef RECORDLISTSCREEN_H_
#define RECORDLISTSCREEN_H_

#include <QWidget>
#include <QTableView>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolBar>
#include <QStringList>
#include <QItemSelection>
#include <QMenu>
#include <QTapAndHoldGesture>
#include <QEvent>
#include <QGestureEvent>

class ClipboardRecords;


class RecordTableView : public QTableView
{
 Q_OBJECT

public:
 RecordTableView(QWidget *parent=0);
 virtual ~RecordTableView();

 void init(void);

 void reloadModel( void );

 void restoreHeaderState( void );
 void restoreColumnWidth(void);

 int getFirstSelectionPos(void);
 void setSelectionToPos(int pos);

 QModelIndex getFirstSelectionProxyIndex(void);
 QModelIndex getFirstSelectionSourceIndex(void);

 QModelIndex convertPosToProxyIndex(int pos);
 QModelIndex convertPosToSourceIndex(int pos);
 int         convertProxyIndexToPos(QModelIndex index);
 int         convertSourceIndexToPos(QModelIndex index);
 QModelIndex convertProxyIndexToSourceIndex(QModelIndex proxyIndex);
 QModelIndex convertSourceIndexToProxyIndex(QModelIndex sourceIndex);

 bool isSelectedSetToTop(void);
 bool isSelectedSetToBottom(void);

 void setTableData(RecordTableData *rtData);
 bool isTableEmpty(void);
 ClipboardRecords *getSelectedRecords(void);

 int getRowCount(void);

 void deleteRecordByPos(int pos);
 void deleteRecordsByPos(QVector<int> vectorPos);


signals:

 void listSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

 void tapAndHoldGestureFinished(const QPoint &);


public slots:

 // Открытие контекстного меню
 void onCustomContextMenuRequested(const QPoint &pos);

 // Вызов настроек
 void settings(void);

 // Слот, срабатывающий после перетаскивания колонки
 void onSectionMoved( int logicalIndex, int oldVisualIndex, int newVisualIndex );
 void onSectionResized( int logicalIndex, int oldSize, int newSize );

 // Слот, обновляющий вид если изменились настройки таблицы конечных записей в конфиге программы
 void onRecordTableConfigChange(void);

protected slots:

 // Реакия на сдвиг засветки клавишами или мышкой
 void onSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected );

 // Слот, который автоматически срабатыват при изменении selection в списке
 // Этот слот нигде не надо прописывать через connect(), так как он
 // является переопределенным, так как его тип virtual protected slot
 virtual void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

 void onClickToRecord(const QModelIndex &index);

protected:

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
 void clickToRecord(const QModelIndex &index);

 bool event(QEvent *event);
 bool gestureEvent(QGestureEvent *event);
 void tapAndHoldGestureTriggered(QTapAndHoldGesture *gesture);

 QPoint mouseStartPos;
 void mousePressEvent(QMouseEvent *event);
 void mouseMoveEvent(QMouseEvent *event);
 void mouseReleaseEvent(QMouseEvent *event);

 void saveColumnWidth(void);

 bool enableMoveSection;

private:

 void startDrag();

};

#endif /* RECORDLISTSCREEN_H_ */
