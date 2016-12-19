#include "models/appConfig/AppConfig.h"
#include "AppConfigPage_Main.h"
#include "ui_AppConfigPage_Main.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QCommonStyle>

extern AppConfig mytetraConfig;


AppConfigPage_Main::AppConfigPage_Main(QWidget *parent)
    : ConfigPage(parent)
    , ui(new Ui::AppConfigPage_Main)
{
  ui->setupUi(this);
  setupUi();
  setupSignals();
}

AppConfigPage_Main::~AppConfigPage_Main()
{
  delete ui;
}


void AppConfigPage_Main::setupUi(void)
{
 qDebug() << "Create main config page";

 // Блок работы с путем до каталога данных
 ui->tetradirInput->setText(mytetraConfig.get_tetradir());

 // Блок работы с путем до корзины
 ui->trashdirInput->setText(mytetraConfig.get_trashdir());

 // Блок работы с размером корзины
 ui->trashsizeInput->setValue(static_cast<int>(mytetraConfig.get_trashsize()));

 // Блок работы с максимальным количеством файлов в корзине
 ui->trashmaxfilecountInput->setValue(mytetraConfig.get_trashmaxfilecount());

 // Блок работы с выбором языка интерфейса
 ui->interfaceLanguage->setMinimumContentsLength(2);
 ui->interfaceLanguage->addItem("en");
 ui->interfaceLanguage->addItem("ru");
 ui->interfaceLanguage->setCurrentIndex(ui->interfaceLanguage->findText(mytetraConfig.get_interfacelanguage(), Qt::MatchCaseSensitive));

 // Блок работы с отображением даты и времени
 QCommonStyle styleHelp;
 ui->dateTimeFormatHelpButton->setIcon( styleHelp.standardIcon(QStyle::SP_MessageBoxQuestion) );

 // Точка устанавливается возле того пункта, который настроен в конфиге
 if(mytetraConfig.getEnableCustomDateTimeFormat()==false)
 {
   ui->disableCustomDateTimeFormat->setChecked(true);
   ui->enableCustomDateTimeFormat->setChecked(false);
   ui->customDateTimeFormat->setEnabled(false);
 }
 else
 {
   // Если разрешен пользовательский формат даты и времени
   ui->disableCustomDateTimeFormat->setChecked(false);
   ui->enableCustomDateTimeFormat->setChecked(true);
   ui->customDateTimeFormat->setEnabled(true);
 }
 ui->customDateTimeFormat->setText( mytetraConfig.getCustomDateTimeFormat() );
}


void AppConfigPage_Main::setupSignals(void)
{
  connect(ui->tetradirButton,SIGNAL(clicked()),this,SLOT(onClickedTetradirSelectDialog()));
  connect(ui->trashdirButton,SIGNAL(clicked()),this,SLOT(onClickedTrashdirSelectDialog()));

  connect(ui->disableCustomDateTimeFormat, SIGNAL( toggled(bool) ), this, SLOT( onDisableCustomDateTimeFormatToggle(bool) ) );
  connect(ui->enableCustomDateTimeFormat, SIGNAL( toggled(bool) ), this, SLOT( onEnableCustomDateTimeFormatToggle(bool) ) );

  connect(ui->dateTimeFormatHelpButton, SIGNAL( clicked() ), this, SLOT( onDateTimeFormatHelpButton() ) );
}


// Действия при нажатии кнопки выбора директории данных
void AppConfigPage_Main::onClickedTetradirSelectDialog(void)
{
  QFileDialog tetradirSelectDialog(this);
  tetradirSelectDialog.setFileMode(QFileDialog::Directory);
  tetradirSelectDialog.setWindowTitle(tr("Select data directory"));
  tetradirSelectDialog.setDirectory(ui->tetradirInput->text());

  if( tetradirSelectDialog.exec() )
    if( !tetradirSelectDialog.directory().absolutePath().isEmpty() )
      ui->tetradirInput->setText( tetradirSelectDialog.directory().absolutePath() );
}


// Действия при нажатии кнопки выбора директории корзины
void AppConfigPage_Main::onClickedTrashdirSelectDialog(void)
{
  QFileDialog trashdirSelectDialog(this);
  trashdirSelectDialog.setFileMode(QFileDialog::Directory);
  trashdirSelectDialog.setWindowTitle(tr("Select trash directory"));
  trashdirSelectDialog.setDirectory(ui->trashdirInput->text());

  if( trashdirSelectDialog.exec() )
    if( !trashdirSelectDialog.directory().absolutePath().isEmpty() )
      ui->trashdirInput->setText( trashdirSelectDialog.directory().absolutePath() );
}


