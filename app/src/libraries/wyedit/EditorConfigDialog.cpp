#include <QStackedWidget>
#include <QDialog>

#include "main.h"
#include "views/appConfigWindow/ConfigDialog.h"

#include "EditorConfigDialog.h"
#include "EditorConfigFont.h"
#include "EditorConfigMisc.h"


EditorConfigDialog::EditorConfigDialog(QWidget *parent) : QWidget(parent)
{
 configDialog=new ConfigDialog();
 
 configDialog->setWindowTitle(tr("Editor settings"));
 
 configDialog->addWidget(new EditorConfigFont(this),tr("Fonts"));
 configDialog->addWidget(new EditorConfigMisc(this),tr("Misc"));
 
 configDialog->exec();
}

