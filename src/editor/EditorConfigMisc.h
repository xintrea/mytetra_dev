#ifndef _EDITORCONFIGMISC_H_
#define	_EDITORCONFIGMISC_H_

#include <QWidget>
#include <QLineEdit>
#include <QFontComboBox>
#include <QSpinBox>

#include "../ConfigPage.h"

class EditorConfig;


class EditorConfigMisc : public ConfigPage
{
 Q_OBJECT

public:
  EditorConfigMisc(QWidget *parent = 0);
  
  int apply_changes(void);
    
private slots:
 
private:
  QSpinBox *indentStep;
  
  EditorConfig *conf;
  
};


#endif	// _EDITORCONFIGMISC_H_

