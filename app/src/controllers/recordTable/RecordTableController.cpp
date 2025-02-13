#include <QObject>
#include <QHeaderView>

#include "main.h"
#include "RecordTableController.h"
#include "controllers/attachTable/AttachTableController.h"
#include "views/record/MetaEditor.h"
#include "views/record/AddNewRecord.h"
#include "views/recordTable/RecordTableView.h"
#include "views/recordTable/RecordTableScreen.h"
#include "views/recordTable/RecordTablePrint.h"
#include "views/mainWindow/MainWindow.h"
#include "views/tree/TreeScreen.h"
#include "views/record/RecordInfoFieldsEditor.h"
#include "views/appConfigWindow/AppConfigDialog.h"
#include "models/recordTable/Record.h"
#include "models/recordTable/RecordTableData.h"
#include "models/recordTable/RecordTableModel.h"
#include "models/recordTable/RecordTableProxyModel.h"
#include "models/appConfig/AppConfig.h"
#include "models/tree/TreeItem.h"
#include "libraries/GlobalParameters.h"
#include "libraries/FixedParameters.h"
#include "libraries/WindowSwitcher.h"
#include "libraries/WalkHistory.h"
#include "libraries/ClipboardRecords.h"
#include "libraries/helpers/DiskHelper.h"
#include "libraries/helpers/ObjectHelper.h"
#include "libraries/wyedit/EditorShowTextDispatcher.h"


extern GlobalParameters globalParameters;
extern AppConfig mytetraConfig;
extern WalkHistory walkHistory;


RecordTableController::RecordTableController(QObject *parent) : QObject(parent)
{
  // Инициализируется область со списком записей
  view=new RecordTableView( qobject_cast<QWidget *>(parent) ); // Вид размещается внутри виджета Screen
  view->setObjectName("recordTableView");
  view->setController(this);

  // Создание модели данных
  recordSourceModel=new RecordTableModel(this);
  recordSourceModel->setObjectName("recordSourceModel");

  recordProxyModel=new RecordTableProxyModel(this);
  recordProxyModel->setSourceModel(recordSourceModel);
  recordProxyModel->setObjectName("recordProxyModel");

  // Модель данных задается для вида
  view->setModel(recordProxyModel);

  connect(this, &RecordTableController::doCloseDetachedWindowsByIdVector,
          EditorShowTextDispatcher::instance(), &EditorShowTextDispatcher::closeWindowByIdVector,
          Qt::QueuedConnection);
}


RecordTableController::~RecordTableController()
{
    // Уничтожение объекта будет происходить при выходе из программы

    // В окружении рабочего стола LXDE есть проблема: если при выходе из MyTetra в буфере обмена
    // будет лежать слепок(ки) записей, то произойдет перезапуск DE.
    // Непонятно как это работает, но проблема есть.
    // Чтобы ее избежать, надо очищать буфер обмена от данных со слепком записи, если таковые в буфере лежат

    // Проверяется, содержит ли буфер обмена данные записи
    const QMimeData *mimeData=QApplication::clipboard()->mimeData();
    if(mimeData!=nullptr && (mimeData->hasFormat(FixedParameters::appTextId+"/records")) ) {
        QApplication::clipboard()->setText(""); // В буфер обмена помещается пустой текст
    }
}


void RecordTableController::init(void)
{
  view->init();
}


RecordTableView *RecordTableController::getView(void)
{
  return view;
}


// Действия при выборе записи. Метод принимает индекс Proxy модели
void RecordTableController::clickToRecord(const QModelIndex &index)
{
  // Так как, возможно, включена сортировка, индекс на экране преобразуется в обычный индекс
  QModelIndex sourceIndex=convertProxyIndexToSourceIndex(index);

  // Позиция записи в списке
  int pos=sourceIndex.row();
  qDebug() << "RecordTableController::clickToRecord() : current item num " << pos;

  initMetaEditorAtClickToRecord(pos);
  initAttachTableAtClickToRecord(pos);
}


