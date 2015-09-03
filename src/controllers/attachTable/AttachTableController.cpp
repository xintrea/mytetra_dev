#include <QObject>
#include <QHeaderView>
#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopServices>
#include <QDialogButtonBox>

#include "main.h"
#include "AttachTableController.h"
#include "views/attachTable/AttachTableView.h"
#include "models/attachTable/AttachTableModel.h"
#include "models/attachTable/AttachTableData.h"
#include "models/appConfig/AppConfig.h"
#include "libraries/GlobalParameters.h"
#include "views/record/MetaEditor.h"
#include "views/tree/TreeScreen.h"
#include "views/dialog/ReduceMessageBox.h"


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
  addSmart(Attach::typeFile);
}


void AttachTableController::onAddLink()
{
  addSmart(Attach::typeLink);
}


void AttachTableController::addSmart(int attachType)
{
  QStringList files=selectFilesForAdding(attachType);

  if(files.size()==0)
    return; // Если ни один файл не выбран

  // Указатель на данные таблицы приаттаченных файлов
  AttachTableData *attachTableData=getAttachTableData();

  if(attachTableData==NULL)
    critical_error("Unset attach table data in AttachTableController::addSmart()");

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

    // Идентификатор аттача
    QString id=get_unical_id();

    // Конструируется Attach, который нужно добавить
    Attach attach(attachType, attachTableData);
    attach.setId( id );
    attach.setFileName(currShortFileName);

    bool result=false;
    if(attachType==Attach::typeFile)
      result=attach.copyFileToBase(currFullFileName); // Файл аттача копируется в базу
    else if(attachType==Attach::typeLink)
    {
      attach.setLink(currFullFileName); // Запоминается куда указывает линк
      result=true;
    }
    else
      critical_error("Unsupport adding mode");

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

  // Обновление иконки аттачей в редакторе
  if(attachTableData->size()>0)
  {
    MetaEditor *edView=find_object<MetaEditor>("editorScreen");
    edView->toAttach->setIcon( edView->iconAttachExists );
  }
}


QStringList AttachTableController::selectFilesForAdding(int attachType)
{
  // Диалог выбора файлов
  QFileDialog fileSelectDialog;

  QString title;
  if(attachType==Attach::typeFile)
    title=tr("Attach file");
  if(attachType==Attach::typeLink)
    title=tr("Add link to file");

  fileSelectDialog.setWindowTitle(title);

  fileSelectDialog.setFileMode(QFileDialog::ExistingFiles); // QFileDialog::ExistingFile
  fileSelectDialog.setNameFilter("*");
  fileSelectDialog.setDirectory(QDir::homePath());

  // Если существует каталог, открытый при предыдущем выборе файла
  QDir appendDir( mytetraConfig.getAttachAppendDir() );
  if( appendDir.exists() )
    fileSelectDialog.setDirectory( appendDir );

  // Отрисовка диалога выбора
  fileSelectDialog.exec();

  // Выясняется список выбранных файлов
  QStringList files=fileSelectDialog.selectedFiles();

  // Запоминается директория, в которой был сделан выбор
  if(files.size()>0)
    mytetraConfig.setAttachAppendDir( fileSelectDialog.directory().absolutePath() );

  return files;
}


