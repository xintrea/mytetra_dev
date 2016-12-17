#include "AppConfigPage_Attach.h"
#include "models/appConfig/AppConfig.h"
#include "ui_AppConfigPage_Attach.h"

#include <QWidget>
#include <QDir>
#include <QColorDialog>


extern AppConfig mytetraConfig;


AppConfigPage_Attach::AppConfigPage_Attach(QWidget *parent)
    : ConfigPage(parent)
    , ui(new Ui::AppConfigPage_Attach)
{
  ui->setupUi(this);
  setupUi();
  setupSignals();
}


void AppConfigPage_Attach::setupUi(void)
{
  qDebug() << "Create attach config page";

  // Галка разрешения/запрещения подсветки записи с прикрепленными файлами
  ui->enableRecordWithAttachHighlight->setChecked( mytetraConfig.getEnableRecordWithAttachHighlight() );

  // Выбор цвета для записи с прикрепленными файлами
  setColorForButtonHighlightColor( QColor( mytetraConfig.getRecordWithAttachHighlightColor() ) );

  // Начальный выбор цвета активируется или отключается в зависимости от галки
  onEnableRecordWithAttachHighlight( mytetraConfig.getEnableRecordWithAttachHighlight() );
}


void AppConfigPage_Attach::setupSignals(void)
{
  connect( ui->enableRecordWithAttachHighlight, SIGNAL( toggled(bool) ), this, SLOT( onEnableRecordWithAttachHighlight(bool) ) );
  connect( ui->buttonHighlightColor, SIGNAL(clicked()), this, SLOT(onClickedHighlightColor()) );
}


void AppConfigPage_Attach::setColorForButtonHighlightColor(QColor iColor)
{
  // Квадратик на кнопке выбора цвета кода
  QPixmap pix(16, 16);
  pix.fill( iColor.rgb() );
  ui->buttonHighlightColor->setIcon(pix);

  highlightColor=iColor;
}


void AppConfigPage_Attach::onEnableRecordWithAttachHighlight(bool state)
{
  ui->labelHighlightColor->setEnabled(state);
  ui->buttonHighlightColor->setEnabled(state);
}


void AppConfigPage_Attach::onClickedHighlightColor()
{
  // Диалог запроса цвета
  QColor selectedColor=QColorDialog::getColor(highlightColor, this);

  // Если цвет выбран, и он правильный
  if(selectedColor.isValid())
    setColorForButtonHighlightColor(selectedColor);
}


// Метод должен возвращать уровень сложности сделанных изменений
// 0 - изменения не требуют перезапуска программы
// 1 - изменения требуют перезапуска программы
int AppConfigPage_Attach::applyChanges(void)
{
  qDebug() << "Apply changes attach";

  int result=0;

  // Сохраняется настройка разрешения/запрещения подсветки записи с прикрепленными файлами
  if(mytetraConfig.getEnableRecordWithAttachHighlight() != ui->enableRecordWithAttachHighlight->isChecked())
    mytetraConfig.setEnableRecordWithAttachHighlight(ui->enableRecordWithAttachHighlight->isChecked());

  // Сохраняется цвет подсветки
  if(mytetraConfig.getRecordWithAttachHighlightColor()!=highlightColor.name())
    mytetraConfig.setRecordWithAttachHighlightColor(highlightColor.name());

  return result;
}
