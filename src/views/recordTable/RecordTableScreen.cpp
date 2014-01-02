#include <QObject>
#include <QMimeData>
#include <QProgressBar>
#include <QMap>
#include <QList>

#include "main.h"
#include "RecordListScreen.h"
#include "RecordTableScreen.h"

#include "views/mainWindow/MainWindow.h"
#include "libraries/ClipboardRecords.h"
#include "models/recordTable/RecordTableModel.h"
#include "models/recordTable/RecordTableData.h"
#include "views/tree/TreeScreen.h"
#include "views/record/MetaEditor.h"
#include "models/appConfig/AppConfig.h"
#include "views/record/AddNewRecord.h"
#include "views/record/RecordInfoFieldsEditor.h"
#include "views/findInBaseScreen/FindScreen.h"
#include "libraries/WalkHistory.h"

extern AppConfig mytetraconfig;
extern WalkHistory walkHistory;


// Виджет, который отображает список записей в ветке
// c кнопками управления

RecordTableScreen::RecordTableScreen(QWidget *parent) : QWidget(parent)
{
 setupActions();
 setupUI();

 // Для вида таблицы данных устанавливается модель
 // Установку модели надо сделать перед соединением сигналов и слотов
 recordModel=new RecordTableModel();
 recordModel->setObjectName("recordmodel");
 recordView->setModel(recordModel);
 
 setupSignals();
 assembly();

 // Нужно установить правила показа контекстного самодельного меню
 // чтобы оно могло вызываться
 assemblyContextMenu();
 recordView->setContextMenuPolicy(Qt::CustomContextMenu);
 
}

RecordTableScreen::~RecordTableScreen()
{

}


void RecordTableScreen::setupActions(void)
{
 // Добавление записи
 // a->setShortcut(tr("Ctrl+X"));
 actionAddNewToEnd = new QAction(tr("Add note"), this);
 actionAddNewToEnd->setStatusTip(tr("Add a new note"));
 actionAddNewToEnd->setIcon(QIcon(":/resource/pic/note_add.svg"));
 connect(actionAddNewToEnd, SIGNAL(triggered()), this, SLOT(addNewToEndContext()));

 // Добавление записи до
 actionAddNewBefore = new QAction(tr("Add note before"), this);
 actionAddNewBefore->setStatusTip(tr("Add a note before selected"));
 connect(actionAddNewBefore, SIGNAL(triggered()), this, SLOT(addNewBeforeContext()));

 // Добавление записи после
 actionAddNewAfter = new QAction(tr("Add note after"), this);
 actionAddNewAfter->setStatusTip(tr("Add a note after selected"));
 connect(actionAddNewAfter, SIGNAL(triggered()), this, SLOT(addNewAfterContext()));

 // Редактирование записи
 actionEditField = new QAction(tr("Edit properties (name, author, tags...)"), this);
 actionEditField->setStatusTip(tr("Edit note properties (name, author, tags...)"));
 actionEditField->setIcon(QIcon(":/resource/pic/note_edit.svg"));
 connect(actionEditField, SIGNAL(triggered()), this, SLOT(editFieldContext()));

 // Удаление записи
 actionDelete = new QAction(tr("Delete note(s)"), this);
 actionDelete->setStatusTip(tr("Delete note(s)"));
 actionDelete->setIcon(QIcon(":/resource/pic/note_delete.svg"));
 connect(actionDelete, SIGNAL(triggered()), this, SLOT(deleteContext()));

 // Удаление записи с копированием в буфер обмена
 actionCut = new QAction(tr("&Cut note(s)"), this);
 actionCut->setStatusTip(tr("Cut notes(s) to clipboard"));
 actionCut->setIcon(QIcon(":/resource/pic/cb_cut.svg"));
 connect(actionCut, SIGNAL(triggered()), this, SLOT(cut()));

 // Копирование записи (записей) в буфер обмена
 actionCopy = new QAction(tr("&Copy note(s)"), this);
 actionCopy->setStatusTip(tr("Copy note(s) to clipboard"));
 actionCopy->setIcon(QIcon(":/resource/pic/cb_copy.svg"));
 connect(actionCopy, SIGNAL(triggered()), this, SLOT(copy()));

 // Вставка записи из буфера обмена
 actionPaste = new QAction(tr("&Paste note(s)"), this);
 actionPaste->setStatusTip(tr("Paste note(s) from clipboard"));
 actionPaste->setIcon(QIcon(":/resource/pic/cb_paste.svg"));
 connect(actionPaste, SIGNAL(triggered()), this, SLOT(paste()));

 // Перемещение записи вверх
 actionMoveUp = new QAction(tr("&Move Up"), this);
 actionMoveUp->setStatusTip(tr("Move note up"));
 actionMoveUp->setIcon(QIcon(":/resource/pic/move_up.svg"));
 connect(actionMoveUp, SIGNAL(triggered()), this, SLOT(moveUp()));

 // Перемещение записи вниз
 actionMoveDn=new QAction(tr("&Move Down"), this);
 actionMoveDn->setStatusTip(tr("Move note down"));
 actionMoveDn->setIcon(QIcon(":/resource/pic/move_dn.svg"));
 connect(actionMoveDn, SIGNAL(triggered()), this, SLOT(moveDn()));

 // Поиск по базе
 actionFindInBase=new QAction(tr("Find in base"), this);
 actionFindInBase->setStatusTip(tr("Find in base"));
 actionFindInBase->setIcon(QIcon(":/resource/pic/find_in_base.svg"));
 connect(actionFindInBase, SIGNAL(triggered()), this, SLOT(findInBaseOpen()));

 // Синхронизация
 actionSyncro=new QAction(tr("Synchronization"), this);
 actionSyncro->setStatusTip(tr("Run synchronization"));
 actionSyncro->setIcon(QIcon(":/resource/pic/synchronization.svg"));
 connect(actionSyncro, SIGNAL(triggered()), this, SLOT(onSyncroClick()));

 // Перемещение по истории посещаемых записей назад
 actionWalkHistoryPrevious=new QAction(tr("Previous viewing note"), this);
 actionWalkHistoryPrevious->setShortcut(tr("Ctrl+<"));
 actionWalkHistoryPrevious->setStatusTip(tr("Previous note has been viewing"));
 actionWalkHistoryPrevious->setIcon(QIcon(":/resource/pic/walk_history_previous.svg"));
 connect(actionWalkHistoryPrevious, SIGNAL(triggered()), this, SLOT(onWalkHistoryPreviousClick()));

 // Перемещение по истории посещаемых записей вперед
 actionWalkHistoryNext=new QAction(tr("Next viewing note"), this);
 actionWalkHistoryNext->setShortcut(tr("Ctrl+>"));
 actionWalkHistoryNext->setStatusTip(tr("Next note has been viewing"));
 actionWalkHistoryNext->setIcon(QIcon(":/resource/pic/walk_history_next.svg"));
 connect(actionWalkHistoryNext, SIGNAL(triggered()), this, SLOT(onWalkHistoryNextClick()));


 // Сразу после создания все действия запрещены
 disableAllActions();
}


