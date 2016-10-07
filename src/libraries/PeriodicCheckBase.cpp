#include <QDateTime>

#include "main.h"
#include "TimerMonitoring.h"
#include "PeriodicCheckBase.h"
#include "models/appConfig/AppConfig.h"
#include "models/tree/KnowTreeModel.h"
#include "views/tree/KnowTreeView.h"
#include "views/mainWindow/MainWindow.h"


extern AppConfig mytetraConfig;


// Переопределяемый метод
bool PeriodicCheckBase::isStartEnabled()
{
  return mytetraConfig.getEnablePeriodicCheckBase();
}


// Действия, происходящие по таймеру
void PeriodicCheckBase::timerEvent(QTimerEvent *event)
{
  Q_UNUSED(event);

  // qDebug() << "In timer working method";

  QDateTime lastSave=knowTreeModel->getLastSaveDateTime();
  QDateTime lastLoad=knowTreeModel->getLastLoadDateTime();

  // Если доступа к файла за текущий сеанс ни разу не производилось, нечего сравнивать
  if(lastSave.isNull() && lastLoad.isNull())
    return;

  QDateTime lastAccess = lastSave > lastLoad ? lastSave : lastLoad;

  // Время последнего изменения файла дерева
  QString fileName=mytetraConfig.get_tetradir()+"/mytetra.xml";
  QFileInfo fileInfo(fileName);
  QDateTime modifyDateTime=fileInfo.lastModified();

  if(modifyDateTime>lastAccess)
  {
    (find_object<MainWindow>("mainwindow"))->reload();

    // Если разрешена выдача сообщения о том, что база данных была изменена
    if(mytetraConfig.getEnablePeriodicCheckMessage())
      showMessageBox(tr("The database was changed by external application.\nMyTetra needs to reload the database tree to keep data consistency."));
  }
}
