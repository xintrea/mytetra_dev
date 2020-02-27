#ifndef ACTIONLOGCONTROLLER_H
#define ACTIONLOGCONTROLLER_H

#include <QObject>
#include <QModelIndexList>

#include "views/actionLog/ActionLogView.h"
#include "models/actionLog/ActionLogModel.h"

// class Attach;
// class ActionLogView;
// class ActionLogModel;
// class ActionLogData;

class ActionLogController : public QObject
{
  Q_OBJECT

public:

  ActionLogController(QObject *parent = nullptr);
  virtual ~ActionLogController();

  ActionLogView *getView(void);

public slots:

  void onCopyClicked();

protected:

  ActionLogView *view;
  ActionLogModel *model;

};

#endif // ACTIONLOGCONTROLLER_H