void RecordTableScreen::setupUI(void)
{
 toolsLine=new QToolBar(this);

 QSize toolBarIconSize(16,16);
 toolsLine->setIconSize(toolBarIconSize);

 insert_action_as_button(toolsLine, actionAddNewToEnd);
 insert_action_as_button(toolsLine, actionEditField);
 insert_action_as_button(toolsLine, actionDelete);
 toolsLine->addSeparator();
 insert_action_as_button(toolsLine, actionCut);
 insert_action_as_button(toolsLine, actionCopy);
 insert_action_as_button(toolsLine, actionPaste);
 toolsLine->addSeparator();
 insert_action_as_button(toolsLine, actionMoveUp);
 insert_action_as_button(toolsLine, actionMoveDn);

 findLine=new QToolBar(this);
 findLine->setIconSize(toolBarIconSize);
 insert_action_as_button(findLine, actionSyncro);
 insert_action_as_button(findLine, actionWalkHistoryPrevious);
 insert_action_as_button(findLine, actionWalkHistoryNext);
 insert_action_as_button(findLine, actionFindInBase);


 recordView=new RecordListScreen(this);
 recordView->setObjectName("recordview");
}


void RecordTableScreen::setupSignals(void)
{
 // Сигнал чтобы показать контекстное меню по правому клику на списке записей
 connect(recordView,SIGNAL(customContextMenuRequested(const QPoint &)),
         this,SLOT(onCustomContextMenuRequested(const QPoint &)));

 // Сигнал чтобы открыть на редактирование параметры записи при двойном клике
 connect(recordView, SIGNAL(doubleClicked(const QModelIndex &)),
         this, SLOT(editFieldContext(void)));




 // Обработка клика на конечной записи
 // connect(recordView, SIGNAL(clicked(const QModelIndex &)),
 //         this, SLOT(onClickToRecord(const QModelIndex&)));

 // Сигнал для обработки изменения засветки в списке конечных записей
 // connect(recordView->selectionModel(), SIGNAL(currentChanged (const QModelIndex&, const QModelIndex&)),
 //        this, SLOT(onSelectionRecordChanged(const QModelIndex&, const QModelIndex&)));


 connect(recordView, SIGNAL(listSelectionChanged ( const QItemSelection &, const QItemSelection &) ),
         this, SLOT(onSelectionChanged ( const QItemSelection &, const QItemSelection &) ));



 // Сигналы для обновления панели инструментов при изменении в selectionModel()
 connect(recordView->selectionModel(), SIGNAL(currentChanged (const QModelIndex&, const QModelIndex&)),
         this, SLOT(toolsUpdate(void)));
 connect(recordView->selectionModel(), SIGNAL(selectionChanged (const QItemSelection&, const QItemSelection&)),
         this, SLOT(toolsUpdate(void)));

 // Сигналы для обновления панели инструментов
 connect(recordView, SIGNAL(activated(const QModelIndex &)),
         this, SLOT(toolsUpdate(void)));
 connect(recordView, SIGNAL(clicked(const QModelIndex &)),
         this, SLOT(toolsUpdate(void)));
 connect(recordView, SIGNAL(doubleClicked(const QModelIndex &)),
         this, SLOT(toolsUpdate(void)));
 connect(recordView, SIGNAL(entered(const QModelIndex &)),
         this, SLOT(toolsUpdate(void)));
 connect(recordView, SIGNAL(pressed(const QModelIndex &)),
         this, SLOT(toolsUpdate(void)));
 connect(QApplication::clipboard(),SIGNAL(dataChanged()),
         this, SLOT(toolsUpdate(void)));
}


