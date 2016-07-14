#ifndef _PERIODICCHECKBASE_H_
#define _PERIODICCHECKBASE_H_

#include "TimerMonitoring.h"


class PeriodicCheckBase : public TimerMonitoring
{
  Q_OBJECT

protected:

  bool isStartEnabled();
  void timerEvent(QTimerEvent *);

};

#endif	/* _PERIODICCHECKBASE_H_ */

