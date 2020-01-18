#ifndef INSTALLDIALOG_H
#define INSTALLDIALOG_H

#include <QDialog>

namespace Ui {
class InstallDialog;
}

class InstallDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InstallDialog(QWidget *parent = nullptr);
    ~InstallDialog();

private:
    Ui::InstallDialog *ui;
};

#endif // INSTALLDIALOG_H
