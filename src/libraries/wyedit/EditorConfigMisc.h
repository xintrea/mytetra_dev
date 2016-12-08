#ifndef _EDITORCONFIGMISC_H_
#define	_EDITORCONFIGMISC_H_

#include <QWidget>
#include <QLineEdit>
#include <QFontComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>

#include "views/appConfigWindow/ConfigPage.h"

class EditorConfig;


class EditorConfigMisc : public ConfigPage
{
 Q_OBJECT

public:
  EditorConfigMisc(QWidget *parent = 0);
  
  int applyChanges(void);
    
private slots:
  void onClickedEditWyEditConfigFile(void);
 
private:

  QLabel *indentStepLabel;
  QLabel *indentStepFlexion;
  QSpinBox *indentStep;

  QPushButton *editWyEditConfigFile;
  
  EditorConfig *conf;

  void setupUi(void);
  void setupSignals(void);
  void assembly(void);
  
};


#endif	// _EDITORCONFIGMISC_H_

