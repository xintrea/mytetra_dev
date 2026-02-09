#ifndef ACTIONLOGCONTROLLER_H
#define ACTIONLOGCONTROLLER_H

#include <QObject>
#include <QModelIndexList>

class ActionLogView;
class ActionLogModel;

// class Attach;
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
