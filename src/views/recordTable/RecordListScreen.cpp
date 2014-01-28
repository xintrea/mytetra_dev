#include <QObject>
#include <QMimeData>
#include <QDrag>
#include <QVector>

#include "main.h"
#include "views/mainWindow/MainWindow.h"
#include "views/record/AddNewRecord.h"
#include "views/tree/TreeScreen.h"
#include "libraries/ClipboardRecords.h"
#include "views/record/RecordInfoFieldsEditor.h"
#include "RecordListScreen.h"
#include "views/recordTable/RecordTableScreen.h"
#include "models/recordTable/RecordTableModel.h"
#include "models/recordTable/RecordTableData.h"
#include "models/appConfig/AppConfig.h"
#include "views/record/MetaEditor.h"
#include "libraries/WalkHistory.h"

extern AppConfig mytetraconfig;
extern WalkHistory walkHistory;


// Виджет, отображащий список записей в ветке


RecordListScreen::RecordListScreen(QWidget *parent) : QListView(parent)
{
 // Установка модели данных
 recordModel=new RecordTableModel();
 recordModel->setObjectName("recordmodel");
 setModel(recordModel);
}


RecordListScreen::~RecordListScreen()
{
 delete recordModel;
}


// Пришлось ввести метод init, так как инициализация невозможна без
// созданных в parent QAction, а создать в parent QAction можно только
// при наличии ссылки на данный объект
void RecordListScreen::init(void)
{
 setupSignals();

 this->setSelectionMode(QAbstractItemView::SingleSelection); // Ранее было ExtendedSelection, но такой режим не подходит для Drag and Drop

 // Нужно установить правила показа контекстного самодельного меню
 // чтобы оно могло вызываться
 assemblyContextMenu();
 setContextMenuPolicy(Qt::CustomContextMenu);
}


void RecordListScreen::setupSignals(void)
{
 // Сигнал чтобы показать контекстное меню по правому клику на списке записей
 connect(this,SIGNAL(customContextMenuRequested(const QPoint &)),
         this,SLOT(onCustomContextMenuRequested(const QPoint &)));

 // Сигнал чтобы открыть на редактирование параметры записи при двойном клике
 connect(this, SIGNAL(doubleClicked(const QModelIndex &)),
         this, SLOT(editFieldContext(void)));

 connect(this, SIGNAL(listSelectionChanged ( const QItemSelection &, const QItemSelection &) ),
         this, SLOT(onSelectionChanged ( const QItemSelection &, const QItemSelection &) ));

 RecordTableScreen *parentPointer=qobject_cast<RecordTableScreen *>(parent());

 // Сигналы для обновления панели инструментов при изменении в selectionModel()
 connect(this->selectionModel(), SIGNAL(currentChanged (const QModelIndex&, const QModelIndex&)),
         parentPointer, SLOT(toolsUpdate(void)));
 connect(this->selectionModel(), SIGNAL(selectionChanged (const QItemSelection&, const QItemSelection&)),
         parentPointer, SLOT(toolsUpdate(void)));

 // Сигналы для обновления панели инструментов
 connect(this, SIGNAL(activated(const QModelIndex &)),
         parentPointer, SLOT(toolsUpdate(void)));
 connect(this, SIGNAL(clicked(const QModelIndex &)),
         parentPointer, SLOT(toolsUpdate(void)));
 connect(this, SIGNAL(doubleClicked(const QModelIndex &)),
         parentPointer, SLOT(toolsUpdate(void)));
 connect(this, SIGNAL(entered(const QModelIndex &)),
         parentPointer, SLOT(toolsUpdate(void)));
 connect(this, SIGNAL(pressed(const QModelIndex &)),
         parentPointer, SLOT(toolsUpdate(void)));
 connect(QApplication::clipboard(),SIGNAL(dataChanged()),
         parentPointer, SLOT(toolsUpdate(void)));
}


