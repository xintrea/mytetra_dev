#ifndef _EDITORCONFIGFONT_H_
#define _EDITORCONFIGFONT_H_

#include <QWidget>

#include "views/appConfigWindow/ConfigPage.h"

class EditorConfig;

namespace Ui {
class EditorConfigFont;
}

class EditorConfigFont : public ConfigPage
{
 Q_OBJECT

public:
  explicit EditorConfigFont(QWidget *parent = 0);
  ~EditorConfigFont();

  void setup_ui(void);
  void setup_signals(void);
  int applyChanges(void);

private slots:
  void on_monospace_applysize_state_changed(int i);

  void on_code_applysize_state_changed(int i);
  void on_code_applyindent_state_changed(int i);

  void on_code_select_color_button_click();
  
private:
  QColor* codeColor;
  EditorConfig *conf;
  Ui::EditorConfigFont* ui;
};

#endif // _EDITORCONFIGFONT_H_

