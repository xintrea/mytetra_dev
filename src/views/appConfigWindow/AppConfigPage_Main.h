#ifndef _APPCONFIGPAGE_MAIN_H_
#define	_APPCONFIGPAGE_MAIN_H_

#include <QWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QGroupBox>
#include <QRadioButton>
#include <QLabel>
#include <QToolButton>

#include "ConfigPage.h"
#include "libraries/MtComboBox.h"


class AppConfigPage_Main : public ConfigPage {
    Q_OBJECT

public:
    AppConfigPage_Main(QWidget *parent = 0);
    ~AppConfigPage_Main(void);

    void setup_ui(void);
    void setup_signals(void);
    void assembly(void);

    int apply_changes(void);

private slots:
    void open_tetradir_select_dialog(void);
    void open_trashdir_select_dialog(void);

    void onDisableCustomDateTimeFormatToggle(bool checked);
    void onEnableCustomDateTimeFormatToggle(bool checked);
    void onDateTimeFormatHelpButton(void);

private:

    QLabel *tetradirLabel;
    QLineEdit *tetradirInput;
    QToolButton *tetradirButton;

    QLabel *trashdirLabel;
    QLineEdit *trashdirInput;
    QToolButton *trashdirButton;

    QLabel *trashsizeLabel;
    QSpinBox  *trashsizeInput;
    QLabel *trashsizeFlexion;

    QLabel *trashmaxfilecountLabel;
    QSpinBox  *trashmaxfilecountInput;
    QLabel *trashmaxfilecountFlexion;

    QLabel *interfaceLanguageLabel;
    MtComboBox *interfaceLanguage;

    // Настройки отображения даты и времени
    QGroupBox *dateTimeFormatBox;
    QRadioButton *disableCustomDateTimeFormat;
    QRadioButton *enableCustomDateTimeFormat;
    QLineEdit *customDateTimeFormat;
    QToolButton *dateTimeFormatHelpButton;
};


#endif	// _APPCONFIGPAGE_MAIN_H_

