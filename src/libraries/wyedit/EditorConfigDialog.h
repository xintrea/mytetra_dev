#ifndef _EDITORCONFIGDIALOG_H_
#define _EDITORCONFIGDIALOG_H_

#include <QWidget>
#include "views/appConfigWindow/ConfigDialog.h"

class EditorConfigDialog : public QWidget
{
 Q_OBJECT

public:
  explicit EditorConfigDialog(QWidget *parent=0);
  void execDialog();

private:
 ConfigDialog *configDialog;
};

#endif // _EDITORCONFIGDIALOG_H_
