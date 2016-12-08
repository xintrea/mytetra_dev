#ifndef _CONFIGPAGE_MISC_H_
#define	_CONFIGPAGE_MISC_H_

#include <QWidget>
#include <QCheckBox>
#include <QGroupBox>
#include <QPushButton>

#include "ConfigPage.h"


class AppConfigPage_Misc : public ConfigPage
{
 Q_OBJECT

public:
  AppConfigPage_Misc(QWidget *parent = 0);
  virtual ~AppConfigPage_Misc(void);

  int applyChanges(void);

private slots:
  void onClickedEditMyTetraConfigFile(void);

protected:

  QCheckBox *cutBranchConfirm;        // Требуется ли показывать предупреждение при вырезании ветки
  QCheckBox *printDebugMessages;      // Выводить ли в консоль отладочные сообщения
  QCheckBox *runInMinimizedWindow;    // Разрешен ли запуск в свернутом окне
  QCheckBox *enableActionLog;         // Разрешено ли логирование действий
  QCheckBox *enableCreateEmptyRecord; // Разрешено ли создание записи, не содержащей текст (а только заголовок)
  QPushButton *editMyTetraConfigFile;

  // Объединяющая рамка
  QGroupBox *historyBox;
  QGroupBox *dangerBox;

  QCheckBox *rememberAtHistoryNavigationCheckBox;
  QCheckBox *rememberAtOrdinarySelectionCheckBox;

  void setupUi(void);
  void setupSignals(void);
  void assembly(void);

};


#endif	// _CONFIGPAGE_MISC_H_

