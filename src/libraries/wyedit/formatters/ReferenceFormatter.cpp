#include <QInputDialog>
#include <QDebug>

#include "ReferenceFormatter.h"

#include "main.h"
#include "../Editor.h"
#include "../EditorConfig.h"
#include "../EditorTextArea.h"
#include "../EditorToolBarAssistant.h"
#include "../EditorCursorPositionDetector.h"
#include "../../TraceLogger.h"


ReferenceFormatter::ReferenceFormatter()
{

}


// Редактирование ссылки
void ReferenceFormatter::onReferenceClicked(void)
{
  TRACELOG

  QString href="";

  // Если курсор установлен на ссылке
  if(editor->cursorPositionDetector->isCursorOnReference())
    href=editor->cursorPositionDetector->referenceHref(); // Текст ссылки

  bool ok;
  QString text = QInputDialog::getText(editor,
                                       tr("Reference or URL"),
                                       tr("Reference or URL:"),
                                       QLineEdit::Normal,
                                       href,
                                       &ok);
  if(! (ok && !text.isEmpty()))
    return;
}


