#include <QPainter>
#include <QPen>

#include "OverTextToolButton.h"


OverTextToolButton::OverTextToolButton(QWidget *parent) : QToolButton(parent)
{
  overText="";
}


void OverTextToolButton::setOverText(QString iText)
{
  overText=iText;

  this->update();
}


void OverTextToolButton::paintEvent(QPaintEvent *event)
{
  QToolButton::paintEvent(event);

  QPainter painter(this); // Создаём новый объект рисовальщика

  /*
  // Проверочная диагональная линия
  painter.setPen(QPen(Qt::red,1,Qt::SolidLine)); // Настройки рисования
  painter.drawLine(0, 0, width(), height()); // Рисование линии
  */

  painter.drawText( 0, 0, width(), height(), Qt::AlignCenter, overText);

}
