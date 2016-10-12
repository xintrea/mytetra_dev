#include <QVBoxLayout>
#include <QDebug>

#include "EditorShowText.h"


EditorShowText::EditorShowText(QWidget *parent) : QDialog(parent)
{
  geomX=0;
  geomY=0;
  geomW=0;
  geomH=0;

  setupUi();
  setupSignals();
  assembly();
}

EditorShowText::~EditorShowText()
{

}

void EditorShowText::setupUi()
{
  QSizePolicy sizePolicy;
  sizePolicy.setHorizontalPolicy(QSizePolicy::Expanding);
  sizePolicy.setVerticalPolicy(QSizePolicy::Expanding);

  setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint); // Скрывается кнопка с вопросом

  textArea=new QTextEdit(this);
  textArea->setAcceptRichText(true);
  textArea->setSizePolicy(sizePolicy);

  textArea->setReadOnly(true); // Показываемый текст можно только просматривать
}


void EditorShowText::setupSignals()
{

}


void EditorShowText::assembly()
{
  QVBoxLayout *mainLayout=new QVBoxLayout(this);
  mainLayout->setContentsMargins(0,0,0,0);

  // Добавляется область текста
  mainLayout->addWidget(textArea);
}


void EditorShowText::setHtml(QString text)
{
  textArea->setHtml(text);
}


void EditorShowText::setDocument(QTextDocument *document)
{
  textArea->setDocument(document);
}


void EditorShowText::hideEvent(QHideEvent *event)
{
  qDebug() << "Hide event of EditorShowText dialog, window X " << this->x() << " Y " << this->y() << " W " << this->width() << " H " << this->height();

  if( this->width()>0 && this->height()>0 && !(this->x()<=0 && this->y()<=0) )
  {
    // Запоминается геометрия
    QRect g=this->frameGeometry();
    qDebug() << "Frame geometry X " << g.x() << " Y " << g.y() << " W " << g.width() << " H " << g.height();
    geomX=g.x();
    geomY=g.y();
    geomW=g.width();
    geomH=g.height();
  }

  QWidget::hideEvent(event);
}


void EditorShowText::showEvent(QShowEvent *event)
{
  qDebug() << "Show event of EditorShowText dialog";

  // Если была запомнена геометрия окна, устанавливается прежняя геометрия
  if(!(geomX==0 && geomY==0 && geomW==0 && geomH==0))
  {
    this->move(geomX,geomY);
  }
  else
    qDebug() << "Previos geometry of EditorShowText dialog is not setted";

  QWidget::showEvent(event);
}


bool EditorShowText::event(QEvent *event)
{
  qDebug() << "EditorShowText event: " << event->type();

  return false;
}
