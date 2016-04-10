#include <QLabel>
#include <QComboBox>
#include <QListWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDialogButtonBox>

#include "IconSelectDialog.h"


IconSelectDialog::IconSelectDialog()
{
  setupUI();
  setupSignals();
  assembly();
}


IconSelectDialog::~IconSelectDialog()
{

}


void IconSelectDialog::setupUI()
{
  // Заголовок окна
  setWindowTitle(tr("Select icon"));

  // Надпись "Раздел"
  sectionLabel.setText(tr("Section"));

  // Кнопки OK и Cancel
  buttonBox.setOrientation(Qt::Horizontal);
  buttonBox.setStandardButtons(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);
}


void IconSelectDialog::setupSignals()
{
  connect( &buttonBox, SIGNAL(accepted()), this, SLOT(okClick(void)) );
  connect( &buttonBox, SIGNAL(rejected()), this, SLOT(reject()) );
}


void IconSelectDialog::assembly()
{
  QHBoxLayout *sectionLayout=new QHBoxLayout(); // Указывать this не нужно, так как он назначится в момент вставки в основной слой
  sectionLayout->addWidget( &sectionLabel );
  sectionLayout->addWidget( &sectionComboBox );

  QVBoxLayout *mainLayout=new QVBoxLayout(); // Указывать this не нужно, так как он назначится когда этот слой станет основным слоем
  mainLayout->addLayout(sectionLayout);
  mainLayout->addWidget( &iconList );
  mainLayout->addWidget( &buttonBox );

  setLayout(mainLayout);
}


void IconSelectDialog::setPath(QString iPath)
{

}

void IconSelectDialog::setDefaultSection(QString iSectionName)
{

}


void IconSelectDialog::okClick()
{

}


QString IconSelectDialog::getSelectFileName()
{
  return "";
}

