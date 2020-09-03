#include "MessageHelper.h"

#include <QString>
#include <QMessageBox>


// Выдача на экран простого окна с сообщением
void showMessageBox(QString message)
{
  QMessageBox msgBox;
  msgBox.setText(message);
  msgBox.exec();
}

