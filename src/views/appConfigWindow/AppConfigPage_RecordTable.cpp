#include "main.h"
#include "AppConfigPage_RecordTable.h"
#include "ui_AppConfigPage_RecordTable.h"
#include "models/appConfig/AppConfig.h"
#include "libraries/FixedParameters.h"
#include "models/recordTable/RecordTableModel.h"
#include "controllers/recordTable/RecordTableController.h"

#include <QMessageBox>

extern AppConfig mytetraConfig;
extern FixedParameters fixedParameters;


AppConfigPage_RecordTable::AppConfigPage_RecordTable(QWidget *parent)
    : ConfigPage(parent)
    , ui(new Ui::AppConfigPage_RecordTable)
{
  qDebug() << "Create record table config page";
  ui->setupUi(this);

  QStringList allFieldNames = fixedParameters.recordFieldAvailableList;
  QMap<QString, QString> descriptionFields = fixedParameters.recordFieldDescription( allFieldNames );
  QStringList showFields = mytetraConfig.getRecordTableShowFields();

  // Создаётся QListWidgetItem для каждого поля, хранимого в записи
  for (const auto& name : allFieldNames) {
    fields[name] = new QListWidgetItem(descriptionFields.value(name), ui->columnsListWidget);
    fields[name]->setFlags(fields[name]->flags() | Qt::ItemIsUserCheckable);
    fields[name]->setCheckState(showFields.contains(name) ? Qt::Checked : Qt::Unchecked);
  }

  ui->showHorizontalHeader->setChecked( mytetraConfig.getRecordTableShowHorizontalHeaders() );
  ui->showVerticalHeader->setChecked( mytetraConfig.getRecordTableShowVerticalHeaders() );

  setupSignals();
}


void AppConfigPage_RecordTable::setupSignals(void)
{
  connect(ui->columnsListWidget, SIGNAL(itemChanged(QListWidgetItem *)), SLOT(onFieldToggle(QListWidgetItem *)));

  // Указатель на контроллер таблицы конечных записей
  RecordTableController *recordTableController = find_object<RecordTableController>("recordTableController");

  // При изменении настроек отображения таблицы конечных записей должен вызываться соответствующий слот контроллера (чтобы перечиталась ширина столбцов)
  connect(this, SIGNAL(recordTableConfigChange()), recordTableController, SLOT(onRecordTableConfigChange()));
}


// Слот, срабатывающий каждый раз когда изменяется чекбокс любого поля
void AppConfigPage_RecordTable::onFieldToggle(QListWidgetItem* item)
{
  bool cantCheckOff = true;

  for (const auto& val : fields.values()) {
     if (val->checkState() == Qt::Checked) {
       cantCheckOff = false;
       break;
     }
  }

  // Если пользователь снял флажки со всех полей
  if(cantCheckOff)
  {
    item->setCheckState(Qt::Checked);
    QMessageBox::warning(this, tr("Warning"), tr("Can't check off all fields."));
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
 QStringList showFields = mytetraConfig.getRecordTableShowFields();
 QStringList fieldsWidth = mytetraConfig.getRecordTableFieldsWidth();
 qDebug() << "showFields" << showFields;
 qDebug() << "fieldsWidth" << fieldsWidth;

 // Запоминание в конфигурацию отображения горизонтальных заголовков
 if(mytetraConfig.getRecordTableShowHorizontalHeaders() != ui->showHorizontalHeader->isChecked())
   mytetraConfig.setRecordTableShowHorizontalHeaders(ui->showHorizontalHeader->isChecked());

 // Запоминание в конфигурацию отображения нумерации строк
 if(mytetraConfig.getRecordTableShowVerticalHeaders() != ui->showVerticalHeader->isChecked())
   mytetraConfig.setRecordTableShowVerticalHeaders(ui->showVerticalHeader->isChecked());

 QStringList addFieldsList; // Список полей, которые добавились в результате настройки
 QStringList removeFieldsList; // Список полей, которые должны удалиться в результате настройки

 // Определение, какие поля нужно добавить, какие удалить
 for (const auto& it : fields.keys()) {
    // Если поле добавилось
    if( !showFields.contains(it) && fields.value(it)->checkState() == Qt::Checked)
      addFieldsList << it;

    // Если поле удалилось
    if( showFields.contains(it) && fields.value(it)->checkState() == Qt::Unchecked )
         removeFieldsList << it;
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
     newFieldsWidth << QString::number( qRound(currentWidth.toFloat() * coefficient) );

   qDebug() << "insertFullWidth" << insertFullWidth;
   qDebug() << "coefficient" << coefficient;
   qDebug() << "newFieldsWidth" << newFieldsWidth;

   // Добавляются ширины добавленных полей
   for(int n=0; n<(newShowFields.size() - showFields.size()); n++)
     newFieldsWidth << QString::number( qRound(insertFieldWidth) );

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
