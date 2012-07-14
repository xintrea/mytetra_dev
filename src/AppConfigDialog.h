#ifndef _APPCONFIGDIALOG_H_
#define _APPCONFIGDIALOG_H_

#include <QWidget>

class ConfigDialog;


class AppConfigDialog : public QWidget
{
 Q_OBJECT

public:
 AppConfigDialog(void);
    
private:
 ConfigDialog *configdialog;
};

#endif // _APPCONFIGDIALOG_H_
