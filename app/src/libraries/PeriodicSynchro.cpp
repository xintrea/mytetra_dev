#include <QDateTime>

#include "main.h"
#include "TimerMonitoring.h"
#include "PeriodicSynchro.h"
#include "models/appConfig/AppConfig.h"
#include "views/mainWindow/MainWindow.h"
#include "libraries/helpers/ObjectHelper.h"


extern AppConfig mytetraConfig;


// Переопределяемый метод
bool PeriodicSynchro::isStartEnabled()
{
  return mytetraConfig.getEnablePeriodicSynchro();
}


// Действия, происходящие по таймеру
void PeriodicSynchro::timerEvent(QTimerEvent *event)
{
  Q_UNUSED(event)

  qDebug() << "In timer PeriodicSynchro working method";

  // Если команда синхронизации пуста, нечего выполнять
  if(mytetraConfig.get_synchrocommand().trimmed().length()==0)
    return;

  // Запуск синхронизации в скрытом режиме
  find_object<MainWindow>("mainwindow")->synchronization(false);
}
