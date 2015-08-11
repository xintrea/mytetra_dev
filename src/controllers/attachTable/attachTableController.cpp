#include <QObject>
#include <QHeaderView>
#include <QFileDialog>

#include "main.h"
#include "AttachTableController.h"
#include "views/attachTable/AttachTableView.h"
#include "models/attachTable/AttachTableModel.h"
#include "models/appConfig/AppConfig.h"
#include "libraries/GlobalParameters.h"
#include "views/record/MetaEditor.h"


extern GlobalParameters globalParameters;
extern AppConfig mytetraConfig;


AttachTableController::AttachTableController(QObject *parent) : QObject(parent)
{
  // Создается область со списком файлов
  view=new AttachTableView( qobject_cast<QWidget *>(parent) ); // Вид размещается внутри виджета Screen
  view->setObjectName("attachTableView");
  view->setController(this);
  view->horizontalHeader()->hide(); // Вид должен быть компактным, заголовки ненужны

  // Создание модели данных (тонкой обертки над AttachTableData)
  model=new AttachTableModel(this);
  model->setObjectName("attachTableModel");

  // Модель данных задается для вида
  view->setModel(model);
}


AttachTableController::~AttachTableController()
{
  delete view;
  delete model;
}


AttachTableView *AttachTableController::getView(void)
{
 return view;
}


void AttachTableController::onAttachFile(void)
{
  // Диалог выбора файлов
  QFileDialog fileSelectDialog;
  fileSelectDialog.setFileMode(QFileDialog::ExistingFiles); // QFileDialog::ExistingFile
  fileSelectDialog.setNameFilter("*");
  fileSelectDialog.setWindowTitle(tr("Attach file"));
  fileSelectDialog.setDirectory(QDir::homePath());
  fileSelectDialog.exec();

  // Выясняется список выбранных файлов
  QStringList files=fileSelectDialog.selectedFiles();

  // Если ни один файл не выбран
  if(files.size()==0)
    return;

  // Перебираются файлы выбранных картинок
  for(int i=0; i<files.size(); ++i)
  {
    // Текущее имя файла
    QString currFileName=files.at(i);

  } // Закончился цикл перебора файлов
}


void AttachTableController::onEditFileName(void)
{

}


void AttachTableController::onDeleteFile(void)
{

}


void AttachTableController::onSwitchToEditor(void)
{
  MetaEditor *edView=find_object<MetaEditor>("editorScreen");
  edView->switchToEditorLayout();
}
