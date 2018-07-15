#ifndef _INDENTSLIDER_H_
#define _INDENTSLIDER_H_

#include <QWidget>
#include <QMouseEvent>


class IndentSlider : public QWidget
{
 Q_OBJECT

 public:
  IndentSlider(int width, int height, QWidget *parent=0);

  // Управление размерами виджета
  void set_widget_geometry(int width, int height);
  void set_widget_width(int width);
  void set_widget_height(int height);

  // Управление размерами прямоугольника, по которому двигаются движки
  void set_indentline_left_pos(int i);
  void set_indentline_right_pos(int i);
  void set_indentline_height(int i);

  // Получение текущих отступов движков в абсолютных координатах виджета
  int get_geometry_textindent_pos(void);
  int get_geometry_leftindent_pos(void);
  int get_geometry_rightindent_pos(void);

  // Получение позиции отступа для отрисовки вертикальной линии отступа
  // в абсолютных координатах виджета.
  // Результат зависит от того, какой движок в данный момент выбран
  int get_current_indentedge_pos(void);

 signals:
  void change_textindent_pos(int i);
  void change_leftindent_pos(int i);
  void change_rightindent_pos(int i);

  void mouse_release(void);

 public slots:
  void set_textindent_pos(int i);
  void set_leftindent_pos(int i);
  void set_rightindent_pos(int i);

 protected:

  virtual void paintEvent(QPaintEvent *event);
  virtual void resizeEvent(QResizeEvent *event);
  virtual void mousePressEvent(QMouseEvent *event);
  virtual void mouseMoveEvent(QMouseEvent *event);
  virtual void mouseReleaseEvent(QMouseEvent *event);

  void draw_textindent(QPainter *painter);
  void draw_leftindent(QPainter *painter);
  void draw_rightindent(QPainter *painter);

  // Размеры виджета
  int widget_width;
  int widget_height;

  // Геометрия прямоугольника по которому двигаются слайдеры
  int indentline_left_pos;
  int indentline_right_pos;
  int indentline_height;

  int indenttriangle_width;
  int indentline_step_draw_marker;
  int indentline_step_marker;

  // Позиции (X-координаты) слайдеров.
  // Позиции указываются относительно прямоугольника
  // по которому двигаются слайдеры
  int textindent_pos;
  int leftindent_pos;
  int rightindent_pos;

  // Какой слайдер выбран в данный момент
  int current_sel_indent;

  enum {
   SEL_NONE,
   SEL_TEXT_INDENT,
   SEL_LEFT_INDENT,
   SEL_RIGHT_INDENT
  };
};

#endif // _INDENTSLIDER_H_
