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

#include "EditorConfigMathExpression.h"
#include "EditorConfig.h"
#include "libraries/GlobalParameters.h"

extern GlobalParameters globalParameters;


EditorConfigMathExpression::EditorConfigMathExpression(QWidget *parent) : ConfigPage(parent)
{
  // Устанавливается указатель на объект работы с конфигфайлом
  conf=find_object<EditorConfig>("editorconfig");

  setupUi();
  setupSignals();
  assembly();
}


void EditorConfigMathExpression::setupUi(void)
{
 // Счетчик для таймера обновления картинки формулы в редакторе формулы
 updateFormulaTimeLabel = new QLabel(tr("Formula update timer"), this);
 updateFormulaTimeSecLabel = new QLabel(tr("sec."), this);

 updateFormulaTimeSpinBox = new QSpinBox(this);
 updateFormulaTimeSpinBox->setMinimum(1);
 updateFormulaTimeSpinBox->setMaximum(20);
 updateFormulaTimeSpinBox->setValue(conf->getMathExpressionUpdateTime());
}


void EditorConfigMathExpression::setupSignals(void)
{

}


void EditorConfigMathExpression::assembly(void)
{
  // Сборка всех блоков настройки отступа в окно
  QGridLayout *configLayout=new QGridLayout();

  // Компановка контролов Счетчика для таймера обновления картинки формулы в редакторе формулы
  configLayout->addWidget(updateFormulaTimeLabel,    0, 0);
  configLayout->addWidget(updateFormulaTimeSpinBox,  0, 1);
  configLayout->addWidget(updateFormulaTimeSecLabel, 0, 2);

  QVBoxLayout *centralLayout=new QVBoxLayout();
  centralLayout->addLayout(configLayout);
  centralLayout->addStretch();

  setLayout(centralLayout);
}


void EditorConfigMathExpression::onClickedEditWyEditConfigFile(void)
{
  // Сбрасываются в файл конфига все возможные изменения, которые, возможно еще не были записаны
  conf->sync();

  editConfigFile( globalParameters.getWorkDirectory()+"/editorconf.ini", 0.8f );
}


int EditorConfigMathExpression::applyChanges(void)
{
 qDebug() << "Apply changes editor math expression";
 
 // Уровень сложности вносимых изменений
 // 0 - изменения можно делать на лету
 // 1 - для принятия изменений нужна перезагрузка
 int difficultChanges=0;

 // Если был изменен счетчик для таймера обновления картинки формулы в редакторе формулы
 if(conf->getMathExpressionUpdateTime()!=updateFormulaTimeSpinBox->value()) {
   conf->setMathExpressionUpdateTime(updateFormulaTimeSpinBox->value());
 }
 
 return difficultChanges;
}
