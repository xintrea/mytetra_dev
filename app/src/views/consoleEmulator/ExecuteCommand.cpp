#include <QtGlobal>
#include <QtDebug>
#include <QString>
#include <QString>
#include <QMessageBox>
#include <QTextCodec>
#include <QtGlobal>

#include "ExecuteCommand.h"
#include "ConsoleEmulator.h"
#include "main.h"
#include "libraries/ActionLogger.h"
#include "libraries/GlobalParameters.h"

extern ActionLogger actionLogger;
extern GlobalParameters globalParameters;


ExecuteCommand::ExecuteCommand(QObject *parent) : QObject(parent)
{
 command="";
 console=NULL;

 QString os=getOsFamily();

 // Определение командной оболочки, которую надо использовать
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


 // Определяется кодек для вывода текста терминального потока
 outputCodec = QTextCodec::codecForName(globalParameters.getConsoleCodepage().toLatin1());
}


ExecuteCommand::~ExecuteCommand()
{
 delete console;
}


QString ExecuteCommand::getOsFamily()
{
    QString os="unix";

    #if defined(Q_WS_WIN) || defined(Q_OS_WIN32) || defined(Q_OS_WINCE) || defined(Q_OS_MSDOS) || defined(Q_OS_CYGWIN)
     os="windows";
    #endif

    return os;
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
    // Процессы 0 и 1 - это однозначно системные процессы, такого PID быть не может
    // Возможно данное условие поборет проблему [WRN] QIODevice::read (QProcess): device not open
    if(process->processId()>1) {
        qDebug() << "Close process, PID" << process->processId();
        
        process->kill();
        process->terminate();
        process->close();
    }
}


// Простой запуск консольных команд на исполнение с ожиданием завершения, возвращет код возврата выполняемой команды
int ExecuteCommand::runSimple()
{
 // Если командный интерпретатор не установлен
 if(shell.length()==0)
  criticalError("ExecuteCommand::run() : Not detect available shell");

 // Выясняется полная команда, которая будет запущена в QProcess
 QString commandLine=shell+" \""+command+"\"";

 // Создается процесс
 QProcess simpleProcess;

 // Запускается команда на исполнение
 return simpleProcess.execute(commandLine);
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

 connect(console, &ConsoleEmulator::cancelConsole, this, &ExecuteCommand::manualCloseProcess);
 connect(process, qOverload<QProcess::ProcessError>(&QProcess::error),
         this,    &ExecuteCommand::errorHanler);


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

