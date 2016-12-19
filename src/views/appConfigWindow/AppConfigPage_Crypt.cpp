#include "AppConfigPage_Crypt.h"
#include "ui_AppConfigPage_Crypt.h"
#include "models/appConfig/AppConfig.h"
#include "libraries/GlobalParameters.h"
#include "views/enterPassword/EnterPassword.h"
#include "models/dataBaseConfig/DataBaseConfig.h"
#include "libraries/crypt/Password.h"


extern AppConfig mytetraConfig;
extern GlobalParameters globalParameters;
extern DataBaseConfig dataBaseConfig;


AppConfigPage_Crypt::AppConfigPage_Crypt(QWidget *parent)
    : ConfigPage(parent)
    , ui(new Ui::AppConfigPage_Crypt)
{
  qDebug() << "Create crypt config page";
  ui->setupUi(this);

  setupUi();
  updateUi();
  setupSignals();
}

AppConfigPage_Crypt::~AppConfigPage_Crypt()
{
  delete ui;
}


void AppConfigPage_Crypt::setupUi(void)
{
  // Точка устанавливается возле того пункта, который настроен в конфиге
  if(mytetraConfig.get_howpassrequest()=="atClickOnCryptBranch")
   ui->howPassRequestRadio1->setChecked(true);
  else
   ui->howPassRequestRadio2->setChecked(true);

  if(mytetraConfig.getPasswordSaveFlag())
   {
    ui->howPassRequestRadio1->setEnabled(false);
    ui->howPassRequestRadio2->setEnabled(false);
    ui->passwordSaveEnable->setChecked(true);
   }
  else
   {
    ui->howPassRequestRadio1->setEnabled(true);
    ui->howPassRequestRadio2->setEnabled(true);
    ui->passwordSaveEnable->setChecked(false);
   }

  ui->autoClosePasswordDelay->setValue(mytetraConfig.get_autoClosePasswordDelay());

  // Устанавливается галка и активность виджета выбора задержки
  onAutoClosePasswordEnableToggle( mytetraConfig.get_autoClosePasswordEnable() );

  // Устанавливается галка согласно настройке из файла конфигурации
  ui->decryptFileToTrashDirectoryEnable->setChecked( mytetraConfig.getEnableDecryptFileToTrashDirectory() );
}


void AppConfigPage_Crypt::updateUi(void)
{
  // Строка состояния пароля
  ui->passRetrieveStatus->setText(getRetrieveStatusText());

  // Кнопка запроса введения пароля
  ui->passRetrieveButton->setText(getRetrieveButtonText());

  // Строка с аннотацией
  ui->passRetrieveAnnotation->setText(getRetrieveAnnotationText());
}


void AppConfigPage_Crypt::setupSignals(void)
{
 // При нажатии кнопки работы с паролем
 connect(ui->passRetrieveButton, SIGNAL(clicked()), SLOT(onPassRetrieveButtonClicked()));

 // При клике на галку разрешения автозакрытия пароля
 connect(ui->autoClosePasswordEnable, SIGNAL(toggled(bool)), SLOT(onAutoClosePasswordEnableToggle(bool)));

 connect(ui->passwordSaveEnable, SIGNAL(toggled(bool)), SLOT(onPasswordSaveEnableToggle(bool)));
}


QString AppConfigPage_Crypt::getRetrieveStatusText(void)
{
 QString status=tr("<b>Status:</b> ");

 // Если в хранилище данных вообще не задан пароль
 if(dataBaseConfig.get_crypt_mode()==0)
  status += dataBaseConfig.get_crypt_mode()==0 ? tr("No password is set. ") : tr("Password is set. ");

 // Если пароль (точнее хеш пароля) хранится локально
 if(mytetraConfig.getPasswordSaveFlag() && !mytetraConfig.getPasswordMiddleHash().isEmpty())
  status += tr("Password is saved locally. ");

 return status;
}


