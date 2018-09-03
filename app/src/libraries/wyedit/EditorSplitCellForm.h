#ifndef _EDITORSPLITCELLFORM_H_
#define _EDITORSPLITCELLFORM_H_

#include <QWidget>
#include <QDialog>
#include <QLabel>
#include <QSpinBox>
#include <QDialogButtonBox>
#include <QCheckBox>


class EditorSplitCellForm : public QDialog
{
 Q_OBJECT

 public:
  EditorSplitCellForm(int fixHorisontalSplit, int fixVerticalSplit);
  ~EditorSplitCellForm();

  int get_num_split_h(void);
  int get_num_split_v(void);

 private:
  int fixHSplit;
  int fixVSplit;

  QCheckBox *checkboxFixHSplit;
  QCheckBox *checkboxFixVSplit;

  QLabel *checkboxLabelFixHSplit;
  QLabel *checkboxLabelFixVSplit;

  QLabel *labelPictogramH;
  QLabel *labelPictogramV;

  QLabel *labelH;
  QLabel *labelV;

  QSpinBox *numSplitH;
  QSpinBox *numSplitV;

  QDialogButtonBox *buttonBox;

};

#endif // _EDITORSPLITCELLFORM_H_