void RecordTableController::initMetaEditorAtClickToRecord(const int pos)
{
  // Внимание! Наверно, всю эту логику следует перенести в MetaEditor. А здесь только получить данные из таблицы

  // Выясняется указатель на объект редактирования текста записи
  MetaEditor *edView=find_object<MetaEditor>("editorScreen");

  edView->setEnabled(true);

  // Выясняется ссылка на таблицу конечных данных
  RecordTableData *table=recordSourceModel->getTableData();

  // Элементы управления редактором становятся доступными только при условии что запись не заблокирована на изменение
  if( table->getField("block", pos)=="1" )
    edView->setReadOnly(true);
  else
    edView->setReadOnly(false);

  // В таблице конечных данных запоминается какая запись была выбрана
  // чтобы затем при выборе этой же подветки засветка автоматически
  // установилась на последнюю рабочую запись
  table->setWorkPos( pos );


  // Устанавливается функция обратного вызова для записи данных редактором на диск
  edView->setSaveCallback(table->editorSaveCallback);

  // Сохраняется текст и картинки в окне редактирования
  find_object<MainWindow>("mainwindow")->saveTextarea();


  // Для новой выбраной записи выясняется директория и основной файл
  QString currentDir =table->getField("dir", pos);
  QString currentFile=table->getField("file", pos);
  QString fullDir=mytetraConfig.get_tetradir()+"/base/"+currentDir;
  QString fullFileName=fullDir+"/"+currentFile;
  qDebug() << " File " << fullFileName << "\n";


  // В редактор заносится информация о приаттаченных к записи файлах
  // Это действие нужно сделать до проверки на открытие той же самой записи (см. далее), т. к. список приаттаченных файлов может измениться
  edView->setMiscField("attachFileNameList", table->getRecord(pos)->getAttachTable().getInnerFileNameOnDiskList().join(","));
  qDebug() << "Set attach file name list: " << edView->getMiscField("attachFileNameList");


  // Если в окне содержимого записи уже находится выбираемая запись
  if(edView->getWorkDirectory()==fullDir &&
     edView->getFileName()==currentFile)
  {
    globalParameters.getWindowSwitcher()->switchFromRecordtableToRecord();
    return;
  }

  // Перед открытием редактора происходит попытка получения текста записи
  // Этот вызов создаст файл с текстом записи, если он еще не создан (подумать, переделать)
  table->getText(pos);

  // Редактору задаются имя файла и директории
  // И дается команда загрузки файла
  edView->setWorkDirectory(fullDir);
  edView->setFileName(currentFile);

  // Если идет работа с зашифрованной записью
  // И если имя директории или имя файла пусты, то это означает что
  // запись не была расшифрована, и редактор должен просто показывать пустой текст
  // ничего не сохранять и не считывать
  qDebug() << "initMetaEditorAtClickToRecord() : id " << table->getField("id", pos);
  qDebug() << "initMetaEditorAtClickToRecord() : name " << table->getField("name", pos);
  qDebug() << "initMetaEditorAtClickToRecord() : crypt " << table->getField("crypt", pos);
  if(table->getField("crypt", pos)=="1")
    if(fullDir.length()==0 || currentFile.length()==0)
      edView->setDirFileEmptyReaction(MetaEditor::DIRFILEEMPTY_REACTION_SUPPRESS_ERROR);

  // В редактор заносится информация, идет ли работа с зашифрованным текстом
  edView->setMiscField("crypt", table->getField("crypt", pos));

  // В редакторе устанавливается функция обратного вызова для чтения данных
  edView->setLoadCallback(table->editorLoadCallback);

  edView->loadTextarea();
  // edView->set_textarea(table->get_text(index.row()));

  // Заполняются прочие инфо-поля
  edView->setName  ( table->getField("name", pos) );
  edView->setAuthor( table->getField("author", pos) );
  edView->setUrl   ( table->getField("url", pos) );
  edView->setTags  ( table->getField("tags", pos) );

  QString id=table->getField("id", pos);
  edView->setMiscField("id", id);

  edView->setMiscField( "title", table->getField("name", pos) );

  // Устанавливается путь до ветки в которой лежит запись (в виде названий веток)
  QString path=qobject_cast<RecordTableScreen *>(parent())->getTreePath();

  // В мобильном интерфейсе редактор должен показывать путь до записи
  if(mytetraConfig.getInterfaceMode()=="mobile")
    edView->setTreePath( path );

  // В редакторе восстанавливается позиция курсора и прокрутки если это необходимо
  if(mytetraConfig.getRememberCursorAtOrdinarySelection())
  {
    edView->setCursorPosition( walkHistory.getCursorPosition(id) );
    edView->setScrollBarPosition( walkHistory.getScrollBarPosition(id) );
  }

  // Обновление иконки аттачей
  if( table->getRecord(pos)->getAttachTablePointer()->size()==0 )
    edView->switchAttachIconExists(false); // Если нет приаттаченных файлов
  else
    edView->switchAttachIconExists(true); // Есть приаттаченные файлы
}