// Действия при нажатии на радиокнопку системных установок даты и времени
void AppConfigPage_Main::onDisableCustomDateTimeFormatToggle(bool checked)
{
  qDebug() << "In onDisableCustomDateTimeFormat(): " << checked;
  if(checked==true)
  {
    ui->customDateTimeFormat->setEnabled(false);
  }

}


// Действия при нажатии на радиокнопку пользовательских установок даты и времени
void AppConfigPage_Main::onEnableCustomDateTimeFormatToggle(bool checked)
{
  qDebug() << "In onEnableCustomDateTimeFormat(): " << checked;

  if(checked==true)
    ui->customDateTimeFormat->setEnabled(true);
}


void AppConfigPage_Main::onDateTimeFormatHelpButton(void)
{
  QString text="Format symbols:\n\
\n\
d: day 1-31, dd: day 01-31,\n\
ddd: day short name, dddd: day long name,\n\
M: month 1-12, MM: month 01-12,\n\
MMM: month short name, MMMM: month long name,\n\
yy: year 00-99, yyyy: year 0000-9999,\n\
h: hour 0-23, hh: hour 00-23,\n\
H: hour 0-23 AM/PM, HH: hour 00-23 AM/PM,\n\
n: minute 0-59, mm: minute 00-59,\n\
s: second 0-59, ss: second 00-59,\n\
AP: use AM/PM display, ap: use am/pm display,\n\
t: timezone\n\
\n\
Sample: dd.MM.yyyy - hh:mm:ss";

  QMessageBox msgBox;
  msgBox.setText(text);
  msgBox.exec();
}


// Метод должен возвращать уровень сложности сделанных изменений
// 0 - изменения не требуют перезапуска программы
// 1 - изменения требуют перезапуска программы
int AppConfigPage_Main::applyChanges(void)
{
 qDebug() << "Apply changes main";
 
 int difficultChanges=0;
 
 // Если был изменен путь к базе, он запоминается в конфигфайл
 if(mytetraConfig.get_tetradir()!=ui->tetradirInput->text())
  {
   QDir dir(ui->tetradirInput->text());

   // Проверяется, допустимо ли имя директории
   if(dir.isReadable()==false || dir.exists()==false)
    QMessageBox::warning(this, tr("Warning"),
                               tr("The data directory does not exists or unavailable for reading."),
                               QMessageBox::Ok); 
   else
    { 
     // Новое имя запоминается в конфиг
     mytetraConfig.set_tetradir(ui->tetradirInput->text());
     difficultChanges=1;
    } 
  } 

 
 // Если был изменен путь к корзине, он запоминается в конфигфайл
 if(mytetraConfig.get_trashdir()!=ui->trashdirInput->text())
  {
   QDir dir(ui->trashdirInput->text());

   // Проверяется, допустимо ли имя директории
   if(dir.isReadable()==false || dir.exists()==false)
    QMessageBox::warning(this, tr("Warning"),
                               tr("The trash directory does not exists or unavailable for reading."),
                               QMessageBox::Ok); 
   else
    { 
     // Новое имя запоминается в конфиг
     mytetraConfig.set_trashdir(ui->trashdirInput->text());
    } 
  } 
 
 
 // Если был изменен размер корзины
 if( mytetraConfig.get_trashsize() != static_cast<unsigned>(ui->trashsizeInput->value()) )
  {
   mytetraConfig.set_trashsize(static_cast<unsigned>(ui->trashsizeInput->value()));
  }

 
 // Если было изменено максимально возможное количество файлов в корзине
 if(mytetraConfig.get_trashmaxfilecount() != ui->trashmaxfilecountInput->value())
  {
   mytetraConfig.set_trashmaxfilecount(ui->trashmaxfilecountInput->value());
  }


 // Если было изменено использование пользовательского формата даты и времени
 if(mytetraConfig.getEnableCustomDateTimeFormat()!=ui->enableCustomDateTimeFormat->isChecked())
  {
   mytetraConfig.setEnableCustomDateTimeFormat(ui->enableCustomDateTimeFormat->isChecked());
  }


 // Если было изменен пользовательский формат даты и времени
 if(mytetraConfig.getCustomDateTimeFormat()!=ui->customDateTimeFormat->text())
  {
   mytetraConfig.setCustomDateTimeFormat(ui->customDateTimeFormat->text());
  }

 
 // Если был изменен язык
 if(mytetraConfig.get_interfacelanguage()!=ui->interfaceLanguage->currentText())
  {
   mytetraConfig.set_interfacelanguage(ui->interfaceLanguage->currentText());
   difficultChanges=1;
  }

 return difficultChanges;
}
