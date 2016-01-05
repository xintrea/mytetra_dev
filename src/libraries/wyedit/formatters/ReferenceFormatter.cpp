#include <QInputDialog>
#include <QDebug>
#include <QDesktopServices>

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
  QString refereceUrl=QInputDialog::getText(editor,
                                            tr("Reference or URL"),
                                            tr("Reference or URL:"),
                                            QLineEdit::Normal,
                                            href,
                                            &ok);
  if(!ok)
    return;

  // Устанавливается текст ссылки
  QTextCharFormat charFormat;
  charFormat.setAnchorHref(refereceUrl);

  // Если текст ссылки задан
  if(refereceUrl.length()>0)
  {
    charFormat.setAnchor(true);
    charFormat.setForeground(QApplication::palette().color(QPalette::Link));
    charFormat.setFontUnderline(true);

    textArea->textCursor().mergeCharFormat(charFormat);
  }
  else
  {
    // Иначе текст ссылки пустой и ссылку надо убрать
    charFormat.setAnchor(false);

    textArea->textCursor().setCharFormat(charFormat);
  }
}


// Действия при выборе контекстного меню редактора "Перейти по ссылке"
void ReferenceFormatter::onContextMenuGotoReference()
{
  QString href=editor->cursorPositionDetector->referenceHref(); // Текст ссылки

  if(href.length()>0)
  {
    QDesktopServices::openUrl(QUrl(href));
  }
}


void ReferenceFormatter::onClickedGotoReference(QString href)
{
  if(href.length()>0)
  {
    QDesktopServices::openUrl(QUrl(href));
  }
}

