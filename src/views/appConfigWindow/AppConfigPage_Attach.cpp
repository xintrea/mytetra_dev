#include <QWidget>
#include <QBoxLayout>
#include <QDir>
#include <QLineEdit>

#include "AppConfigPage_Attach.h"
#include "models/appConfig/AppConfig.h"

extern AppConfig mytetraConfig;


AppConfigPage_Attach::AppConfigPage_Attach(QWidget *parent) : ConfigPage(parent)
{
  qDebug() << "Create attach config page";

  enableRecordWithAttachHighlight.setText( tr("Enable highlight notes with attachments") );
  enableRecordWithAttachHighlight.setChecked( mytetraConfig.getEnableRecordWithAttachHighlight() );

  labelHighlightColor.setText( tr("Highlight color: ") );
  setColorForButtonHighlightColor( QColor( mytetraConfig.getRecordWithAttachHighlightColor() ) );

  // Слой для надписи выбора цвета и кнопки выбора цвета
  QHBoxLayout *colorLayout = new QHBoxLayout;
  colorLayout->addWidget(&labelHighlightColor);
  colorLayout->addWidget(&buttonHighlightColor);
  colorLayout->addStretch();


  // Слой для рамки
  QVBoxLayout *decorBoxlayout = new QVBoxLayout;
  decorBoxlayout->addWidget(&enableRecordWithAttachHighlight);
  decorBoxlayout->addLayout(colorLayout);

  // Рамка
  decorBox.setTitle( tr("Notes with attachments decor") );
  decorBox.setLayout(decorBoxlayout);

  // Собирается основной слой
  QVBoxLayout *centralLayout=new QVBoxLayout();
  centralLayout->addWidget(&decorBox);
  centralLayout->addStretch();

  // Основной слой устанавливается
  setLayout(centralLayout);
}


void AppConfigPage_Attach::setColorForButtonHighlightColor(QColor iColor)
{
  // Квадратик на кнопке выбора цвета кода
  QPixmap pix(16, 16);
  pix.fill(iColor.rgb());
  buttonHighlightColor.setIcon(pix);

  highlightColor=iColor;
}


// Метод должен возвращать уровень сложности сделанных изменений
// 0 - изменения не требуют перезапуска программы
// 1 - изменения требуют перезапуска программы
int AppConfigPage_Attach::applyChanges(void)
{
  qDebug() << "Apply changes attach";

  int result=0;

  /*
  // Сохраняется настройка подтверждения для действия "cut" на ветке
  if(mytetraConfig.get_cutbranchconfirm()!=cutBranchConfirm->isChecked())
    mytetraConfig.set_cutbranchconfirm(cutBranchConfirm->isChecked());

  // Сохраняется настройка отображения отладочных сообщений в консоли
  if(mytetraConfig.get_printdebugmessages()!=printDebugMessages->isChecked())
    mytetraConfig.set_printdebugmessages(printDebugMessages->isChecked());

  // Сохраняется настройка режима запуска MyTetra - обычный или свернутый
  if(mytetraConfig.get_runinminimizedwindow()!=runInMinimizedWindow->isChecked())
    mytetraConfig.set_runinminimizedwindow(runInMinimizedWindow->isChecked());

  // Сохраняется настройка разрешения/запрещения лога действий
  if(mytetraConfig.getEnableLogging()!=enableActionLog->isChecked())
  {
    mytetraConfig.setEnableLogging(enableActionLog->isChecked());
    result=1;
  }

  // Сохраняется настройка нужно ли вспоминать позицию курсора при перемещении
  // по истории
  if(mytetraConfig.getRememberCursorAtHistoryNavigation()!=rememberAtHistoryNavigationCheckBox->isChecked())
    mytetraConfig.setRememberCursorAtHistoryNavigation(rememberAtHistoryNavigationCheckBox->isChecked());

  // Сохраняется настройка нужно ли пытаться вспоминать позицию курсора при
  // обычном выборе записи
  if(mytetraConfig.getRememberCursorAtOrdinarySelection()!=rememberAtOrdinarySelectionCheckBox->isChecked())
    mytetraConfig.setRememberCursorAtOrdinarySelection(rememberAtOrdinarySelectionCheckBox->isChecked());
  */

  return result;
}