void RecordTableController::initAttachTableAtClickToRecord(const int pos)
{
  // Выясняется ссылка на таблицу конечных данных
  RecordTableData *table=recordSourceModel->getTableData();

  // Устанавливается таблица приаттаченных файлов
  AttachTableController *attachTableController=find_object<AttachTableController>("attachTableController");
  attachTableController->setAttachTableData( table->getRecord(pos)->getAttachTablePointer() );
}


void RecordTableController::switchMetaEditorToEditorOrAttach(QModelIndex index)
{
  // Выясняется столбец, по которому был клик
  int column=index.column();
  qDebug() << "RecordTableController::switchMetaEditorToAttachLayoutIfNeed() : current column " << column;

  QString fieldName=mytetraConfig.getRecordTableShowFields().at(column);

  MetaEditor *edView=find_object<MetaEditor>("editorScreen");
  if(fieldName=="hasAttach" || fieldName=="attachCount") // Если это столбец с иконкой аттачей или количеством аттачей
    edView->switchToAttachLayout(); // Редактор переключается на слой работы с аттачами
  else
    edView->switchToEditorLayout(); // Если клик не по иконке аттачей в таблице, то открывается текст записи
}


bool RecordTableController::isTableNotExists(void)
{
 if( recordSourceModel->getTableData()==NULL )
   return true;
 else
   return false;
}


// Установка нового набора данных для списка записей
void RecordTableController::setTableData(RecordTableData *rtData)
{
  qDebug() << "In RecordTableView setTableData() start";

  // Обновление набора данных с последующим выбором первой строки
  // может быть очень длительным, поэтому показывается что процесс выполняется
  // QCursor cursor_wait=QCursor(Qt::BusyCursor);
  // qApp->setOverrideCursor(cursor_wait);
  find_object<MainWindow>("mainwindow")->setCursor(Qt::BusyCursor);

  // Указатель на данные сообщается источнику данных
  recordSourceModel->setTableData(rtData);

  // Надо обязательно сбросить selection model
  view->selectionModel()->clear();

  bool isSelection=false;

  // Если список конечных записей не пуст
  if(recordSourceModel->rowCount()>0)
  {
    // Нужно выяснить, на какой записи ранее стояло выделение
    int workPos=rtData->getWorkPos();

    // Если номер записи допустимый
    if(workPos>=0 && workPos<recordSourceModel->rowCount())
    {
      // Выделение устанавливается на нужную запись
      // selectionModel()->setCurrentIndex( model()->index( workPos, 0 ) , QItemSelectionModel::SelectCurrent);
      view->selectRow(workPos);
      view->scrollTo( view->currentIndex() ); // QAbstractItemView::PositionAtCenter

      isSelection=true;
    }
  }

  // Если выделения записи нет
  if( !isSelection )
  {
    // Надо очистить поля области редактировния
    find_object<MetaEditor>("editorScreen")->clearAll();

    // При выборе записи обновление инструментов было бы вызвано автоматически
    // в альтернативной ветке (там "эмулируется" клик по записи)
    // А так как записей нет, то обновление инструментов надо
    // вызвать принудительно
    qobject_cast<RecordTableScreen *>(parent())->toolsUpdate();
  }

  // qApp->restoreOverrideCursor();
  find_object<MainWindow>("mainwindow")->unsetCursor();

  qDebug() << "In RecordTableView set new model stop";
}


// Заполнение переданного объекта буфера обмена данными из указанных записей
// Индексы QModelIndexList передаются от Proxy модели
void RecordTableController::addRecordsToClipboard(ClipboardRecords *clipboardRecords, QModelIndexList itemsForCopy)
{
  // Выясняется ссылка на таблицу конечных данных
  RecordTableData *table=recordSourceModel->getTableData();

  // Перебираются записи и вносятся в буфер обмена
  for(int i=0; i<itemsForCopy.size(); ++i)
  {
    QModelIndex index=convertProxyIndexToSourceIndex( itemsForCopy.at(i) );

    // Образ записи, включающий все текстовые данные (текст записи, свойства записи, перечень приаттаченных файлов)
    Record record=table->getRecordFat( index.row() );

    clipboardRecords->addRecord( record );
  }
}


int RecordTableController::getRowCount(void)
{
  return recordProxyModel->rowCount();
}


