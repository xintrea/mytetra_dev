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
class RecordTableController;


class RecordTableView : public QTableView
{
 Q_OBJECT

public:
 RecordTableView(QWidget *parent=0);
 virtual ~RecordTableView();

 void setController(RecordTableController *pController);

 void init(void);

 void restoreHeaderState( void );
 void restoreColumnWidth(void);

 int getFirstSelectionPos(void);
 QString getFirstSelectionId(void);

 void setSelectionToPos(int pos);

 QModelIndex getFirstSelectionProxyIndex(void);
 QModelIndex getFirstSelectionSourceIndex(void);

 bool isSelectedSetToTop(void);
 bool isSelectedSetToBottom(void);

 ClipboardRecords *getSelectedRecords(void);

 void moveCursorToNewRecord(int mode, int pos);

 void switchSelectionMode(void);

 void updateRow(int rowNum);

signals:

 void listSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

 void tapAndHoldGestureFinished(const QPoint &);


public slots:

 // Открытие контекстного меню
 void onCustomContextMenuRequested(const QPoint &mousePos);

 // Слот, срабатывающий после перетаскивания колонки
 void onSectionMoved( int logicalIndex, int oldVisualIndex, int newVisualIndex );
 void onSectionResized( int logicalIndex, int oldSize, int newSize );

 // Вызов действий из контекстного меню или из контроллера для редактирования инфополей записи
 void editFieldContext(void);

protected slots:

 // Реакия на сдвиг засветки клавишами или мышкой
 void onSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected );

 // Слот, который автоматически срабатыват при изменении selection в списке
 // Этот слот нигде не надо прописывать через connect(), так как он
 // является переопределенным, так как его тип virtual protected slot
 virtual void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

 void onPressToRecord(const QModelIndex &index);
 void onClickToRecord(const QModelIndex &index);

protected:

 QMenu *contextMenu;
 RecordTableController *controller;

 void setupSignals(void);

 void assemblyContextMenu(void);

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
