#ifndef __ATTACHTABLEVIEW_H__
#define __ATTACHTABLEVIEW_H__

#include <QTableView>
#include <QObject>

// Отображение таблицы с файлами (только таблица)


class AttachTableController;
class QWidget;
class QMenu;
class QGestureEvent;
class QTapAndHoldGesture;

class AttachTableView : public QTableView
{
  Q_OBJECT

public:
  AttachTableView(QWidget *parent=nullptr);
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
