#ifndef __ACTIONLOGVIEW_H__
#define __ACTIONLOGVIEW_H__

#include <QObject>
#include <QWidget>
#include <QTableView>
#include <QMenu>
#include <QGestureEvent>
#include <QTapAndHoldGesture>

class ActionLogController;

// Отображение таблицы с файлами (только таблица)

class ActionLogView : public QTableView
{
  Q_OBJECT

public:
  ActionLogView(QWidget *parent=0);
  virtual ~ActionLogView();

  void init(void);

  void setController(ActionLogController *pController);

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
  bool gestureEvent(QGestureEvent *event);
  void tapAndHoldGestureTriggered(QTapAndHoldGesture *gesture);

  ActionLogController *controller;

  QMenu *contextMenu;
};

#endif // __ACTIONLOGVIEW_H__