// Выбрано действие "Сохранить как..."
void AttachTableController::onSaveAsAttach(void)
{
  QList<QString> selectedId=getSelectedId();

  // Если ни один аттач не выбран
  if(selectedId.size()==0)
  {
    QMessageBox msgBox;
    msgBox.setText(tr("Please select any attach(es) for save to your directory."));
    msgBox.exec();

    return;
  }

  // Если выбран только один аттач
  if(selectedId.size()==1)
  {
    // Диалог выбора имени файла
    QFileDialog fileSelectDialog;
    fileSelectDialog.setFileMode(QFileDialog::ExistingFile); // Один файл
    fileSelectDialog.setWindowTitle(tr("Save as..."));
    fileSelectDialog.setDirectory(QDir::homePath());
    fileSelectDialog.setAcceptMode(QFileDialog::AcceptSave); // Чтобы была кнока "Сохранить" а не "Выбрать"
    fileSelectDialog.setFileMode(QFileDialog::AnyFile); // Чтобы кнопка "Сохранить" была активной

    // Если существует каталог, открытый при предыдущем сохранении
    QDir saveAsDir( mytetraConfig.getAttachSaveAsDir() );
    if( saveAsDir.exists() )
      fileSelectDialog.setDirectory( saveAsDir );

    // В диалоге устанавливается имя файла выбранного аттача
    QString id=selectedId.at(0);
    AttachTableData *attachTableData=getAttachTableData();
    QString fileName=attachTableData->getFileNameById(id);
    QString fullFileName=attachTableData->getAbsoluteInnerFileNameById(id);
    fileSelectDialog.selectFile(fileName);

    // Отрисовка диалога выбора
    int dialogResult=fileSelectDialog.exec();
    if(dialogResult==QDialog::Rejected)
      return;


    // Запоминается директория, в которой был сделан выбор
    mytetraConfig.setAttachSaveAsDir( fileSelectDialog.directory().absolutePath() );


    // Выясняется список выбранных файлов
    QStringList selectFiles=fileSelectDialog.selectedFiles();

    // Должен быть выбран только один файл
    if(selectFiles.size()!=1)
    {
      QMessageBox msgBox;
      msgBox.setText(tr("For save sigle file you must set single result file name."));
      msgBox.exec();

      return;
    }


    // Проверка наличия исходного файла (ведь по каким-то причинам его может не быть, например после какого-нибудь сбоя)
    QFile file(fullFileName);

    if(file.exists()==false)
    {
      QMessageBox msgBox;
      msgBox.setText(QObject::tr("Can't save file %1. File %2 not exists in database.").arg(fileName).arg(fullFileName));
      msgBox.exec();

      return;
    }


    // Указанный пользователем файл для сохранения аттача
    QString targetFileName=selectFiles.at(0);

    // Сохранение
    bool result=file.copy( targetFileName );

    if(!result)
    {
      QMessageBox msgBox;
      msgBox.setText(QObject::tr("Can't save file %1 to %2. Any i/o problem.").arg(fileName).arg(targetFileName));
      msgBox.exec();

      return;
    }
  }
  else if(selectedId.size()>1) // Если выбрано несколько аттачей
  {
    // Диалог выбора имени директории
    QFileDialog fileSelectDialog;
    fileSelectDialog.setFileMode(QFileDialog::DirectoryOnly); // Выбор директории
    fileSelectDialog.setWindowTitle(tr("Save attaches to directory..."));
    fileSelectDialog.setDirectory(QDir::homePath());

    // Если существует каталог, открытый при предыдущем сохранении
    QDir saveAsDir( mytetraConfig.getAttachSaveAsDir() );
    if( saveAsDir.exists() )
      fileSelectDialog.setDirectory( saveAsDir );

    // Отрисовка диалога выбора
    int dialogResult=fileSelectDialog.exec();
    if(dialogResult==QDialog::Rejected)
      return;


    // Запоминается директория, в которой был сделан выбор
    mytetraConfig.setAttachSaveAsDir( fileSelectDialog.directory().absolutePath() );


    // Выбранная директория
    QString toDir=fileSelectDialog.directory().absolutePath();

    AttachTableData *attachTableData=getAttachTableData();

    // Перебор выбранных для сохранения аттачей
    foreach(QString id, selectedId)
    {
      QString fileName=attachTableData->getFileNameById(id);
      QString fromFileName=attachTableData->getInnerFileNameById(id);
      QString fromFullFileName=attachTableData->getAbsoluteInnerFileNameById(id);
      QString toFullFileName=toDir+"/"+fileName;

      QFile file(fromFullFileName);

      if(file.exists()==false)
      {
        QMessageBox msgBox;
        msgBox.setText(QObject::tr("Can't save file %1. File %2 not exists in database.").arg(fileName).arg(fromFullFileName));
        msgBox.exec();

        return;
      }

      bool result=file.copy(toFullFileName);

      if(!result)
      {
        QMessageBox msgBox;
        msgBox.setText(QObject::tr("Can't save file %1 to directory %2. Any i/o problem.").arg(fileName).arg(toFullFileName));
        msgBox.exec();

        return;
      }
    }
  }
}


