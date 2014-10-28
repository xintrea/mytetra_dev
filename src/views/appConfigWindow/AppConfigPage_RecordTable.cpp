#include <QWidget>
#include <QBoxLayout>
#include <QLabel>
#include <QStringList>

#include "AppConfigPage_RecordTable.h"
#include "models/appConfig/AppConfig.h"
#include "libraries/FixedParameters.h"

extern AppConfig mytetraConfig;
extern FixedParameters fixedParameters;


AppConfigPage_RecordTable::AppConfigPage_RecordTable(QWidget *parent) : ConfigPage(parent)
{
  qDebug() << "Create record table config page";

  QStringList allFieldNames=fixedParameters.recordFieldAvailableList();
  QMap<QString, QString> descriptionFields=fixedParameters.recordFieldDescription( allFieldNames );
  QStringList showFields=mytetraConfig.getRecordTableShowFields();

  // Создаются чекбоксы для каждого поля, хранимого в записи
  for(int i=0; i<allFieldNames.size(); i++)
  {
    QString name=allFieldNames[i];
    fields[ name ]=new QCheckBox(this);

    fields[ name ]->setText( tr(descriptionFields.value(name).toLocal8Bit().data()) );

    if( showFields.contains(name) )
      fields[ name ]->setCheckState( Qt::Checked );
  }

  // Собирается основной слой
  QVBoxLayout *central_layout=new QVBoxLayout();

  showHorizontalHeader=new QCheckBox(this);
  showHorizontalHeader->setText( tr("Show horisontal header") );

  showVerticalHeader=new QCheckBox(this);
  showVerticalHeader->setText( tr("Show row number") );

  central_layout->addWidget(showHorizontalHeader);
  central_layout->addWidget(showVerticalHeader);
  foreach(QCheckBox *currentCheckBox, fields)
    central_layout->addWidget(currentCheckBox);

  // Устанавливается основной слой
  setLayout(central_layout);
}


// Метод должен возвращать уровень сложности сделанных изменений
// 0 - изменения не требуют перезапуска программы
// 1 - изменения требуют перезапуска программы
int AppConfigPage_RecordTable::apply_changes(void)
{
 qDebug() << "Apply changes record table";



 return 0;
}
