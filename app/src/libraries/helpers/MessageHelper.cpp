#include <QString>
#include <QMessageBox>

#include "MessageHelper.h"


// Выдача на экран простого окна с сообщением
void showMessageBox(QString message)
{
  QMessageBox msgBox;
  msgBox.setText(message);
  msgBox.exec();
}

