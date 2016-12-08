#include <QtGlobal>
#include <QtDebug>
#include <QString>
#include <QString>
#include <QMessageBox>
#include <QTextCodec>

#include "ExecuteCommand.h"
#include "ConsoleEmulator.h"
#include "main.h"
#include "libraries/ActionLogger.h"

extern ActionLogger actionLogger;


ExecuteCommand::ExecuteCommand(QObject *parent) : QObject(parent)
{
 command="";
 console=NULL;

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
     shell="sh -c";
   else if(system("bash -c exit")==0)
     shell="bash -c";
  }
 else if(os=="windows")
  {
   shell="cmd.exe /C";
  }

 if(shell.length()==0)
 {
   criticalError("Can't detect sh, bash or cmd shell.");
   return;
 }


 // Выясняется кодировка локали
 QTextCodec *localeCodec = QTextCodec::codecForLocale();
 QString localeCodepage=localeCodec->name(); // Возможные варианты "windows-1251" для Windows

 // Для Windows с русской кодировкой исправляется кодировка локали, так как в консли локаль CP866
 if(localeCodepage=="windows-1251")
   localeCodepage="CP866";

 // Определяется кодек для вывода текста терминального потока
 outputCodec = QTextCodec::codecForName(localeCodepage.toLocal8Bit());
}


ExecuteCommand::~ExecuteCommand()
{
 delete console;
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


// Слот, срабатывающий когда пользователь хочет вручную закрыть процесс
void ExecuteCommand::manualCloseProcess(void)
{
 qDebug() << "Manual close process, PID" << process->pid();

 closeProcess();

 isManualClose=true;
}


void ExecuteCommand::closeProcess(void)
{
 qDebug() << "Close process, PID" << process->pid();

 process->kill();
 process->terminate();
 process->close();
}


// Метод с основным циклом ожидания завершения процесса
void ExecuteCommand::run(bool visible)
{
 // Если командный интерпретатор не установлен
 if(shell.length()==0)
  criticalError("ExecuteCommand::run() : Not detect available shell");

 isError=false;
 isManualClose=false;

 // Выясняется полная команда, которая будет запущена в QProcess
 // QString commandLine=shell.toAscii()+" \""+command.toAscii()+"\"";
 QString commandLine=shell+" \""+command+"\"";

 // Создается виджет эмулятора консоли
 console=new ConsoleEmulator();
 console->setWindowTitle(windowTitle);
 console->setMessageText(messageText);
 console->setConsoleOutput(commandLine+"\n");

 if(visible)
   console->show();

 qDebug() << "Run shell" << shell;
 qDebug() << "Run command" << command;

 // Создается процесс 
 process=new QProcess();

 connect(console, SIGNAL(cancelConsole()), this, SLOT(manualCloseProcess()));
 connect(process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(errorHanler(QProcess::ProcessError)));


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
    printOutput(process, console);

    // Разгружается основной цикл обработки событий приложения
    if((rand()%10)==1)
      qApp->processEvents();
  }

 Sleeper::sleep(1);
 qApp->processEvents();
 printOutput(process, console); // Считываются остатки из стандартного вывода, если они есть

 closeProcess();
 qApp->processEvents();
 printOutput(process, console); // Считываются остатки из стандартного вывода, если они есть

 if((isError==false && process->exitCode()==0) || isManualClose)
  console->hide();
 else
  {
   console->switchToErrorView();

   // Окно с консолью было просто видно (чтобы показывать вывод), а теперь запускается как модальное, чтобы началось ожидание нажатия кнопки Close
   console->exec();
  }

 delete process;

 emit finishWork();

 qDebug() << "Process stop";
}


void ExecuteCommand::printOutput(QProcess *process, ConsoleEmulator *console)
{
  // Считываются данные из стандартного потока запущенного процесса
  char buf[1024];
  int readBytes=process->readLine(buf, sizeof(buf));

  // Если считаны какие-то символы
  if(readBytes>=1)
  {
    /*
    QByteArray encodedString = "...";
    QTextCodec *codec = QTextCodec::codecForName("KOI8-R");
    QString string = codec->toUnicode(encodedString);
    */

    // Преобразование в QString, необходимо чтобы исключать строки с нулями
    // QString output=QString::fromLocal8Bit(buf); // Ранее было fromAscii, потом fromLatin1
    QString output=outputCodec->toUnicode(buf);

    if(output.length()>0)
    {
      console->addConsoleOutput(output);
      qApp->processEvents();
      qDebug() << "[Console] " << output;
    }
  }
}


void ExecuteCommand::errorHanler(QProcess::ProcessError error)
{
 qDebug() << "ExecuteCommand::errorHanler(): Detect error! Error code: " << error;

 QMap<QString, QString> data;
 data["errCode"]=QString::number(error);
 actionLogger.addAction("syncroProcessError", data);

 isError=true;
}