// Получение номера первого выделенного элемента на экране
int RecordTableController::getFirstSelectionPos(void)
{
  return view->getFirstSelectionPos();
}


// Получение ID первого выделенного элемента на экране
QString RecordTableController::getFirstSelectionId(void)
{
  return view->getFirstSelectionId();
}


void RecordTableController::setSelectionToPos(int pos)
{
  view->setSelectionToPos(pos);
}


void RecordTableController::setSelectionToId(QString id)
{
  // Выясняется ссылка на таблицу конечных данных
  RecordTableData *table=recordSourceModel->getTableData();

  // Если таблица конечных данных задана
  // (Не задана таблица может быть по причине если ветка зашифрована и введен неверный пароль, или при вводе пароля была нажата отмена)
  if(table!=nullptr)
  {
    // Номер записи в Source данных
    int pos=table->getPosById(id);

    view->setSelectionToPos( convertSourcePosToProxyPos(pos) );
  }
}


QModelIndex RecordTableController::convertIdToSourceIndex(QString id)
{
  // Выясняется ссылка на таблицу конечных данных
  RecordTableData *table=recordSourceModel->getTableData();

  // Номер записи в Source данных
  int sourcePos=table->getPosById(id);

  return convertPosToSourceIndex(sourcePos);
}


QModelIndex RecordTableController::convertIdToProxyIndex(QString id)
{
  // Выясняется ссылка на таблицу конечных данных
  RecordTableData *table=recordSourceModel->getTableData();

  // Номер записи в Source данных
  int sourcePos=table->getPosById(id);
  int proxyPos=convertSourcePosToProxyPos(sourcePos);

  return convertPosToProxyIndex(proxyPos);
}


QModelIndex RecordTableController::convertPosToProxyIndex(int pos)
{
  if(pos<0 || pos>=recordProxyModel->rowCount())
    return QModelIndex();

  QModelIndex index = recordProxyModel->index( pos, 0 );

  return index;
}


QModelIndex RecordTableController::convertPosToSourceIndex(int pos)
{
  if(pos<0 || pos>=recordProxyModel->rowCount())
    return QModelIndex();

  QModelIndex proxyIndex=convertPosToProxyIndex(pos);
  QModelIndex index = recordProxyModel->mapToSource( proxyIndex );

  return index;
}


// Преобразование Proxy индекса в позицию на экране (так, как это будет выглядеть при Proxy модели)
int RecordTableController::convertProxyIndexToPos(QModelIndex index)
{
  if(!index.isValid())
    return -1;

  return index.row();
}


// Преобразование Source индекса в позицию на экране (так, как это будет выглядеть при Source модели)
int RecordTableController::convertSourceIndexToPos(QModelIndex index)
{
  if(!index.isValid())
    return -1;

  return index.row();
}


QModelIndex RecordTableController::convertProxyIndexToSourceIndex(QModelIndex proxyIndex)
{
  if(!proxyIndex.isValid())
    return QModelIndex();

  QModelIndex index = recordProxyModel->mapToSource( proxyIndex );

  return index;
}


QModelIndex RecordTableController::convertSourceIndexToProxyIndex(QModelIndex sourceIndex)
{
  if(!sourceIndex.isValid())
    return QModelIndex();

  QModelIndex index = recordProxyModel->mapFromSource( sourceIndex );

  return index;
}


int RecordTableController::convertSourcePosToProxyPos(int sourcePos)
{
  QModelIndex proxyIndex=recordProxyModel->mapFromSource( recordSourceModel->index(sourcePos, 0) );
  return proxyIndex.row();
}


int RecordTableController::convertProxyPosToSourcePos(int proxyPos)
{
  QModelIndex sourceIndex=recordProxyModel->mapToSource( recordProxyModel->index(proxyPos, 0) );
  return sourceIndex.row();
}


// Копирование отмеченных записей в буфер обмена с удалением
// из таблицы конечных записей
void RecordTableController::cut(void)
{
    copy();
    deleteRecords();
}


// Копирование отмеченных записей в буфер обмена
void RecordTableController::copy(void)
{
    // Надо сохранить запись, так как перед копированием в буфер обмена запись
    // обязательно должна быть сохранена, иначе редактирование,
    // которое было после открытия записи, потеряется
    find_object<MetaEditor>("editorScreen")->saveTextarea();

    // Объект с записями помещается в буфер обмена
    QApplication::clipboard() -> setMimeData( view->getSelectedRecords() );
}


