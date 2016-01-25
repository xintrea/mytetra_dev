#ifndef __ACTIONLOGCONTROLLER_H__
#define __ACTIONLOGCONTROLLER_H__

#include <QObject>
#include <QModelIndexList>

#include "models/actionLog/Attach.h"
#include "views/actionLog/ActionLogView.h"
#include "models/actionLog/ActionLogModel.h"
#include "models/actionLog/ActionLogData.h"

// class Attach;
// class ActionLogView;
// class ActionLogModel;
// class ActionLogData;

class ActionLogController : public QObject
{
  Q_OBJECT

public:

  ActionLogController(QObject *parent = 0);
  virtual ~ActionLogController();

  ActionLogView *getView(void);
  void setActionLogData(ActionLogData *ActionLogData);
  ActionLogData *getActionLogData();

  QList<QString> getSelectedId(void);

public slots:

  void onAddAttach(void);
  void onAddLink(void);
  void onEditFileName(void);
  void onDeleteAttach(void);
  void onOpenAttach(void);
  void onShowAttachInfo(void);
  void onSaveAsAttach(void);

  void onSwitchToEditor(void);


protected:

  void addSmart(QString attachType);
  QStringList selectFilesForAdding(QString attachType);
  void saveAttachToUserPlace(QString fromFullFileName, QString toFullFileName, QString attachType, bool isAttachCrypt);

  ActionLogView *view;
  ActionLogModel *model;

};

#endif // __ACTIONLOGCONTROLLER_H__
