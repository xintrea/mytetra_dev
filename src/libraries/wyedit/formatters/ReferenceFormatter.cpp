#include <QInputDialog>
#include <QDebug>
#include <QDesktopServices>

#include "ReferenceFormatter.h"

#include "main.h"
#include "views/mainWindow/MainWindow.h"
#include "views/tree/KnowTreeView.h"
#include "models/tree/KnowTreeModel.h"
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
  // TRACELOG

  QString href="";

  // Если курсор установлен на ссылке
  if(editor->cursorPositionDetector->isCursorOnReference())
    href=editor->cursorPositionDetector->referenceHref(); // Выясняется текст ссылки

  // Создание виджета запроса URL с указанием редактора как родительского виджета
  QInputDialog inputDialog(editor);

  // Установка ширины виджета запроса URL
  int dialogWidth=int(0.8*(float)textArea->width());
  inputDialog.setMinimumWidth( dialogWidth );
  inputDialog.resize(inputDialog.size());

  inputDialog.setWindowTitle(tr("Reference or URL"));
  inputDialog.setLabelText(tr("Reference or URL"));
  inputDialog.setTextValue(href);
  inputDialog.setTextEchoMode(QLineEdit::Normal);

  bool ok=inputDialog.exec();
  QString refereceUrl=inputDialog.textValue();

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

  onClickedGotoReference(href);
}


void ReferenceFormatter::onClickedGotoReference(QString href)
{
  if(href.length()==0)
    return;

  // Если клик по обычной ссылке
  if(!isHrefInternal(href))
  {
    QDesktopServices::openUrl(QUrl(href));
  }
  else
  {
    // Иначе клик по внутренней ссылке

    // Пролучение ID из ссылки
    QString recordId=getIdFromInternalHref(href);

    // todo: вынести следующий код в отдельный метод главного окна

    // Нахождение ветки, в которой лежит данная запись
    QStringList pathToRecord=static_cast<KnowTreeModel*>(find_object<KnowTreeView>("knowTreeView")->model())->getRecordPath(recordId);

    find_object<MainWindow>("mainwindow")->setTreePosition( pathToRecord );
    find_object<MainWindow>("mainwindow")->setRecordtablePositionById( recordId );
  }
}


bool ReferenceFormatter::isHrefInternal(QString href)
{
  if(href.contains(QRegExp("^mytetra:\\/\\/note\\/\\w+$")))
    return true;
  else
    return false;
}


QString ReferenceFormatter::getIdFromInternalHref(QString href)
{
  if(!isHrefInternal(href))
    return "";

  href.replace(QRegExp("^mytetra:\\/\\/note\\/"), "");

  return href;
}


// Слот используется для "открепления" от ссылки, то есть чтобы при нажатии пробела после ссылки, ссылка не продолжала "тянуться"
void ReferenceFormatter::onTextChanged(void)
{
  // TRACELOG

  // Создается дополнительный курсор как копия основного курсора
  QTextCursor cursor=textArea->textCursor();

  // Запоминается его позиция
  int cursorPosition=cursor.position();

  // Выделяется последний символ
  cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);
  QString prevCharacterAsString=cursor.selectedText();
  QChar prevCharacter;
  if(prevCharacterAsString.length()==1)
    prevCharacter=prevCharacterAsString[0];

  // qDebug() << "Prev char: [" << prevCharacter << "]";

  // Если последний символ не является пробелом или символом-разделителем
  if( !prevCharacter.isSpace() )
    return;


  // Дополнительный курсор снова устанавливается на начальную позицию
  cursor.setPosition(cursorPosition);
  cursor.movePosition(QTextCursor::PreviousCharacter); // И смещается на одну позицию назад

  // Наличие форматирования ссылкой у предыдущего, текущего и последующего символа
  bool anchorPrevious=false;
  bool anchorCurrent=false;
  bool anchorNext=false;

  // Выясняется форматирование предыдущего символа
  anchorPrevious=cursor.charFormat().isAnchor();

  // Выясняется форматирование текущего символа
  cursor.movePosition(QTextCursor::NextCharacter);
  anchorCurrent=cursor.charFormat().isAnchor();

  // Выясняется форматирование последующего символа
  cursor.movePosition(QTextCursor::NextCharacter);
  anchorNext=cursor.charFormat().isAnchor();

  // Если предыдущий и текущий сивол имеют форматирование ссылки, а последующий - обычный
  if(anchorPrevious && anchorCurrent && !anchorNext)
  {
    QTextCharFormat charFormat;

    // Текущий символ становится обычным
    charFormat.setAnchor(false);
    charFormat.setForeground(QApplication::palette().color(QPalette::Text));
    charFormat.setFontUnderline(false);

    // Дополнительный курсор снова устанавливается на начальную позицию
    cursor.setPosition(cursorPosition);
    cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);

    cursor.mergeCharFormat(charFormat);
  }

}


// Временно не используется
/*
void ReferenceFormatter::onCursorPositionChanged(void)
{
  // TRACELOG

}


// При изменении документа

void ReferenceFormatter::onContentsChange(int position, int charsRemoved, int charsAdded)
{
  // TRACELOG

}
*/