// Вставка записей из буфера обмена
void RecordTableController::paste(void)
{
  // Проверяется, содержит ли буфер обмена данные нужного формата
  const QMimeData *mimeData=QApplication::clipboard()->mimeData();
  if(mimeData==nullptr)
    return;
  if( ! (mimeData->hasFormat(FixedParameters::appTextId+"/records")) )
    return;

  // Создается ссылка на буфер обмена
  QClipboard *clipboardBuf=QApplication::clipboard();

  // Извлечение объекта из буфера обмена
  // const clipboardrecords *rcd=new clipboardrecords();
  const ClipboardRecords *clipboardRecords;
  clipboardRecords=qobject_cast<const ClipboardRecords *>(clipboardBuf->mimeData());
  // clipboardRecords->print();

  // Выясняется количество записей в буфере
  int nList=clipboardRecords->getCount();

  // Пробегаются все записи в буфере
  for(int i=0;i<nList;i++)
    addNew(GlobalParameters::AddNewRecordBehavior::ADD_TO_END, clipboardRecords->getRecord(i));

  // Обновление на экране ветки, на которой стоит засветка,
  // так как количество хранимых в ветке записей поменялось
  find_object<TreeScreen>("treeScreen")->updateSelectedBranch();
}


// Слот для добавления новой записи в конец таблицы
void RecordTableController::addNewToEndContext(void)
{
  qDebug() << "In slot add_new_toend_context()";

  addNewRecord(GlobalParameters::AddNewRecordBehavior::ADD_TO_END);
}


// Слот для добавления новой записи перед выделенной строкой
void RecordTableController::addNewBeforeContext(void)
{
  qDebug() << "In slot add_new_before_context()";

  addNewRecord(GlobalParameters::AddNewRecordBehavior::ADD_BEFORE);
}


// Слот для добавления новой записи после выделенной строки
void RecordTableController::addNewAfterContext(void)
{
  qDebug() << "In slot add_new_after_context()";

  addNewRecord(GlobalParameters::AddNewRecordBehavior::ADD_AFTER);
}


// Вызов окна добавления данных в таблицу конечных записей
void RecordTableController::addNewRecord(int mode)
{
  qDebug() << "In add_new_record()";

  // Создается окно ввода данных
  // При клике Ok внутри этого окна, будет создана временная директория
  // с картинками, содержащимися в тексте
  AddNewRecord addNewRecordWin( view );
  int i=addNewRecordWin.exec();
  if(i==QDialog::Rejected)
    return; // Была нажата отмена, ничего не нужно делать

  // Имя директории, в которой расположены файлы картинок, используемые в тексте и приаттаченные файлы
  QString directory=addNewRecordWin.getImagesDirectory();

  // todo: сделать заполнение таблицы приаттаченных файлов

  Record record;
  record.switchToFat();
  record.setText( addNewRecordWin.getField("text") );
  record.setField("name",   addNewRecordWin.getField("name"));
  record.setField("author", addNewRecordWin.getField("author"));
  record.setField("url",    addNewRecordWin.getField("url"));
  record.setField("tags",   addNewRecordWin.getField("tags"));
  record.setPictureFiles( DiskHelper::getFilesFromDirectory(directory, "*.png") );

  // Пока что принята концепция, что файлы нельзя приаттачить в момент создания записи
  // Запись должна быть создана, потом можно аттачить файлы.
  // Это ограничение для "ленивого" программинга, но пока так
  // record.setAttachFiles( DiskHelper::getFilesFromDirectory(directory, "*.bin") );

  // Временная директория с картинками и приаттаченными файлами удаляется
  DiskHelper::removeDirectory(directory);

  // Введенные данные добавляются (все только что введенные данные передаются в функцию addNew() незашифрованными)
  addNew(mode, record);

  // После добавления новой записи редактор всегда должен переключаться на слой текста
  // (а не оставаться на слое аттачей, если он ранее был активным)
  MetaEditor *edView=find_object<MetaEditor>("editorScreen");
  edView->switchToEditorLayout();
}


// Функция добавления новой записи в таблицу конечных записей
// Принимает полный формат записи
void RecordTableController::addNew(int mode, Record record)
{
    qDebug() << "In add_new()";

    // Получение Source-индекса первой выделенной строки
    QModelIndex posIndex=view->getFirstSelectionSourceIndex();

    // Вставка новых данных, возвращаемая позиция - это позиция в Source данных
    int selPos=recordSourceModel->addTableData(mode,
                                               posIndex,
                                               record);

    if(selPos>=0)
    {
        view->moveCursorToNewRecord(mode, convertSourcePosToProxyPos(selPos) );

        // Сохранение дерева веток
        find_object<TreeScreen>("treeScreen")->saveKnowTree();
    }
}


