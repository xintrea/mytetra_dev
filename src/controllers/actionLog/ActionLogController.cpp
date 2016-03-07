#include <QObject>
#include <QHeaderView>
#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopServices>
#include <QDialogButtonBox>

#include "main.h"
#include "ActionLogController.h"
#include "views/actionLog/ActionLogView.h"
#include "models/appConfig/AppConfig.h"
#include "libraries/GlobalParameters.h"
#include "libraries/crypt/CryptService.h"
#include "libraries/DiskHelper.h"
#include "views/record/MetaEditor.h"
#include "views/tree/TreeScreen.h"
#include "views/dialog/ReduceMessageBox.h"


extern GlobalParameters globalParameters;
extern AppConfig mytetraConfig;


ActionLogController::ActionLogController(QObject *parent) : QObject(parent)
{
  // Создается область со списком записей лога
  view=new ActionLogView( qobject_cast<QWidget *>(parent) ); // Вид размещается внутри виджета Screen
  view->setObjectName("ActionLogView");
  view->setController(this);

  // Создание модели данных
  model=new ActionLogModel(this);
  model->setObjectName("ActionLogModel");

  // Модель данных задается для вида
  view->setModel(model);
}


ActionLogController::~ActionLogController()
{
  delete view;
  delete model;
}


ActionLogView *ActionLogController::getView(void)
{
  return view;
}


void ActionLogController::onCopyClicked()
{
  // Перечень индексов ячеек, которые были выбраны
  QModelIndexList indexes=view->selectionModel()->selectedIndexes();

  if(indexes.size() < 1)
    return;

  // Индексы выбранных ячеек идут не так как на экране, поэтому их нужно отсортировать
  qSort(indexes);

  // Размещаемый в буфере обмена текст
  QString selectedText;

  for (int i=0; i<indexes.size(); ++i)
  {
    QVariant data = model->data(indexes.at(i));
    QString text = data.toString();

    // Добавляется текст из ячейки
    selectedText.append(text);

    // Если не последняя ячейка
    if(i<indexes.size()-1)
    {
      if(indexes.at(i).row() == indexes.at(i+1).row()) // Если текущая ячейка на той же строке что и последующая ячейка
        selectedText.append('\t');
      else // Иначе последующая ячейка на другой строке
        selectedText.append('\n');
    }
  }

  QApplication::clipboard()->setText(selectedText);
}

