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


};

#endif // __ATTACHTABLECONTROLLER_H__
