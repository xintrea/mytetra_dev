#include "../../main.h"

#include "EditorConfigMisc.h"
#include "ui_EditorConfigMisc.h"
#include "EditorConfig.h"
#include "libraries/GlobalParameters.h"

#include <QDebug>

extern GlobalParameters globalParameters;


EditorConfigMisc::EditorConfigMisc(QWidget *parent)
    : ConfigPage(parent)
    , ui(new Ui::EditorConfigMisc)
{
  ui->setupUi(this);
  // Устанавливается указатель на объект работы с конфигфайлом
  conf=find_object<EditorConfig>("editorconfig");

  setupUi();
  setupSignals();
}

EditorConfigMisc::~EditorConfigMisc()
{
  delete ui;
}


void EditorConfigMisc::setupUi(void)
{
 ui->indentStep->setValue(conf->get_indent_step());
}


void EditorConfigMisc::setupSignals(void)
{
  connect(ui->editWyEditConfigFile, SIGNAL(clicked()), this, SLOT(onClickedEditWyEditConfigFile()));
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
 if(conf->get_indent_step()!=ui->indentStep->value()) {
  conf->set_indent_step(ui->indentStep->value());
 }

 return difficultChanges;
}
