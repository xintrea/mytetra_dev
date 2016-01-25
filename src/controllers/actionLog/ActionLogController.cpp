#include <QObject>
#include <QHeaderView>
#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopServices>
#include <QDialogButtonBox>

#include "main.h"
#include "ActionLogController.h"
#include "views/actionLog/ActionLogView.h"
#include "models/actionLog/ActionLogModel.h"
#include "models/actionLog/ActionLogData.h"
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
  // Создается область со списком файлов
  view=new ActionLogView( qobject_cast<QWidget *>(parent) ); // Вид размещается внутри виджета Screen
  view->setObjectName("ActionLogView");
  view->setController(this);
  view->init(); // Инициализация запускается только после установки контроллера

  // Создание модели данных (тонкой обертки над ActionLogData)
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


// Основной метод установки указателя на данные
void ActionLogController::setActionLogData(ActionLogData *ActionLogData)
{
  model->setData(QModelIndex(), QVariant::fromValue(ActionLogData), ActionLog_ROLE_TABLE_DATA);
}


ActionLogData *ActionLogController::getActionLogData()
{
  return (model->data(QModelIndex(), ActionLog_ROLE_TABLE_DATA)).value<ActionLogData *>();
}


void ActionLogController::onAddAttach(void)
{
  addSmart("file");
}


void ActionLogController::onAddLink()
{
  addSmart("link");
}


void ActionLogController::addSmart(QString attachType)
{
  QStringList files=selectFilesForAdding(attachType);

  if(files.size()==0)
    return; // Если ни один файл не выбран

  // Указатель на данные таблицы приаттаченных файлов
  ActionLogData *ActionLogData=getActionLogData();

  if(ActionLogData==NULL)
    criticalError("Unset attach table data in ActionLogController::addSmart()");

  // Перебираются выбранные в диалоге файлы
  for(int i=0; i<files.size(); ++i)
  {
    // Текущее полное имя файла
    QString currFullFileName=files.at(i);

    QFileInfo currFileInfo(currFullFileName);

    // Если пользователь выбрал директорию (директорию выбирать нельзя, пока что можно выбирать только файлы)
    if(currFileInfo.isDir())
    {
      showMessageBox(tr("Can't add directory. Please, select files."));
      break;
    }

    qDebug() << "Select file from disk: " << currFullFileName;

    // Текущее короткое имя файла
    QString currShortFileName=currFileInfo.fileName();

    // Идентификатор аттача
    QString id=get_unical_id();

    // Конструируется Attach, который нужно добавить
    Attach attach(attachType, ActionLogData);
    attach.setField("id", id);
    attach.setField("fileName", currShortFileName);

    bool result=false;
    if(attachType=="file")
      result=attach.copyFileToBase(currFullFileName); // Файл аттача копируется в базу
    else if(attachType=="link")
    {
      attach.setField("link", currFullFileName); // Запоминается куда указывает линк
      result=true;
    }
    else
      criticalError("Unsupport adding mode");


    // Если запись, к которой добавляется аттач, зашифрована
    if( ActionLogData->isRecordCrypt() )
      attach.encrypt();


    if(result)
    {
      // Данные аттача добавляются в таблицу приаттаченных файлов
      ActionLogData->addAttach(attach);
    }
    else
    {
      showMessageBox(tr("An error occurred while copying file(s). File(s) can't attach."));
      break;
    }

  } // Закончился цикл перебора файлов

  // Сохранение дерева веток
  find_object<TreeScreen>("treeScreen")->saveKnowTree();

  // Обновление иконки аттачей в редакторе
  MetaEditor *edView=find_object<MetaEditor>("editorScreen");
  if(ActionLogData->size()>0)
    edView->switchAttachIconExists(true);
  else
    edView->switchAttachIconExists(false);
}


