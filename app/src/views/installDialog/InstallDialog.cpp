#include "InstallDialog.h"
#include "ui_InstallDialog.h"

#include "main.h"


InstallDialog::InstallDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InstallDialog)
{
    ui->setupUi(this);

    this->setupSignals();

    ui->m_title->setText( tr("Welcome to MyTetra v.")+QString::number(APPLICATION_RELEASE_VERSION)+'.'+QString::number(APPLICATION_RELEASE_SUBVERSION)+'.'+QString::number(APPLICATION_RELEASE_MICROVERSION)+"!" );

    ui->m_text->setText( tr("Please, select application mode:") );
}


InstallDialog::~InstallDialog()
{
    delete ui;
}


void InstallDialog::setupSignals()
{
    connect(ui->m_radioButtonStandart, &QRadioButton::toggled,
            this, &InstallDialog::onRadioButtonStandartToggled);

    connect(ui->m_radioButtonPortable, &QRadioButton::toggled,
            this, &InstallDialog::onRadioButtonPortableToggled);
}

void InstallDialog::setEnablePortable(bool enablePortable)
{
    m_enablePortable=enablePortable;
}


void InstallDialog::setStandartData(QString dataDirName, QString homePath)
{
    m_standartDataDirName=dataDirName;
    m_standartHomePath=homePath;
}


void InstallDialog::setPortableData(QString fullCurrentPath)
{
    m_portableFullCurrentPath=fullCurrentPath;
}


void InstallDialog::update()
{

    ui->m_labelStandart->setText( tr("Create subdirectory \"%1\"\nin user directory \"%2\",\nand create application files in it.").
                                  arg(m_standartDataDirName).
                                  arg(m_standartHomePath) );

    if(m_enablePortable)
    {
        ui->m_labelPortable->setText( tr("Create application files\nin current directory \"%1\".").
                                      arg(m_portableFullCurrentPath) );
    }
    else
    {
        ui->m_labelPortable->setText( tr("Can't create portable version.\nСan't write data to MyTetra binary file directory \"%1\".").
                                      arg(m_portableFullCurrentPath) );
    }


}


void InstallDialog::onRadioButtonStandartToggled(bool state)
{
    ui->m_radioButtonPortable->setChecked(!state);
}


void InstallDialog::onRadioButtonPortableToggled(bool state)
{
    ui->m_radioButtonStandart->setChecked(!state);
}


InstallDialog::InstallType InstallDialog::getInstallType()
{
    if( ui->m_radioButtonStandart->isChecked() )
    {
        return InstallDialog::InstallType::Standart;
    }
    else
    {
        return InstallDialog::InstallType::Portable;
    }


}


