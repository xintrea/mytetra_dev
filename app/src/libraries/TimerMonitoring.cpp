#include "main.h"
#include "TimerMonitoring.h"
#include "models/tree/KnowTreeModel.h"
#include "views/tree/KnowTreeView.h"


TimerMonitoring::TimerMonitoring(void)
{
  // По сути, это синглтон. Синглтон назначает себе имя сам
  setObjectName("timerMonitoring");

  isFirstStart=true;

  knowTreeView=NULL;
  knowTreeModel=NULL;
}


TimerMonitoring::~TimerMonitoring(void)
{

}


// Инициализация класса должна происходить после инициализации KnowTreeView
void TimerMonitoring::init()
{
  setDelay(0);
  timerId=0;

  knowTreeView=find_object<KnowTreeView>("knowTreeView");
  knowTreeModel=qobject_cast<KnowTreeModel*>( knowTreeView->model() );
}


void TimerMonitoring::setDelay(int sec)
{
  delay=sec;

  // Если таймер работает, он перезапускается с новой задержкой
  if(timerId!=0)
  {
    stop();
    start();
  }
}


void TimerMonitoring::start()
{
  // Если периодическая проверка не разрешена в конфигурации
  if( !isStartEnabled() )
    return; // Таймер не запускается
  else // Иначе, если это первый старт, то нужно напрямую первый раз вызвать исполнение действий
    if( isFirstStart )
    {
      timerEvent(NULL);
      isFirstStart=false;
    }

  // Установка автоматического повтора действий
  timerId=startTimer(delay*1000); // Периодичность таймера должна задаваться в миллисекундах
  qDebug() << "Start timer with delay: " << delay << " ID: " << timerId;
}


void TimerMonitoring::stop()
{
  killTimer(timerId);
  timerId=0;
}