void RecordTableController::onEditFieldContext(void)
{
    view->editFieldContext();
}


// При выборе пункта "Блокировка записи" в контекстном меню
void RecordTableController::onBlockContext(void)
{
  // Получение индекса выделенного элемента
  QModelIndexList selectItems=view->selectionModel()->selectedIndexes();
  QModelIndex index=selectItems.at(0);

  // Номер строки в базе
  QModelIndex sourceIndex=convertProxyIndexToSourceIndex(index);
  int pos=sourceIndex.row();

  // Выясняется ссылка на таблицу конечных данных
  RecordTableData *table=recordSourceModel->getTableData();

  // Если строка не заблокирована
  if(table->getField("block", pos)!="1")
  {
    // Устанавливается значение в базе
    table->setField("block", "1", pos);

    // Обновляется строка на экране
    int viewPos=index.row(); // Номер строки на экране
    view->updateRow(viewPos);

    find_object<TreeScreen>("treeScreen")->saveKnowTree(); // Сохранение дерева веток
  }
  else
  {
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Question);

    msgBox.setText(tr("Unblock this note?"));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    int ret = msgBox.exec();

    if(ret==QMessageBox::Yes)
    {
      table->setField("block", "", pos);

      find_object<TreeScreen>("treeScreen")->saveKnowTree(); // Сохранение дерева веток
    }
  }

  // По записи происходит виртуальный клик, чтобы интерфейс принял новое состояние блокировки записи
  this->clickToRecord(index);
}


// Действия при нажатии кнопки редактирования записи
void RecordTableController::editFieldContext(QModelIndex proxyIndex)
{
  qDebug() << "RecordTableController::editFieldContext()";

  QModelIndex sourceIndex=convertProxyIndexToSourceIndex(proxyIndex);
  int pos=sourceIndex.row(); // Номер строки в базе

  // Создается окно ввода данных, после выхода из этой функции окно должно удалиться
  RecordInfoFieldsEditor editRecordWin;

  // Выясняется ссылка на таблицу конечных данных
  RecordTableData *table=recordSourceModel->getTableData();

  // Поля окна заполняются начальными значениями
  editRecordWin.setField("id",     table->getField("id",     pos) );
  editRecordWin.setField("dir",    table->getField("dir",    pos) );
  editRecordWin.setField("name",   table->getField("name",   pos) );
  editRecordWin.setField("author", table->getField("author", pos) );
  editRecordWin.setField("url",    table->getField("url",    pos) );
  editRecordWin.setField("tags",   table->getField("tags",   pos) );

  // Если запись заблокирована
  if(table->getField("block",   pos)=="1")
    editRecordWin.setReadOnly(true);

  int i=editRecordWin.exec();
  if(i==QDialog::Rejected)
    return; // Была нажата отмена, ничего не нужно делать

  // Измененные данные записываются
  editField(pos,
            editRecordWin.getField("name"),
            editRecordWin.getField("author"),
            editRecordWin.getField("url"),
            editRecordWin.getField("tags"));
}


// Функция сохранения отредактированных полей записи в таблицу конечных записей
void RecordTableController::editField(int pos,
                                      QString name,
                                      QString author,
                                      QString url,
                                      QString tags)
{
  qDebug() << "In edit_field()";

  // Выясняется ссылка на таблицу конечных данных
  RecordTableData *table=recordSourceModel->getTableData();

  // Переданные отредактированные поля преобразуются в вид имя-значение
  QMap<QString, QString> editData;
  editData["name"]=name;
  editData["author"]=author;
  editData["url"]=url;
  editData["tags"]=tags;

  // Обновление новых данных в таблице конечных записей
  table->editRecordFields(pos, editData);

  // Обновление инфополей в области редактирования записи
  MetaEditor *metaEditor=find_object<MetaEditor>("editorScreen");
  metaEditor->setName(name);
  metaEditor->setAuthor(author);
  metaEditor->setUrl(url);
  metaEditor->setTags(tags);

  // Сохранение дерева веток
  find_object<TreeScreen>("treeScreen")->saveKnowTree();
}


