#ifndef _TIMERMONITORING_H_
#define _TIMERMONITORING_H_

#include <QObject>

class KnowTreeView;
class KnowTreeModel;

class TimerMonitoring : public QObject
{
  Q_OBJECT

public:
  TimerMonitoring(void);
  virtual ~TimerMonitoring(void);

  void init();

  void setDelay(int sec);
  void start();
  void stop();

 
protected:

  virtual bool isStartEnabled()=0;
  virtual void timerEvent(QTimerEvent *)=0;

  bool isFirstStart;

  int delay;
  int timerId;

  KnowTreeView  *knowTreeView;
  KnowTreeModel *knowTreeModel;
};

#endif	/* _TIMERMONITORING_H_ */

