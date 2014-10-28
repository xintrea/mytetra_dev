#include <QWidget>
#include <QBoxLayout>
#include <QLabel>
#include <QStringList>

#include "AppConfigPage_Synchro.h"
#include "models/appConfig/AppConfig.h"

extern AppConfig mytetraConfig;
extern FixedParameters fixedParameters;


AppConfigPage_RecordTable::AppConfigPage_RecordTable(QWidget *parent) : ConfigPage(parent)
{
  qDebug() << "Create synchro config page";

  QStringList allFieldNames=fixedParameters.recordFieldAvailableList();
  QMap<QString, QString> descriptionFields=fixedParameters.recordFieldDescription( allFieldNames );
  QStringList showFields=mytetraConfig.recordFieldAvailableList();

  for(int i=0; i<allFieldNames.size(); i++)
  {
    QString name=allFieldNames[i];
    showFields[ name ]=new QCheckBox(this);

    showFields[ name ]->setText( tr(descriptionFields[name]) );

    if( showFields.contains(name) )
      showFields[ name ]->setCheckState( Qt::Checked );
  }

  // Собирается основной слой
  QVBoxLayout *central_layout=new QVBoxLayout();

  showHorizontalHeader=new QCheckBox(this);
  showHorizontalHeader.setText( tr("Show horisontal header") );

  showVerticalHeader=new QCheckBox(this);
  showVerticalHeader.setText( tr("Show row number") );











}


// Метод должен возвращать уровень сложности сделанных изменений
// 0 - изменения не требуют перезапуска программы
// 1 - изменения требуют перезапуска программы
int AppConfigPage_Synchro::apply_changes(void)
{
 qDebug() << "Apply changes record table";



 return 0;
}
