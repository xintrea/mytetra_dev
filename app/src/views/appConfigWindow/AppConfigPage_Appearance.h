#ifndef APPCONFIGPAGE_APPEARANCE_H
#define APPCONFIGPAGE_APPEARANCE_H

#include <QWidget>
#include <QGroupBox>
#include <QCheckBox>
#include <QLabel>

#include "ConfigPage.h"
#include "libraries/MtComboBox.h"


class AppConfigPage_Appearance : public ConfigPage
{
    Q_OBJECT

public:
    AppConfigPage_Appearance(QWidget *parent = nullptr);
    virtual ~AppConfigPage_Appearance();

    int applyChanges(void);

  protected:

    void setupUi(void);
    void setupSignals(void);
    void assembly(void);

    void setupThemeComboBox(void);
    void setupIconSizeComboBox(void);

    // Объединяющая рамка
    QGroupBox *behaviorBox;
    QGroupBox *interfaceBox;

    QLabel *themeLabel;
    MtComboBox *themeNameComboBox;

    QLabel *iconSizeLabel;
    MtComboBox *iconSizeComboBox;

    QCheckBox *runInMinimizedWindow; // Разрешен ли запуск в свернутом окне
    QCheckBox *dockableWindowsBehavior; // Поведение открепляемых окон
};

#endif // APPCONFIGPAGE_APPEARANCE_H
