#ifndef EDITORCONFIGMATHEXPRESSION_H
#define	EDITORCONFIGMATHEXPRESSION_H

#include <QWidget>
#include <QLineEdit>
#include <QFontComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>

#include "views/appConfigWindow/ConfigPage.h"

class EditorConfig;


class EditorConfigMathExpression : public ConfigPage
{
 Q_OBJECT

public:
  EditorConfigMathExpression(QWidget *parent = nullptr);
  
  int applyChanges(void);
    
private slots:
  void onClickedEditWyEditConfigFile(void);
 
private:

  // Счетчик для таймера обновления картинки формулы в редакторе формулы
  QLabel *updateFormulaTimeLabel;
  QSpinBox *updateFormulaTimeSpinBox;
  QLabel *updateFormulaTimeSecLabel;

  EditorConfig *conf;

  void setupUi(void);
  void setupSignals(void);
  void assembly(void);
  
};


#endif	// EDITORCONFIGMATHEXPRESSION_H