// Обработка клика по удалению записи в контекстном меню и по кнопке на панели
void RecordTableController::deleteContext(void)
{
  // Создается окно с вопросом нужно удалять запись (записи) или нет
  QMessageBox messageBox(view);
  messageBox.setWindowTitle("Delete");
  messageBox.setText(tr("Are you sure to delete this record(s)?"));
  QAbstractButton *cancelButton =messageBox.addButton(tr("Cancel"), QMessageBox::RejectRole);
  QAbstractButton *deleteButton =messageBox.addButton(tr("Delete"), QMessageBox::AcceptRole);

  Q_UNUSED(cancelButton);

  messageBox.exec();
  if (messageBox.clickedButton() == deleteButton)
  {
    // Выбранные данные удаляются
    deleteRecords();
  }

}


// Удаление отмеченных записей
void RecordTableController::deleteRecords(void)
{
  qDebug() << "RecordTableView::delete_records()";

  // Получение списка Item-элементов, подлежащих удалению. Индексы Proxy модели
  QModelIndexList itemsForDelete=view->selectionModel()->selectedIndexes();

  // Проверка, выбраны ли записи
  if(itemsForDelete.count()==0)
  {
    qDebug() << "Records for delete not selected.";

    QMessageBox msgBox;
    msgBox.setText("Please select at least one record for delete.");
    msgBox.exec();

    return;
  }

  // Сбор в массив всех идентификаторов, которые нужно удалить
  // Напрямую пробегать массив item-элементов и удалять из него нельзя
  // так как итератор начинает указывать на несуществующие элементы
  QVector<QString> delIds;
  QVector<int> delRows;
  QModelIndexList::iterator it;
  for(it=itemsForDelete.begin(); it!=itemsForDelete.end(); it++)
  {
    QModelIndex currIdx;
    currIdx=*it;

    QString appendId=currIdx.data(RECORD_ID_ROLE).toString();

    // Если идентификатор не содержится в перечне удаляемых идентификаторов
    // это может произойти если видно несколько столбцов - у каждой ячейки будет один и тот же идентификатор записи
    if(!delIds.contains(appendId))
    {
      qDebug() << "Mark for delete item id " << appendId;
      delIds.append( appendId );
      delRows.append( currIdx.row() );
    }
  }


  // Массив удаляемых номеров строк (в Proxy-нумерации) сортируется так чтоб вначале были индексы с наибольшим номером
  std::sort(delRows.begin(), delRows.end(), std::greater<int>());
  int lastRowNum=delRows[0]; // Максимальный номер удаляемой строки

  // Номер строки на который надо установить засветку после удаления
  // Засветка устанавливается на запись, следующую после последней удаляемой
  int selectionRowNum=lastRowNum+1-delRows.count();
  qDebug() << "After delete cursor set to" << selectionRowNum << "row";

  // Надо очистить поля области редактировния, чтобы редактор не пытался сохранить текущую открытую, но удаленную запись
  find_object<MetaEditor>("editorScreen")->clearAll();

  // Вызывается закрытие открепляемых окон для удаляемых записей
  emit doCloseDetachedWindowsByIdVector(delIds);

  // Вызывается удаление отмеченных записей
  removeRowsByIdList(delIds);

  // Сохранение дерева веток
  find_object<TreeScreen>("treeScreen")->saveKnowTree();

  // Обновление на экране ветки, на которой стоит засветка,
  // так как количество хранимых в ветке записей поменялось
  find_object<TreeScreen>("treeScreen")->updateSelectedBranch();

  // Установка курсора на нужную позицию
  if(selectionRowNum>=0 && selectionRowNum<recordProxyModel->rowCount())
    view->selectRow(selectionRowNum);

  // Если таблица конечных записей пуста
  if(recordProxyModel->rowCount()==0)
  {
    // Нужно очистить поле редактирования чтобы невидно было текста
    // последней удаленной записи
    find_object<MetaEditor>("editorScreen")->clearAll();
  }

  qobject_cast<RecordTableScreen *>(parent())->toolsUpdate();
}


// Удаление одной записи по идентификатору
void RecordTableController::removeRowById(QString delId)
{
  QVector<QString> delIds;
  delIds.append( delId );
  removeRowsByIdList(delIds);
}


