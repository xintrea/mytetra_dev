#include <QObject>
#include <QHeaderView>
#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopServices>
#include <QDialogButtonBox>
#include <QApplication>
#include <QClipboard>
#include <algorithm>

#include "ActionLogController.h"
#include "views/actionLog/ActionLogView.h"
#include "models/appConfig/AppConfig.h"
#include "libraries/GlobalParameters.h"
#include "models/actionLog/ActionLogModel.h"


extern GlobalParameters globalParameters;
extern AppConfig mytetraConfig;


ActionLogController::ActionLogController(QObject *parent) : QObject(parent)
{
  // Создается область со списком записей лога
  view=new ActionLogView( qobject_cast<QWidget *>(parent) ); // Вид размещается внутри виджета Screen
  view->setObjectName("actionLogView");
  view->setController(this);

  // Создание модели данных
  model=new ActionLogModel(this);
  model->setObjectName("actionLogModel");

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
  std::sort( indexes.begin(), indexes.end() );

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

  // Выделенный текст лога вставляется в системный буфер обмена
  QApplication::clipboard()->setText(selectedText);
}

