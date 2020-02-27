#ifndef EDITORCONFIGTOOLBARS_H
#define	EDITORCONFIGTOOLBARS_H

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
class EditorToolbarSettingsScreen;

class EditorConfigToolbars : public ConfigPage
{
 Q_OBJECT

public:
  EditorConfigToolbars(QWidget *parent = nullptr);
  virtual ~EditorConfigToolbars(void);
  
  int applyChanges(void);
    
private slots:

  // Запуск диалога распределения кнопок (команд) по панелям инструментов редактора
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

  // Диалог настройки панелей инструментов
  EditorToolbarSettingsScreen *toolbuttonsScreen=nullptr;
  
};


#endif	// EDITORCONFIGTOOLBARS_H

