#include <QWidget>
#include <QBoxLayout>
#include <QDir>
#include <QLineEdit>
#include <QColorDialog>
#include <QGroupBox>

#include "AppConfigPage_Attach.h"
#include "models/appConfig/AppConfig.h"

extern AppConfig mytetraConfig;


AppConfigPage_Attach::AppConfigPage_Attach(QWidget *parent) : ConfigPage(parent)
{
  setupUi();
  setupSignals();
  assembly();
}


void AppConfigPage_Attach::setupUi(void)
{
  qDebug() << "Create attach config page";

  // Галка разрешения/запрещения подсветки записи с прикрепленными файлами
  enableRecordWithAttachHighlight.setText( tr("Enable highlight notes with attachments") );
  enableRecordWithAttachHighlight.setChecked( mytetraConfig.getEnableRecordWithAttachHighlight() );

  // Выбор цвета для записи с прикрепленными файлами
  labelHighlightColor.setText( tr("Highlight color: ") );
  setColorForButtonHighlightColor( QColor( mytetraConfig.getRecordWithAttachHighlightColor() ) );

  // Начальный выбор цвета активируется или отключается в зависимости от галки
  onEnableRecordWithAttachHighlight( mytetraConfig.getEnableRecordWithAttachHighlight() );
}


void AppConfigPage_Attach::setupSignals(void)
{
  connect( &enableRecordWithAttachHighlight, SIGNAL( toggled(bool) ), this, SLOT( onEnableRecordWithAttachHighlight(bool) ) );
  connect( &buttonHighlightColor, SIGNAL(clicked()), this, SLOT(onClickedHighlightColor()) );
}


void AppConfigPage_Attach::assembly(void)
{
  // Слой для надписи выбора цвета и кнопки выбора цвета
  QHBoxLayout *colorLayout = new QHBoxLayout;
  colorLayout->addWidget( &labelHighlightColor );
  colorLayout->addWidget( &buttonHighlightColor );
  colorLayout->addStretch();

  // Слой для рамки
  QVBoxLayout *decorBoxlayout = new QVBoxLayout(this);
  decorBoxlayout->addWidget( &enableRecordWithAttachHighlight );
  decorBoxlayout->addLayout( colorLayout );

  // Рамка
  decorBox = new QGroupBox(this);
  decorBox->setTitle( tr("Displaying notes with attachments") );
  decorBox->setLayout( decorBoxlayout );

  // Собирается основной слой
  QVBoxLayout *centralLayout=new QVBoxLayout();
  centralLayout->addWidget( decorBox );
  centralLayout->addStretch();

  // Основной слой устанавливается
  setLayout( centralLayout );
}


void AppConfigPage_Attach::setColorForButtonHighlightColor(QColor iColor)
{
  // Квадратик на кнопке выбора цвета кода
  QPixmap pix(16, 16);
  pix.fill( iColor.rgb() );
  buttonHighlightColor.setIcon(pix);

  highlightColor=iColor;
}


void AppConfigPage_Attach::onEnableRecordWithAttachHighlight(bool state)
{
  labelHighlightColor.setEnabled(state);
  buttonHighlightColor.setEnabled(state);
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
  if(mytetraConfig.getEnableRecordWithAttachHighlight()!=enableRecordWithAttachHighlight.isChecked())
    mytetraConfig.setEnableRecordWithAttachHighlight(enableRecordWithAttachHighlight.isChecked());

  // Сохраняется цвет подсветки
  if(mytetraConfig.getRecordWithAttachHighlightColor()!=highlightColor.name())
    mytetraConfig.setRecordWithAttachHighlightColor(highlightColor.name());

  return result;
}
