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

 
private:

  void timerEvent(QTimerEvent *);

  int delay;
  int timerId;

  KnowTreeView  *knowTreeView;
  KnowTreeModel *knowTreeModel;
};

#endif	/* _TIMERMONITORING_H_ */