QStringList ActionLogController::selectFilesForAdding(QString attachType)
{
  // Диалог выбора файлов
  QFileDialog fileSelectDialog;

  QString title;
  if(attachType=="file")
    title=tr("Attach file");
  if(attachType=="link")
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
  int dialogResult=fileSelectDialog.exec();

  // Если была нажата отмена
  if(dialogResult==QDialog::Rejected)
    return QStringList();

  // Выясняется список выбранных файлов
  QStringList files=fileSelectDialog.selectedFiles();

  // Запоминается директория, в которой был сделан выбор
  if(files.size()>0)
    mytetraConfig.setAttachAppendDir( fileSelectDialog.directory().absolutePath() );

  return files;
}


// Выбрано действие "Сохранить как..."
void ActionLogController::onSaveAsAttach(void)
{
  QList<QString> selectedId=getSelectedId();

  // Если ни один аттач не выбран
  if(selectedId.size()==0)
  {
    showMessageBox(tr("Please select any attach(es) for save to your directory."));
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
    ActionLogData *ActionLogData=getActionLogData();
    QString attachType=ActionLogData->getAttach(id).getField("type");
    QString fileName=ActionLogData->getFileNameById(id);
    QString fullFileName=ActionLogData->getAbsoluteInnerFileNameById(id);
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
      showMessageBox(tr("For save sigle file you must set single result file name."));
      return;
    }

    // Указанное пользователем имя файла для сохранения аттача, взятое из формы ввода
    QString targetFileName=selectFiles.at(0);

    // Непосредственное сохранение файла
    saveAttachToUserPlace(fullFileName, targetFileName, attachType, ActionLogData->isRecordCrypt());
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

    ActionLogData *ActionLogData=getActionLogData();

    // Перебор выбранных для сохранения аттачей
    foreach(QString id, selectedId)
    {
      QString attachType=ActionLogData->getAttach(id).getField("type");
      QString fileName=ActionLogData->getFileNameById(id);
      QString fromFileName=ActionLogData->getInnerFileNameById(id);
      QString fromFullFileName=ActionLogData->getAbsoluteInnerFileNameById(id);
      QString toFullFileName=toDir+"/"+fileName;

      // Непосредственное сохранение файла
      saveAttachToUserPlace(fromFullFileName, toFullFileName, attachType, ActionLogData->isRecordCrypt());
    }
  }
}


void ActionLogController::saveAttachToUserPlace(QString fromFullFileName, QString toFullFileName, QString attachType, bool isAttachCrypt)
{
  // Проверка наличия исходного файла (ведь по каким-то причинам его может не быть, например после какого-нибудь сбоя)
  QFile file(fromFullFileName);

  if(file.exists()==false)
  {
    showMessageBox(tr("Can't save file. File %1 not exists in database.").arg(fromFullFileName));
    return;
  }

  // Сохранение
  bool result=file.copy( toFullFileName );

  if(!result)
  {
    showMessageBox(tr("Can't save file %1. Any i/o problem.").arg(toFullFileName));
    return;
  }

  // Расшифровка файла, если он был зашифрован и данные хранились в базе (то есть, это именно тип file, а не линк на файл)
  if(isAttachCrypt && attachType=="file")
    CryptService::decryptFile(globalParameters.getCryptKey(), toFullFileName);
}


void ActionLogController::onEditFileName(void)
{
  QList<QString> selectedId=getSelectedId();

  // Если ни один аттач не выбран
  if(selectedId.size()==0)
    return;

  // Если выбрано больше одного аттача
  if(selectedId.size()>1)
  {
    showMessageBox(tr("Please select single attach for edit."));
    return;
  }

  // Имя файла выбранного аттача
  QString id=selectedId.at(0);
  ActionLogData *ActionLogData=getActionLogData();
  QString fileName=ActionLogData->getFileNameById(id);

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
    showMessageBox(tr("Cant save file with empty name."));
    return;
  }

  // Данные изменяются
  Attach tempAttach=ActionLogData->getAttach(id);
  tempAttach.setField("fileName", newFileName);
  ActionLogData->modifyAttach(id, tempAttach);

  // Сохранение дерева веток
  find_object<TreeScreen>("treeScreen")->saveKnowTree();
}


