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
 enableSwitch();

 // Редактор является встраиваемым, поэтому работа кнопки Back у него идет через callback функцию
 // MetaEditor *edView=find_object<MetaEditor>("editorScreen"); // Выясняется указатель на объект редактирования текста записи
 MetaEditor *metaEditor=globalParameters.getMetaEditor();
 metaEditor->setBackCallback( this->switchFromRecordToRecordtable ); // Устанавливается функция обратного вызова при клике на кнопку Back
}


void WindowSwitcher::enableSwitch(void)
{
 // Для десктопа переключение окон виджетов вообще не должно включаться
 if(mytetraConfig.getInterfaceMode()=="desktop")
 {
   enableSwitcher=false;
   return;
 }

 enableSwitcher=true;
}


void WindowSwitcher::disableSwitch(void)
{
 enableSwitcher=false;
}


bool WindowSwitcher::getSwitchStatus(void)
{
 return enableSwitcher;
}


void WindowSwitcher::switchFromTreeToRecordtable(void)
{
 if(!enableSwitcher)
   return;

 // Скрываются все прочие области
 globalParameters.getTreeScreen()->hide();
 globalParameters.getMetaEditor()->hide();
 // globalParameters.getFindScreen()->hide();

 QWidget *object=static_cast<QWidget *>( globalParameters.getRecordTableScreen() );
 object->show();
 mytetraConfig.setFocusWidget(object->objectName());
}


void WindowSwitcher::switchFromTreeToFindInBase(void)
{
 if(!enableSwitcher)
   return;

}


// Статическая функция, используется редактором как callback функция при нажатии кнопки back в редакторе конечной записи
void WindowSwitcher::switchFromRecordToRecordtable(void)
{
 if(globalParameters.getWindowSwitcher()==NULL)
   return;

 // Если переключение запрещено
 if(!globalParameters.getWindowSwitcher()->getSwitchStatus())
   return;

 if(mytetraConfig.getInterfaceMode()!="mobile") // В статическом методе использовать нестатическую переменну enableSwitcher нельзя
   return;

 // Скрываются все прочие области
 globalParameters.getTreeScreen()->hide();
 globalParameters.getMetaEditor()->hide();
 // globalParameters.getFindScreen()->hide();

 QWidget *object=static_cast<QWidget *>( globalParameters.getRecordTableScreen() );
 object->show();
 mytetraConfig.setFocusWidget(object->objectName());
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
 // globalParameters.getFindScreen()->hide();

 QWidget *object=static_cast<QWidget *>( globalParameters.getMetaEditor() );
 object->show();
 mytetraConfig.setFocusWidget(object->objectName());
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
 // globalParameters.getFindScreen()->hide();

 QWidget *object=static_cast<QWidget *>( globalParameters.getTreeScreen() );
 object->show();
 mytetraConfig.setFocusWidget(object->objectName());
}


// Закрытие окна FindInBase с переходом на окно, откуда оно было открыто
void WindowSwitcher::closeFindInBase(void)
{
 if(!enableSwitcher)
   return;

}


// Слот, срабатывающий при нажатии на кнопку перехода к поиску по базе в любом виджете
void WindowSwitcher::findInBaseClick(void)
{
  // Определяется ссылка на виджет поиска
  FindScreen *findScreen=find_object<FindScreen>("findScreenDisp");

  // Если виджет не показан, он выводится на экран, и наоборот
  if(findScreen->isVisible()==false)
    findScreen->widgetShow();
  else
   findScreen->widgetHide();
}


void WindowSwitcher::restoreFocusWidget()
{
  if(!enableSwitcher)
    return;

  // Скрываются все прочие области
  globalParameters.getTreeScreen()->hide();
  globalParameters.getRecordTableScreen()->hide();
  globalParameters.getMetaEditor()->hide();
  // globalParameters.getFindScreen()->hide();

  QString widgetName=mytetraConfig.getFocusWidget();

  // Оформить через case
  if(widgetName=="treeScreen")
  {
    globalParameters.getTreeScreen()->show();
    globalParameters.getTreeScreen()->setFocus();
    return;
  }

  if(widgetName=="recordTableScreen")
  {
    globalParameters.getRecordTableScreen()->show();
    globalParameters.getRecordTableScreen()->setFocus();
    return;
  }

  if(widgetName=="editorScreen")
  {
    globalParameters.getMetaEditor()->show();
    globalParameters.getMetaEditor()->setFocus();
    return;
  }

  // Доработать
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
