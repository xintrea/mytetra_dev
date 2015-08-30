#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H

#include <QDialog>

// Виджет, похожий на QMessageBox, только позволяющий выделять и копировать текст
// (т. к. в Windows невозможно скопировать текст, отображаемый в QMessageBox)


class MessageBox : public QDialog
{
  Q_OBJECT

public:
  MessageBox(QWidget *parent = 0);

  void setText(QString iText);
  void setDetailText(QString iDetailText);
  void setButtonFlag(QString iDetailText);

protected:

  QLabel *text;
  QLineEdit *detailText;
  QDialogButtonBox *buttonBox;
  QPushButton *OkButton;
  QPushButton *CancelButton;

};

#endif // MESSAGEBOX_H
