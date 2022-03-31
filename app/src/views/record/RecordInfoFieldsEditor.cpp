#include <QWidget>
#include <QTextEdit>
#include <QtDebug>
#include <QSizePolicy>
#include <QIcon>
#include <QMessageBox>
#include <QTextDocumentFragment>

#include "main.h"
#include "InfoFieldEnter.h"
#include "RecordInfoFieldsEditor.h"
#include "libraries/ShortcutManager.h"
#include "libraries/helpers/DebugHelper.h"


extern ShortcutManager shortcutManager;


// Окно редактирования информационных полей записи (не текста записи!)
// Оно появляется при двойном клике на записи или при клике на кнопку редактирования полей записи

RecordInfoFieldsEditor::RecordInfoFieldsEditor( QWidget * parent, Qt::WindowFlags f) : QDialog(parent, f)
{
  setupUI();
  setupShortcuts();
  setupSignals();
  assembly();
}


RecordInfoFieldsEditor::~RecordInfoFieldsEditor()
{

}


void RecordInfoFieldsEditor::setupUI(void)
{
 // Ввод инфополей записи
 infoField=new InfoFieldEnter();

 // Кнопки OK и Cancel
 buttonBox=new QDialogButtonBox();
 buttonBox->setOrientation(Qt::Horizontal);
 buttonBox->setStandardButtons(QDialogButtonBox::Ok|QDialogButtonBox::NoButton|QDialogButtonBox::Cancel);
}


// Может вызваться повторно при изменении схемы горячих клавиш
void RecordInfoFieldsEditor::setupShortcuts(void)
{
    qDebug() << "Setup shortcut for" << this->metaObject()->className();

    // На кнопку OK назначается комбинация клавиш Ctrl+Enter
    QPushButton *okButton=buttonBox->button(QDialogButtonBox::Ok); // Выясняется указатель на кнопку OK
    okButton->setShortcut( shortcutManager.getKeySequence("misc-editConfirm") ); // Устанавливается шорткат
    okButton->setToolTip( shortcutManager.getKeySequenceAsText("misc-editConfirm") ); // ToolTip зависит от шортката
}


void RecordInfoFieldsEditor::setupSignals(void)
{
    connect(buttonBox, &QDialogButtonBox::accepted, this, &RecordInfoFieldsEditor::okClick);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &RecordInfoFieldsEditor::reject);

    // Обновление горячих клавиш, если они были изменены
    connect(&shortcutManager, &ShortcutManager::updateWidgetShortcut, this, &RecordInfoFieldsEditor::setupShortcuts);
}


void RecordInfoFieldsEditor::assembly(void)
{
 // Размещалка элементов
 QVBoxLayout *layout=new QVBoxLayout();
 layout->setContentsMargins(8, 8, 8, 8);
 layout->setSpacing(10);

 // Добавление элементов в размещалку
 layout->addWidget(infoField);
 layout->addWidget(buttonBox,0,Qt::AlignRight);

 layout->setSizeConstraint(QLayout::SetFixedSize);

 setLayout(layout);

 // setSizePolicy(QSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum));

 // Фокус устанавливается на поле ввода названия записи
 infoField->setFocusToStart();
}


void RecordInfoFieldsEditor::okClick(void)
{
 QString message="";

 // Проверка наличия названия записи
 if(infoField->getField("name").length()==0)
  message=message+tr("Please enter the note's <b>title</b>.");

 // Если что-то не заполнено, выдается предупреждение
 if(message.length()>0)
  {
   QMessageBox::warning(this,tr("The note's fields cannot be modified"),message,
                        QMessageBox::Close);
   return;
  }
 else
  {
   emit(accept());
  }
}


QString RecordInfoFieldsEditor::getField(QString name)
{
 if(infoField->checkFieldName(name)==true)
  return infoField->getField(name);
 else
  criticalError("Can not get field "+name+" in editrecord method get_field");

 return QString();
}


void RecordInfoFieldsEditor::setField(QString name, QString value)
{
 if(infoField->checkFieldName(name)==true)
  infoField->setField(name,value);
 else
  criticalError("Can not set field "+name+" in editrecord method set_field");
}


void RecordInfoFieldsEditor::setReadOnly(bool state)
{
  infoField->setReadOnly(state);
}


bool RecordInfoFieldsEditor::isReadOnly()
{
  return infoField->isReadOnly();
}

