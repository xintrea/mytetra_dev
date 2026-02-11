#ifndef __OVERTEXTTOOLBUTTON_H__
#define __OVERTEXTTOOLBUTTON_H__

#include <QToolButton>


// Виджет, похожий на QToolButton, но позволяющий писать поверх иконки


class QPaintEvent;

class OverTextToolButton : public QToolButton
{
  Q_OBJECT

public:
  OverTextToolButton(QWidget *parent = nullptr);

  void setOverText(QString iText);


protected:

  void paintEvent(QPaintEvent *event);

  QString overText;

};

#endif // __OVERTEXTTOOLBUTTON_H__
