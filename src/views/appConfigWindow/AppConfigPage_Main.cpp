#include <QString>
#include <QWidget>
#include <QLabel>
#include <QBoxLayout>
#include <QDir>
#include <QLineEdit>
#include <QAbstractSpinBox>
#include <QToolButton>
#include <QFileDialog>
#include <QMessageBox>

#include "AppConfigPage_Main.h"
#include "models/appConfig/AppConfig.h"

extern AppConfig mytetraConfig;


AppConfigPage_Main::AppConfigPage_Main(QWidget *parent) : ConfigPage(parent)
{
 qDebug() << "Create main config page";
 
 
 // Блок работы с путем до каталога данных
 QLabel *tetradirLabel=new QLabel(this);
 tetradirLabel->setText(tr("Data directory"));

 tetradirInput=new QLineEdit(this);
 tetradirInput->setMinimumWidth(350);
 tetradirInput->setText(mytetraConfig.get_tetradir());

 QToolButton *tetradirButton=new QToolButton(this);
 tetradirButton->setText(tr("..."));
 connect(tetradirButton,SIGNAL(clicked()),this,SLOT(open_tetradir_select_dialog()));

 QHBoxLayout *tetradirLayout=new QHBoxLayout();
 tetradirLayout->addWidget(tetradirInput);
 tetradirLayout->addWidget(tetradirButton);
 

 // Блок работы с путем до корзины
 QLabel *trashdirLabel=new QLabel(this);
 trashdirLabel->setText(tr("Trash directory"));

 trashdirInput=new QLineEdit(this);
 trashdirInput->setMinimumWidth(350);
 trashdirInput->setText(mytetraConfig.get_trashdir());

 QToolButton *trashdirButton=new QToolButton(this);
 trashdirButton->setText(tr("..."));
 connect(trashdirButton,SIGNAL(clicked()),this,SLOT(open_trashdir_select_dialog()));

 QHBoxLayout *trashdirLayout=new QHBoxLayout();
 trashdirLayout->addWidget(trashdirInput);
 trashdirLayout->addWidget(trashdirButton);

 
 // Блок работы с размером корзины
 QLabel *trashsizeLabel=new QLabel(this);
 trashsizeLabel->setText(tr("Trash size"));
 
 trashsizeInput=new QSpinBox(this);
 trashsizeInput->setRange(1,1000);
 trashsizeInput->setValue(mytetraConfig.get_trashsize());
 
 QLabel *trashsizeFlexion=new QLabel(this);
 trashsizeFlexion->setText(tr("Mb"));

 QHBoxLayout *trashsizeLayout=new QHBoxLayout();
 trashsizeLayout->addWidget(trashsizeInput);
 trashsizeLayout->addWidget(trashsizeFlexion);


 // Блок работы с максимальным количеством файлов в корзине
 QLabel *trashmaxfilecountLabel=new QLabel(this);
 trashmaxfilecountLabel->setText(tr("Maximum files in trash"));
 
 trashmaxfilecountInput=new QSpinBox(this);
 trashmaxfilecountInput->setRange(1,3000);
 trashmaxfilecountInput->setValue(mytetraConfig.get_trashmaxfilecount());
 
 QLabel *trashmaxfilecountFlexion=new QLabel(this);
 trashmaxfilecountFlexion->setText(tr("files"));

 QHBoxLayout *trashmaxfilecountLayout=new QHBoxLayout();
 trashmaxfilecountLayout->addWidget(trashmaxfilecountInput);
 trashmaxfilecountLayout->addWidget(trashmaxfilecountFlexion);


 // Блок работы с выбором языка интерфейса
 QLabel *interfaceLanguageLabel=new QLabel(this);
 interfaceLanguageLabel->setText(tr("Language"));

 interfaceLanguage=new MtComboBox(this);
 interfaceLanguage->setMinimumContentsLength(2);
 interfaceLanguage->addItem("en");
 interfaceLanguage->addItem("ru");
 interfaceLanguage->setCurrentIndex(interfaceLanguage->findText(mytetraConfig.get_interfacelanguage(), Qt::MatchCaseSensitive));

 QHBoxLayout *interfaceLanguageLayout=new QHBoxLayout();
 interfaceLanguageLayout->addWidget(interfaceLanguage);
 interfaceLanguageLayout->addStretch();


 // Сборка всех блоков в окно
 QGridLayout *dirLayout=new QGridLayout();
 dirLayout->addWidget(tetradirLabel,0,0);
 dirLayout->addLayout(tetradirLayout,0,1);
 dirLayout->addWidget(trashdirLabel,1,0);
 dirLayout->addLayout(trashdirLayout,1,1);


 QGridLayout *otherSettingLayout=new QGridLayout();

 otherSettingLayout->addWidget(trashsizeLabel,0,0);
 otherSettingLayout->addLayout(trashsizeLayout,0,1);

 otherSettingLayout->addWidget(trashmaxfilecountLabel,1,0);
 otherSettingLayout->addLayout(trashmaxfilecountLayout,1,1);

 otherSettingLayout->addWidget(interfaceLanguageLabel,2,0);
 otherSettingLayout->addLayout(interfaceLanguageLayout,2,1);

 otherSettingLayout->setColumnStretch(1,100);


 QVBoxLayout *centralLayout=new QVBoxLayout();
 centralLayout->addLayout(dirLayout);
 centralLayout->addLayout(otherSettingLayout);
 centralLayout->addStretch();
 
 setLayout(centralLayout);
}


