#ifndef __ATTACHTABLEVIEW_H__
#define __ATTACHTABLEVIEW_H__

#include <QObject>
#include <QWidget>
#include <QTableView>
#include <QMenu>
#include <QGestureEvent>
#include <QTapAndHoldGesture>

class AttachTableController;

// Отображение таблицы с файлами (только таблица)

class AttachTableView : public QTableView
{
  Q_OBJECT

public:
  AttachTableView(QWidget *parent=0);
  virtual ~AttachTableView();

  void init(void);

  void setController(AttachTableController *pController);

  int getFirstSelectionPos(void);


signals:

  void tapAndHoldGestureFinished(const QPoint &);


protected slots:

  void onCustomContextMenuRequested(const QPoint &pos);


protected:

  void setupSignals(void);
  void assemblyContextMenu(void);

  bool event(QEvent *event);
  virtual void resizeEvent(QResizeEvent *event);
  virtual void paintEvent(QPaintEvent *event);
  bool gestureEvent(QGestureEvent *event);
  void tapAndHoldGestureTriggered(QTapAndHoldGesture *gesture);

  AttachTableController *controller;

  QMenu *contextMenu;
};

#endif // __ATTACHTABLEVIEW_H__
