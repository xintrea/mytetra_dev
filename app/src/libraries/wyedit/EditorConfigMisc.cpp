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

 // Размер табуляции для клавиши Tab
 tabStopDistanceLabel=new QLabel(this);
 tabStopDistanceLabel->setText(tr("Tab size"));

 tabStopDistanceFlexionLabel=new QLabel(this);
 tabStopDistanceFlexionLabel->setText(tr("letters"));

 tabStopDistanceSpinBox=new QSpinBox(this);
 tabStopDistanceSpinBox->setMinimum(1);
 tabStopDistanceSpinBox->setMaximum(50);
 tabStopDistanceSpinBox->setValue(conf->get_tab_size());

 // Кнопка редактирования файла конфигурации WyEdit
 editWyEditConfigFile=new QPushButton(this);
 editWyEditConfigFile->setText(tr("Edit config file"));
 editWyEditConfigFile->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed, QSizePolicy::ToolButton));
}


void EditorConfigMisc::setupSignals(void)
{
  connect(editWyEditConfigFile, &QPushButton::clicked,
          this,                 &EditorConfigMisc::onClickedEditWyEditConfigFile);
}


void EditorConfigMisc::assembly(void)
{
  // Сборка всех блоков настройки отступа в окно
  QGridLayout *configLayout=new QGridLayout();
  configLayout->addWidget(indentStepLabel,   0, 0);
  configLayout->addWidget(indentStep,        0, 1);
  configLayout->addWidget(indentStepFlexion, 0, 2);

  // Компановка контролов "Шаг дистанции для клавиши Tab"
  configLayout->addWidget(tabStopDistanceLabel,         1, 0);
  configLayout->addWidget(tabStopDistanceSpinBox,       1, 1);
  configLayout->addWidget(tabStopDistanceFlexionLabel,  1, 2);

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

  editConfigFile( globalParameters.getWorkDirectory()+"/editorconf.ini", 0.8f );
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

 // Если был изменен размер табуляции для клавиши Tab
 if(conf->get_tab_size()!=tabStopDistanceSpinBox->value()) {
   conf->set_tab_size(tabStopDistanceSpinBox->value());
 }
 
 return difficultChanges;
}
