#include <QDateTime>

#include "main.h"
#include "TimerMonitoring.h"
#include "PeriodicSyncro.h"
#include "models/appConfig/AppConfig.h"
#include "views/mainWindow/MainWindow.h"


extern AppConfig mytetraConfig;


// Переопределяемый метод
bool PeriodicSyncro::isStartEnabled()
{
  return mytetraConfig.getEnablePeriodicSyncro();
}


// Действия, происходящие по таймеру
void PeriodicSyncro::timerEvent(QTimerEvent *event)
{
  Q_UNUSED(event);

  qDebug() << "In timer PeriodicSyncro working method";

  // Если команда синхронизации пуста, нечего выполнять
  if(mytetraConfig.get_synchrocommand().trimmed().length()==0)
    return;

  // Запуск синхронизации в скрытом режиме
  find_object<MainWindow>("mainwindow")->synchronization(false);
}
