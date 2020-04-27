#include <QtGlobal>
#include <QtDebug>
#include <QString>
#include <QString>
#include <QMessageBox>
#include <QTextCodec>
#include <QtGlobal>

#include "CommandRun.h"
#include "ConsoleEmulator.h"
#include "main.h"
#include "libraries/ActionLogger.h"
#include "libraries/GlobalParameters.h"

extern ActionLogger actionLogger;
extern GlobalParameters globalParameters;


CommandRun::CommandRun(QObject *parent) : QObject(parent)
{
    QString os=getOsFamily();

    // Определение командной оболочки, которую надо использовать
    m_shell="";
    if(os=="unix")
    {
        if(system("sh -c exit")==0)
            m_shell="sh -c";
        else if(system("bash -c exit")==0)
            m_shell="bash -c";
    }
    else if(os=="windows")
    {
        m_shell="cmd.exe /C";
    }

    if(m_shell.length()==0)
    {
        criticalError("Can't detect sh, bash or cmd shell.");
    }


    // Определяется кодек для вывода текста терминального потока
    m_outputCodec = QTextCodec::codecForName(globalParameters.getConsoleCodepage().toLatin1());
}


CommandRun::~CommandRun()
{
    if(m_process)
    {
        delete m_process;
    }

    if(m_console)
    {
        delete m_console;
    }
}


QString CommandRun::getOsFamily()
{
    QString os="unix";

    #if defined(Q_WS_WIN) || defined(Q_OS_WIN32) || defined(Q_OS_WINCE) || defined(Q_OS_MSDOS) || defined(Q_OS_CYGWIN)
     os="windows";
    #endif

    return os;
}


void CommandRun::setCommand(QString cmd)
{
    m_command=cmd;
}


void CommandRun::setWindowTitle(QString title)
{
    m_windowTitle=title;
}


void CommandRun::setMessageText(QString text)
{
    m_messageText=text;
}


// Создание процесса, в котором будут выполняться команды
void CommandRun::createProcessAndConsole()
{
    // Создается процесс
    if(m_process)
    {
        m_process->blockSignals(true);
        m_process->disconnect();
        delete m_process;
    }
    m_process=new QProcess();


    // Для процесса создается эмулятор консоли, где будет отображаться ход процесса
    if(m_console)
    {
        delete m_console;
    }
    m_console=new ConsoleEmulator();
    m_console->setWindowTitle(m_windowTitle);
    m_console->setMessageText(m_messageText);
    m_console->setConsoleOutput( this->getCommandForProcessExecute()+"\n" );

    qDebug() << "Run shell" << m_shell;
    qDebug() << "Run command" << m_command;
}


// Удаление процесса, в котором выполнялись команды
void CommandRun::removeProcessAndConsole(void)
{
    // Удаление процесса
    if(m_process)
    {
        // todo: разобраться с проблемой [WRN] QIODevice::read (QProcess): device not open
        if(m_process->processId()<=1) {
            qDebug() << QString("Abnormal execute command process PID %1 for close").arg( m_process->processId() );
        }

        // Останавливается процесс
        m_process->terminate();
        m_process->kill();
        m_process->close();

        delete m_process;
    }

    // Удаление консоли
    if(m_console)
    {
        delete m_console;
    }

    qDebug() << "Execute command process remove.";
}


// Команда, которая должна запускаться в процессе
QString CommandRun::getCommandForProcessExecute()
{
    return m_shell+" \""+m_command+"\"";
}


// Простой запуск консольных команд на исполнение с ожиданием завершения,
// Метод возвращет код возврата выполняемой команды.
// Консоль не создается, команду просто выполняется в процессе
int CommandRun::runSimple()
{
    // Если командный интерпретатор не установлен
    if(m_shell.length()==0)
        criticalError("ExecuteCommand::run() : Not detect available shell");

    // Создается процесс
    QProcess simpleProcess;

    // Запускается команда на исполнение
    return simpleProcess.execute( this->getCommandForProcessExecute() );
}


// Запуск процесса выполнения команды
void CommandRun::run(bool visible)
{
    // Если командный интерпретатор не установлен
    if(m_shell.length()==0)
        criticalError("ExecuteCommand::run() : Not detect available shell");

    m_isError=false;
    m_isManualClose=false;

    // Создается процесс и консоль к нему
    this->createProcessAndConsole();

    if(visible)
        m_console->show();


    // Объединение вывода стандартного канала и канала ошибок
    m_process->setProcessChannelMode(QProcess::MergedChannels);


    // Обработка кнопки Cancel в виджете эмулятора консоли
    connect(m_console, &ConsoleEmulator::cancelConsole,
            this, &CommandRun::onManualCloseProcess);

    // Обработка ошибки, если таковая возникнет при работе процесса
    connect(m_process, &QProcess::errorOccurred,
            this, &CommandRun::onProcessError);

    // Отслеживание стандартного консольного вывода
    connect(m_process, &QProcess::readyReadStandardOutput,
            this, &CommandRun::onReadyReadStandardOutput );

    // Отслеживание завершения запущенного процесса
    // (Сигнал finished перегружен, поэтому новый синтаксис надо писать в виде замыкания, поэтому оставлен старый синтаксис)
    connect(m_process, SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(onProcessFinish(int, QProcess::ExitStatus)) );


    // Запускается команда на исполнение внутри процесса
    m_process->start( this->getCommandForProcessExecute() );
}


// Слот, срабатывающий когда пользователь нажал Cancel в эмуляторе консоли
void CommandRun::onManualCloseProcess(void)
{
    qDebug() << "Manual close process, PID" << m_process->pid();

    m_isManualClose=true;

    removeProcessAndConsole();
}


void CommandRun::onReadyReadStandardOutput()
{
    this->printOutput();
}


void CommandRun::onProcessFinish(int exitCode, QProcess::ExitStatus exitStatus)
{
    this->printOutput();

    // Если была ошибка, и консоль не закрывалась вручную
    if((m_isError or exitCode!=0 or exitStatus==QProcess::ExitStatus::CrashExit) and !m_isManualClose)
    {
        m_console->switchToErrorView();

        // Окно с консолью было просто видно (чтобы показывать вывод),
        // а теперь запускается как модальное, чтобы началось ожидание нажатия кнопки Close
        m_console->exec();
    }
    else
    {
        // Иначе выполнение команды успешно завершилось, и консоль скрывается
        m_console->hide();
    }

    this->removeProcessAndConsole();

    emit finishWork();
}


void CommandRun::onProcessError(QProcess::ProcessError error)
{
    qDebug() << "Execute command detect error! Error code: " << error;

    QMap<QString, QString> data;
    data["errCode"]=QString::number(error);
    actionLogger.addAction("syncroProcessError", data);

    m_isError=true;
}


// Вывод стандартного вывода процесса в эмулятор консоли
void CommandRun::printOutput() const
{
    if(!m_process or !m_console)
    {
        return;
    }

    // Преобразование в QString, необходимо чтобы исключать строки с нулями
    QString output=m_outputCodec->toUnicode( m_process->readAllStandardOutput() );

    if(output.length()>0)
    {
        m_console->addConsoleOutput(output);
        qDebug() << "[Console] " << output;

        // qApp->processEvents();
    }
}
