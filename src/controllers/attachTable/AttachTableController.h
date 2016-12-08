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

  QList<QString> getSelectedId(void);

public slots:

  void onAddAttach(void);
  void onAddAttachFromUrl(void);
  void onAddLink(void);
  void onEditFileName(void);
  void onDeleteAttach(void);
  void onOpenAttach(void);
  void onShowAttachInfo(void);
  void onSaveAsAttach(void);

  void onSwitchToEditor(void);


protected:

  void addSmart(QString attachType);
  bool addAttach(QString attachType, QString currFullFileName, QString currShortFileName);
  void saveState();
  QStringList selectFilesForAdding(QString attachType);
  void saveAttachToUserPlace(QString fromFullFileName, QString toFullFileName, QString attachType, bool isAttachCrypt);

  AttachTableView *view;
  AttachTableModel *model;

};

#endif // __ATTACHTABLECONTROLLER_H__
