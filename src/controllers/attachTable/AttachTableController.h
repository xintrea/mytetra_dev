#ifndef __ATTACHTABLECONTROLLER_H__
#define __ATTACHTABLECONTROLLER_H__

#include <QObject>
#include <QModelIndexList>

#include "models/attachTable/Attach.h"
#include "views/attachTable/AttachTableView.h"
#include "models/attachTable/AttachTableModel.h"
#include "models/attachTable/AttachTableData.h"

// class Attach;
// class AttachTableView;
// class AttachTableModel;
// class AttachTableData;

class AttachTableController : public QObject
{
  Q_OBJECT

public:
  AttachTableController(QObject *parent = 0);
  virtual ~AttachTableController();

  AttachTableView *getView(void);
  void setAttachTableData(AttachTableData *attachTableData);
  AttachTableData *getAttachTableData();

public slots:

  void onAttachFile(void);
  void onEditFileName(void);
  void onDeleteFile(void);
  void onOpenFile(void);
  void onInfo(void);

  void onSwitchToEditor(void);


protected:
  AttachTableView *view;
  AttachTableModel *model;

};

#endif // __ATTACHTABLECONTROLLER_H__