void RecordListScreen::onSelectionChanged(const QItemSelection &selected,
                                          const QItemSelection &deselected )
{
 QModelIndex selectRecord;
 QModelIndex deselectRecord;

 if(!selected.indexes().isEmpty())
  selectRecord=selected.indexes().at(0);

 if(!deselected.indexes().isEmpty())
  deselectRecord=deselected.indexes().at(0);

 /*
 qDebug() << "RecordListScreen::onSelectionChanged()";
 qDebug() << "Current index. row() " << selectRecord.row() << " isValid() " << selectRecord.isValid();
 qDebug() << "Previous index. row() " << deselectRecord.row() << " isValid() " << deselectRecord.isValid();
 */

 // return;

 if(selectRecord.isValid())
  onClickToRecord(selectRecord);
}


// Действия при выборе строки таблицы конечных записей
void RecordListScreen::onClickToRecord(const QModelIndex &index)
{
 qDebug() << "RecordListScreen::onClickToRecord() : current item num " << index.row();

 if(index.row()<0) return;

 // Выясняется указатель на объект редактирования текста записи
 MetaEditor *edView=find_object<MetaEditor>("editorview");

 // Выясняется ссылка на таблицу конечных данных
 RecordTableData *table=recordModel->getTableData();

 // В таблице конечных данных запоминается какая запись была выбрана
 // чтобы затем при выборе этой же подветки засветка автоматически
 // установилась на последнюю рабочую запись
 table->setWorkPos( index.row() );


 // Устанавливается функция обратного вызова для записи данных
 edView->set_save_callback(table->editorSaveCallback);

 // Сохраняется текст и картинки в окне редактирования
 find_object<MainWindow>("mainwindow")->saveTextarea();


 // Для новой выбраной записи выясняется директория и основной файл
 QString currentDir =table->getInfoField("dir", index.row());
 QString currentFile=table->getInfoField("file", index.row());
 QString fullDir=mytetraconfig.get_tetradir()+"/base/"+currentDir;
 QString fullFileName=fullDir+"/"+currentFile;
 qDebug() << " File " << fullFileName << "\n";

 // Если в окне содержимого записи уже находится выбираемая запись
 if(edView->get_work_directory()==fullDir &&
    edView->get_file_name()==currentFile) return;

 // Редактору задаются имя файла и директории
 // И дается команда загрузки файла
 edView->set_work_directory(fullDir);
 edView->set_file_name(currentFile);

 // Если идет работа с зашифрованной записью
 // И если имя директории или имя файла пусты, то это означает что
 // запись не была расшифрована, и редактор должен просто показывать пустой текст
 // ничего не сохранять и не считывать
 qDebug() << "RecordListScreen::onClickToRecord() : id " << table->getInfoField("id", index.row());
 qDebug() << "RecordListScreen::onClickToRecord() : name " << table->getInfoField("name", index.row());
 qDebug() << "RecordListScreen::onClickToRecord() : crypt " << table->getInfoField("crypt", index.row());
 if(table->getInfoField("crypt", index.row())=="1")
  if(fullDir.length()==0 || currentFile.length()==0)
   edView->setDirFileEmptyReaction(MetaEditor::DIRFILEEMPTY_REACTION_SUPPRESS_ERROR);

 // В редактор заносится информация, идет ли работа с зашифрованным текстом
 edView->setMiscField("crypt", table->getInfoField("crypt", index.row()));

 // В редакторе устанавливается функция обратного вызова для чтения данных
 edView->set_load_callback(table->editorLoadCallback);

 edView->load_textarea();
 // edView->set_textarea(table->get_text(index.row()));

 // Заполняются прочие инфо-поля
 edView->setName  ( table->getInfoField("name", index.row()) );
 edView->setAuthor( table->getInfoField("author",index.row()) );
 edView->setUrl   ( table->getInfoField("url", index.row()) );
 edView->setTags  ( table->getInfoField("tags", index.row()) );

 QString id=table->getInfoField("id", index.row());
 edView->setMiscField("id", id);


 // Восстанавливается позиция курсора и прокрутки если это необходимо
 if(mytetraconfig.getRememberCursorAtOrdinarySelection())
  {
   edView->setCursorPosition( walkHistory.getCursorPosition(id) );
   edView->setScrollBarPosition( walkHistory.getScrollBarPosition(id) );
  }

}


