#include "EditorIndentSliderAssistant.h"
#include "indentslider/IndentSlider.h"
#include "EditorTextArea.h"
#include "main.h"


EditorIndentSliderAssistant::EditorIndentSliderAssistant(QObject *parent, EditorTextArea *iTextArea) : QObject(parent)
{
  if(parent==NULL)
    criticalError("Call "+QString(__FUNCTION__)+" with NULL of parent.");

  if(iTextArea==NULL)
    criticalError("Call "+QString(__FUNCTION__)+" with NULL of TextArea.");

  // Создается линейка отступов
  indentSlider=new IndentSlider(qobject_cast<QWidget *>(parent)->width(), 16, qobject_cast<QWidget *>(parent));
  indentSlider->setObjectName("editor_tb_indentslider");

  // Запоминается указатель на область редактирования
  textArea=iTextArea;

  clear();
  setupSignals();
}


EditorIndentSliderAssistant::~EditorIndentSliderAssistant()
{
  delete indentSlider;
}


void EditorIndentSliderAssistant::setupSignals()
{
  // Линейка отступов
  connect(indentSlider, &IndentSlider::change_textindent_pos,  this, &EditorIndentSliderAssistant::onChangeTextindentPos);
  connect(indentSlider, &IndentSlider::change_leftindent_pos,  this, &EditorIndentSliderAssistant::onChangeLeftindentPos);
  connect(indentSlider, &IndentSlider::change_rightindent_pos, this, &EditorIndentSliderAssistant::onChangeRightindentPos);
  connect(indentSlider, &IndentSlider::mouse_release,          this, &EditorIndentSliderAssistant::onMouseRelease);

  connect(this, &EditorIndentSliderAssistant::send_set_textindent_pos, indentSlider,&IndentSlider::set_textindent_pos);
  connect(this, &EditorIndentSliderAssistant::send_set_leftindent_pos, indentSlider,&IndentSlider::set_leftindent_pos);
  connect(this, &EditorIndentSliderAssistant::send_set_rightindent_pos,indentSlider,&IndentSlider::set_rightindent_pos);
}


void EditorIndentSliderAssistant::clear()
{
  currentTextIndent=0;
  currentLeftIndent=0;
  currentRightIndent=0;
}


IndentSlider *EditorIndentSliderAssistant::getIndentSlider()
{
  return indentSlider;
}


// Синхронизация размеров линейки отступов относительно области редактирования
void EditorIndentSliderAssistant::onUpdateGeometry()
{
  // Синхронизируется ширина виджета линейки отступов
  indentSlider->set_widget_width(textArea->width());

  // Синхронизируется геометрия линейки отступов
  int leftPos=textArea->get_indent_started_left();
  int rightPos=textArea->get_indent_started_right();
  indentSlider->set_indentline_left_pos(leftPos);
  indentSlider->set_indentline_right_pos(rightPos);

  indentSlider->update();
}


void EditorIndentSliderAssistant::updateToActualFormat(void)
{
  int i;

  i=(int)textArea->textCursor().blockFormat().textIndent();
  if(currentTextIndent!=i)
  {
    emit send_set_textindent_pos(i);
    currentTextIndent=i;
  }

  i=(int)textArea->textCursor().blockFormat().leftMargin();
  if(currentLeftIndent!=i)
  {
    emit send_set_leftindent_pos(i);
    currentLeftIndent=i;
  }

  i=(int)textArea->textCursor().blockFormat().rightMargin();
  if(currentRightIndent!=i)
  {
    emit send_set_rightindent_pos(i);
    currentRightIndent=i;
  }
}


// Действия при перемещении движка абзацного отступа
void EditorIndentSliderAssistant::onChangeTextindentPos(int i)
{
  // Создание форматирования
  QTextBlockFormat indentFormatting;
  indentFormatting.setTextIndent(i);

  // Форматирование для добавления отступа
  textArea->textCursor().mergeBlockFormat(indentFormatting);

  // Редактор запоминает отступ, чтобы было с чем сравнивать
  // при перемещении курсора со строки на строку
  currentTextIndent=i;

  textArea->show_indetedge(true);
  textArea->set_indentedge_pos(indentSlider->get_current_indentedge_pos()-textArea->lineWidth()-1);
}


// Действия при перемещении движка левого отступа
void EditorIndentSliderAssistant::onChangeLeftindentPos(int i)
{
  // Создание форматирования
  QTextBlockFormat indentFormatting;
  indentFormatting.setLeftMargin(i);

  // Форматирование для добавления отступа
  textArea->textCursor().mergeBlockFormat(indentFormatting);

  // Редактор запоминает отступ, чтобы было с чем сравнивать
  // при перемещении курсора со строки на строку
  currentLeftIndent=i;

  textArea->show_indetedge(true);
  textArea->set_indentedge_pos(indentSlider->get_current_indentedge_pos()-textArea->lineWidth()-1);
}


// Действия при перемещении движка правого отступа
void EditorIndentSliderAssistant::onChangeRightindentPos(int i)
{
  // Создание форматирования
  QTextBlockFormat indentFormatting;
  indentFormatting.setRightMargin(i);

  // Форматирование для добавления отступа
  textArea->textCursor().mergeBlockFormat(indentFormatting);

  // Редактор запоминает отступ, чтобы было с чем сравнивать
  // при перемещении курсора со строки на строку
  currentRightIndent=i;

  textArea->show_indetedge(true);
  textArea->set_indentedge_pos(indentSlider->get_current_indentedge_pos()-textArea->lineWidth()-1);
}


// Действия в момент отпускания кнопки мышки над линейкой отступов
void EditorIndentSliderAssistant::onMouseRelease(void)
{
  textArea->show_indetedge(false); // Скрывается вертикальная линия
  textArea->set_indentedge_pos(0); // Координата вертикальной линии обнуляется
}


void EditorIndentSliderAssistant::setVisible(bool flag)
{
  indentSlider->setVisible(flag);
}