void ActionLogController::onDeleteAttach(void)
{
  QList<QString> selectedId=getSelectedId();

  // Если ни один аттач не выбран
  if(selectedId.size()==0)
  {
    showMessageBox(tr("Please select any attach(es) for delete."));
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
  ActionLogData *ActionLogData=getActionLogData();

  foreach( QString id, selectedId )
    ActionLogData->deleteAttach(id);


  // Сохранение дерева веток
  find_object<TreeScreen>("treeScreen")->saveKnowTree();

  // Обновление иконки аттачей в редакторе
  if(ActionLogData->size()==0)
  {
    MetaEditor *edView=find_object<MetaEditor>("editorScreen");
    edView->switchAttachIconExists(false);
  }
}


// Открытие аттача (аттачей) на просмотр
void ActionLogController::onOpenAttach(void)
{
  ActionLogData *ActionLogData=getActionLogData();

  QList<QString> selectedId=getSelectedId();

  foreach( QString id, selectedId )
  {
    QString fullFileName=ActionLogData->getAbsoluteInnerFileNameById(id);

    // Если запись зашифрована и открывается файл (не линк), аттач надо скопировать в директорию корзины и расшифровать
    if( ActionLogData->isRecordCrypt() && ActionLogData->getAttach(id).getField("type")=="file")
    {
      if(mytetraConfig.getEnableDecryptFileToTrashDirectory())
      {
        fullFileName=DiskHelper::copyFileToTrash(fullFileName); // Копирование
        CryptService::decryptFile(globalParameters.getCryptKey(), fullFileName); // Расшифровка
      }
      else
      {
        showMessageBox(tr("Can't preview encrypted attach file %1.\n\nUse \"Save As...\" button or enable decrypt to temporary file in settings.").arg(fullFileName));
        continue;
      }
    }

    qDebug() << "Open file: "+fullFileName;

    // QUrl urlFile;
    // urlFile.fromLocalFile(fullFileName);
    QUrl urlFile("file:"+fullFileName);

    // Открытие файла средствами операционной системы
    QDesktopServices::openUrl(urlFile);
  }
}


void ActionLogController::onShowAttachInfo(void)
{
  QList<QString> selectedId=getSelectedId();

  // Если ни один аттач не выбран
  if(selectedId.size()==0)
    return;

  // Если выбрано больше одного аттача
  if(selectedId.size()>1)
  {
    showMessageBox(tr("Please select single attach for see info."));
    return;
  }

  QString id=selectedId.at(0);
  ActionLogData *ActionLogData=getActionLogData();

  ReduceMessageBox messageBox;
  messageBox.setText("Attach file info");
  messageBox.setDetailedText(QString("<pre><p style='font-family:monospace'>")+\
                             "<b>Attach:</b> "+ActionLogData->getFileNameById(id)+"\n"+\
                             "<b>Attach ID:</b> "+id+"\n"+\
                             "<b>Attach type:</b> "+ActionLogData->getAttach(id).getField("type")+"\n"+\
                             "<b>File size:</b> "+QString::number( ActionLogData->getAttach(id).getFileSize() )+" bytes\n"+\
                             "<b>Full path:</b> "+ActionLogData->getAbsoluteInnerFileNameById(id)+\
                             "</p></pre>"
                             );
  messageBox.setDetailedTextReadOnly(true);
  messageBox.setStandardButtons(QDialogButtonBox::Ok); // Для двух кнопок можно так: QDialogButtonBox::Ok | QDialogButtonBox::Cancel

  // int result=messageBox.exec();
  // qDebug() << "Result code: " << result;
  messageBox.exec();
}


void ActionLogController::onSwitchToEditor(void)
{
  MetaEditor *edView=find_object<MetaEditor>("editorScreen");
  edView->switchToEditorLayout();
}


// Получение списка идентификаторов аттачей, выделенных в представлении
QList<QString> ActionLogController::getSelectedId(void)
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

      selectedId.append( model->data(selectItems.at(i), ActionLog_ROLE_ID).toString() );
    }
  }

  return selectedId;
}
