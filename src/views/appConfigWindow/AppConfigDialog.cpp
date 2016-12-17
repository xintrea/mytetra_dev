#include <QStackedWidget>
#include <QDialog>
#include <QDebug>

#include "main.h"
#include "ConfigDialog.h"
#include "AppConfigDialog.h"
#include "AppConfigPage_Main.h"
#include "AppConfigPage_Crypt.h"
#include "AppConfigPage_Misc.h"
#include "AppConfigPage_Synchro.h"
#include "AppConfigPage_RecordTable.h"
#include "AppConfigPage_Attach.h"
#include "models/appConfig/AppConfig.h"

extern AppConfig mytetraConfig;


AppConfigDialog::AppConfigDialog(const QString &firstPageName, QWidget *parent)
    : QWidget(parent)
{
 if(mytetraConfig.getInterfaceMode()=="mobile") // if(true)
 {
   qDebug() << "Screen size X Y: " << getScreenSizeX() << getScreenSizeY();
   this->setMinimumSize(getScreenSizeX(), getScreenSizeY());
   this->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
 }

 configDialog=new ConfigDialog(this);
 configDialog->setWindowTitle(tr("MyTetra settings"));
 
 pageMain       =configDialog->addWidget(new AppConfigPage_Main(this),        tr("Main"));
 pageCrypt      =configDialog->addWidget(new AppConfigPage_Crypt(this),       tr("Crypt"));
 pageSynchro    =configDialog->addWidget(new AppConfigPage_Synchro(this),     tr("Synchro"));
 pageRecordTable=configDialog->addWidget(new AppConfigPage_RecordTable(this), tr("Note area"));
 pageAttach     =configDialog->addWidget(new AppConfigPage_Attach(this),      tr("Attaches"));
 pageMisc       =configDialog->addWidget(new AppConfigPage_Misc(this),        tr("Misc"));

 // После добавления страниц нужно сделать корректировку размера базового виджета
 configDialog->adjustSize();

 configDialog->updateListWidth();

 // Центрирование окна относительно родителя. TODO: может сделать сохранение/восстановление позиции.
 if (parent && parent->geometry().isValid())
 {
     int left = parent->geometry().center().x() - configDialog->geometry().width() / 2;
     int top = parent->geometry().center().y() - configDialog->geometry().height() / 2;
     move(left, top);
 }

 // Если указано имя виджета настроек, происходит переключение на него, иначе будет выбран первый виджет (pageMain)
 changePage(firstPageName);
}


void AppConfigDialog::execDialog()
{
    configDialog->exec();
}


// Переход на нужную страницу настроек (переход совершается кодом извне)
void AppConfigDialog::changePage(QString name)
{

 QListWidgetItem *item=NULL;

 if(name.isEmpty() || name=="pageMain") item=pageMain;
 if(name=="pageCrypt") item=pageCrypt;
 if(name=="pageSynchro") item=pageSynchro;
 if(name=="pageRecordTable") item=pageRecordTable;
 if(name=="pageAttach") item=pageAttach;
 if(name=="pageMisc") item=pageMisc;

 if(item!=NULL)
   configDialog->externalChangePage(item);
 else
   qDebug() << "AppConfigDialog::changePage cant find item for name: " << name;
}
