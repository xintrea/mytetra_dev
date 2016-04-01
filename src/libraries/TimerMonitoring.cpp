#include "main.h"
#include "TimerMonitoring.h"
#include "models/appConfig/AppConfig.h"
#include "models/tree/KnowTreeModel.h"
#include "views/tree/KnowTreeView.h"
#include "views/mainWindow/MainWindow.h"

extern AppConfig mytetraConfig;


TimerMonitoring::TimerMonitoring(void)
{
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
  // knowTreeModel=qobject_cast<KnowTreeModel*>( knowTreeView->model() );
  knowTreeModel=(KnowTreeModel*)( knowTreeView->model() );
}


void TimerMonitoring::setDelay(int sec)
{
  if(timerId!=0)
    killTimer(timerId);

  delay=sec;
}


void TimerMonitoring::start()
{
  timerId=startTimer(delay*100); // Периодичность таймера должна задаваться в миллисекундах
}


void TimerMonitoring::stop()
{
  killTimer(timerId);
}


// Действия, происходящие по таймеру
void TimerMonitoring::timerEvent(QTimerEvent *)
{
  // Время последнего сохранения файла дерева
  QDateTime lastSaveDateTime=knowTreeModel->getLastSaveDateTime();

  // Если сохранение за текущий сеанс ни разу не производилось, нечего сравнивать
  if(lastSaveDateTime.isNull())
    return;

  // Время последнего изменения файла дерева
  QString fileName=mytetraConfig.get_tetradir()+"/mytetra.xml";
  QFileInfo fileInfo(fileName);
  QDateTime modifyDateTime=fileInfo.lastModified();

  if(modifyDateTime>lastSaveDateTime)
  {
    (find_object<MainWindow>("mainwindow"))->reload();

    showMessageBox(tr("Database changet at another program. Reload for work with actual data."));
  }
}
