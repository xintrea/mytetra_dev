#ifndef _PERIODICSYNCHRO_H_
#define _PERIODICSYNCHRO_H_

#include "TimerMonitoring.h"


class PeriodicSynchro : public TimerMonitoring
{
  Q_OBJECT

protected:

  bool isStartEnabled();
  void timerEvent(QTimerEvent *event);

};

#endif	/* _PERIODICSYNCHRO_H_ */

