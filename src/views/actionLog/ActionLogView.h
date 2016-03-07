#ifndef __ACTIONLOGVIEW_H__
#define __ACTIONLOGVIEW_H__

#include <QObject>
#include <QWidget>
#include <QTableView>
#include <QMenu>
#include <QGestureEvent>
#include <QTapAndHoldGesture>

// Отображение лога в виде таблицы (только таблица)

class ActionLogController;

class ActionLogView : public QTableView
{
  Q_OBJECT

public:
  ActionLogView(QWidget *parent=0);
  virtual ~ActionLogView();

  void init();
  void setController(ActionLogController *pController);

protected:


  ActionLogController *controller;

  QMenu contextMenu;

  virtual void resizeEvent(QResizeEvent *event);

  void assemblyContextMenu(void);

};

#endif // __ACTIONLOGVIEW_H__
