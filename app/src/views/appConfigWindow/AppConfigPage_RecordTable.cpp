#include <QWidget>
#include <QBoxLayout>
#include <QLabel>
#include <QStringList>
#include <QMessageBox>

#include "main.h"
#include "AppConfigPage_RecordTable.h"
#include "models/appConfig/AppConfig.h"
#include "libraries/FixedParameters.h"
#include "models/recordTable/RecordTableModel.h"
#include "controllers/recordTable/RecordTableController.h"

extern AppConfig mytetraConfig;
extern FixedParameters fixedParameters;


AppConfigPage_RecordTable::AppConfigPage_RecordTable(QWidget *parent) : ConfigPage(parent)
{
  qDebug() << "Create record table config page";

  QStringList allFieldNames=fixedParameters.recordFieldAvailableList;
  QMap<QString, QString> descriptionFields=fixedParameters.recordFieldDescription( allFieldNames );
  QStringList showFields=mytetraConfig.getRecordTableShowFields();

  // Создаются чекбоксы для каждого поля, хранимого в записи
  for(int i=0; i<allFieldNames.size(); i++)
  {
    QString name=allFieldNames[i];
    fields[ name ]=new QCheckBox(this);

    fields[ name ]->setText( descriptionFields.value(name) ); // Ранее было tr(descriptionFields.value(name).toLocal8Bit().data()), разобраться зачем

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
  central_layout->addStretch();

  // Устанавливается основной слой
  setLayout(central_layout);

  setupSignals();
}


void AppConfigPage_RecordTable::setupSignals(void)
{
  QMapIterator<QString, QCheckBox *> i(fields);
  while (i.hasNext())
  {
    i.next();
    connect(i.value(), SIGNAL(toggled(bool)),
            this, SLOT(onFieldToggle(bool)));
  }


  // Указатель на контроллер таблицы конечных записей
  RecordTableController *recordTableController=find_object<RecordTableController>("recordTableController");

  // При изменении настроек отображения таблицы конечных записей должен вызываться соответствующий слот контроллера (чтобы перечиталась ширина столбцов)
  connect(this, SIGNAL(recordTableConfigChange()),
          recordTableController, SLOT(onRecordTableConfigChange()));
}


// Слот, срабатывающий каждый раз когда изменяется чекбокс любого поля
void AppConfigPage_RecordTable::onFieldToggle(bool flag)
{
 Q_UNUSED(flag);

 int count=0;

  QMapIterator<QString, QCheckBox *> i(fields);
  while (i.hasNext())
  {
    i.next();

    if(i.value()->isChecked())
      count++;
  }


  // Если пользователь снял флажки со всех полей
  if(count==0)
  {
    QMessageBox msgBox;
    msgBox.setText("Can't check off all fields.");
    msgBox.exec();

    fields["name"]->setCheckState( Qt::Checked );
  }

}


// Метод должен возвращать уровень сложности сделанных изменений
// 0 - изменения не требуют перезапуска программы
// 1 - изменения требуют перезапуска программы
int AppConfigPage_RecordTable::applyChanges(void)
{
 qDebug() << "Apply changes record table";

 // Запоминается ширина полей
 // Это надо сделать в первую очередь, потому что в дальнейшем после перечитывания модели и
 // установки заголовков таблицы конечных записей слетают ширины полей (устанавливаюся в 100 px самим Qt)
 QStringList showFields=mytetraConfig.getRecordTableShowFields();
 QStringList fieldsWidth=mytetraConfig.getRecordTableFieldsWidth();
 qDebug() << "showFields" << showFields;
 qDebug() << "fieldsWidth" << fieldsWidth;


 // Запоминание в конфигурацию отображения горизонтальных заголовков
 if(mytetraConfig.getRecordTableShowHorizontalHeaders()!=showHorizontalHeader->isChecked())
   mytetraConfig.setRecordTableShowHorizontalHeaders(showHorizontalHeader->isChecked());

 // Запоминание в конфигурацию отображения нумерации строк
 if(mytetraConfig.getRecordTableShowVerticalHeaders()!=showVerticalHeader->isChecked())
   mytetraConfig.setRecordTableShowVerticalHeaders(showVerticalHeader->isChecked());


 QStringList addFieldsList; // Список полей, которые добавились в результате настройки
 QStringList removeFieldsList; // Список полей, которые должны удалиться в результате настройки

 // Определение, какие поля нужно добавить, какие удалить
 QMapIterator<QString, QCheckBox *> i(fields);
 while (i.hasNext())
 {
   i.next();

   // Если поле добавилось
   if( !showFields.contains(i.key()) && i.value()->isChecked())
     addFieldsList << i.key();

   // Если поле удалилось
   if( showFields.contains(i.key()) && !i.value()->isChecked() )
     removeFieldsList << i.key();
 }

 qDebug() << "addFieldsList" << addFieldsList;
 qDebug() << "removeFieldsList" << removeFieldsList;

 // Результирующий список полей
 QStringList newShowFields;

 // Добавление полей в результирующий список
 newShowFields=showFields+addFieldsList;

 // Удаление полей в результирующем списке
 foreach(QString removeFieldName, removeFieldsList)
   newShowFields.removeAll(removeFieldName);

 qDebug() << "newShowFields" << newShowFields;

 // Установка имен полей в конфигурацию
 mytetraConfig.setRecordTableShowFields(newShowFields);


 // Если полей становится больше чем было
 if( newShowFields.size() > showFields.size() )
 {
   qDebug() << "Add fields width process...";

   // Ширина всех полей
   float fullWidth=0.0;
   foreach(QString currentWidth, fieldsWidth)
     fullWidth+=currentWidth.toFloat();

   qDebug() << "fullWidth" << fullWidth;

   // Уменьшается ширина существующих полей
   QStringList newFieldsWidth;
   float insertFieldWidth=100.0;
   float insertFullWidth=insertFieldWidth * ( newShowFields.size() - showFields.size() );
   float coefficient=(fullWidth-insertFullWidth)/fullWidth;
   foreach(QString currentWidth, fieldsWidth)
     newFieldsWidth << QString::number( (int)(currentWidth.toFloat()*coefficient) );

   qDebug() << "insertFullWidth" << insertFullWidth;
   qDebug() << "coefficient" << coefficient;
   qDebug() << "newFieldsWidth" << newFieldsWidth;

   // Добавляются ширины добавленных полей
   for(int n=0; n<(newShowFields.size() - showFields.size()); n++)
     newFieldsWidth << QString::number( (int)insertFieldWidth );

   qDebug() << "newFieldsWidth" << newFieldsWidth;

   // Новые ширины полей запомниаются в конфигурацию
   mytetraConfig.setRecordTableFieldsWidth(newFieldsWidth);
 }


 // Если полей становится меньше чем было
 if( newShowFields.size() < showFields.size() )
 {
   qDebug() << "Remove fields width process...";

   QStringList newFieldsWidth=fieldsWidth;

   qDebug() << "newFieldsWidth" << newFieldsWidth;

   for(int n=0; n<(newShowFields.size() - showFields.size()); n++)
     newFieldsWidth.removeLast();

   qDebug() << "newFieldsWidth in result" << newFieldsWidth;

   // Установка новых ширин полей в конфигурацию
   mytetraConfig.setRecordTableFieldsWidth(newFieldsWidth);
 }


 // Если полей столько же сколько и было
 if( newShowFields.size() == showFields.size() )
 {
   qDebug() << "Count of field not changed. Set previous fields width" << fieldsWidth;

   // Установка запомненных ширин полей в конфигурацию
   // Так как это значение в конфигурации было искажено в момент переподключения модели
   mytetraConfig.setRecordTableFieldsWidth(fieldsWidth);
 }

 emit recordTableConfigChange();

 return 0;
}
