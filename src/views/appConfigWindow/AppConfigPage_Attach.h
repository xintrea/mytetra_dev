#ifndef _CONFIGPAGE_ATTACH_H_
#define _CONFIGPAGE_ATTACH_H_

#include <QWidget>

#include "ConfigPage.h"

namespace Ui {
class AppConfigPage_Attach;
}

class AppConfigPage_Attach : public ConfigPage
{
 Q_OBJECT

public:
  explicit AppConfigPage_Attach(QWidget *parent = 0);
  ~AppConfigPage_Attach();

  void setupUi(void);
  void setupSignals(void);

  int applyChanges(void);

private slots:

  void onEnableRecordWithAttachHighlight(bool);
  void onClickedHighlightColor();

protected:

  void setColorForButtonHighlightColor(QColor iColor);

  QColor highlightColor;

private:
  Ui::AppConfigPage_Attach* ui;
};


#endif // _CONFIGPAGE_ATTACH_H_

