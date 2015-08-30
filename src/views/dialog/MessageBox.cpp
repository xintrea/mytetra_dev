#include <QVBoxLayout>

#include "main.h"
#include "MessageBox.h"
#include "views/mainWindow/MainWindow.h"



MessageBox::MessageBox(QWidget *parent) : QDialog(parent)
{
  setupUI();
  setupSignals();
  assembly();
}


void MessageBox::setupUI(void)
{
  int w=find_object<MainWindow>("mainwindow")->width();
  int h=find_object<MainWindow>("mainwindow")->height();

  this->resize(w*2/3, h/3);
}


void MessageBox::setupSignals(void)
{
  connect(&buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(&buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}


void MessageBox::assembly(void)
{
  // Размещалка элементов
  QVBoxLayout *layout=new QVBoxLayout(this);

  layout->addWidget(&text);
  layout->addWidget(&detailedText);
  layout->addWidget(&buttonBox);
}


void MessageBox::setText(QString iText)
{
  text.setText(iText);
}


void MessageBox::setDetailedText(QString iDetailedText)
{
  detailedText.setText(iDetailedText);
}


void MessageBox::setDetailedTextReadOnly(bool iReadOnly)
{
  detailedText.setReadOnly(iReadOnly);
}


void MessageBox::setStandardButtons(QFlags<QDialogButtonBox::StandardButton> buttons)
{
  buttonBox.setStandardButtons(buttons);
}

