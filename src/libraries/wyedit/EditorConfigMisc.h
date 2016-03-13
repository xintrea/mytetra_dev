#ifndef _EDITORCONFIGMISC_H_
#define	_EDITORCONFIGMISC_H_

#include <QWidget>
#include <QLineEdit>
#include <QFontComboBox>
#include <QSpinBox>

#include "views/appConfigWindow/ConfigPage.h"

class EditorConfig;


class EditorConfigMisc : public ConfigPage
{
 Q_OBJECT

public:
  EditorConfigMisc(QWidget *parent = 0);
  
  int applyChanges(void);
    
private slots:
 
private:
  QSpinBox *indentStep;
  
  EditorConfig *conf;
  
};


#endif	// _EDITORCONFIGMISC_H_

