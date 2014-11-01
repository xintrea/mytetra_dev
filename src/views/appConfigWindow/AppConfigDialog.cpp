#include <QStackedWidget>
#include <QDialog>
#include <QDebug>

#include "ConfigDialog.h"
#include "AppConfigDialog.h"
#include "AppConfigPage_Main.h"
#include "AppConfigPage_Crypt.h"
#include "AppConfigPage_Misc.h"
#include "AppConfigPage_Synchro.h"
#include "AppConfigPage_RecordTable.h"


AppConfigDialog::AppConfigDialog(QString firstPageName="") : QWidget()
{
 configDialog=new ConfigDialog();
 configDialog->set_window_title(tr("MyTetra settings"));
 
 pageMain       =configDialog->add_widget(new AppConfigPage_Main(this),        tr("Main"));
 pageCrypt      =configDialog->add_widget(new AppConfigPage_Crypt(this),       tr("Crypt"));
 pageSynchro    =configDialog->add_widget(new AppConfigPage_Synchro(this),     tr("Synchro"));
 pageRecordTable=configDialog->add_widget(new AppConfigPage_RecordTable(this), tr("Note area"));
 pageMisc       =configDialog->add_widget(new AppConfigPage_Misc(this),        tr("Misc"));

 configDialog->updateListWidth();

 if(firstPageName.size()>0)
   changePage(firstPageName);

 configDialog->exec();
}


// Переход на нужную страницу настроек (переход совершается кодом извне)
void AppConfigDialog::changePage(QString name)
{

 QListWidgetItem *item=NULL;

 if(name=="pageMain") item=pageMain;
 if(name=="pageCrypt") item=pageCrypt;
 if(name=="pageSynchro") item=pageSynchro;
 if(name=="pageRecordTable") item=pageRecordTable;
 if(name=="pageMisc") item=pageMisc;

 if(item!=NULL)
   configDialog->externalChangePage(item);
 else
   qDebug() << "AppConfigDialog::changePage cant find item for name: " << name;
}
