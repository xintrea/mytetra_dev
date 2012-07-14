#include <QPainter>
#include <QPen>
#include <QtDebug>

#include "IndentSlider.h"
#include "../Editor.h"


IndentSlider::IndentSlider(int width, int height, QWidget *parent) : QWidget(parent)
{
 // Устанавливаются начальные размеры виджета
 set_widget_geometry(width,height);

 indentline_left_pos=0;
 indentline_right_pos=0;
 indentline_height=10;

 textindent_pos=0;
 leftindent_pos=0;
 rightindent_pos=0;

 indenttriangle_width=8;

 current_sel_indent=SEL_NONE;
}


// Установка геометрии виджета
void IndentSlider::set_widget_geometry(int width, int height)
{
 set_widget_width(width);
 set_widget_height(height);
}


void IndentSlider::set_widget_width(int width)
{
 widget_width=width;

 // Ширину жёстко устанавливать нельзя, закомментировано
 // setMinimumWidth(width);
 // setMaximumWidth(width);
}


void IndentSlider::set_widget_height(int height)
{
 widget_height=height;

 setMinimumHeight(height+1);
 setMaximumHeight(height+1);
}


void IndentSlider::set_indentline_left_pos(int i)
{
 indentline_left_pos=i;
}


void IndentSlider::set_indentline_right_pos(int i)
{
 indentline_right_pos=i;
}


void IndentSlider::set_indentline_height(int i)
{
 indentline_height=i;
}


// Абсолютные координаты движка абзацного отступа
int IndentSlider::get_geometry_textindent_pos(void)
{
 return indentline_left_pos+leftindent_pos+textindent_pos;
}


// Абсолютные координаты движка левого отступа
int IndentSlider::get_geometry_leftindent_pos(void)
{
 return indentline_left_pos+leftindent_pos;
}


// Абсолютные координаты движка правого отступа
int IndentSlider::get_geometry_rightindent_pos(void)
{
 return indentline_right_pos-rightindent_pos;
}


void IndentSlider::resizeEvent(QResizeEvent *event)
{
 Q_UNUSED( event );
 qDebug() << "Textarea geometry X Y " << this->geometry().x() << " " << this->geometry().y();
 qDebug() << "Textarea geometry Width Height " << this->geometry().width() << " " << this->geometry().height();
 qDebug() << "Textarea frame geometry Width Height " << this->frameGeometry().width() << " " << this->frameGeometry().height();
}


void IndentSlider::paintEvent(QPaintEvent *event)
{
 // Отрисовка родительского класса
 QWidget::paintEvent(event);

 QPainter painter(this);
 painter.setRenderHint(QPainter::Antialiasing,false);
 painter.setRenderHint(QPainter::HighQualityAntialiasing,false);

 // Настраивается перо
 QPen pen(Qt::black); //Qt::darkGray
 pen.setStyle(Qt::SolidLine);
 painter.setPen(pen);

 // Рисуется прямоугольник
 painter.drawRect(indentline_left_pos,widget_height/2-indentline_height/2,
                  indentline_right_pos-indentline_left_pos,indentline_height);

 // Рисуются движки отступов
 QBrush brush(Qt::darkGray);
 painter.setBrush(brush);
 QPainter *p=&painter;
 draw_textindent(p);
 draw_leftindent(p);
 draw_rightindent(p);
}


// Движок абзацного отступа
void IndentSlider::draw_textindent(QPainter *painter)
{
 QPointF points[5];
 points[0]=QPointF((float) (indentline_left_pos+leftindent_pos+textindent_pos), (float) (widget_height/2));
 points[1]=QPointF((float) (indentline_left_pos+leftindent_pos+textindent_pos+indenttriangle_width/2), (float) (widget_height/4));
 points[2]=QPointF((float) (indentline_left_pos+leftindent_pos+textindent_pos+indenttriangle_width/2), (float) (0));
 points[3]=QPointF((float) (indentline_left_pos+leftindent_pos+textindent_pos-indenttriangle_width/2), (float) (0));
 points[4]=QPointF((float) (indentline_left_pos+leftindent_pos+textindent_pos-indenttriangle_width/2), (float) (widget_height/4));

 painter->drawConvexPolygon(points,5);
}


// Движок левого отступа
void IndentSlider::draw_leftindent(QPainter *painter)
{
 QPointF points[5];
 points[0]=QPointF((float) (indentline_left_pos+leftindent_pos), (float) (widget_height/2));
 points[1]=QPointF((float) (indentline_left_pos+leftindent_pos-indenttriangle_width/2), (float) (widget_height*3/4));
 points[2]=QPointF((float) (indentline_left_pos+leftindent_pos-indenttriangle_width/2), (float) (widget_height));
 points[3]=QPointF((float) (indentline_left_pos+leftindent_pos+indenttriangle_width/2), (float) (widget_height));
 points[4]=QPointF((float) (indentline_left_pos+leftindent_pos+indenttriangle_width/2), (float) (widget_height*3/4));

 painter->drawConvexPolygon(points,5);
}


