#include <QTranslator>
#include <QDebug>
#include <QStringList>
#include <QDialogButtonBox>
#include <QMessageBox>

#include "InstallDialog.h"
#include "ui_InstallDialog.h"

#include "main.h"


InstallDialog::InstallDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InstallDialog)
{
    this->setupAutoLangTranslation();

    ui->setupUi(this);

    this->setupSignals();

    ui->m_title->setText( tr("Welcome to MyTetra v.")+QString::number(APPLICATION_RELEASE_VERSION)+'.'+QString::number(APPLICATION_RELEASE_SUBVERSION)+'.'+QString::number(APPLICATION_RELEASE_MICROVERSION)+"!" );

    ui->m_text->setText( tr("Please select a knowledge base installation mode:") );
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

    connect(ui->m_buttonBox, &QDialogButtonBox::accepted,
            this, &InstallDialog::onAccepted);
}


// Автоматическое определение языка системы
void InstallDialog::setupAutoLangTranslation()
{
    QString lang=QLocale().system().name().split('_').first().toLower();
    QStringList availableLang={"en", "ru"};

    if( !availableLang.contains(lang) )
    {
        lang="en";
    }

    qDebug() << "Auto detect lang in installator: " << lang;

    // Подключение перевода интерфейса
    QString langFileName=":/resource/translations/mytetra_"+lang+".qm";
    qDebug() << "Lang file: " << langFileName;

    bool loadResult=m_langTranslator.load(langFileName);

    if(loadResult)
    {
        qDebug() << "Success load translation file";
        qApp->installTranslator(&m_langTranslator); // Транслятор устанавливается в объекте приложения

        // Запоминается автоопределенный язык
        m_autoDetectLang=lang;
    }
    else
    {
        qDebug() << "Can't load translation file";
    }
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
        ui->m_radioButtonPortable->setEnabled(false);
    }
}


void InstallDialog::onRadioButtonStandartToggled(bool state)
{
    if(m_enablePortable)
    {
        ui->m_radioButtonPortable->setChecked(!state);
    }
}


void InstallDialog::onRadioButtonPortableToggled(bool state)
{
    ui->m_radioButtonStandart->setChecked(!state);
}


void InstallDialog::onAccepted()
{
    if( !ui->m_radioButtonStandart->isChecked() &&
        !ui->m_radioButtonPortable->isChecked() )
    {
        QMessageBox msgBox;
        msgBox.setText(tr("Please select one of install mode."));
        msgBox.exec();
    }
    else
    {
        this->accept();
    }
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


QString InstallDialog::getAutoDetectLang()
{
    return m_autoDetectLang;
}


