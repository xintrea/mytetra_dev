#ifndef _CONFIGPAGE_ATTACH_H_
#define	_CONFIGPAGE_ATTACH_H_

#include <QWidget>
#include <QCheckBox>
#include <QGroupBox>
#include <QLabel>
#include <QToolButton>

#include "ConfigPage.h"


class AppConfigPage_Attach : public ConfigPage
{
 Q_OBJECT

public:
  AppConfigPage_Attach(QWidget *parent = 0);

  void setupUi(void);
  void setupSignals(void);
  void assembly(void);

  int applyChanges(void);

private slots:

  void onEnableRecordWithAttachHighlight(bool);
  void onClickedHighlightColor();

protected:

  void setColorForButtonHighlightColor(QColor iColor);

  // Галка разрешения/запрещения подсветки записи с прикрепленными файлами
  QCheckBox enableRecordWithAttachHighlight;

  // Выбор цвета для записи с прикрепленными файлами
  QLabel labelHighlightColor;
  QToolButton buttonHighlightColor;
  QColor highlightColor;

  // Объединяющая рамка
  QGroupBox decorBox;
};


#endif	// _CONFIGPAGE_ATTACH_H_

