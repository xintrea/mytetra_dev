#ifndef _EDITORTABLEPROPERTIESFORM_H_
#define _EDITORTABLEPROPERTIESFORM_H_

#include <QDialog>


class QLabel;
class QSpinBox;
class QDialogButtonBox;

class EditorTablePropertiesForm : public QDialog
{
 Q_OBJECT

 public:
  EditorTablePropertiesForm();

  int  get_columns(void);
  int  get_rows(void);
  int  get_width(void);

 private:
  QLabel *labelColumns;
  QLabel *labelRows;
  QLabel *labelWidth;
  QLabel *labelPercent;

  QSpinBox *spinColumns;
  QSpinBox *spinRows;
  QSpinBox *spinWidth;

  QDialogButtonBox *buttonBox;

};

#endif // _EDITORTABLEPROPERTIESFORM_H_
