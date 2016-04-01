#include "main.h"
#include "TimerMonitoring.h"
#include "models/appConfig/AppConfig.h"
#include "models/tree/KnowTreeModel.h"
#include "views/tree/KnowTreeView.h"
#include "views/mainWindow/MainWindow.h"

extern AppConfig mytetraConfig;


TimerMonitoring::TimerMonitoring(void)
{
  // По сути, это синглтон. Синглтон назначает себе имя сам
  setObjectName("timerMonitoring");

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
  if(timerId!=0)
    killTimer(timerId);

  delay=sec;
}


void TimerMonitoring::start()
{
  timerId=startTimer(delay*1000); // Периодичность таймера должна задаваться в миллисекундах

  qDebug() << "Start timer with ID: " << timerId;
}


void TimerMonitoring::stop()
{
  killTimer(timerId);
}


// Действия, происходящие по таймеру
void TimerMonitoring::timerEvent(QTimerEvent *)
{
  // qDebug() << "In timer working method";

  QDateTime lastSave=knowTreeModel->getLastSaveDateTime();
  QDateTime lastLoad=knowTreeModel->getLastLoadDateTime();

  // Если доступа к файла за текущий сеанс ни разу не производилось, нечего сравнивать
  if(lastSave.isNull() && lastLoad.isNull())
    return;

  QDateTime lastAccess= lastSave > lastLoad ? lastSave : lastLoad;

  // Время последнего изменения файла дерева
  QString fileName=mytetraConfig.get_tetradir()+"/mytetra.xml";
  QFileInfo fileInfo(fileName);
  QDateTime modifyDateTime=fileInfo.lastModified();

  if(modifyDateTime>lastAccess)
  {
    (find_object<MainWindow>("mainwindow"))->reload();

    showMessageBox(tr("Database changet at another program. Reload for work with actual data."));
  }
}