void RecordTableScreen::assembly(void)
{
 recordTableToolsLayout=new QHBoxLayout();
 recordTableToolsLayout->addWidget(toolsLine);
 recordTableToolsLayout->addStretch();
 recordTableToolsLayout->addWidget(findLine);
 
 recordTableScreenLayout=new QVBoxLayout();
 recordTableScreenLayout->setObjectName("recordtablescreen_QVBoxLayout");

 recordTableScreenLayout->addLayout(recordTableToolsLayout);
 recordTableScreenLayout->addWidget(recordView);

 setLayout(recordTableScreenLayout);

 // Границы убираются, так как данный объект будет использоваться как виджет
 QLayout *lt;
 lt=layout();
 lt->setContentsMargins(0,2,0,0);
}


// Установка нового набора данных для списка записей
void RecordTableScreen::setTableData(RecordTableData *rtData)
{
 qDebug() << "In recordtablescreen set_tabledata() start";
 
 // Обновление набора данных с последующим выбором первой строки
 // может быть очень длительным, поэтому показывается что процесс выполняется
 // QCursor cursor_wait=QCursor(Qt::BusyCursor);
 // qApp->setOverrideCursor(cursor_wait);
 find_object<MainWindow>("mainwindow")->setCursor(Qt::BusyCursor);
 
 // Указатель на данные сообщается источнику данных
 recordModel->setTableData(rtData);

 // Надо обязательно сбросить selection model для списка записей
 recordView->selectionModel()->clear();
 
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
     recordView->selectionModel()->setCurrentIndex( recordView->model()->index( workPos, 0 ) , QItemSelectionModel::SelectCurrent);

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
    toolsUpdate();
  }
 
 // qApp->restoreOverrideCursor();
 find_object<MainWindow>("mainwindow")->unsetCursor();
 
 qDebug() << "In recordtablescreen set new model stop";
}


// Действия при изменении местоположения указателя в таблице конечных записей
/*
void RecordTableScreen::onSelectionRecordChanged(const QModelIndex &indexCurrent,
                                                 const QModelIndex &indexPrevious)
{
 return;

 qDebug() << "RecordTableScreen::onSelectionRecordChanged()";
 qDebug() << "Current index. row() " << indexCurrent.row() << " isValid() " << indexCurrent.isValid();
 qDebug() << "Previous index. row() " << indexPrevious.row() << " isValid() " << indexPrevious.isValid();

 // if(indexPrevious.isValid()==false)
 //  return;

 onClickToRecord(indexCurrent);
}
*/

