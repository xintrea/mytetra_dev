#include <QWidget>
#include <QTextEdit>
#include <QtDebug>
#include <QSizePolicy>
#include <QIcon>
#include <QMessageBox>
#include <QTextDocumentFragment>

#include "InfoFieldEnter.h"
#include "RecordInfoFieldsEditor.h"
#include "main.h"


// Окно редактирования информационных полей записи (не текста записи!)
// Оно появляется при двойном клике на записи или при клике на кнопку редактирования полей записи

#if QT_VERSION < 0x050000
RecordInfoFieldsEditor::RecordInfoFieldsEditor( QWidget * parent, Qt::WFlags f) : QDialog(parent, f)
#else
RecordInfoFieldsEditor::RecordInfoFieldsEditor( QWidget * parent, Qt::WindowFlags f) : QDialog(parent, f)
#endif
{
  setupUI();
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
 
 // На кнопку OK назначается комбинация клавиш Ctrl+Enter
 QPushButton *OkButton=buttonBox->button(QDialogButtonBox::Ok); // Выясняется указатель на кнопку OK
 OkButton->setShortcut( QKeySequence(Qt::CTRL + Qt::Key_Return) ); // Устанавливается шорткат
 OkButton->setToolTip(tr("Ctrl+Enter"));
}


void RecordInfoFieldsEditor::setupSignals(void)
{
 connect(buttonBox, SIGNAL(accepted()), this, SLOT(okClick(void)));
 connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}


void RecordInfoFieldsEditor::assembly(void)
{
 // Размещалка элементов
 QVBoxLayout *layout=new QVBoxLayout();
 layout->setMargin(8);
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