// Удаление записей по указанному списку идентификаторов
void RecordTableController::removeRowsByIdList(QVector<QString> delIds)
{
  qDebug() << "Remove rows by ID list: " << delIds;

  // Выясняется ссылка на таблицу конечных данных
  RecordTableData *table=recordSourceModel->getTableData();

  if(table==nullptr)
    return;

  for(int i=0;i<delIds.count();i++)
  {
    QString id=delIds[i];
    QModelIndex idx=convertIdToProxyIndex(id);

    // Удаляется строка в Proxy модели
    // Proxy модель сама должна уведомить вид о своем изменении, так как именно она подключена к виду
    recordProxyModel->removeRow(idx.row());
  }
}


// Перемещение записи вверх
void RecordTableController::moveUp(void)
{
  qDebug() << "In moveup()";

  // Получение номера первой выделенной строки
  int pos=view->getFirstSelectionPos();

  // Выясняется ссылка на таблицу конечных данных
  RecordTableData *table=recordSourceModel->getTableData();

  // Перемещение текущей записи вверх
  table->moveUp(pos);

  // Установка засветки на перемещенную запись
  view->setSelectionToPos(pos-1);

  // Сохранение дерева веток
  find_object<TreeScreen>("treeScreen")->saveKnowTree();
}


// Перемещение записи вниз
void RecordTableController::moveDn(void)
{
  qDebug() << "In movedn()";

  // Получение номера первой выделенной строки
  int pos=view->getFirstSelectionPos();

  // Выясняется ссылка на таблицу конечных данных
  RecordTableData *table=recordSourceModel->getTableData();

  // Перемещение текущей записи вниз
  table->moveDn(pos);

  // Установка засветки на перемещенную запись
  view->setSelectionToPos(pos+1);

  // Сохранение дерева веток
  find_object<TreeScreen>("treeScreen")->saveKnowTree();
}


// Клик по пункту "Сортировка" в контекстном меню
void RecordTableController::onSortClick(void)
{
  RecordTableScreen *parentPointer=qobject_cast<RecordTableScreen *>(parent());

  // Если сортировка еще не включена
  if( !view->isSortingEnabled() )
  {
    // Включается сортировка
    view->setSortingEnabled(true);

    recordProxyModel->setSortRole(SORT_ROLE);

    // Включается сортировка по нужному столбцу
    int n=parentPointer->actionSort->data().toInt(); // В actionSort хранится номер столбца, по которому нужно сортировать
    qDebug() << "Sort column number " << n;
    recordProxyModel->sort(n);

    // Треугольничек сортировки переставляется на нужный столбец
    view->horizontalHeader()->setSortIndicator(n, Qt::AscendingOrder);

    // Запрещается передвижение заголовков столбцов
    // так как после переноса неправильно устанавливается треугольничек сортировки, он остается на том же по счету столбце
    // horizontalHeader()->setSectionsMovable(false);
  }
  else
  {
    // Оменяется сортировка
    view->setSortingEnabled(false);
    recordProxyModel->setSortRole(Qt::InitialSortOrderRole);
    recordProxyModel->invalidate();

    // Разрешается передвижение заголовков столбцов
    // horizontalHeader()->setSectionsMovable(true);
  }

  qobject_cast<RecordTableScreen *>(parent())->toolsUpdate();
}


// Слот, срабатывающий при вызове настроек
void RecordTableController::settings(void)
{
  AppConfigDialog dialog("pageRecordTable", view); // view нужен чтобы пробрасывать иконку приложения
  dialog.exec();

  // Todo: Возвращение фокуса почему-то не работает, надо разбираться
  // (а может просто не выделяется виджет, в Qt5 вделенный виджет не виден в дефолтной схеме)
  // qDebug() << "Set focus to RecordTableView";
  // this->setFocus();
}


void RecordTableController::onRecordTableConfigChange(void)
{
  recordSourceModel->onRecordTableConfigChange();
  view->restoreColumnWidth();
  view->restoreHeaderState();
}


void RecordTableController::onPrintClick(void)
{
  RecordTableScreen *parentPointer=qobject_cast<RecordTableScreen *>(parent());

  RecordTablePrint printDialog(parentPointer);
  printDialog.setModel(recordProxyModel);
  printDialog.generateHtmlTableFromModel();
  printDialog.setTitleToHtml( recordSourceModel->getTableData()->getItem()->getPathAsNameWithDelimeter(" / ") );
  printDialog.exec();
}


void RecordTableController::onSwitchSelectionMode()
{
  view->switchSelectionMode();
}


void RecordTableController::setFocusToBaseWidget()
{
    view->setFocus();
}
