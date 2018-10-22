#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QDebug>
#include <QPushButton>

#include "EditorMultiLineInputDialog.h"
#include "libraries/ShortcutManager.h"


extern ShortcutManager shortcutManager;


EditorMultiLineInputDialog::EditorMultiLineInputDialog(QWidget *parent) : QDialog(parent)
{
  sizeCoefficient=1.0;

  setupUi();
  setupShortcuts();
  setupSignals();
  assembly();
}


EditorMultiLineInputDialog::~EditorMultiLineInputDialog()
{
  if(textArea!=NULL)
    delete textArea;

  if(buttonBox!=NULL)
    delete buttonBox;
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
  buttonBox->setOrientation(Qt::Horizontal);
  buttonBox->setStandardButtons(QDialogButtonBox::Ok|QDialogButtonBox::NoButton|QDialogButtonBox::Cancel);

  // На кнопку OK назначается комбинация клавиш Ctrl+Enter и она устанавливается как кнопка по умолчанию
  QPushButton *OkButton=buttonBox->button(QDialogButtonBox::Ok); // Выясняется указатель на кнопку OK
  OkButton->setAutoDefault(true);
  OkButton->setDefault(true);


  // С кнопки Cancel снимается флаг срабатывания по умолчанию,
  // это нужно для тулкитов основанных на GTK и прочих подобных библиотеках
  QPushButton *CancelButton=buttonBox->button(QDialogButtonBox::Cancel); // Выясняется указатель на кнопку Cancel
  CancelButton->setAutoDefault(false);
  CancelButton->setDefault(false);


  // Устанавливается размер окна, основанный на размере виджета, из которого
  // этот виджет был вызван
  if(this->parent()!=0)
    if(this->parent()->isWidgetType())
        updateSize();
}


void EditorMultiLineInputDialog::setupShortcuts(void)
{
    QPushButton *OkButton=buttonBox->button(QDialogButtonBox::Ok); // Выясняется указатель на кнопку OK
    OkButton->setShortcut( shortcutManager.getKeySequence("misc-editConfirm") ); // Устанавливается шорткат
    OkButton->setToolTip(shortcutManager.getKeySequenceAsText("misc-editConfirm")); // ToolTip зависит от шортката
}


void EditorMultiLineInputDialog::setupSignals()
{
  connect(buttonBox, &QDialogButtonBox::accepted, this, &EditorMultiLineInputDialog::accept);
  connect(buttonBox, &QDialogButtonBox::rejected, this, &EditorMultiLineInputDialog::reject);
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


void EditorMultiLineInputDialog::setSizeCoefficient(float f)
{
  sizeCoefficient=f;

  updateSize();
}


void EditorMultiLineInputDialog::updateSize()
{
  QWidget *parentWidget=qobject_cast<QWidget *>(this->parent());
  QRect geom(parentWidget->pos(), parentWidget->size());

  geom.setHeight( (int)((float)geom.height()*sizeCoefficient) );
  geom.setWidth ( (int)((float)geom.width()*sizeCoefficient) );

  qDebug() << "Parent window geometry " << geom.x() << geom.y() << geom.width() << geom.height();

  setMinimumSize(geom.width(), geom.height());
}

