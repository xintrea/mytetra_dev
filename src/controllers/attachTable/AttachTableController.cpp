#include <QObject>
#include <QHeaderView>
#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopServices>

#include "main.h"
#include "AttachTableController.h"
#include "views/attachTable/AttachTableView.h"
#include "models/attachTable/AttachTableModel.h"
#include "models/attachTable/AttachTableData.h"
#include "models/appConfig/AppConfig.h"
#include "libraries/GlobalParameters.h"
#include "views/record/MetaEditor.h"
#include "views/tree/TreeScreen.h"


extern GlobalParameters globalParameters;
extern AppConfig mytetraConfig;


AttachTableController::AttachTableController(QObject *parent) : QObject(parent)
{
  // Создается область со списком файлов
  view=new AttachTableView( qobject_cast<QWidget *>(parent) ); // Вид размещается внутри виджета Screen
  view->setObjectName("attachTableView");
  view->setController(this);
  view->init(); // Инициализация запускается только после установки контроллера

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


// Основной метод установки указателя на данные
void AttachTableController::setAttachTableData(AttachTableData *attachTableData)
{
  model->setData(QModelIndex(), QVariant::fromValue(attachTableData), ATTACHTABLE_ROLE_TABLE_DATA);
}


AttachTableData *AttachTableController::getAttachTableData()
{
  return (model->data(QModelIndex(), ATTACHTABLE_ROLE_TABLE_DATA)).value<AttachTableData *>();
}


void AttachTableController::onAddAttach(void)
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

  // Указатель на данные таблицы приаттаченных файлов
  AttachTableData *attachTableData=getAttachTableData();

  if(attachTableData==NULL)
    critical_error("Unset attach table data in AttachTableController::onAttachFile()");

  // Перебираются выбранные в диалоге файлы
  for(int i=0; i<files.size(); ++i)
  {
    // Текущее полное имя файла
    QString currFullFileName=files.at(i);

    QFileInfo currFileInfo(currFullFileName);

    // Если пользователь выбрал директорию (директорию выбирать нельзя, пока что можно выбирать только файлы)
    if(currFileInfo.isDir())
    {
      QMessageBox msgBox;
      msgBox.setText(tr("Can't add directory. Please, select files."));
      msgBox.exec();

      break;
    }

    qDebug() << "Select file from disk: " << currFullFileName;

    // Текущее короткое имя файла
    QString currShortFileName=currFileInfo.fileName();

    // Конструируется Attach, который нужно добавить
    QString id=get_unical_id();
    Attach attach(Attach::typeFile, attachTableData);
    attach.setId( id );
    attach.setFileName(currShortFileName);

    qDebug() << "Before copy file " << currFullFileName;

    // Файл аттача копируется в базу
    bool result=attach.copyFileToBase(currFullFileName);

    qDebug() << "After copy file";

    if(result)
    {
      // Данные аттача добавляются в таблицу приаттаченных файлов
      attachTableData->addAttach(attach);
    }
    else
    {
      QMessageBox msgBox;
      msgBox.setText(tr("An error occurred while copying file(s). File(s) can't attach."));
      msgBox.exec();

      break;
    }

  } // Закончился цикл перебора файлов

  // Сохранение дерева веток
  find_object<TreeScreen>("treeScreen")->saveKnowTree();
}


void AttachTableController::onEditFileName(void)
{

}


void AttachTableController::onDeleteAttach(void)
{
  qDebug() << "In slot AttachTableController::onDeleteFile()";

  QMessageBox msgBox;
  msgBox.setText(tr("Do you want to delete attach file(s)?"));
  msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::Cancel);
  int ret = msgBox.exec();

  if(ret!=QMessageBox::Ok)
    return;

  AttachTableData *attachTableData=getAttachTableData();

  QList<QString> selectedId=getSelectedId();

  foreach( QString id, selectedId )
    attachTableData->deleteAttach(id);

  // Сохранение дерева веток
  find_object<TreeScreen>("treeScreen")->saveKnowTree();
}


void AttachTableController::onOpenAttach(void)
{
  qDebug() << "In slot AttachTableController::onOpenAttach()";

  AttachTableData *attachTableData=getAttachTableData();

  QList<QString> selectedId=getSelectedId();

  foreach( QString id, selectedId )
  {
    QString fullFileName=attachTableData->getAbsoluteInnerFileNameById(id);
    qDebug() << "Open file: "+fullFileName;

    // QUrl urlFile;
    // urlFile.fromLocalFile(fullFileName);
    QUrl urlFile("file:"+fullFileName);

    // Открытие файла средствами операционной системы
    QDesktopServices::openUrl(urlFile);
  }
}


void AttachTableController::onShowAttachInfo(void)
{
  qDebug() << "Model row: " << model->rowCount();
}


void AttachTableController::onSwitchToEditor(void)
{
  MetaEditor *edView=find_object<MetaEditor>("editorScreen");
  edView->switchToEditorLayout();
}


// Получение списка идентификаторов аттачей, выделенных в представлении
QList<QString> AttachTableController::getSelectedId(void)
{
  qDebug() << "In slot AttachTableController::onDeleteFile()";

  QList<QString> selectedId;

  // Получение списка выделенных в таблице на экране Item-элементов
  QModelIndexList selectItems=view->selectionModel()->selectedIndexes();

  // Перебор выделенных элементов. Так как имеем несколько столбцов, то для одной строки будет несколько QModelIndex
  int previousRow=-1;
  for(int i=0; i<selectItems.size(); i++)
  {
    int row=selectItems.at(i).row();

    // Строка обратабывается только один раз (из-за того что для одной строки несколько QModelIndex)
    if(row!=previousRow)
    {
      previousRow=row;

      selectedId.append( model->data(selectItems.at(i), ATTACHTABLE_ROLE_ID).toString() );
    }
  }

  return selectedId;
}
