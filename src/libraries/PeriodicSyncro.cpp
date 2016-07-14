#include <QDateTime>

#include "main.h"
#include "TimerMonitoring.h"
#include "PeriodicSyncro.h"
#include "models/appConfig/AppConfig.h"


extern AppConfig mytetraConfig;


// Переопределяемый метод
bool PeriodicSyncro::isStartEnabled()
{
  return mytetraConfig.getEnablePeriodicSyncro();
}


// Действия, происходящие по таймеру
void PeriodicSyncro::timerEvent(QTimerEvent *)
{
  qDebug() << "In timer PeriodicSyncro working method";

}
