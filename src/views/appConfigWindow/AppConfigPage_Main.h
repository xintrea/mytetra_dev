#ifndef _APPCONFIGPAGE_MAIN_H_
#define	_APPCONFIGPAGE_MAIN_H_

#include <QWidget>
#include <QLineEdit>
#include <QSpinBox>

#include "ConfigPage.h"
#include "libraries/MtComboBox.h"


class AppConfigPage_Main : public ConfigPage
{
 Q_OBJECT

public:
  AppConfigPage_Main(QWidget *parent = 0);
  int apply_changes(void);
    
private slots:
  void open_tetradir_select_dialog(void);
  void open_trashdir_select_dialog(void);

private:
  QLineEdit *tetradirInput;
  QLineEdit *trashdirInput;
  QSpinBox  *trashsizeInput;
  QSpinBox  *trashmaxfilecountInput;
  MtComboBox *interfaceLanguage;

};


#endif	// _APPCONFIGPAGE_MAIN_H_

