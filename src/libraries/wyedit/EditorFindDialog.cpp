#include <QPushButton>
#include <QLineEdit>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QtGlobal>
#include <QtDebug>
#include <QShowEvent>

#include "Editor.h"
#include "EditorConfig.h"
#include "EditorFindDialog.h"


EditorFindDialog::EditorFindDialog(QWidget *parent) : QDialog(parent)
{
  setup_ui();
  setup_signals();
  assembly();

  QShowEvent event;
  showEvent(&event);
}


void EditorFindDialog::setup_ui(void)
{
  lineEdit=new QLineEdit();
  lineEdit->setMinimumWidth(120);

  mathCase=new QCheckBox(tr("&Case sensitive"));
  wholeWords=new QCheckBox(tr("&Whole words only"));
  searchBackward=new QCheckBox(tr("Search &backward"));

  findButton=new QPushButton(tr("&Find"));
  findButton->setDefault(true);
  findButton->setEnabled(false);

  this->setWindowTitle(tr("Find in the text"));
}


void EditorFindDialog::setup_signals(void)
{
  connect(lineEdit, SIGNAL(textChanged(const QString &)),
          this, SLOT(enable_find_button(const QString &)));

  connect(findButton, SIGNAL(clicked()),
          this, SLOT(find_clicked()));
}


void EditorFindDialog::assembly(void)
{
  QHBoxLayout *findLineLayout=new QHBoxLayout();
  findLineLayout->addWidget(lineEdit);
  findLineLayout->addWidget(findButton);

  QVBoxLayout *centralLayout=new QVBoxLayout();
  centralLayout->addLayout(findLineLayout);
  centralLayout->addWidget(mathCase);
  centralLayout->addWidget(wholeWords);
  centralLayout->addWidget(searchBackward);
  
  this->setLayout(centralLayout);

  this->setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::MSWindowsFixedSizeDialogHint | Qt::WindowCloseButtonHint);
}


// Действия при нажатии кнопки Find
void EditorFindDialog::find_clicked(void)
{
  QString text=lineEdit->text();

  QTextDocument::FindFlags flags=0;
  if(mathCase->isChecked())      flags|=QTextDocument::FindCaseSensitively;
  if(wholeWords->isChecked())    flags|=QTextDocument::FindWholeWords;
  if(searchBackward->isChecked())flags|=QTextDocument::FindBackward;

  emit find_text(text, flags);
}


// Кнопка поиска активна только тогда, когда есть текст для поиска
void EditorFindDialog::enable_find_button(const QString &text)
{
  findButton->setEnabled(!text.isEmpty());
}


void EditorFindDialog::hideEvent(QHideEvent *event)
{
  qDebug() << "Hide event of find dialog, window x " << this->x() << " y " << this->y() << " width " << this->width() << " height " << this->height();

  if( this->width()>0 && this->height()>0 && !(this->x()<=0 && this->y()<=0) )
  {
    // Получение ссылки в parent виджете на нужное поле
    EditorConfig *edConf=qobject_cast<Editor *>(parent())->editorConfig;

    // Запоминается геометрия
    QRect g=this->frameGeometry();
    qDebug() << "Frame geometry X " << g.x() << " Y " << g.y() << " W " << g.width() << " H" << g.height();
    QString gs=QString::number(g.x())+","+
               QString::number(g.y())+","+
               QString::number(g.width())+","+
               QString::number(g.height());
    edConf->set_finddialog_geometry(gs);
  }

  QWidget::hideEvent(event);
}


void EditorFindDialog::showEvent(QShowEvent *event)
{
  qDebug() << "Show event of find dialog";

  lineEdit->setFocus();

  // Получение ссылки в parent виджете на нужное свойство
  EditorConfig *edConf=qobject_cast<Editor *>(parent())->editorConfig;

  QString geometry=edConf->get_finddialog_geometry();

  // Если была запомнена геометрия окна, устанавливается прежняя геометрия
  if(!geometry.isEmpty())
  {
    QStringList geometry_split=geometry.split(",");
    int x=geometry_split.at(0).toInt();
    int y=geometry_split.at(1).toInt();
    // int w=geometry_split.at(2).toInt();
    // int h=geometry_split.at(3).toInt();
    this->move(x,y);
  }
  else
    qDebug() << "Previos geometry of find dialog is not setted";

  QWidget::showEvent(event);
}

