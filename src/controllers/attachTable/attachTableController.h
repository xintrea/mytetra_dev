#ifndef __ATTACHTABLECONTROLLER_H__
#define __ATTACHTABLECONTROLLER_H__

#include <QObject>
#include <QModelIndexList>

class Attach;
class AttachTableView;
class AttachTableModel;

class AttachTableController : public QObject
{
  Q_OBJECT

public:
  AttachTableController(QObject *parent = 0);
  virtual ~AttachTableController();

  AttachTableView *getView(void);

protected:
  AttachTableView *view;
  AttachTableModel *model;

};

#endif // __ATTACHTABLECONTROLLER_H__
