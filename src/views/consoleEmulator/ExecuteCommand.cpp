#include <QtGlobal>
#include <QtDebug>
#include <QString>
#include <QString>
#include <QMessageBox>

#include "ExecuteCommand.h"
#include "ConsoleEmulator.h"
#include "main.h"


ExecuteCommand::ExecuteCommand(QWidget *parent) : QDialog(parent)
{
 command="";

 // Определяется, какую командную оболочку надо использовать

 // Вначале определяется вид операционной системы
 QString os="unix";
 #if defined(Q_WS_WIN) || defined(Q_OS_WIN32) || defined(Q_OS_WINCE) || defined(Q_OS_MSDOS) || defined(Q_OS_CYGWIN)
  os="windows";
 #endif

 // Определение командной оболочки
 shell="";
 if(os=="unix")
  {
   if(system("sh -c exit")==0)
    {
     shell="sh -c";
     return;
    }

   if(system("bash -c exit")==0)
    {
     shell="bash -c";
     return;
    }
  }
 else if(os=="windows")
  {
   shell="cmd.exe /C";
  }
}


ExecuteCommand::~ExecuteCommand()
{
 
}


void ExecuteCommand::setCommand(QString cmd)
{
 command=cmd;
}


void ExecuteCommand::setWindowTitle(QString title)
{
 windowTitle=title;
}


void ExecuteCommand::setMessageText(QString text)
{
 messageText=text;
}


// Слот, срабатывающий когда нужно закрыть процесс
void ExecuteCommand::closeProcess(void)
{
 qDebug() << "Delete process, PID" << process->pid();

 process->kill();
 process->terminate();
 process->close();
}


void ExecuteCommand::run(void)
{
 // Если командный интерпретатор не установлен
 if(shell.length()==0)
  critical_error("ExecuteCommand::run() : Not detect available shell");

 // Выясняется полная команда, которая будет запущена в QProcess
 // QString commandLine=shell.toAscii()+" \""+command.toAscii()+"\"";
 QString commandLine=shell+" \""+command+"\"";


 // Создается виджет эмулятора консоли
 ConsoleEmulator console;

 console.setWindowTitle(windowTitle);
 console.setMessageText(messageText);
 console.setConsoleOutput(commandLine+"\n");
 console.show();

 qDebug() << "Run shell" << shell;
 qDebug() << "Run command" << command;

 isError=false;

 // Создается процесс 
 process=new QProcess();

 connect(&console, SIGNAL(cancelConsole()), this, SLOT(closeProcess()));
 connect(this, SIGNAL(error()), this, SLOT(errorHanler()));


 // Объединение вывода стандартного канала и канала ошибок
 process->setProcessChannelMode(QProcess::MergedChannels);

 // В процессе запускается команда на исполнение
 process->start(commandLine);

 // Интерфейс обновляется
 qApp->processEvents();

 qDebug() << "Process started";

 // Пока процесс не закончил свою работу
 while(process->state()!=QProcess::NotRunning)
  {
   // Считываются данные из стандартного потока запущенного процесса
   char buf[1024];
   int readBytes=process->readLine(buf, sizeof(buf));

   // Если считаны какие-то символы
   if(readBytes>=1)
    {
     // Преобразование в QString, необходимо чтобы исключать строки с нулями
     QString output=QString::fromLatin1(buf); // Ранее было fromAscii

     if(output.length()>0)
      {
       console.addConsoleOutput(output);
       qApp->processEvents();
       qDebug() << "[Console] " << output;
      }
    }

   if((rand()%10)==1)
    qApp->processEvents();
  }

 closeProcess();

 if(isError==false && process->exitCode()==0 )
  console.hide();
 else
  console.switchToErrorView();

 delete process;

 qDebug() << "Process stop";
}


void ExecuteCommand::errorHanler(void)
{
 qDebug() << "ExecuteCommand::errorHanler() : Detect error!";

 isError=true;
}


/*
void ExecuteCommand::run(void)
{
 if(shell.length()==0)
  critical_error("ExecuteCommand::run() : Not detect available shell");

 QString commandLine=shell.toAscii()+" \""+command.toAscii()+"\"";

 ConsoleEmulator console;
 console.setWindowTitle(windowTitle);
 console.setMessageText(messageText);
 console.setConsoleOutput(commandLine);
 console.show();


 qDebug() << "Run shell" << shell;
 qDebug() << "Run command" << command;


 QProcess process;
 process.start(commandLine);

 qDebug() << "Process started";

 qApp->processEvents();

 while(process.state()!=QProcess::NotRunning)
  {
   if(process.waitForReadyRead())
    {
     QString output=QString::fromAscii(process.readAll().data());

     if(output.length()>0)
      {
       console.addConsoleOutput(output);

       qApp->processEvents();

       qDebug() << "[Console] " << output;
      }
    }

   // if((rand()%10)==1) qApp->processEvents();
  }

 console.hide();

 qDebug() << "Process stop";
}
*/
