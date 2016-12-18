#include <QStackedWidget>
#include <QDialog>

#include "main.h"
#include "views/appConfigWindow/ConfigDialog.h"

#include "EditorConfigDialog.h"
#include "EditorConfigFont.h"
#include "EditorConfigMisc.h"


EditorConfigDialog::EditorConfigDialog(QWidget *parent) : QWidget(parent)
{
  configDialog=new ConfigDialog(this);

  configDialog->setWindowTitle(tr("Editor settings"));

  QListWidgetItem* pageFonts = configDialog->addWidget(new EditorConfigFont(this),tr("Fonts"));
  configDialog->addWidget(new EditorConfigMisc(this),tr("Misc"));

  configDialog->adjustSize();

  if (parent && parent->geometry().isValid())
  {
      int left = parent->geometry().center().x() - configDialog->geometry().width() / 2;
      int top = parent->geometry().center().y() - configDialog->geometry().height() / 2;
      move(left, top);
  }

  configDialog->externalChangePage(pageFonts);
}

void EditorConfigDialog::execDialog()
{
  configDialog->exec();
}
