#ifndef __OVERTEXTTOOLBUTTON_H__
#define __OVERTEXTTOOLBUTTON_H__

#include <QToolButton>
#include <QPaintEvent>


// Виджет, похожий на QToolButton, но позволяющий писать поверх иконки


class OverTextToolButton : public QToolButton
{
  Q_OBJECT

public:
  OverTextToolButton(QWidget *parent = 0);

  void setOverText(QString iText);


protected:

  void paintEvent(QPaintEvent *event);

  QString overText;

};

#endif // __OVERTEXTTOOLBUTTON_H__
