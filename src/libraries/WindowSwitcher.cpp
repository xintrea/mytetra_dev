#include "WindowSwitcher.h"

#include "main.h"
#include "models/appConfig/AppConfig.h"
#include "libraries/GlobalParameters.h"
#include "views/tree/TreeScreen.h"
#include "views/record/MetaEditor.h"
#include "views/recordTable/RecordTableScreen.h"
#include "views/findInBaseScreen/FindScreen.h"

extern AppConfig mytetraConfig;
extern GlobalParameters globalParameters;


WindowSwitcher::WindowSwitcher(QObject *parent) : QObject(parent)
{
 if(mytetraConfig.getInterfaceMode()=="mobile")
   enableSwitcher=true;
 else
 {
   enableSwitcher=false;
   return;
 }

 // Редактор является встраиваемым, поэтому работа кнопки Back у него идет через callback функцию
 // MetaEditor *edView=find_object<MetaEditor>("editorScreen"); // Выясняется указатель на объект редактирования текста записи
 MetaEditor *metaEditor=globalParameters.getMetaEditor();
 metaEditor->set_back_callback( this->switchFromRecordToRecordtable ); // Устанавливается функция обратного вызова при клике на кнопку Back
}


void WindowSwitcher::switchFromTreeToRecordtable(void)
{
 if(!enableSwitcher)
   return;

 // Скрываются все прочие области
 globalParameters.getTreeScreen()->hide();
 globalParameters.getMetaEditor()->hide();
 globalParameters.getFindScreen()->hide();

 globalParameters.getRecordTableScreen()->show();
}


void WindowSwitcher::switchFromTreeToFindInBase(void)
{
 if(!enableSwitcher)
   return;

}


// Статическая функция, используется редактором как callback функция при нажатии кнопки back в редакторе конечной записи
void WindowSwitcher::switchFromRecordToRecordtable(void)
{
 if(mytetraConfig.getInterfaceMode()!="mobile") // В статическом методе использовать нестатическую переменну enableSwitcher нельзя

 // Скрываются все прочие области
 globalParameters.getTreeScreen()->hide();
 globalParameters.getMetaEditor()->hide();
 globalParameters.getFindScreen()->hide();

 globalParameters.getRecordTableScreen()->show();
}


void WindowSwitcher::switchFromRecordToFindInBase(void)
{
 if(!enableSwitcher)
   return;

}


void WindowSwitcher::switchFromRecordtableToRecord(void)
{
 if(!enableSwitcher)
   return;

 // Скрываются все прочие области
 globalParameters.getTreeScreen()->hide();
 globalParameters.getRecordTableScreen()->hide();
 globalParameters.getFindScreen()->hide();

 globalParameters.getMetaEditor()->show();
}


void WindowSwitcher::switchFromRecordtableToFindInBase(void)
{
 if(!enableSwitcher)
   return;

}


void WindowSwitcher::switchFromRecordtableToTree(void)
{
 if(!enableSwitcher)
   return;

 // Скрываются все прочие области
 globalParameters.getMetaEditor()->hide();
 globalParameters.getRecordTableScreen()->hide();
 globalParameters.getFindScreen()->hide();

 globalParameters.getTreeScreen()->show();
}


// Закрытие окна FindInBase с переходом на окно, откуда оно было открыто
void WindowSwitcher::closeFindInBase(void)
{
 if(!enableSwitcher)
   return;

}


void WindowSwitcher::switchDirectTo(QString widgetName)
{
  // Скрываются все прочие области
  globalParameters.getTreeScreen()->hide();
  globalParameters.getRecordTableScreen()->hide();
  globalParameters.getMetaEditor()->hide();
  globalParameters.getFindScreen()->hide();

  // Оформить через case
  if(widgetName=="knowTreeView")
  {
    globalParameters.getTreeScreen()->show();
    globalParameters.getTreeScreen()->setFocus();
    return;
  }

  if(widgetName=="recordTableView")
  {
    globalParameters.getRecordTableScreen()->show();
    globalParameters.getRecordTableScreen()->setFocus();
    return;
  }

  if(widgetName=="textArea")
  {
    globalParameters.getMetaEditor()->show();
    globalParameters.getMetaEditor()->setFocus();
    return;
  }

  if(widgetName=="findTableView")
  {
    globalParameters.getFindScreen()->show();
    globalParameters.getFindScreen()->setFocus();
    return;
  }


  // Остальные случаи - активируется дерево записей
  globalParameters.getTreeScreen()->show();
  globalParameters.getTreeScreen()->setFocus();
}
