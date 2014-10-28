#include <QStackedWidget>
#include <QDialog>

#include "ConfigDialog.h"
#include "AppConfigDialog.h"
#include "AppConfigPage_Main.h"
#include "AppConfigPage_Crypt.h"
#include "AppConfigPage_Misc.h"
#include "AppConfigPage_Synchro.h"
#include "AppConfigPage_RecordTable.h"



AppConfigDialog::AppConfigDialog(void) : QWidget()
{
 ConfigDialog *configdialog=new ConfigDialog();
 
 configdialog->set_window_title(tr("MyTetra settings"));
 
 configdialog->add_widget(new AppConfigPage_Main,        tr("Main"));
 configdialog->add_widget(new AppConfigPage_Crypt,       tr("Crypt"));
 configdialog->add_widget(new AppConfigPage_Synchro,     tr("Synchro"));
 configdialog->add_widget(new AppConfigPage_RecordTable, tr("Note area"));
 configdialog->add_widget(new AppConfigPage_Misc,        tr("Misc"));

 configdialog->updateListWidth();

 configdialog->exec();
}

