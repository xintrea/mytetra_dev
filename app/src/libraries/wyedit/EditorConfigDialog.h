#ifndef EDITORCONFIGDIALOG_H
#define EDITORCONFIGDIALOG_H

#include <QWidget>
#include "views/appConfigWindow/ConfigDialog.h"


class EditorConfigDialog : public QWidget
{
 Q_OBJECT

public:
 EditorConfigDialog(QWidget *parent=nullptr);
    
private:
 ConfigDialog *configDialog;
};

#endif // EDITORCONFIGDIALOG_H
