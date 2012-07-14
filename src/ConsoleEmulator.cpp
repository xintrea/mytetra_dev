#include <QScrollBar>
#include <QWidget>

#include "ConsoleEmulator.h"
#include "main.h"
#include "AppConfig.h"
#include "FindScreen.h"

extern AppConfig mytetraconfig;


ConsoleEmulator::ConsoleEmulator(QWidget *parent) : QWidget(parent)
{
 setupUI();
 setupSignals();
 assembly();
}


ConsoleEmulator::~ConsoleEmulator()
{

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
 if(mytetraconfig.getSyncroConsoleDetails()==false)
  consoleOutput->hide();

 waitClock=new WaitClock(this);
}


void ConsoleEmulator::setupSignals(void)
{
 connect(buttonCancel, SIGNAL(clicked()), this, SLOT(onCancelClick()));
 connect(buttonDetails, SIGNAL(clicked()), this, SLOT(onDetailsClick()));
}


void ConsoleEmulator::assembly(void)
{
 QHBoxLayout *hLayout = new QHBoxLayout;
 hLayout->addWidget(waitClock);
 hLayout->addWidget(messageLabel);
 hLayout->addWidget(buttonDetails);
 hLayout->addWidget(buttonCancel);


 QVBoxLayout *vLayout = new QVBoxLayout;
 vLayout->addLayout(hLayout);
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


void ConsoleEmulator::onDetailsClick(void)
{
 if(consoleOutput->isHidden())
  {
   consoleOutput->show();
   mytetraconfig.setSyncroConsoleDetails(true);
  }
 else
  {
   consoleOutput->hide();
   mytetraconfig.setSyncroConsoleDetails(false);
  }

 this->adjustSize();
}
