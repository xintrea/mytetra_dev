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


  // Область настройки видимости заголовков
  showHorizontalHeader=new QCheckBox(this);
  showHorizontalHeader->setText( tr("Show horisontal header") );
  if(mytetraConfig.getRecordTableShowHorizontalHeaders())
    showHorizontalHeader->setCheckState( Qt::Checked );

  showVerticalHeader=new QCheckBox(this);
  showVerticalHeader->setText( tr("Show row number") );
  if(mytetraConfig.getRecordTableShowVerticalHeaders())
    showVerticalHeader->setCheckState( Qt::Checked );

  QVBoxLayout *vboxVisibleHeaders = new QVBoxLayout;
  vboxVisibleHeaders->addWidget(showHorizontalHeader);
  vboxVisibleHeaders->addWidget(showVerticalHeader);

  QGroupBox *groupBoxVisibleHeaders = new QGroupBox(tr("Headers and numbers visible"));
  groupBoxVisibleHeaders->setLayout(vboxVisibleHeaders);


  // Область настройки видимости столбцов
  QVBoxLayout *vboxVisibleColumns = new QVBoxLayout;
  foreach(QCheckBox *currentCheckBox, fields)
    vboxVisibleColumns->addWidget(currentCheckBox);

  QGroupBox *groupBoxVisibleColumns = new QGroupBox(tr("Columns visible"));
  groupBoxVisibleColumns->setLayout(vboxVisibleColumns);


  // Собирается основной слой
  QVBoxLayout *central_layout=new QVBoxLayout();

  central_layout->addWidget(groupBoxVisibleHeaders);
  central_layout->addWidget(groupBoxVisibleColumns);

  // Устанавливается основной слой
  setLayout(central_layout);
}


// Метод должен возвращать уровень сложности сделанных изменений
// 0 - изменения не требуют перезапуска программы
// 1 - изменения требуют перезапуска программы
int AppConfigPage_RecordTable::apply_changes(void)
{
 qDebug() << "Apply changes record table";

 if(mytetraConfig.getRecordTableShowHorizontalHeaders()!=showHorizontalHeader->isChecked())
   mytetraConfig.setRecordTableShowHorizontalHeaders(showHorizontalHeader->isChecked());

 if(mytetraConfig.getRecordTableShowVerticalHeaders()!=showVerticalHeader->isChecked())
   mytetraConfig.setRecordTableShowVerticalHeaders(showVerticalHeader->isChecked());


 QStringList showFields=mytetraConfig.getRecordTableShowFields();
 QStringList fieldsWidth=mytetraConfig.getRecordTableFieldsWidth();

 QMapIterator<QString, QCheckBox *> i(fields);
 while (i.hasNext())
 {
   i.next();
   if(!showFields.contains(i.key()) && i.value()->isChecked())
   {
     showFields << i.key();
     mytetraConfig.setRecordTableShowFields(showFields);


     QStringList newFieldsWidth;
     int insertWidth=25;
     foreach(QString currentWidth, fieldsWidth)
       if(currentWidth.toInt()>insertWidth)
         newFieldsWidth << QString::number( currentWidth.toInt()-insertWidth );
       else
         newFieldsWidth << currentWidth;

     newFieldsWidth << QString::number( insertWidth );

     mytetraConfig.setRecordTableFieldsWidth(newFieldsWidth);
   }
 }


 return 0;
}