// Движок правого отступа
void IndentSlider::draw_rightindent(QPainter *painter)
{
 QPointF points[5];
 points[0]=QPointF((float) (indentline_right_pos-rightindent_pos), (float) (widget_height/2));
 points[1]=QPointF((float) (indentline_right_pos-(rightindent_pos-indenttriangle_width/2)), (float) (widget_height*3/4));
 points[2]=QPointF((float) (indentline_right_pos-(rightindent_pos-indenttriangle_width/2)), (float) (widget_height));
 points[3]=QPointF((float) (indentline_right_pos-(rightindent_pos+indenttriangle_width/2)), (float) (widget_height));
 points[4]=QPointF((float) (indentline_right_pos-(rightindent_pos+indenttriangle_width/2)), (float) (widget_height*3/4));

 painter->drawConvexPolygon(points,5);
}


// Получение позиции отступа для отрисовки вертикальной линии отступа
// Вертикальная линия может рисоваться для любого движка
// Данный метод выясняет, какой движок в данный момент выбран
// и возвращяет его X-координату в данном виджете IndentSlider
int IndentSlider::get_current_indentedge_pos(void)
{
 if(current_sel_indent==SEL_NONE)
  return 0;

 if(current_sel_indent==SEL_TEXT_INDENT)
  return get_geometry_textindent_pos();

 if(current_sel_indent==SEL_LEFT_INDENT)
  return get_geometry_leftindent_pos();

 if(current_sel_indent==SEL_RIGHT_INDENT)
  return get_geometry_rightindent_pos();

 return 0;
}


// Обработка нажатия мышки на области виджета
void IndentSlider::mousePressEvent(QMouseEvent *event)
{
 event->accept();

 // Определяются координаты клика мышкой
 QPoint mouse_click_xy=event->pos();
 int x=mouse_click_xy.x();
 int y=mouse_click_xy.y();

 qDebug() << "Mouse click " << x << " " << y;

 // Определяется, какой движок был выбран
 current_sel_indent=SEL_NONE;
 if(y<(widget_height/2) &&
    x>(indentline_left_pos+leftindent_pos+textindent_pos-indenttriangle_width/2) &&
    x<(indentline_left_pos+leftindent_pos+textindent_pos+indenttriangle_width/2))
  current_sel_indent=SEL_TEXT_INDENT;
 else if(y>widget_height/2 &&
         x>indentline_left_pos+leftindent_pos-indenttriangle_width/2 &&
         x<indentline_left_pos+leftindent_pos+indenttriangle_width/2)
  current_sel_indent=SEL_LEFT_INDENT;
 else if(y>widget_height/2 &&
         x>indentline_right_pos-(rightindent_pos+indenttriangle_width/2) &&
         x<indentline_right_pos-(rightindent_pos-indenttriangle_width/2))
  current_sel_indent=SEL_RIGHT_INDENT;

 // Если мышка при клике не указывала ни на один движок
 if(current_sel_indent==SEL_NONE)return;

 qDebug() << "Sel indenter " << current_sel_indent;
}


void IndentSlider::mouseMoveEvent(QMouseEvent *event)
{
 event->accept();

 // Определяются текущие координаты мышки
 QPoint mouse_xy=event->pos();
 int x=mouse_xy.x();
 // int y=mouse_xy.y();

 // Если ни один движок не был выбран
 if(current_sel_indent==SEL_NONE)return;

 // qDebug() << "Move indenter to " << x;

 // Обработка перемещения мышкой движка абзацного отступа
 if(current_sel_indent==SEL_TEXT_INDENT)
  {
   if(x<indentline_left_pos)x=indentline_left_pos;
   if(x>indentline_right_pos)x=indentline_right_pos;
   textindent_pos=x-indentline_left_pos-leftindent_pos;

   emit change_textindent_pos(textindent_pos);
  }

 // Обработка перемещения мышкой движка левого отступа
 if(current_sel_indent==SEL_LEFT_INDENT)
  {
   leftindent_pos=x-indentline_left_pos;
   if(leftindent_pos<0)leftindent_pos=0;
   if(leftindent_pos>indentline_right_pos-indentline_left_pos)
    leftindent_pos=indentline_right_pos-indentline_left_pos;

   emit change_leftindent_pos(leftindent_pos);

   // При перемещении левого отступа может изменится абзацный отступ
   if(indentline_left_pos+leftindent_pos+textindent_pos < indentline_left_pos)
    {
     textindent_pos=-leftindent_pos;
     emit change_textindent_pos(textindent_pos);
    }
   if(indentline_left_pos+leftindent_pos+textindent_pos > indentline_right_pos)
    {
     textindent_pos=indentline_right_pos-indentline_left_pos-leftindent_pos;
     emit change_textindent_pos(textindent_pos);
    }
  }

 // Обработка перемещения мышкой движка правого отступа
 if(current_sel_indent==SEL_RIGHT_INDENT)
  {
   rightindent_pos=indentline_right_pos-x;
   if(rightindent_pos<0)rightindent_pos=0;
   if(rightindent_pos>indentline_right_pos-indentline_left_pos)
    rightindent_pos=indentline_right_pos-indentline_left_pos;

   emit change_rightindent_pos(rightindent_pos);
  }

 // Графика виджета обновляется для отрисовки нового положения движков
 emit update();
}


void IndentSlider::mouseReleaseEvent(QMouseEvent *event)
{
 event->accept();

 current_sel_indent=SEL_NONE;

 emit mouse_release();
}


void IndentSlider::set_textindent_pos(int i)
{
 textindent_pos=i;
 emit update();
}

void IndentSlider::set_leftindent_pos(int i)
{
 leftindent_pos=i;
 emit update();
}

void IndentSlider::set_rightindent_pos(int i)
{
 rightindent_pos=i;
 emit update();
}
