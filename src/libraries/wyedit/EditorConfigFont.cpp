#include "EditorConfigFont.h"
#include "ui_EditorConfigFont.h"
#include "EditorConfig.h"

#include "../../main.h"

#include <QColorDialog>
#include <QDebug>

EditorConfigFont::EditorConfigFont(QWidget *parent)
    : ConfigPage(parent)
    , ui(new Ui::EditorConfigFont)
{
  qDebug() << "Create editor config font widget";
  ui->setupUi(this);

 // Устанавливается указатель на объект работы с конфигфайлом
 // conf=this->parent()->parent()->parent()->editorconfig;
 conf=find_object<EditorConfig>("editorconfig");

 setup_ui();
 setup_signals();
}


void EditorConfigFont::setup_ui(void)
{
 // Шрифт
 QFont defaultFont;
 defaultFont.fromString(conf->get_default_font());
 ui->defaultFontselect->setCurrentFont(defaultFont);

 // Размер
 ui->defaultFontsize->setValue(conf->get_default_font_size());

 // Шрифт
 QFont monoFont;
 monoFont.fromString(conf->get_monospace_font());
 ui->monospaceFontselect->setCurrentFont(monoFont);

 // Размер
 ui->monospaceFontsize->setValue(conf->get_monospace_font_size());
 ui->monospaceFontsize->setEnabled( conf->get_monospace_font_size_apply() );

 // Галка применения размера при нажатии кнопки моноширинного форматирования
 ui->monospaceFontsizeapply->setChecked( conf->get_monospace_font_size_apply() );

 // Шрифт
 QFont codeFont;
 codeFont.fromString(conf->get_code_font());
 ui->codeFontselect->setCurrentFont(codeFont);

 // Размер
 ui->codeFontsize->setValue(conf->get_code_font_size());
 ui->codeFontsize->setEnabled( conf->get_code_font_size_apply() );

 // Галка применения размера при нажатии кнопки форматирования кода
 ui->codeFontsizeapply->setChecked( conf->get_code_font_size_apply() );

 // Размер отступа
 ui->codeIndentsize->setValue(conf->get_code_indent_size());
 ui->codeIndentsize->setEnabled( conf->get_code_indent_size_apply() );

 // Галка применения размера отступа
 ui->codeIndentsizeapply->setChecked( conf->get_code_indent_size_apply() );

 // Кнопка выбора цвета текста кода
 codeColor = new QColor(conf->get_code_font_color());

 // Квадратик на кнопке выбора цвета кода
 QPixmap pix(16, 16);
 pix.fill(codeColor->rgb());
 ui->codeSelectColorButton->setIcon(pix);
}


void EditorConfigFont::setup_signals(void)
{
 connect(ui->monospaceFontsizeapply,SIGNAL(stateChanged(int)),this,SLOT(on_monospace_applysize_state_changed(int)));

 connect(ui->codeFontsizeapply,SIGNAL(stateChanged(int)),this,SLOT(on_code_applysize_state_changed(int)));
 connect(ui->codeIndentsizeapply,SIGNAL(stateChanged(int)),this,SLOT(on_code_applyindent_state_changed(int)));

 connect(ui->codeSelectColorButton, SIGNAL(clicked()), this, SLOT(on_code_select_color_button_click()));
}


void EditorConfigFont::on_monospace_applysize_state_changed(int i)
{
 if(i==Qt::Checked) ui->monospaceFontsize->setEnabled(true);
 else               ui->monospaceFontsize->setEnabled(false);
}


void EditorConfigFont::on_code_applysize_state_changed(int i)
{
 if(i==Qt::Checked) ui->codeFontsize->setEnabled(true);
 else               ui->codeFontsize->setEnabled(false);
}


void EditorConfigFont::on_code_applyindent_state_changed(int i)
{
 if(i==Qt::Checked) ui->codeIndentsize->setEnabled(true);
 else               ui->codeIndentsize->setEnabled(false);
}


void EditorConfigFont::on_code_select_color_button_click()
{
 // Исходный цвет
 QColor color(conf->get_code_font_color());

 // Диалог запроса цвета
 QColor selectedColor=QColorDialog::getColor(color, this);

 // Если цвет выбран, и он правильный
 if(selectedColor.isValid())
  {
   // Меняется цвет кнопки (не работает в Ubuntu 10.04)
   // codeSelectColorButton->setPalette(QPalette(selectedColor));

   // Меняется цвет квадратика на кнопке
   QPixmap pix(16, 16);
   pix.fill(selectedColor);
   ui->codeSelectColorButton->setIcon(pix);

   // Запоминается новый цвет
   codeColor->setNamedColor(selectedColor.name());
  }
}


int EditorConfigFont::applyChanges(void)
{
 qDebug() << "Apply changes editor font";
 
 // Уровень сложности вносимых изменений
 // 0 - изменения можно делать на лету
 // 1 - для принятия изменений нужна перезагрузка
 int difficultChanges=0;

 
 // Если был изменен основной шрифт
 if(conf->get_default_font()!=ui->defaultFontselect->currentFont().toString()) {
  conf->set_default_font(ui->defaultFontselect->currentFont().toString());
 }
 
 // Если был изменен размер основного шрифта
 if(conf->get_default_font_size()!=ui->defaultFontsize->value()) {
  conf->set_default_font_size(ui->defaultFontsize->value());
 }


 // Если был изменен моноширинный шрифт
 if(conf->get_monospace_font()!=ui->monospaceFontselect->currentFont().toString()) {
  conf->set_monospace_font(ui->monospaceFontselect->currentFont().toString());
 }
 
 // Если был изменен размер моноширинного шрифта
 if(conf->get_monospace_font_size()!=ui->monospaceFontsize->value()) {
  conf->set_monospace_font_size(ui->monospaceFontsize->value());
 }

 // Если была изменена галка разрешения установки размера моноширинного шрифта
 if(conf->get_monospace_font_size_apply()!=ui->monospaceFontsizeapply->isChecked()) {
  conf->set_monospace_font_size_apply(ui->monospaceFontsizeapply->isChecked());
 }
 

 // Если был изменен шрифт форматирования кода
 if(conf->get_code_font()!=ui->codeFontselect->currentFont().toString()) {
  conf->set_code_font(ui->codeFontselect->currentFont().toString());
 }

 // Если был изменен размер шрифта форматирования кода
 if(conf->get_code_font_size()!=ui->codeFontsize->value()) {
  conf->set_code_font_size(ui->codeFontsize->value());
 }

 // Если была изменена галка разрешения установки размера шрифта кода
 if(conf->get_code_font_size_apply()!=ui->codeFontsizeapply->isChecked()) {
  conf->set_code_font_size_apply(ui->codeFontsizeapply->isChecked());
 }

 // Если был изменен отступ шрифта кода
 if(conf->get_code_indent_size()!=ui->codeIndentsize->value()) {
  conf->set_code_indent_size(ui->codeIndentsize->value());
 }

 // Если была изменена галка разрешения установки отступа шрифта кода
 if(conf->get_code_indent_size_apply()!=ui->codeIndentsizeapply->isChecked()) {
  conf->set_code_indent_size_apply(ui->codeIndentsizeapply->isChecked());
 }

 // Если был изменен цвет начертания кода
 if(conf->get_code_font_color()!=codeColor->name())
  conf->set_code_font_color(codeColor->name());

 return difficultChanges;
}
