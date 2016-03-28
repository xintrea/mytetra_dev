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
  int applyChanges(void);

private slots:

protected:

  void setColorForButtonHighlightColor(QColor iColor);

  // Галка разрешения/запрещения подсветки записи с прикрепленными файлами
  QCheckBox enableRecordWithAttachHighlight;

  // Цвет строки для записи с прикрепленными файлами
  QLabel labelHighlightColor;
  QToolButton buttonHighlightColor;
  QColor highlightColor;

  // Объединяющая рамка
  QGroupBox decorBox;
};


#endif	// _CONFIGPAGE_ATTACH_H_

