#include <QString>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

#include "ConfigEditorHelper.h"
#include "libraries/wyedit/EditorMultiLineInputDialog.h"
#include "libraries/helpers/DebugHelper.h"


extern QObject *pMainWindow;


ConfigEditorHelper::ConfigEditorHelper()
{

}


// Редактирование произвольного конфиг-файла (конфиг программы, конфиг редактора)
void ConfigEditorHelper::editConfigFile( QString fileName, double sizeCoefficient )
{
  // Окно диалога для редактирования файла конфига
  EditorMultiLineInputDialog dialog( qobject_cast<QWidget *>(pMainWindow) );
  dialog.setWordWrapMode(QTextOption::NoWrap);
  dialog.setWindowTitle(QObject::tr("Edit config file (Be careful!)"));
  dialog.setSizeCoefficient( sizeCoefficient );

  QFile file(fileName);
  if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    criticalError("Can't open config file "+fileName);
  }

  // Установка в окне текста файла
  dialog.setText( QString::fromUtf8(file.readAll()) );

  // Текст из файла получен, файл больше не требуется
  file.close();

  // Если была нажата отмена
  if(dialog.exec()!=QDialog::Accepted)
    return;

  // Если текст в окне не менялся
  if(!dialog.isModified())
    return;


  // Файл конфига открывается на запись
  if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
  {
    criticalError("Can't open config file for write: "+fileName);
  }

  // Измененный текст записывается в файл
  QTextStream out(&file);
  out << dialog.getText();

  // Текст записан, файл больше не требуется
  file.close();

  // Для принятия изменений требуется перезапустить программу
  QMessageBox::warning(qobject_cast<QWidget *>(pMainWindow),
                       QObject::tr("Warning"),
                       QObject::tr("The program will have to be restarted for changes to take effect."),
                       QMessageBox::Ok);
  exit(0);
}
