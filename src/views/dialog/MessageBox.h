#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H

#include <QDialog>
#include <QLabel>
#include <QTextEdit>
#include <QDialogButtonBox>
#include <QFlags>


// Виджет, похожий на QMessageBox, только позволяющий выделять и копировать текст
// (т. к. в Windows невозможно скопировать текст, отображаемый в QMessageBox)


class MessageBox : public QDialog
{
  Q_OBJECT

public:
  MessageBox(QWidget *parent = 0);

  void setText(QString iText);
  void setDetailedText(QString iDetailedText);
  void setDetailedTextReadOnly(bool iReadOnly);
  void setStandardButtons(QFlags<QDialogButtonBox::StandardButton> buttons);

protected:

  void setupSignals(void);
  void setupUI(void);
  void assembly(void);


  QLabel text;
  QTextEdit detailedText;
  QDialogButtonBox buttonBox;


};

#endif // MESSAGEBOX_H