// Действия при нажатии кнопки выбора директории данных
void AppConfigPage_Main::open_tetradir_select_dialog(void)
{
 QFileDialog tetradirSelectDialog(this);
 tetradirSelectDialog.setFileMode(QFileDialog::Directory);
 tetradirSelectDialog.setWindowTitle(tr("Select data directory"));
 tetradirSelectDialog.setDirectory(tetradirInput->text());
 
 tetradirSelectDialog.exec();
 
 tetradirInput->setText(tetradirSelectDialog.directory().absolutePath());
}


// Действия при нажатии кнопки выбора директории корзины
void AppConfigPage_Main::open_trashdir_select_dialog(void)
{
 QFileDialog trashdirSelectDialog(this);
 trashdirSelectDialog.setFileMode(QFileDialog::Directory);
 trashdirSelectDialog.setWindowTitle(tr("Select trash directory"));
 trashdirSelectDialog.setDirectory(trashdirInput->text());
 
 trashdirSelectDialog.exec();
 
 trashdirInput->setText(trashdirSelectDialog.directory().absolutePath());
}

// Метод должен возвращать уровень сложности сделанных изменений
// 0 - изменения не требуют перезапуска программы
// 1 - изменения требуют перезапуска программы
int AppConfigPage_Main::apply_changes(void)
{
 qDebug() << "Apply changes main";
 
 int difficultChanges=0;
 
 // Если был изменен путь к базе, он запоминается в конфигфайл
 if(mytetraConfig.get_tetradir()!=tetradirInput->text())
  {
   QDir dir(tetradirInput->text());

   // Проверяется, допустимо ли имя директории
   if(dir.isReadable()==false || dir.exists()==false)
    QMessageBox::warning(this, tr("Warning"),
                               tr("The data directory does not exists or unavailable for reading."),
                               QMessageBox::Ok); 
   else
    { 
     // Новое имя запоминается в конфиг
     mytetraConfig.set_tetradir(tetradirInput->text());
     difficultChanges=1;
    } 
  } 

 
 // Если был изменен путь к корзине, он запоминается в конфигфайл
 if(mytetraConfig.get_trashdir()!=trashdirInput->text())
  {
   QDir dir(trashdirInput->text());

   // Проверяется, допустимо ли имя директории
   if(dir.isReadable()==false || dir.exists()==false)
    QMessageBox::warning(this, tr("Warning"),
                               tr("The trash directory does not exists or unavailable for reading."),
                               QMessageBox::Ok); 
   else
    { 
     // Новое имя запоминается в конфиг
     mytetraConfig.set_trashdir(trashdirInput->text());
    } 
  } 
 
 
 // Если был изменен размер корзины
 if( (int)mytetraConfig.get_trashsize() != (int)trashsizeInput->text().toInt() )
  {
   mytetraConfig.set_trashsize(trashsizeInput->text().toInt());
  }

 
 // Если было изменено максимально возможное количество файлов в корзине
 if(mytetraConfig.get_trashmaxfilecount()!=trashmaxfilecountInput->text().toInt())
  {
   mytetraConfig.set_trashmaxfilecount(trashmaxfilecountInput->text().toInt());
  }

 
 // Если был изменен язык
 if(mytetraConfig.get_interfacelanguage()!=interfaceLanguage->currentText())
  {
   mytetraConfig.set_interfacelanguage(interfaceLanguage->currentText());
   difficultChanges=1;
  }

 return difficultChanges;
}
