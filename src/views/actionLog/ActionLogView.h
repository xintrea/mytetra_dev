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

  void setController(ActionLogController *pController);

protected:

  virtual void resizeEvent(QResizeEvent *event);

  ActionLogController *controller;
};

#endif // __ACTIONLOGVIEW_H__
