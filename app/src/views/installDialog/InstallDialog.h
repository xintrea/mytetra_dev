#ifndef INSTALLDIALOG_H
#define INSTALLDIALOG_H

#include <QDialog>
#include <QTranslator>

// Окно для настройки развертывания базовых файлов приложения


namespace Ui {
class InstallDialog;
}

class InstallDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InstallDialog(QWidget *parent = nullptr);
    ~InstallDialog();

    enum InstallType {
        Standart=0,
        Portable=1
    };

    void setEnablePortable(bool enablePortable);
    void setStandartData(QString dataDirName, QString homePath);
    void setPortableData(QString fullCurrentPath);

    void update();

    InstallDialog::InstallType getInstallType();
    QString getAutoDetectLang();

protected slots:

    void onRadioButtonStandartToggled(bool state);
    void onRadioButtonPortableToggled(bool state);
    void onAccepted();

protected:

    void setupSignals();
    void setupAutoLangTranslation();

    Ui::InstallDialog *ui;

    QTranslator m_langTranslator; // Транслятор должен существовать все время работы окна

    bool m_enablePortable=true;

    QString m_autoDetectLang="en";

    QString m_standartDataDirName;
    QString m_standartHomePath;

    QString m_portableFullCurrentPath;
};

#endif // INSTALLDIALOG_H
