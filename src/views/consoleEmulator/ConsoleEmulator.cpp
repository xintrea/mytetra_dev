#include <QScrollBar>
#include <QWidget>

#include "ConsoleEmulator.h"
#include "main.h"
#include "models/appConfig/AppConfig.h"
#include "views/findInBaseScreen/FindScreen.h"
#include "libraries/ActionLogger.h"

extern AppConfig mytetraConfig;
extern ActionLogger actionLogger;

ConsoleEmulator::ConsoleEmulator(QWidget *parent) : QDialog(parent)
{
 isError=false;

 setupUI();
 setupSignals();
 assembly();
}


ConsoleEmulator::~ConsoleEmulator()
{
 delete waitClock;
 delete messageLabel;
 delete buttonCancel;
 delete buttonDetails;
 delete consoleOutput;
 delete escShortcut;
}


void ConsoleEmulator::setupUI(void)
{
 messageLabel=new QLabel(this);

 buttonDetails=new QToolButton(this);
 buttonDetails->setIcon(QIcon(":/resource/pic/expand_console.svg"));

 buttonCancel=new QPushButton(this);
 buttonCancel->setText(tr("Cancel"));
 buttonCancel->setDefault(true);

 consoleOutput=new QTextEdit(this);
 consoleOutput->setReadOnly(true);
 consoleOutput->setFontFamily("monospace");
 if(mytetraConfig.getSyncroConsoleDetails()==false)
  consoleOutput->hide();

 waitClock=new WaitClock(this);

 escShortcut=new QShortcut(QKeySequence("Esc"),  this);
}


void ConsoleEmulator::setupSignals(void)
{
 connect(buttonCancel, SIGNAL(clicked()), this, SLOT(onCancelClick()));
 connect(buttonDetails, SIGNAL(clicked()), this, SLOT(onDetailsClick()));

 connect(escShortcut, SIGNAL(activated()), this, SLOT(onCancelClick()));
}


void ConsoleEmulator::assembly(void)
{
 upToolbar = new QHBoxLayout;
 upToolbar->addWidget(waitClock);
 upToolbar->addWidget(messageLabel);
 upToolbar->addStretch();
 upToolbar->addWidget(buttonDetails);
 upToolbar->addWidget(buttonCancel);


 QVBoxLayout *vLayout = new QVBoxLayout;
 vLayout->addLayout(upToolbar);
 vLayout->addWidget(consoleOutput);

 buttonCancel->setFocus();

 setLayout(vLayout);
}


void ConsoleEmulator::setMessageText(QString text)
{
 messageLabel->setText(text);
}


void ConsoleEmulator::setConsoleOutput(QString text)
{
 consoleOutput->setPlainText(text);
 consoleOutput->moveCursor(QTextCursor::End);

 QScrollBar *v = consoleOutput->verticalScrollBar();
 v->setValue(v->maximum());
}


void ConsoleEmulator::clearConsoleOutput(void)
{
 consoleOutput->setPlainText("");
}


void ConsoleEmulator::addConsoleOutput(QString text)
{
 consoleOutput->setPlainText( consoleOutput->toPlainText()+text );
 consoleOutput->moveCursor(QTextCursor::End);

 QScrollBar *v = consoleOutput->verticalScrollBar();
 v->setValue(v->maximum());
}


void ConsoleEmulator::onCancelClick(void)
{
 qDebug() << "ConsoleEmulator::onCancelClick() : Click cancel";

 this->close(); // Будет сгенерировано событие closeEvent
}


void ConsoleEmulator::closeEvent(QCloseEvent *event)
{
 qDebug() << "ConsoleEmulator::closeEvent() : Detect close event";

 emit cancelConsole();

 event->accept();
}


// Переопределенный метод, вызываемый для обработки событий
bool ConsoleEmulator::event(QEvent *event)
{
  // qDebug() << "ConsoleEmulator::event:" << event->type();

  if(event->type()==QEvent::Hide)
  {
    qDebug() << "Emit signal dialogHide";
    emit dialogHide();
  }

  return QDialog::event(event);
}


void ConsoleEmulator::onDetailsClick(void)
{
 if(consoleOutput->isHidden())
  {
   consoleOutput->show();
   mytetraConfig.setSyncroConsoleDetails(true);
  }
 else
  {
   consoleOutput->hide();
   mytetraConfig.setSyncroConsoleDetails(false);
  }

 this->adjustSize();
}


void ConsoleEmulator::switchToErrorView(void)
{
 qDebug() << "ConsoleEmulator::switchToErrorView() : Detect error!";

 isError=true;

 actionLogger.addAction("syncroError");

 // Верхняя строка скрывается
 // QLayoutItem *child;
 // while ((child = upToolbar->takeAt(0)) != 0)
 //   child->widget()->hide();

 // Сообщение об обнаруженной ошибке
 messageLabel->setText("<b>"+tr("Commands running error")+"</b>");

 // Консольный вывод показывается, так как он возможно не был открыт, если не был выбран развернутый вид
 consoleOutput->show();
}