void RecordTableScreen::onSelectionChanged(const QItemSelection &selected,
                                           const QItemSelection &deselected )
{
 QModelIndex selectRecord;
 QModelIndex deselectRecord;

 if(!selected.indexes().isEmpty())
  selectRecord=selected.indexes().at(0);

 if(!deselected.indexes().isEmpty())
  deselectRecord=deselected.indexes().at(0);

 /*
 qDebug() << "RecordTableScreen::onSelectionChanged()";
 qDebug() << "Current index. row() " << selectRecord.row() << " isValid() " << selectRecord.isValid();
 qDebug() << "Previous index. row() " << deselectRecord.row() << " isValid() " << deselectRecord.isValid();
 */

 // return;
 
 if(selectRecord.isValid())
  onClickToRecord(selectRecord);
}


// Действия при выборе пункта таблицы конечных записей
void RecordTableScreen::onClickToRecord(const QModelIndex &index)
{
 qDebug() << "RecordTableScreen::onClickToRecord() : current item num " << index.row();

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
 qDebug() << "RecordTableScreen::onClickToRecord() : id " << table->getInfoField("id", index.row());
 qDebug() << "RecordTableScreen::onClickToRecord() : name " << table->getInfoField("name", index.row());
 qDebug() << "RecordTableScreen::onClickToRecord() : crypt " << table->getInfoField("crypt", index.row());
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
void RecordTableScreen::addNewToEndContext(void)
{
 qDebug() << "In slot add_new_toend_context()";

 addNewRecord(ADD_NEW_RECORD_TO_END);
}


// Слот для вызова добавления новой записи перед выделенной строкой
void RecordTableScreen::addNewBeforeContext(void)
{
 qDebug() << "In slot add_new_before_context()";

 addNewRecord(ADD_NEW_RECORD_BEFORE);
}


// Слот для вызова добавления новой записи после выделенной строки
void RecordTableScreen::addNewAfterContext(void)
{
 qDebug() << "In slot add_new_after_context()";

 addNewRecord(ADD_NEW_RECORD_AFTER);
}


// Вызов окна добавления данных в таблицу конечных записей
void RecordTableScreen::addNewRecord(int mode)
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
void RecordTableScreen::addNew(int mode,
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
 recordView->selectionModel()->setCurrentIndex(selIdx,QItemSelectionModel::ClearAndSelect);

 // Сохранение дерева веток
 find_object<TreeScreen>("treeview")->saveKnowTree();
}


// Слот, срабатывающий при нажатии кнопки редактирования записи
void RecordTableScreen::editFieldContext(void)
{
 qDebug() << "In edit_field_context()";

 // Получение индекса выделенного элемента
 QModelIndexList selectItems=recordView->selectionModel()->selectedIndexes();
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
void RecordTableScreen::editField(int pos,
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


void RecordTableScreen::deleteContext(void)
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


// Удаление отмеченных записей
void RecordTableScreen::deleteRecords(void)
{
 qDebug() << "recordtablescreen::delete_records()";
 
 // Запоминание позиции, на которой стоит курсор списка
 int beforeIndex=(recordView->selectionModel()->currentIndex()).row();

 // Выясняется количество элементов в таблице
 int totalRowCount=recordModel->rowCount();
 
 // Получение списка Item-элементов, подлежащих удалению
 QModelIndexList itemsForDelete=recordView->selectionModel()->selectedIndexes();

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
   recordView->selectionModel()->setCurrentIndex(selIdx,QItemSelectionModel::ClearAndSelect);
  }
 else
  {
   // Иначе таблица конечных записей пуста, курсор устанавливать ненужно

   // Нужно очистить поле редактирования чтобы невидно было текста
   // последней удаленной записи
   find_object<MetaEditor>("editorview")->clearAll();
  }

 toolsUpdate();
}


// Копирование отмеченных записей в буфер обмена с удалением 
// из таблицы конечных записей
void RecordTableScreen::cut(void)
{
 // Надо сохранить запись, так как перед копированием в буфер обмена запись 
 // обязательно должна быть сохранена, иначе редактирование, 
 // которое было после открытия записи и до нажатия Cut, потеряется
 find_object<MetaEditor>("editorview")->save_textarea();

 copy();
 deleteRecords();
}


// Копирование отмеченных записей в буфер обмена
void RecordTableScreen::copy(void)
{
 // Получение списка Item-элементов, подлежащих копированию
 QModelIndexList itemsForCopy=recordView->selectionModel()->selectedIndexes();

 // Список возвращается в произвольном порядке, не в таком как на экране
 // поэтому его нужно отсортировать по QModelIndex
 qSort(itemsForCopy.begin(),itemsForCopy.end());

 qDebug() << "Copy to buffer records";
 for(int i=0; i<itemsForCopy.size(); ++i)
  qDebug() << itemsForCopy.at(i).data().toString();
 
  // Создается ссылка на буфер обмена
 QClipboard *clipboardBuf=QApplication::clipboard();

 // Создается объект с данными для заполнения буфера обмена
 static int fillFlag=0;
 if(fillFlag==1)
  {
   const ClipboardRecords *clipboardRecordsPrevios;
   clipboardRecordsPrevios=qobject_cast<const ClipboardRecords *>(clipboardBuf->mimeData());
   if(clipboardRecordsPrevios!=NULL)delete clipboardRecordsPrevios;
   fillFlag=0;
  } 
 ClipboardRecords *clipboardRecords=new ClipboardRecords();
 fillFlag=1;

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

 // Печатается содержимое буфера в консоль
 // rcd->print();

 // Объект с записями помещается в буфер обмена
 clipboardBuf->setMimeData(clipboardRecords);
}


// Вставка записей из буфера обмена
void RecordTableScreen::paste(void)
{
 // Проверяется, содержит ли буфер обмена данные нужного формата
 if(!(QApplication::clipboard()->mimeData()->hasFormat("mytetra/records"))) 
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


// Отключение всех действий
void RecordTableScreen::disableAllActions(void)
{
 actionAddNewToEnd->setEnabled(false);
 actionAddNewBefore->setEnabled(false);
 actionAddNewAfter->setEnabled(false);
 actionEditField->setEnabled(false);
 actionDelete->setEnabled(false);

 actionCut->setEnabled(false);
 actionCopy->setEnabled(false);
 actionPaste->setEnabled(false);

 actionMoveUp->setEnabled(false);
 actionMoveDn->setEnabled(false);
}


void RecordTableScreen::toolsUpdate(void)
{
 qDebug() << "recordtablescreen::tools_update()";

 // Отключаются все действия
 disableAllActions();

 if(recordModel->getTableData()==NULL)
  return;

 // Выясняется, содержит ли текущая ветка подчиненные ветки
 /*
 QModelIndex index = find_object<TreeScreen>("treeview")->get_selection_model()->currentIndex();
 TreeItem *item = find_object<TreeScreen>("treeview")->kntrmodel->getItem(index);
 int branch_have_children=0;
 if(item->childCount()>0)branch_have_children=1;
 */

 // Включаются те действия которые разрешены

 // Добавление записи
 // Добавлять можно к любой ветке
 actionAddNewToEnd->setEnabled(true);

 // Добавление записи до
 // Добавлять "до" можно только тогда, когда выбрана только одна строка
 if((recordView->selectionModel()->hasSelection() &&
     (recordView->selectionModel()->selectedRows()).size()==1))
    actionAddNewBefore->setEnabled(true);

 // Добавление записи после
 // Добавлять "после" можно только тогда, когда выбрана только одна строка
 if((recordView->selectionModel()->hasSelection() &&
     (recordView->selectionModel()->selectedRows()).size()==1))
  actionAddNewAfter->setEnabled(true);

 // Редактирование записи
 // Редактировать можно только тогда, когда выбрана только одна строка
 if(recordView->selectionModel()->hasSelection() &&
    (recordView->selectionModel()->selectedRows()).size()==1)
  actionEditField->setEnabled(true);

 // Удаление записи
 // Пункт активен только если запись (или записи) выбраны в списке
 if(recordView->selectionModel()->hasSelection())
  actionDelete->setEnabled(true);

 // Удаление с копированием записи в буфер обмена
 // Пункт активен только если запись (или записи) выбраны в списке
 if(recordView->selectionModel()->hasSelection())
  actionCut->setEnabled(true);

 // Копирование записи в буфер обмена
 // Пункт активен только если запись (или записи) выбраны в списке
 if(recordView->selectionModel()->hasSelection())
  actionCopy->setEnabled(true);

 // Вставка записи из буфера обмена
 // Вставлять записи можно только тогда, когда выбрана
 // только одна строка (добавляется после выделеной строки)
 // или не выбрано ни одной строки (тогда добавляется в конец списка)
 // или записей вообще нет
 // И проверяется, содержит ли буфер обмена данные нужного формата
 if((recordView->selectionModel()->hasSelection() && (recordView->selectionModel()->selectedRows()).size()==1) ||
     recordView->selectionModel()->hasSelection()==false ||
     recordView->model()->rowCount()==0)
  if(QApplication::clipboard()->mimeData()->hasFormat("mytetra/records")) 
   actionPaste->setEnabled(true);

 // Перемещение записи вверх
 // Пункт возможен только когда выбрана одна строка
 // и указатель стоит не на начале списка
 if((recordView->selectionModel()->hasSelection() && (recordView->selectionModel()->selectedRows()).size()==1) &&
    isSelectedSetToTop()==false )
  actionMoveUp->setEnabled(true);

 // Перемещение записи вниз
 // Пункт возможен только когда выбрана одна строка
 // и указатель стоит не в конце списка
 if((recordView->selectionModel()->hasSelection() && (recordView->selectionModel()->selectedRows()).size()==1) &&
    isSelectedSetToBottom()==false )
  actionMoveDn->setEnabled(true);

 // Обновляется состояние области редактирования текста
 if(recordView->selectionModel()->hasSelection() &&
    recordModel->rowCount()>0)
  {
   qDebug() << "In table select present";
   qDebug() << "In table row count is" << recordModel->rowCount();
   find_object<MetaEditor>("editorview")->set_textarea_editable(true);
  }
 else
  {
   qDebug() << "In table select non present";
   find_object<MetaEditor>("editorview")->set_textarea_editable(false);
  }
}


void RecordTableScreen::assemblyContextMenu(void)
{
  // Конструирование меню
  contextMenu=new QMenu(this);
  
  contextMenu->addAction(actionAddNewToEnd);
  contextMenu->addAction(actionAddNewBefore);
  contextMenu->addAction(actionAddNewAfter);
  contextMenu->addSeparator();
  contextMenu->addAction(actionEditField);
  contextMenu->addAction(actionDelete);
  contextMenu->addSeparator();
  contextMenu->addAction(actionCut);
  contextMenu->addAction(actionCopy);
  contextMenu->addAction(actionPaste);
}


// Открытие контекстного меню в таблице конечных записей
void RecordTableScreen::onCustomContextMenuRequested(const QPoint &pos)
{
  qDebug("In on_customContextMenuRequested");
 
  // Включение отображения меню на экране
  // menu.exec(event->globalPos());
  contextMenu->exec(recordView->viewport()->mapToGlobal(pos));
}


// Перемещение записи вверх
void RecordTableScreen::moveUp(void)
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
void RecordTableScreen::moveDn(void)
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
int RecordTableScreen::getFirstSelectionPos(void)
{
 // Получение списка выделенных Item-элементов
 QModelIndexList selectItems=recordView->selectionModel()->selectedIndexes();

 if(selectItems.isEmpty())
  return -1; // Если ничего не выделено
 else
  return (selectItems.at(0)).row(); // Индекс первого выделенного элемента
}


bool RecordTableScreen::isSelectedSetToTop(void)
{
 if(getFirstSelectionPos()==0)return true;
 else return false;
}


bool RecordTableScreen::isSelectedSetToBottom(void)
{
 if(getFirstSelectionPos()==recordView->model()->rowCount()-1)return true;
 else return false;
}


// Установка засветки в нужную строку
void RecordTableScreen::setSelectionToPos(int pos)
{
 if(pos>(recordModel->rowCount()-1))
  return;
 
 // Создание индекса из номера
 QModelIndex selIdx=recordModel->index(pos);

 // Установка засветки на нужный индекс
 recordView->selectionModel()->setCurrentIndex(selIdx,QItemSelectionModel::ClearAndSelect);
}


// Действия при нажатии на кнопку поиска по базе
void RecordTableScreen::findInBaseOpen(void)
{
 // Определяется ссылка на виджет поиска
 FindScreen *findScreen=find_object<FindScreen>("findscreendisp");
 
 // Если виджет не показан, он выводится на экран, и наоборот
 if(findScreen->isVisible()==false)
  findScreen->widgetShow();
 else
  findScreen->widgetHide();
}


// Действия при нажатии кнопки синхронизации
void RecordTableScreen::onSyncroClick(void)
{
 find_object<MainWindow>("mainwindow")->synchronization();
}


void RecordTableScreen::onWalkHistoryPreviousClick(void)
{
 find_object<MainWindow>("mainwindow")->goWalkHistoryPrevious();
}


void RecordTableScreen::onWalkHistoryNextClick(void)
{
 find_object<MainWindow>("mainwindow")->goWalkHistoryNext();
}
