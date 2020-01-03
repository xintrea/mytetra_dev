#ifndef PERIODICCHECKBASE_H
#define PERIODICCHECKBASE_H

#include "TimerMonitoring.h"


class PeriodicCheckBase : public TimerMonitoring
{
  Q_OBJECT

protected:

  bool isStartEnabled();
  void timerEvent(QTimerEvent *event);

};

#endif	/* PERIODICCHECKBASE_H */

