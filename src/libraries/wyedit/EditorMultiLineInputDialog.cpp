#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QDebug>

#include "EditorMultiLineInputDialog.h"


EditorMultiLineInputDialog::EditorMultiLineInputDialog(QWidget *parent) : QDialog(parent)
{
 setupUi();
 setupSignals();
 assembly();
}


EditorMultiLineInputDialog::~EditorMultiLineInputDialog()
{
 if(textArea!=NULL) delete textArea;
 if(buttonBox!=NULL) delete buttonBox;
}


void EditorMultiLineInputDialog::setupUi()
{
 QSizePolicy sizePolicy;
 sizePolicy.setHorizontalPolicy(QSizePolicy::Expanding);
 sizePolicy.setVerticalPolicy(QSizePolicy::Expanding);

 textArea=new QTextEdit(this);
 textArea->setAcceptRichText(false);
 textArea->setSizePolicy(sizePolicy);

 buttonBox=new QDialogButtonBox(Qt::Horizontal);
 buttonBox->addButton(tr("OK"),QDialogButtonBox::AcceptRole);
 buttonBox->addButton(tr("Cancel"),QDialogButtonBox::RejectRole);

 // Устанавливается размер окна, равный виджету, из которого
 // этот виджет был вызван
 if(this->parent()->isWidgetType())
  {
   QWidget *parentWidget=qobject_cast<QWidget *>(this->parent());
   QRect geom(parentWidget->pos(), parentWidget->size());

   qDebug() << "Parent window geometry " << geom.x() << geom.y() << geom.width() << geom.height();

   setMinimumSize(parentWidget->size());
  }
}


void EditorMultiLineInputDialog::setupSignals()
{
 connect(buttonBox,SIGNAL(accepted()),this,SLOT(accept()));
 connect(buttonBox,SIGNAL(rejected()),this,SLOT(reject()));
}


void EditorMultiLineInputDialog::assembly()
{
 QVBoxLayout *mainLayout=new QVBoxLayout(this);

 // Добавляется область текста
 mainLayout->addWidget(textArea);

 // Добавляется линейка кнопок OK и Cancel
 mainLayout->addWidget(buttonBox);
}


void EditorMultiLineInputDialog::setText(QString text)
{
 textArea->setPlainText(text);
}


QString EditorMultiLineInputDialog::getText()
{
 return textArea->toPlainText();
}


// Выясняется, был ли изменен текст, показанный в диалоге
bool EditorMultiLineInputDialog::isModified() 
{ 
 return textArea->document()->isModified(); 
}


void EditorMultiLineInputDialog::setWordWrapMode(QTextOption::WrapMode mode)
{
  textArea->setWordWrapMode(mode);
}