// Слот для вызова добавления новой записи в конец таблицы
void RecordListScreen::addNewToEndContext(void)
{
 qDebug() << "In slot add_new_toend_context()";

 addNewRecord(ADD_NEW_RECORD_TO_END);
}


// Слот для вызова добавления новой записи перед выделенной строкой
void RecordListScreen::addNewBeforeContext(void)
{
 qDebug() << "In slot add_new_before_context()";

 addNewRecord(ADD_NEW_RECORD_BEFORE);
}


// Слот для вызова добавления новой записи после выделенной строки
void RecordListScreen::addNewAfterContext(void)
{
 qDebug() << "In slot add_new_after_context()";

 addNewRecord(ADD_NEW_RECORD_AFTER);
}


// Вызов окна добавления данных в таблицу конечных записей
void RecordListScreen::addNewRecord(int mode)
{
 qDebug() << "In add_new_record()";

 // Создается окно ввода данных
 // При клике Ok внутри этого окна, будет создана временная директория
 // с картинками, содержащимися в тексте
 AddNewRecord addNewRecordWin;
 int i=addNewRecordWin.exec();
 if(i==QDialog::Rejected)return; // Была нажата отмена, ничего ненужно делать


 // Имя директории, в которой расположены файлы картинок, используемые в тексте
 QString directory=addNewRecordWin.getImagesDirectory();

 // Получение набора файлов картинок в виде структуры
 QMap<QString, QByteArray> files=get_files_from_directory(directory, "*.png");

 // Временная директория с картинками удаляется
 remove_directory(directory);

 QMap<QString, QString> fields;
 fields["name"]  =addNewRecordWin.getField("name");
 fields["author"]=addNewRecordWin.getField("author");
 fields["url"]   =addNewRecordWin.getField("url");
 fields["tags"]  =addNewRecordWin.getField("tags");

 // Введенные данные добавляются
 addNew(mode,
         fields,
         addNewRecordWin.getField("text"),
         files);
}


// Функция добавления новой записи в таблицу конечных записей
void RecordListScreen::addNew(int mode,
                                QMap<QString, QString> fields,
                                QString text,
                                QMap<QString, QByteArray> files)
{
 qDebug() << "In add_new()";

 // Получение номера первой выделенной строки
 int pos=getFirstSelectionPos();
 if(pos<0)pos=0; // Если ничего небыло выбрано

 // Вставка новых данных
 int selPos=recordModel->addTableData(mode,
                                      pos,
                                      fields,
                                      text,
                                      files);

 // Установка курсора на только что созданную позицию
 QModelIndex selIdx=recordModel->index(selPos); // Создание индекса из номера
 selectionModel()->setCurrentIndex(selIdx,QItemSelectionModel::ClearAndSelect);

 // Сохранение дерева веток
 find_object<TreeScreen>("treeview")->saveKnowTree();
}


// Слот, срабатывающий при нажатии кнопки редактирования записи
void RecordListScreen::editFieldContext(void)
{
 qDebug() << "In edit_field_context()";

 // Получение индекса выделенного элемента
 QModelIndexList selectItems=selectionModel()->selectedIndexes();
 QModelIndex index=selectItems.at(0);
 int pos=(selectItems.at(0)).row();

 // Создается окно ввода данных, после выхода из этой функции окно должно удалиться
 RecordInfoFieldsEditor editRecordWin;

  // Выясняется ссылка на таблицу конечных данных
 RecordTableData *table=recordModel->getTableData();

 // Поля окна заполняются начальными значениями
 editRecordWin.setField("name",  table->getInfoField("name",index.row()) );
 editRecordWin.setField("author",table->getInfoField("author",index.row()) );
 editRecordWin.setField("url",   table->getInfoField("url",index.row()) );
 editRecordWin.setField("tags",  table->getInfoField("tags",index.row()) );


 int i=editRecordWin.exec();
 if(i==QDialog::Rejected)return; // Была нажата отмена, ничего ненужно делать

 // Введенные данные обновляются
 editField(pos,
            editRecordWin.getField("name"),
            editRecordWin.getField("author"),
            editRecordWin.getField("url"),
            editRecordWin.getField("tags"));

 // Нужно перерисовать окно редактирования чтобы обновились инфополя
 // делается это путем "повторного" выбора текущего пункта
 onClickToRecord(index); // Раньше было select()
}


