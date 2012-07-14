#ifndef _EDITORADDTABLEFORM_H_
#define _EDITORADDTABLEFORM_H_

#include <QDialog>


class QLabel;
class QSpinBox;
class QDialogButtonBox;

class EditorAddTableForm : public QDialog
{
 Q_OBJECT

 public:
  EditorAddTableForm();

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

#endif // _EDITORADDTABLEFORM_H_
