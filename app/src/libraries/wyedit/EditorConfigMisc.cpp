#include <QString>
#include <QWidget>
#include <QBoxLayout>
#include <QDir>
#include <QLineEdit>
#include <QAbstractSpinBox>
#include <QtDebug>
#include <QLabel>
#include <QGroupBox>

#include "../../main.h"

#include "EditorConfigMisc.h"
#include "EditorConfig.h"
#include "libraries/GlobalParameters.h"

extern GlobalParameters globalParameters;


EditorConfigMisc::EditorConfigMisc(QWidget *parent) : ConfigPage(parent)
{
  // Устанавливается указатель на объект работы с конфигфайлом
  conf=find_object<EditorConfig>("editorconfig");

  setupUi();
  setupSignals();
  assembly();
}


void EditorConfigMisc::setupUi(void)
{
 // Шаг изменения отступа
 indentStepLabel=new QLabel(this);
 indentStepLabel->setText(tr("Indent step"));

 indentStepFlexion=new QLabel(this);
 indentStepFlexion->setText(tr("pixels"));

 indentStep=new QSpinBox(this);
 indentStep->setMinimum(1);
 indentStep->setMaximum(250);
 indentStep->setValue(conf->get_indent_step());


 // Кнопка редактирования файла конфигурации WyEdit
 editWyEditConfigFile=new QPushButton(this);
 editWyEditConfigFile->setText(tr("Edit config file"));
 editWyEditConfigFile->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed, QSizePolicy::ToolButton));
}


void EditorConfigMisc::setupSignals(void)
{
  connect(editWyEditConfigFile, SIGNAL(clicked()), this, SLOT(onClickedEditWyEditConfigFile()));
}


void EditorConfigMisc::assembly(void)
{
  // Сборка всех блоков настройки отступа в окно
  QGridLayout *configLayout=new QGridLayout();
  configLayout->addWidget(indentStepLabel,   0, 0);
  configLayout->addWidget(indentStep,         0, 1);
  configLayout->addWidget(indentStepFlexion, 0, 2);


  // Группировщик виджетов для опасной зоны
  QGroupBox *dangerBox=new QGroupBox(this);
  dangerBox->setTitle(tr("Danger actions (Attention!)"));

  // Виджеты вставляются в группировщик опасной зоны
  QVBoxLayout *dangerLayout = new QVBoxLayout();
  dangerLayout->addWidget(editWyEditConfigFile);
  dangerBox->setLayout(dangerLayout);


  QVBoxLayout *centralLayout=new QVBoxLayout();
  centralLayout->addLayout(configLayout);
  centralLayout->addWidget(dangerBox);
  centralLayout->addStretch();

  setLayout(centralLayout);
}


void EditorConfigMisc::onClickedEditWyEditConfigFile(void)
{
  // Сбрасываются в файл конфига все возможные изменения, которые, возможно еще не были записаны
  conf->sync();

  editConfigFile( globalParameters.getWorkDirectory()+"/editorconf.ini", 0.8 );
}


int EditorConfigMisc::applyChanges(void)
{
 qDebug() << "Apply changes editor misc";
 
 // Уровень сложности вносимых изменений
 // 0 - изменения можно делать на лету
 // 1 - для принятия изменений нужна перезагрузка
 int difficultChanges=0;

  // Если был изменен размер основного шрифта
 if(conf->get_indent_step()!=indentStep->value()) {
  conf->set_indent_step(indentStep->value());
 }
 
 return difficultChanges;
}
