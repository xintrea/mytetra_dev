#ifndef APPCONFIGPAGE_HISTORY_H
#define APPCONFIGPAGE_HISTORY_H

#include <QWidget>
#include <QGroupBox>
#include <QCheckBox>

#include "ConfigPage.h"


class AppConfigPage_History : public ConfigPage
{
    Q_OBJECT

public:
    AppConfigPage_History(QWidget *parent = nullptr);
    virtual ~AppConfigPage_History();

    int applyChanges(void);

  protected:

    void setupUi(void);
    void setupSignals(void);
    void assembly(void);

    // Объединяющая рамка
    QGroupBox *historyBox;

    QCheckBox *rememberAtHistoryNavigationCheckBox;
    QCheckBox *rememberAtOrdinarySelectionCheckBox;
};

#endif // APPCONFIGPAGE_HISTORY_H
