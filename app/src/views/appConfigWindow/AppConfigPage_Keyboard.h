#ifndef _CONFIGPAGE_KEYBOARD_H_
#define	_CONFIGPAGE_KEYBOARD_H_

#include <QWidget>
#include <QCheckBox>
#include <QGroupBox>
#include <QPushButton>
#include <QVBoxLayout>

#include "ConfigPage.h"


class AppConfigPage_Keyboard : public ConfigPage
{
 Q_OBJECT

public:
  AppConfigPage_Keyboard(QWidget *parent = nullptr);
  virtual ~AppConfigPage_Keyboard(void);

  int applyChanges(void);

private slots:
  void onClickedEditShortcutConfigFile(void);

protected:

  void setupUi(void);
  void setupSignals(void);
  void assembly(void);


  // Кнопка настройки конфигурации горячих клавиш
  QPushButton *editShortcutConfigFile;

  // Объединяющая рамка
  QGroupBox *shortcutBox;

  // Группировщик для виждетов внутри объединяющей рамки
  QVBoxLayout *shortcutLayout;

};


#endif // _CONFIGPAGE_KEYBOARD_H_

