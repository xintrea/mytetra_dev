#ifndef DATABASESMANAGEMENTCONTROLLER_H
#define DATABASESMANAGEMENTCONTROLLER_H

#include <QObject>

#include "views/databasesManagement/DatabasesManagementTable.h"
#include "models/databasesManagement/DatabasesManagementModel.h"


class DatabasesManagementController : public QObject
{
  Q_OBJECT

public:

  DatabasesManagementController(QObject *parent = nullptr);
  virtual ~DatabasesManagementController();

  DatabasesManagementTable *getView(void);

public slots:

  void onSelectClicked();
  void onCreateClicked();
  void onAddClicked();
  void onDeleteClicked();
  void onCopyClicked();

  void onDoubleClicked(const QModelIndex &index);

protected:

  void addDatabase(const QString &dbPath,
                   const QString &trashPath);

  bool switchToDatabase(const QString &dbPath,
                        const QString &trashPath);

  DatabasesManagementTable *view;
  DatabasesManagementModel *model;

};

#endif // DATABASESMANAGEMENTCONTROLLER_H
