#ifndef _CONFIGPAGE_SYNCHRO_H_
#define	_CONFIGPAGE_SYNCHRO_H_

#include <QWidget>
#include <QCheckBox>
#include <QLineEdit>
#include <QGroupBox>

#include "ConfigPage.h"


class AppConfigPage_Synchro : public ConfigPage {
    Q_OBJECT

public:
    AppConfigPage_Synchro(QWidget *parent = 0);
    int apply_changes(void);

private slots:

protected:

    QLineEdit *synchroCommand;

    QCheckBox *synchroOnStartup;
    QCheckBox *synchroOnExit;

    // Объединяющая рамка
    QGroupBox *synchroOnBox;
};


#endif	// _CONFIGPAGE_SYNCHRO_H_

