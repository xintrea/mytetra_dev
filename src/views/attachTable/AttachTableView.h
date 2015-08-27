#ifndef __ATTACHTABLEVIEW_H__
#define __ATTACHTABLEVIEW_H__

#include <QObject>
#include <QWidget>
#include <QTableView>
#include <QMenu>

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

  virtual void resizeEvent(QResizeEvent *event);

protected:

  void setupSignals(void);
  void assemblyContextMenu(void);

  AttachTableController *controller;

  QMenu *contextMenu;
};

#endif // __ATTACHTABLEVIEW_H__