void AttachTableController::onEditFileName(void)
{
  QList<QString> selectedId=getSelectedId();

  // Если ни один аттач не выбран
  if(selectedId.size()==0)
    return;

  // Если выбрано больше одного аттача
  if(selectedId.size()>1)
  {
    QMessageBox msgBox;
    msgBox.setText(tr("Please select single attach for edit."));
    msgBox.exec();

    return;
  }

  // Имя файла выбранного аттача
  QString id=selectedId.at(0);
  AttachTableData *attachTableData=getAttachTableData();
  QString fileName=attachTableData->getFileNameById(id);

  // Запрос нового имени файла
  bool isOk;
  QString newFileName = QInputDialog::getText(view,
                                              tr("File name editing"),
                                              tr("File name:"),
                                              QLineEdit::Normal,
                                              fileName,
                                              &isOk);

  if(!isOk)
    return; // Была нажата кнопка Cancel

  if(newFileName.size()==0)
  {
    QMessageBox msgBox;
    msgBox.setText(tr("Cant save file with empty name."));
    msgBox.exec();

    return;
  }

  // Данные изменяются
  Attach tempAttach=attachTableData->getAttach(id);
  tempAttach.setFileName(newFileName);
  attachTableData->modifyAttach(id, tempAttach);

  // Сохранение дерева веток
  find_object<TreeScreen>("treeScreen")->saveKnowTree();
}


void AttachTableController::onDeleteAttach(void)
{
  QList<QString> selectedId=getSelectedId();

  // Если ни один аттач не выбран
  if(selectedId.size()==0)
  {
    QMessageBox msgBox;
    msgBox.setText(tr("Please select any attach(es) for delete."));
    msgBox.exec();

    return;
  }

  // Запрос подтверждения об удалении
  QMessageBox msgBox;
  msgBox.setText(tr("Do you want to delete attach file(s)?"));
  msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::Cancel);
  int ret = msgBox.exec();

  if(ret!=QMessageBox::Ok)
    return;


  // Удаление выбранных аттачей
  AttachTableData *attachTableData=getAttachTableData();

  foreach( QString id, selectedId )
    attachTableData->deleteAttach(id);


  // Сохранение дерева веток
  find_object<TreeScreen>("treeScreen")->saveKnowTree();

  // Обновление иконки аттачей в редакторе
  if(attachTableData->size()==0)
  {
    MetaEditor *edView=find_object<MetaEditor>("editorScreen");
    edView->toAttach->setIcon( edView->iconAttachNotExists );
  }
}


void AttachTableController::onOpenAttach(void)
{
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
  QList<QString> selectedId=getSelectedId();

  // Если ни один аттач не выбран
  if(selectedId.size()==0)
    return;

  // Если выбрано больше одного аттача
  if(selectedId.size()>1)
  {
    QMessageBox msgBox;
    msgBox.setText(tr("Please select single attach for see info."));
    msgBox.exec();

    return;
  }

  QString id=selectedId.at(0);
  AttachTableData *attachTableData=getAttachTableData();

  ReduceMessageBox messageBox;
  messageBox.setText("Attach file info");
  messageBox.setDetailedText(QString("<pre><p style='font-family:monospace'>")+\
                             "<b>Attach:</b> "+attachTableData->getFileNameById(id)+"\n"+\
                             "<b>Attach ID:</b> "+id+"\n"+\
                             "<b>Attach type:</b> "+attachTableData->getAttach(id).getTypeAsName()+"\n"+\
                             "<b>File size:</b> "+QString::number( attachTableData->getAttach(id).getFileSize() )+" bytes\n"+\
                             "<b>Full path:</b> "+attachTableData->getAbsoluteInnerFileNameById(id)+\
                             "</p></pre>"
                             );
  messageBox.setDetailedTextReadOnly(true);
  messageBox.setStandardButtons(QDialogButtonBox::Ok); // Для двух кнопок можно так: QDialogButtonBox::Ok | QDialogButtonBox::Cancel

  // int result=messageBox.exec();
  // qDebug() << "Result code: " << result;
  messageBox.exec();
}


void AttachTableController::onSwitchToEditor(void)
{
  MetaEditor *edView=find_object<MetaEditor>("editorScreen");
  edView->switchToEditorLayout();
}


// Получение списка идентификаторов аттачей, выделенных в представлении
QList<QString> AttachTableController::getSelectedId(void)
{
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
