#ifndef _PERIODICSYNCRO_H_
#define _PERIODICSYNCRO_H_

#include "TimerMonitoring.h"

// Это переопределенный класс от TimerMonitoring


class PeriodicSyncro : public TimerMonitoring
{
  Q_OBJECT

protected:

  bool isStartEnabled();
  void timerEvent(QTimerEvent *event);

};

#endif	/* _PERIODICSYNCRO_H_ */

