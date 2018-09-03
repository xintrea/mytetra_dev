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


/*
// Говорят, что hideEvent/showEvent не относится к свертке/развертке, т.е. "свернутые" окна по-прежнему считаются видимыми.
// Поэтому надо ловить QWindowStateChangeEvent. Но это не подтвердилось в KDE.
// В нем нормально отрабатывают hideEvent/showEvent при сворачивании-разворачивании основного окна
// События hideEvent/showEvent не генерируются в Windows 7 (в других не проверено), но в нем решаемым этим кодом проблеммы нет
void EditorShowText::changeEvent(QEvent *event)
{
  qDebug() << "EditorShowText changeEvent: " << event->type();

  if(event->type() == QEvent::WindowStateChange) {
    QWindowStateChangeEvent* WindowStateChangeEvent = (QWindowStateChangeEvent*)event;
    if(!(WindowStateChangeEvent->oldState() & Qt::WindowMaximized) && isMaximized())
    {
      geomX=x();
      geomY=y();
      geomW=width();
      geomH=height();
    }
  }
}
*/
