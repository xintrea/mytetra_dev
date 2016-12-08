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
#include "libraries/crypt/CryptService.h"
#include "libraries/DiskHelper.h"
#include "views/record/MetaEditor.h"
#include "views/tree/TreeScreen.h"
#include "views/dialog/ReduceMessageBox.h"
#include "libraries/Downloader.h"


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
  addSmart("file");
}


void AttachTableController::onAddLink()
{
  addSmart("link");
}


void AttachTableController::onAddAttachFromUrl(void)
{
  // Окно запроса URL
  QInputDialog inputDialog(view);

  // Установка ширины окна запроса URL
  int dialogWidth=int(0.9*(float)view->width());
  inputDialog.setMinimumWidth( dialogWidth );
  inputDialog.resize(inputDialog.size());

  // Основные настройки окна запроса URL
  inputDialog.setWindowTitle(tr("Enter file URL"));
  inputDialog.setLabelText(tr("Enter file URL"));
  inputDialog.setTextEchoMode(QLineEdit::Normal);

  // Запуск окна запроса URL
  bool ok=inputDialog.exec();

  // Если при вводе URL была нажата отмена или введено пустой URL
  if(!ok || inputDialog.textValue().trimmed()=="")
    return;

  // Перечень скачиваемых файлов из одного файла
  QStringList downloadReferences=(QStringList() << inputDialog.textValue());

  // Виджет скачивания файлов
  Downloader downloader;
  downloader.setAboutText(tr("Download file"));
  downloader.setDownloadMode(Downloader::disk);
  downloader.setSaveDirectory( QDir::tempPath() );
  downloader.setReferencesList(downloadReferences);

  // Установка ширины для виджета скачивания файлов
  downloader.setMinimumWidth( dialogWidth );
  downloader.resize( downloader.size() );

  // На передний план
  downloader.raise();

  // Запуск виджета скачивания файлов
  downloader.run();

  // Если при скачивании файлов появились какие-то ошибки
  if(!downloader.isSuccess())
  {
    QMessageBox msgBox;
    msgBox.setText(tr("Probably error in a process of files download."));

    if(downloader.getErrorLog().length()>0)
      msgBox.setDetailedText(downloader.getErrorLog());

    msgBox.exec();

    return; // Файл был один, и при ошибке скачивания не происходит сохранения файла
  }

  // Здесь закачка завершена, и надо сохранить файлы как Attach
  QMap<QString, QString> referencesAndFileNames=downloader.getReferencesAndFileNames();

  int sucessLoadCount=0;
  foreach (QString reference, referencesAndFileNames.keys())
  {
    QString shortFileName=referencesAndFileNames.value(reference);
    QString fullFileName=downloader.getSaveDirectory()+"/"+shortFileName;

    QUrl url(reference);
    QString displayName=url.fileName();

    if( addAttach("file", fullFileName, displayName) )
      sucessLoadCount++;

    // Исходный файл в директории закачки удаляется
    QFile(fullFileName).remove();
  }

  // Дерево записей сохраняется
  saveState();
}


void AttachTableController::addSmart(QString attachType)
{
  QStringList files=selectFilesForAdding(attachType);

  if(files.size()==0)
    return; // Если ни один файл не выбран

  // Перебираются выбранные в диалоге файлы
  int sucessLoadCount=0;
  for(int i=0; i<files.size(); ++i)
  {
    // Текущее полное имя файла
    QString currFullFileName=files.at(i);

    QFileInfo currFileInfo(currFullFileName);

    // Если пользователь выбрал директорию (директорию выбирать нельзя, пока что можно выбирать только файлы)
    if(currFileInfo.isDir())
    {
      showMessageBox(tr("Cannot add a directory. Please select a file(s)."));
      break;
    }

    qDebug() << "Select file from disk: " << currFullFileName;

    // Текущее короткое имя файла
    QString currShortFileName=currFileInfo.fileName();

    // Добавляется файл аттача
    if( addAttach(attachType, currFullFileName, currShortFileName) )
      sucessLoadCount++;
    else
      break;
  } // Закончился цикл перебора файлов

  // Дерево записей сохраняется
  saveState();
}


bool AttachTableController::addAttach(QString attachType, QString currFullFileName, QString currShortFileName)
{
  // Выясняется указатель на данные таблицы приаттаченных файлов
  AttachTableData *attachTableData=getAttachTableData();
  if(attachTableData==NULL)
    criticalError("Unset attach table data in AttachTableController::addAttach()");

  // Конструируется Attach, который нужно добавить
  Attach attach(attachType, attachTableData);
  attach.setField("id", getUnicalId());
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
  if( attachTableData->isRecordCrypt() )
    attach.encrypt();


  if(result)
  {
    // Данные аттача добавляются в таблицу приаттаченных файлов
    attachTableData->addAttach(attach);
    return true;
  }
  else
  {
    showMessageBox(tr("Error copying file(s). Unable to attach file(s)."));
    return false;
  }
}


void AttachTableController::saveState()
{
  // Сохранение дерева веток
  find_object<TreeScreen>("treeScreen")->saveKnowTree();


  // Указатель на данные таблицы приаттаченных файлов, чтобы обновить иконку аттачей в редакторе
  AttachTableData *attachTableData=getAttachTableData();
  if(attachTableData==NULL)
    criticalError("Unset attach table data in AttachTableController::addAttach()");

  // Указатель на редактор
  MetaEditor *edView=find_object<MetaEditor>("editorScreen");

  if(attachTableData->size()>0)
    edView->switchAttachIconExists(true);
  else
    edView->switchAttachIconExists(false);
}


