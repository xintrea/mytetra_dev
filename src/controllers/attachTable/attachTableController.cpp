#include <QObject>
#include <QHeaderView>

#include "main.h"
#include "attachTableController.h"
#include "views/attachTable/AttachTableView.h"
#include "models/appConfig/AppConfig.h"
#include "libraries/GlobalParameters.h"


extern GlobalParameters globalParameters;
extern AppConfig mytetraConfig;


AttachTableController::AttachTableController(QObject *parent) : QObject(parent)
{
  // Создается область со списком файлов
  view=new AttachTableView( qobject_cast<QWidget *>(parent) ); // Вид размещается внутри виджета Screen
  view->setObjectName("attachTableView");
  view->setController(this);

  // Создание модели данных (тонкой обертки над AttachTableData)
  attachTableModel=new AttachTableModel(this);
  attachTableModel->setObjectName("attachTableModel");

  // Модель данных задается для вида
  view->setModel(attachTableModel);
}


AttachTableController::~AttachTableController()
{

}


