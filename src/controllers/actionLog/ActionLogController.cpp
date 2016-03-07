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
  QString selectedText;

  // Перечень индексов ячеек, которые были выбраны
  QModelIndexList indexes=view->selectionModel()->selectedIndexes();

  // Индекс, помогающий найти переход на новую строку
  QModelIndex previous = indexes.first();
  indexes.removeFirst();

  foreach(QModelIndex current, indexes)
  {
    QVariant data = model->data(current);
    QString text = data.toString();

    // Добавляется текст из ячейки
    selectedText.append(text);

    // Если текущая ячейка на той же строке что и предыдущая
    if(current.row() == previous.row())
    {
      selectedText.append('\t');
    }
    else // Иначе текущая ячейка на другой строке, что и предыдущая, а значит нужно добавить перевод строки
    {
      selectedText.append('\n');
    }

    previous = current;
  }

  QApplication::clipboard()->setText(selectedText);
}

