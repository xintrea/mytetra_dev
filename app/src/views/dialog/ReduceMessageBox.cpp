#include <QVBoxLayout>

#include "main.h"
#include "ReduceMessageBox.h"
#include "views/mainWindow/MainWindow.h"



ReduceMessageBox::ReduceMessageBox(QWidget *parent) : QDialog(parent)
{
  setupUI();
  setupSignals();
  assembly();
}


void ReduceMessageBox::setupUI(void)
{
  int w=find_object<MainWindow>("mainwindow")->width();
  int h=find_object<MainWindow>("mainwindow")->height();

  this->resize(w*2/3, h/3);
}


void ReduceMessageBox::setupSignals(void)
{
  connect(&buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(&buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}


void ReduceMessageBox::assembly(void)
{
  // Размещалка элементов
  QVBoxLayout *layout=new QVBoxLayout(this);

  layout->addWidget(&text);
  layout->addWidget(&detailedText);
  layout->addWidget(&buttonBox);
}


void ReduceMessageBox::setText(QString iText)
{
  text.setText(iText);
}


void ReduceMessageBox::setDetailedText(QString iDetailedText)
{
  detailedText.setText(iDetailedText);
}


void ReduceMessageBox::setDetailedTextReadOnly(bool iReadOnly)
{
  detailedText.setReadOnly(iReadOnly);
}


void ReduceMessageBox::setStandardButtons(QFlags<QDialogButtonBox::StandardButton> buttons)
{
  buttonBox.setStandardButtons(buttons);
}

