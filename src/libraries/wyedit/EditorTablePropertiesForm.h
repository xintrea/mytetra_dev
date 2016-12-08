#ifndef _EDITORTABLEPROPERTIESFORM_H_
#define _EDITORTABLEPROPERTIESFORM_H_

#include <QDialog>
#include <QToolButton> // Почему-то не работает предварительное определение класса (этап линковки), приходится включать инклюдом
#include <QLabel>
#include <QSpinBox>
#include <QDialogButtonBox>
#include <QColor>


class EditorTablePropertiesForm : public QDialog
{
 Q_OBJECT

public:

  enum TableAlign{Left,
                  Center,
                  Right};

  EditorTablePropertiesForm();
  virtual ~EditorTablePropertiesForm();

  void setTableWidth(int iWidth);
  int getTableWidth(void);

  void setBorderWidth(int iWidth);
  int getBorderWidth(void);

  void setBackgroundColor(QColor iColor);
  QColor getBackgroundColor(void);

  void setTableAlign(TableAlign iAlign);
  int getTableAlign();

protected slots:

  void onClickedButtonBackgroundColor();

  void onToggleButtonAlignLeft();
  void onToggleButtonAlignCenter();
  void onToggleButtonAlignRight();

private:

  // Ширина таблицы
  QLabel labelTableWidth;
  QLabel labelTablePercent;
  QSpinBox spinTableWidth;

  // Толщина линий таблицы
  QLabel labelBorderWidth;
  QLabel labelBorderPix;
  QSpinBox spinBorderWidth;

  // Цвет фона таблицы
  QLabel labelBackgroundColor;
  QToolButton buttonBackgroundColor;
  QColor backgroundColor;

  QLabel labelAlign;
  QToolButton buttonAlignLeft;
  QToolButton buttonAlignCenter;
  QToolButton buttonAlignRight;
  TableAlign tableAlign;
  bool directSetAlign; // Флаг, включаемый при программной установке нажатой кнопки. Он нужен, чтобы исключить рекурсию

  // Кнопки Ok и Concel
  QDialogButtonBox buttonBox;

  void setupUi();
  void setupSignals();
  void assembly();

  void setColorForButtonBackgroundColor(QColor iColor);
};

#endif // _EDITORTABLEPROPERTIESFORM_H_
