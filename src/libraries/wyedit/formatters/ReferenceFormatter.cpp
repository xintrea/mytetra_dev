#include <QInputDialog>
#include <QDebug>

#include "ReferenceFormatter.h"

#include "main.h"
#include "../Editor.h"
#include "../EditorConfig.h"
#include "../EditorTextArea.h"
#include "../EditorToolBarAssistant.h"
#include "../../TraceLogger.h"


ReferenceFormatter::ReferenceFormatter()
{

}


// Редактирование ссылки
void ReferenceFormatter::onReferenceClicked(void)
{
  TRACELOG


  // Если нет выделения


  // Если есть выделение


  bool ok;
  QString text = QInputDialog::getText(editor,
                                       tr("Reference or URL"),
                                       tr("Reference or URL:"),
                                       QLineEdit::Normal,
                                       "",
                                       &ok);
  if(! (ok && !text.isEmpty()))
    return;
}