QStringList AttachTableController::selectFilesForAdding(QString attachType)
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
void AttachTableController::onSaveAsAttach(void)
{
  QList<QString> selectedId=getSelectedId();

  // Если ни один аттач не выбран
  if(selectedId.size()==0)
  {
    showMessageBox(tr("Please, select at least one attached file to save."));
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
    QString attachType=attachTableData->getAttach(id).getField("type");
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
      showMessageBox(tr("To save single file you must set single result file name."));
      return;
    }

    // Указанное пользователем имя файла для сохранения аттача, взятое из формы ввода
    QString targetFileName=selectFiles.at(0);

    // Непосредственное сохранение файла
    saveAttachToUserPlace(fullFileName, targetFileName, attachType, attachTableData->isRecordCrypt());
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
      QString attachType=attachTableData->getAttach(id).getField("type");
      QString fileName=attachTableData->getFileNameById(id);
      QString fromFileName=attachTableData->getInnerFileNameById(id);
      QString fromFullFileName=attachTableData->getAbsoluteInnerFileNameById(id);
      QString toFullFileName=toDir+"/"+fileName;

      // Непосредственное сохранение файла
      saveAttachToUserPlace(fromFullFileName, toFullFileName, attachType, attachTableData->isRecordCrypt());
    }
  }
}


void AttachTableController::saveAttachToUserPlace(QString fromFullFileName, QString toFullFileName, QString attachType, bool isAttachCrypt)
{
  // Проверка наличия исходного файла (ведь по каким-то причинам его может не быть, например после какого-нибудь сбоя)
  QFile file(fromFullFileName);

  if(file.exists()==false)
  {
    showMessageBox(tr("Unable to save the file: file %1 not found in the database.").arg(fromFullFileName));
    return;
  }

  // Сохранение
  bool result=file.copy( toFullFileName );

  if(!result)
  {
    showMessageBox(tr("Unable to save the file: file %1 input/output error.").arg(toFullFileName));
    return;
  }

  // Расшифровка файла, если он был зашифрован и данные хранились в базе (то есть, это именно тип file, а не линк на файл)
  if(isAttachCrypt && attachType=="file")
    CryptService::decryptFile(globalParameters.getCryptKey(), toFullFileName);
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
    showMessageBox(tr("Please select single attach for edit."));
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
    showMessageBox(tr("Cant save file with empty name."));
    return;
  }

  // Данные изменяются
  Attach tempAttach=attachTableData->getAttach(id);
  tempAttach.setField("fileName", newFileName);
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
    showMessageBox(tr("Please select any attach(es) for delete."));
    return;
  }

  // Запрос подтверждения об удалении
  QMessageBox msgBox;
  msgBox.setText(tr("Do you want to delete attached file(s)?"));
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
    edView->switchAttachIconExists(false);
  }
}


// Открытие аттача (аттачей) на просмотр
void AttachTableController::onOpenAttach(void)
{
  AttachTableData *attachTableData=getAttachTableData();

  QList<QString> selectedId=getSelectedId();

  foreach( QString id, selectedId )
  {
    QString fullFileName=attachTableData->getAbsoluteInnerFileNameById(id);

    // Если запись зашифрована и открывается файл (не линк), аттач надо скопировать в директорию корзины и расшифровать
    if( attachTableData->isRecordCrypt() && attachTableData->getAttach(id).getField("type")=="file")
    {
      if(mytetraConfig.getEnableDecryptFileToTrashDirectory())
      {
        fullFileName=DiskHelper::copyFileToTrash(fullFileName); // Копирование
        CryptService::decryptFile(globalParameters.getCryptKey(), fullFileName); // Расшифровка
      }
      else
      {
        showMessageBox(tr("Unable to preview the encrypted attached file %1.\nPlease, use «Save As...» button, or\nenable «Decrypt to a temporary file» option in Settings.").arg(fullFileName));
        continue;
      }
    }

    qDebug() << "Open file: "+fullFileName;

    // Открытие файла средствами операционной системы
    QUrl urlFile("file:"+fullFileName);
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
    showMessageBox(tr("Please, select a single file to see information about one."));
    return;
  }

  QString id=selectedId.at(0);
  AttachTableData *attachTableData=getAttachTableData();

  ReduceMessageBox messageBox;
  messageBox.setText("Attach file info");
  messageBox.setDetailedText(QString("<pre><p style='font-family:monospace'>")+\
                             "<b>Attach:</b> "+attachTableData->getFileNameById(id)+"\n"+\
                             "<b>Attach ID:</b> "+id+"\n"+\
                             "<b>Attach type:</b> "+attachTableData->getAttach(id).getField("type")+"\n"+\
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

  // В редакторе обновляется информация о приаттаченных к записи файлах
  edView->setMiscField("attachFileNameList", getAttachTableData()->getInnerFileNameOnDiskList().join(","));
  qDebug() << "Set attach file name list at switch to editor: " << edView->getMiscField("attachFileNameList");

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