// Функция сохранения отредактированных полей записи в таблицу конечных записей
void RecordListScreen::editField(int pos,
                                 QString name,
                                 QString author,
                                 QString url,
                                 QString tags)
{
 qDebug() << "In edit_field()";

 // Выясняется ссылка на таблицу конечных данных
 RecordTableData *table=recordModel->getTableData();

 // Переданные отредактированные поля преобразуются в вид имя-значение
 QMap<QString, QString> editData;
 editData["name"]=name;
 editData["author"]=author;
 editData["url"]=url;
 editData["tags"]=tags;

 // Обновление новых данных в таблице конечных записей
 table->editRecordFields(pos, editData);

 // Обновление инфополей в области редактирования записи
 MetaEditor *metaEditor=find_object<MetaEditor>("editorview");
 metaEditor->setName(name);
 metaEditor->setAuthor(author);
 metaEditor->setUrl(url);
 metaEditor->setTags(tags);

 // Сохранение дерева веток
 find_object<TreeScreen>("treeview")->saveKnowTree();
}


void RecordListScreen::deleteContext(void)
{
 // Создается окно с вопросом нужно удалять запись (записи) или нет
 QMessageBox messageBox(this);
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


// Удаление записи с указанным номером (счет с нуля)
void RecordListScreen::deleteRecordByPos(int pos)
{
 QVector<int> vectorPos;
 vectorPos.append(pos);
 recordModel->removeRowsByList( vectorPos ); // Удаление
}


// Удаление записей с указанным номером (счет с нуля)
void RecordListScreen::deleteRecordsByPos(QVector<int> vectorPos)
{
 recordModel->removeRowsByList( vectorPos ); // Удаление
}


// Удаление отмеченных записей
void RecordListScreen::deleteRecords(void)
{
 qDebug() << "RecordListScreen::delete_records()";

 // Запоминание позиции, на которой стоит курсор списка
 int beforeIndex=(selectionModel()->currentIndex()).row();

 // Выясняется количество элементов в таблице
 int totalRowCount=recordModel->rowCount();

 // Получение списка Item-элементов, подлежащих удалению
 QModelIndexList itemsForDelete=selectionModel()->selectedIndexes();

 // Проверка, выбраны ли записи
 if(itemsForDelete.count()==0)
 {
   qDebug() << "Records for delete not selected.";
   return;
 }

 // Сбор в массив всех индексов, которые нужно удалить
 // Напрямую пробегать массив item-элементов и удалять из него нельзя
 // так как итератор начинает указывать на несуществующие элементы
 QVector<int> delIdx;
 QModelIndexList::iterator it;
 for(it=itemsForDelete.begin(); it!=itemsForDelete.end(); it++)
 {
  QModelIndex currIdx;
  currIdx=*it;
  qDebug() << "Mark for delete item num " << currIdx.row();
  delIdx.append( currIdx.row() );
 }

 // Массив удаляемых индексов сортируется так чтоб вначале
 // были индексы с наименьшим номером
 qSort(delIdx.begin(), delIdx.end(), qLess<int>());

 // Поиск индекса, на который надо установить засветку после удаления
 int i=0,selectionIndex=-1;
 for(i=0;i<totalRowCount;i++)
  {
   // Если позиция не помечена на удаление, она считается
   if(!delIdx.contains(i))
    {
     selectionIndex++;

     // На первой непомеченной после курсора позиции обработка прекращается
     // Значение selection_index в этот момент и есть нужный индекс
     if(i>beforeIndex)break;
    }
  }
 qDebug() << "After delete cursor set to" << selectionIndex << "row";

 // Надо очистить поля области редактировния, чтобы редактор не пытался сохранить текущую открытую, но удаленную запись
 find_object<MetaEditor>("editorview")->clearAll();

 // Вызывается удаление отмеченных записей
 recordModel->removeRowsByList(delIdx);

 // Сохранение дерева веток
 find_object<TreeScreen>("treeview")->saveKnowTree();

 // Обновление на экране ветки, на которой стоит засветка,
 // так как количество хранимых в ветке записей поменялось
 find_object<TreeScreen>("treeview")->updateSelectedBranch();

 // Установка курсора на нужную позицию
 if(selectionIndex>=0)
  {
   // Создание индекса из номера
   QModelIndex selIdx=recordModel->index(selectionIndex);

   // Установка курсора
   selectionModel()->setCurrentIndex(selIdx,QItemSelectionModel::ClearAndSelect);
  }
 else
  {
   // Иначе таблица конечных записей пуста, курсор устанавливать ненужно

   // Нужно очистить поле редактирования чтобы невидно было текста
   // последней удаленной записи
   find_object<MetaEditor>("editorview")->clearAll();
  }

 qobject_cast<RecordTableScreen *>(parent())->toolsUpdate();
}


void RecordListScreen::assemblyContextMenu(void)
{
  // Конструирование меню
  contextMenu=new QMenu(this);

  RecordTableScreen *parentPointer=qobject_cast<RecordTableScreen *>(parent());

  contextMenu->addAction(parentPointer->actionAddNewToEnd);
  contextMenu->addAction(parentPointer->actionAddNewBefore);
  contextMenu->addAction(parentPointer->actionAddNewAfter);
  contextMenu->addSeparator();
  contextMenu->addAction(parentPointer->actionEditField);
  contextMenu->addAction(parentPointer->actionDelete);
  contextMenu->addSeparator();
  contextMenu->addAction(parentPointer->actionCut);
  contextMenu->addAction(parentPointer->actionCopy);
  contextMenu->addAction(parentPointer->actionPaste);
}


// Открытие контекстного меню в таблице конечных записей
void RecordListScreen::onCustomContextMenuRequested(const QPoint &pos)
{
  qDebug("In on_customContextMenuRequested");

  // Включение отображения меню на экране
  // menu.exec(event->globalPos());
  contextMenu->exec( viewport()->mapToGlobal(pos) );
}


// Перемещение записи вверх
void RecordListScreen::moveUp(void)
{
 qDebug() << "In moveup()";

 // Получение номера первой выделенной строки
 int pos=getFirstSelectionPos();

 // Выясняется ссылка на таблицу конечных данных
 RecordTableData *table=recordModel->getTableData();

 // Перемещение текущей записи вверх
 table->moveUp(pos);

 // Установка засветки на перемещенную запись
 setSelectionToPos(pos-1);

 // Сохранение дерева веток
 find_object<TreeScreen>("treeview")->saveKnowTree();

}


// Перемещение записи вниз
void RecordListScreen::moveDn(void)
{
 qDebug() << "In movedn()";

 // Получение номера первой выделенной строки
 int pos=getFirstSelectionPos();

 // Выясняется ссылка на таблицу конечных данных
 RecordTableData *table=recordModel->getTableData();

 // Перемещение текущей записи вниз
 table->moveDn(pos);

 // Установка засветки на перемещенную запись
 setSelectionToPos(pos+1);

 // Сохранение дерева веток
 find_object<TreeScreen>("treeview")->saveKnowTree();

}


// Получение номера первого выделенного элемента
int RecordListScreen::getFirstSelectionPos(void)
{
 // Получение списка выделенных Item-элементов
 QModelIndexList selectItems=selectionModel()->selectedIndexes();

 if(selectItems.isEmpty())
  return -1; // Если ничего не выделено
 else
  return (selectItems.at(0)).row(); // Индекс первого выделенного элемента
}


bool RecordListScreen::isSelectedSetToTop(void)
{
 if(getFirstSelectionPos()==0)return true;
 else return false;
}


bool RecordListScreen::isSelectedSetToBottom(void)
{
 if(getFirstSelectionPos()==model()->rowCount()-1)return true;
 else return false;
}


// Установка засветки в нужную строку
void RecordListScreen::setSelectionToPos(int pos)
{
 if(pos>(recordModel->rowCount()-1))
  return;

 // Создание индекса из номера
 QModelIndex selIdx=recordModel->index(pos);

 // Установка засветки на нужный индекс
 selectionModel()->setCurrentIndex(selIdx,QItemSelectionModel::ClearAndSelect);
}


// Копирование отмеченных записей в буфер обмена с удалением
// из таблицы конечных записей
void RecordListScreen::cut(void)
{
 // Надо сохранить запись, так как перед копированием в буфер обмена запись
 // обязательно должна быть сохранена, иначе редактирование,
 // которое было после открытия записи и до нажатия Cut, потеряется
 find_object<MetaEditor>("editorview")->save_textarea();

 copy();
 deleteRecords();
}


// Копирование отмеченных записей в буфер обмена
void RecordListScreen::copy(void)
{
 // Объект с записями помещается в буфер обмена
 QApplication::clipboard() -> setMimeData( getSelectedRecords() );
}


// Вставка записей из буфера обмена
void RecordListScreen::paste(void)
{
 // Проверяется, содержит ли буфер обмена данные нужного формата
 const QMimeData *mimeData=QApplication::clipboard()->mimeData();
 if(mimeData==NULL)
  return;
 if( ! (mimeData->hasFormat("mytetra/records")) )
  return;

 // Создается ссылка на буфер обмена
 QClipboard *clipboardBuf=QApplication::clipboard();

 // Извлечение объекта из буфера обмена
 // const clipboardrecords *rcd=new clipboardrecords();
 const ClipboardRecords *clipboardRecords;
 clipboardRecords=qobject_cast<const ClipboardRecords *>(clipboardBuf->mimeData());
 clipboardRecords->print();

 // Выясняется количество записей в буфере
 int nList=clipboardRecords->getRecordsNum();

 // Пробегаются все записи в буфере
 for(int i=0;i<nList;i++)
  {
   QMap<QString, QString> fields;

   // Получение из буфера полей записи с нужным номером
   fields=clipboardRecords->getRecordFields(i);

   // Запоминается текст записи
   QString text=fields["text"];

   // Текст записи из списка инфополей удаляется
   fields.remove("text");

   qDebug() << "RecordTableScreen::paste() : fields " << fields;

   addNew(ADD_NEW_RECORD_TO_END,
           fields,
           text,
           clipboardRecords->getRecordFiles(i));
  }

 // Обновление на экране ветки, на которой стоит засветка,
 // так как количество хранимых в ветке записей поменялось
 find_object<TreeScreen>("treeview")->updateSelectedBranch();
}


// Реакция на нажатие кнопок мышки
void RecordListScreen::mousePressEvent(QMouseEvent *event)
{
 // Если нажата левая кнопка мыши
 if(event->buttons() == Qt::LeftButton)
  {
   mouseStartPos=event->pos();
  }

 QListView::mousePressEvent(event);
}


// Реакция на движение мышкой
void RecordListScreen::mouseMoveEvent(QMouseEvent *event)
{
 // Если при движении нажата левая кнопка мышки
 if(event->buttons() & Qt::LeftButton)
  {
   // Выясняется расстояние от места начала нажатия
   int distance=(event->pos() - mouseStartPos).manhattanLength();

   if(distance >= QApplication::startDragDistance()*2)
    {
       startDrag();
    }
  }

 QListView::mouseMoveEvent(event);
}


// Реакция на отпускание клавиши мышки
void RecordListScreen::mouseReleaseEvent(QMouseEvent *event)
{
 QListView::mouseReleaseEvent(event);
}


void RecordListScreen::startDrag()
{
 qDebug() << "Start record drag\n";

 // Если действительно выбрана строка
 if( currentIndex().isValid() )
  {
   // Копирование выделенных строк в объект переноса
   QDrag *drag=new QDrag(this);
   drag->setMimeData( getSelectedRecords() );

   // Запуск операции перетаскивания объекта (start - запуск перетаскивания)
   unsigned int result=drag->exec(Qt::MoveAction);

   if(result) // ==Qt::MoveAction
    {
     // Сюда код дойдет после того, как перетаскивание будет закончено

     qDebug() << "Success drag and drop move record";

     // Здесь сделать удаление переносимой записи

    }
  }
}


// Установка нового набора данных для списка записей
void RecordListScreen::setTableData(RecordTableData *rtData)
{
 qDebug() << "In RecordListScreen setTableData() start";

 // Обновление набора данных с последующим выбором первой строки
 // может быть очень длительным, поэтому показывается что процесс выполняется
 // QCursor cursor_wait=QCursor(Qt::BusyCursor);
 // qApp->setOverrideCursor(cursor_wait);
 find_object<MainWindow>("mainwindow")->setCursor(Qt::BusyCursor);

 // Указатель на данные сообщается источнику данных
 recordModel->setTableData(rtData);

 // Надо обязательно сбросить selection model
 selectionModel()->clear();

 // Если список конечных записей не пуст
 bool removeSelection=true;
 if(recordModel->rowCount()>0)
  {
   // Нужно выяснить, на какой записи ранее стояло выделение
   int workPos=rtData->getWorkPos();

   // Если номер записи допустимый
   if(workPos>0 && workPos<recordModel->rowCount())
    {
     // Выделение устанавливается на нужную запись
     selectionModel()->setCurrentIndex( model()->index( workPos, 0 ) , QItemSelectionModel::SelectCurrent);

     removeSelection=false;
    }
  }

 // Если выделение устанавливать ненужно
 if(removeSelection)
  {
    // Надо очистить поля области редактировния
    find_object<MetaEditor>("editorview")->clearAll();

    // При выборе записи обновление инструментов было бы вызвано автоматически
    // в альтернативной ветке (там "эмулируется" клик по записи)
    // А так как записей нет, то обновление инструментов надо
    // вызвать принудительно
    qobject_cast<RecordTableScreen *>(parent())->toolsUpdate();
  }

 // qApp->restoreOverrideCursor();
 find_object<MainWindow>("mainwindow")->unsetCursor();

 qDebug() << "In RecordListScreen set new model stop";
}


RecordTableData *RecordListScreen::getTableData(void)
{
 return recordModel->getTableData();
}


int RecordListScreen::getRowCount(void)
{
 return recordModel->rowCount();
}


ClipboardRecords *RecordListScreen::getSelectedRecords(void)
{
 // Получение списка Item-элементов, подлежащих копированию
 QModelIndexList itemsForCopy=selectionModel()->selectedIndexes();

 // Список возвращается в произвольном порядке, не в таком как на экране
 // поэтому его нужно отсортировать по QModelIndex
 qSort(itemsForCopy.begin(),itemsForCopy.end());

 qDebug() << "Get selected records";
 for(int i=0; i<itemsForCopy.size(); ++i)
  qDebug() << itemsForCopy.at(i).data().toString();


 // Объект с данными для заполнения буфера обмена
 ClipboardRecords *clipboardRecords=new ClipboardRecords();
 clipboardRecords->clear();

 // Выясняется ссылка на таблицу конечных данных
 RecordTableData *table=recordModel->getTableData();

 // Перебираются записи и вносятся в буфер обмена
 for(int i=0; i<itemsForCopy.size(); ++i)
  {
   // Образ записи, включающий все текстовые поля (и HTML-код записи как "text")
   QMap<QString, QString> exemplar=table->getRecordExemplar( (itemsForCopy.at(i)).row() );

   // Имя директории, в которой расположена запись и ее файлы
   QString directory=mytetraconfig.get_tetradir()+"/base/"+exemplar["dir"];

   clipboardRecords->addRecord( exemplar, get_files_from_directory(directory, "*.png") );
  }

 return clipboardRecords;
}


// Переопределенный слот (virtual protected slot)
void RecordListScreen::selectionChanged(const QItemSelection &selected,
                                        const QItemSelection &deselected )
{
 // qDebug() << "RecordListScreen::selectionChanged()";

 emit listSelectionChanged(selected, deselected);
}
