#include "InstallDialog.h"
#include "ui_InstallDialog.h"

InstallDialog::InstallDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InstallDialog)
{
    ui->setupUi(this);
}

InstallDialog::~InstallDialog()
{
    delete ui;
}