QString AppConfigPage_Crypt::getRetrieveButtonText(void)
{
 // Если в хранилище данных вообще не задан пароль
 return dataBaseConfig.get_crypt_mode()==0 ? tr("Set a password") : tr("Change password");
}


QString AppConfigPage_Crypt::getRetrieveAnnotationText(void)
{
 // Если в хранилище данных вообще не задан пароль
 return dataBaseConfig.get_crypt_mode()==0 ?
    tr("A password will be used to encrypt the item that you selected. Use \"Encrypt item\" or \"Decrypt item\" in context menu.") :
    tr("If you change your password all encrypted item will be re-encrypted with a new password.");
}


// Действия при нажатии кнопки работы с паролем
void AppConfigPage_Crypt::onPassRetrieveButtonClicked(void)
{
 // Если в хранилище данных вообще не задан пароль
 if(dataBaseConfig.get_crypt_mode()==0)
  {
   // Включается диалог запроса пароля "с нуля"

   Password password;

   if(password.retrievePassword()==false) return;

   updateUi();
   return;
  }
 else
  {
   // Включается диалог изменения пароля
   Password password;

   password.replacePassword();
  }
 
}


void AppConfigPage_Crypt::onPasswordSaveEnableToggle(bool checked)
{
 if(checked)
  {
   ui->howPassRequestRadio1->setEnabled(false);
   ui->howPassRequestRadio2->setEnabled(false);
  }
 else
  {
   ui->howPassRequestRadio1->setEnabled(true);
   ui->howPassRequestRadio2->setEnabled(true);
  }
}


// Действие при клике на галку настройки автоматического закрытия окна пароля
void AppConfigPage_Crypt::onAutoClosePasswordEnableToggle(bool checked)
{
 // Устанавливается галка и активность виджета выбора задержки
 if(checked)
  {
   ui->autoClosePasswordEnable->setChecked(true);
   ui->autoClosePasswordDelay->setEnabled(true);
  }
 else
  {
   ui->autoClosePasswordEnable->setChecked(false);
   ui->autoClosePasswordDelay->setEnabled(false);
  }
}


// Метод должен возвращать уровень сложности сделанных изменений
// 0 - изменения не требуют перезапуска программы
// 1 - изменения требуют перезапуска программы
int AppConfigPage_Crypt::applyChanges(void)
{
 qDebug() << "Apply changes crypt";

 if(ui->howPassRequestRadio1->isChecked() &&
    mytetraConfig.get_howpassrequest()=="atStartProgram")
  mytetraConfig.set_howpassrequest("atClickOnCryptBranch");

 if(ui->howPassRequestRadio2->isChecked() &&
    mytetraConfig.get_howpassrequest()=="atClickOnCryptBranch")
  mytetraConfig.set_howpassrequest("atStartProgram");

 if(ui->autoClosePasswordEnable->isChecked()!=mytetraConfig.get_autoClosePasswordEnable())
  mytetraConfig.set_autoClosePasswordEnable( ui->autoClosePasswordEnable->isChecked() );

 if(ui->autoClosePasswordDelay->value()!=mytetraConfig.get_autoClosePasswordDelay())
  mytetraConfig.set_autoClosePasswordDelay( ui->autoClosePasswordDelay->value() );

 if(ui->passwordSaveEnable->isChecked()!=mytetraConfig.getPasswordSaveFlag())
  {
   mytetraConfig.setPasswordSaveFlag( ui->passwordSaveEnable->isChecked() );
   
   // Если галка установлена что хранить локально пароль ненужно
   if(!ui->passwordSaveEnable->isChecked())
    {
     // Промежуточный хеш пароля удаляется 
     mytetraConfig.setPasswordMiddleHash("");
    }
  }

 if(ui->decryptFileToTrashDirectoryEnable->isChecked()!=mytetraConfig.getEnableDecryptFileToTrashDirectory())
   mytetraConfig.setEnableDecryptFileToTrashDirectory( ui->decryptFileToTrashDirectoryEnable->isChecked() );

 return 0;
}
