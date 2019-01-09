#ifndef EDITORCONFIGKEYBOARD_H
#define	EDITORCONFIGKEYBOARD_H

#include <QWidget>
#include <QLineEdit>
#include <QFontComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QVBoxLayout>

#include "views/appConfigWindow/ConfigPage.h"

class EditorConfig;


class EditorConfigKeyboard : public ConfigPage
{
 Q_OBJECT

public:
  EditorConfigKeyboard(QWidget *parent = nullptr);
  virtual ~EditorConfigKeyboard(void);
  
  int applyChanges(void);
    
private slots:
  void onClickedEditToolButtonsConfigFile(void);

protected:

  QPushButton *editToolButtonsConfigFile;

  void setupUi(void);
  void setupSignals(void);
  void assembly(void);

private:

  EditorConfig *conf;

  QLabel *indentStepLabel;
  QLabel *indentStepFlexion;
  QSpinBox *indentStep;
  
};


#endif	// EDITORCONFIGKEYBOARD_H

