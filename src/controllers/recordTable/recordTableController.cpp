#include "recordTableController.h"

RecordTableController::recordTableController(QObject *parent) : QObject(parent)
{
  // Инициализируется область со списком записей
  recordTableView=new RecordTableView(this);
  recordTableView->setObjectName("recordTableView");

  // Создание модели данных
  recordSourceModel=new RecordTableModel(this);
  recordSourceModel->setObjectName("recordSourceModel");

  recordProxyModel=new RecordTableProxyModel(this);
  recordProxyModel->setSourceModel(recordSourceModel);
  recordProxyModel->setObjectName("recordProxyModel");

  // Модель данных задается для вида
  recordTableView->setModel(recordProxyModel);
}


void RecordTableController::init(void)
{
  recordTableView->init();
}


RecordTableView *RecordTableController::getView(void)
{
  return recordTableView;
}


bool RecordTableController::isTableEmpty(void)
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
 selectionModel()->clear();

 // Если список конечных записей не пуст
 bool removeSelection=true;
 if(recordSourceModel->rowCount()>0)
  {
   // Нужно выяснить, на какой записи ранее стояло выделение
   int workPos=rtData->getWorkPos();

   // Если номер записи допустимый
   if(workPos>0 && workPos<recordSourceModel->rowCount())
    {
     // Выделение устанавливается на нужную запись
     // selectionModel()->setCurrentIndex( model()->index( workPos, 0 ) , QItemSelectionModel::SelectCurrent);
     selectRow(workPos);
     scrollTo( currentIndex() ); // QAbstractItemView::PositionAtCenter

     removeSelection=false;
    }
  }

 // Если выделение устанавливать ненужно
 if(removeSelection)
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


int RecordTableController::getRowCount(void)
{
  return recordProxyModel.rowCount();
}


// Копирование отмеченных записей в буфер обмена с удалением
// из таблицы конечных записей
void RecordTableController::cut(void)
{
 // Надо сохранить запись, так как перед копированием в буфер обмена запись
 // обязательно должна быть сохранена, иначе редактирование,
 // которое было после открытия записи и до нажатия Cut, потеряется
 find_object<MetaEditor>("editorScreen")->save_textarea();

 copy();
 deleteRecords();
}


// Копирование отмеченных записей в буфер обмена
void RecordTableController::copy(void)
{
 // Объект с записями помещается в буфер обмена
 QApplication::clipboard() -> setMimeData( getSelectedRecords() );
}


// Вставка записей из буфера обмена
void RecordTableController::paste(void)
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
 find_object<TreeScreen>("treeScreen")->updateSelectedBranch();
}


// Слот для вызова добавления новой записи в конец таблицы
void RecordTableController::addNewToEndContext(void)
{
 qDebug() << "In slot add_new_toend_context()";

 addNewRecord(ADD_NEW_RECORD_TO_END);
}


// Слот для вызова добавления новой записи перед выделенной строкой
void RecordTableController::addNewBeforeContext(void)
{
 qDebug() << "In slot add_new_before_context()";

 addNewRecord(ADD_NEW_RECORD_BEFORE);
}


// Слот для вызова добавления новой записи после выделенной строки
void RecordTableController::addNewAfterContext(void)
{
 qDebug() << "In slot add_new_after_context()";

 addNewRecord(ADD_NEW_RECORD_AFTER);
}


// Вызов окна добавления данных в таблицу конечных записей
void RecordTableController::addNewRecord(int mode)
{
 qDebug() << "In add_new_record()";

 // Создается окно ввода данных
 // При клике Ok внутри этого окна, будет создана временная директория
 // с картинками, содержащимися в тексте
 AddNewRecord addNewRecordWin;
 int i=addNewRecordWin.exec();
 if(i==QDialog::Rejected)
   return; // Была нажата отмена, ничего ненужно делать


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
void RecordTableController::addNew(int mode,
                              QMap<QString, QString> fields,
                              QString text,
                              QMap<QString, QByteArray> files)
{
 qDebug() << "In add_new()";

 // Получение Source-индекса первой выделенной строки
 QModelIndex posIndex=getFirstSelectionSourceIndex();

 // Вставка новых данных
 int selPos=recordSourceModel->addTableData(mode,
                                      posIndex,
                                      fields,
                                      text,
                                      files);

 // Установка курсора на только что созданную позицию
 /*
 QModelIndex selIdx=recordSourceModel->index(selPos, 0); // Создание индекса из номера
 selectionModel()->setCurrentIndex(selIdx,QItemSelectionModel::ClearAndSelect);
 */

 // В QTableView некорректно работает установка на только что созданную строку
 // Это как-то связано с отрисовкой виджета QTableView
 // Прокрутка к только что созданной строке через selectRow() показывает только
 // верхнюю часть новой строки. Чтобы этого избежать, при добавлении в конец
 // таблицы конечных записей, установка прокрутки делается через scrollToBottom()
 if(mode==ADD_NEW_RECORD_TO_END ||
    ( mode==ADD_NEW_RECORD_AFTER && selPos>=(recordSourceModel->rowCount()-1) ) )
   scrollToBottom();

 selectRow(selPos);

 // Сохранение дерева веток
 find_object<TreeScreen>("treeScreen")->saveKnowTree();
}


// Слот, срабатывающий при нажатии кнопки редактирования записи
void RecordTableController::editFieldContext(void)
{
 qDebug() << "In edit_field_context()";

 // Получение индекса выделенного элемента
 QModelIndexList selectItems=selectionModel()->selectedIndexes();
 QModelIndex index=selectItems.at(0);
 int pos=(selectItems.at(0)).row();

 // Создается окно ввода данных, после выхода из этой функции окно должно удалиться
 RecordInfoFieldsEditor editRecordWin;

  // Выясняется ссылка на таблицу конечных данных
 RecordTableData *table=recordSourceModel->getTableData();

 // Поля окна заполняются начальными значениями
 editRecordWin.setField("name",  table->getField("name",index.row()) );
 editRecordWin.setField("author",table->getField("author",index.row()) );
 editRecordWin.setField("url",   table->getField("url",index.row()) );
 editRecordWin.setField("tags",  table->getField("tags",index.row()) );


 int i=editRecordWin.exec();
 if(i==QDialog::Rejected)
   return; // Была нажата отмена, ничего ненужно делать

 // Введенные данные обновляются
 editField(pos,
            editRecordWin.getField("name"),
            editRecordWin.getField("author"),
            editRecordWin.getField("url"),
            editRecordWin.getField("tags"));

 // Нужно перерисовать окно редактирования чтобы обновились инфополя
 // делается это путем "повторного" выбора текущего пункта
 clickToRecord(index); // Раньше было select()
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


void recordTableController::deleteContext(void)
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
void RecordTableController::deleteRecordByPos(int pos)
{
 QVector<int> vectorPos;
 vectorPos.append(pos);
 recordSourceModel->removeRowsByList( vectorPos ); // Удаление
}


// Удаление записей с указанным номером (счет с нуля)
void RecordTableController::deleteRecordsByPos(QVector<int> vectorPos)
{
 recordSourceModel->removeRowsByList( vectorPos ); // Удаление
}


// Удаление отмеченных записей
void RecordTableController::deleteRecords(void)
{
 qDebug() << "RecordTableView::delete_records()";

 // Запоминание позиции, на которой стоит курсор списка
 int beforeIndex=(selectionModel()->currentIndex()).row();

 // Выясняется количество элементов в таблице
 int totalRowCount=recordSourceModel->rowCount();

 // Получение списка Item-элементов, подлежащих удалению
 QModelIndexList itemsForDelete=selectionModel()->selectedIndexes();

 // Проверка, выбраны ли записи
 if(itemsForDelete.count()==0)
 {
   qDebug() << "Records for delete not selected.";

   QMessageBox msgBox;
   msgBox.setText("Please select at least one record for delete.");
   msgBox.exec();

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

   // Если номер не содержится в перечне удаляемых строк
   // это может произойти если видно несколько столбцов - у каждой ячейки будет один и тот же номер строки
   if(!delIdx.contains(currIdx.row()))
   {
     qDebug() << "Mark for delete item num " << currIdx.row();
     delIdx.append( currIdx.row() );
   }
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
 find_object<MetaEditor>("editorScreen")->clearAll();

 // Вызывается удаление отмеченных записей
 recordSourceModel->removeRowsByList(delIdx);

 // Сохранение дерева веток
 find_object<TreeScreen>("treeScreen")->saveKnowTree();

 // Обновление на экране ветки, на которой стоит засветка,
 // так как количество хранимых в ветке записей поменялось
 find_object<TreeScreen>("treeScreen")->updateSelectedBranch();

 // Установка курсора на нужную позицию
 if(selectionIndex>=0)
 {
   /*
   // Создание индекса из номера
   QModelIndex selIdx=recordSourceModel->index(selectionIndex, 0);
   // Установка курсора
   selectionModel()->setCurrentIndex(selIdx,QItemSelectionModel::ClearAndSelect);
   */
   selectRow(selectionIndex);
 }
 else
 {
   // Иначе таблица конечных записей пуста, курсор устанавливать ненужно

   // Нужно очистить поле редактирования чтобы невидно было текста
   // последней удаленной записи
   find_object<MetaEditor>("editorScreen")->clearAll();
 }

 qobject_cast<RecordTableScreen *>(parent())->toolsUpdate();
}


// Перемещение записи вверх
void RecordTableController::moveUp(void)
{
  qDebug() << "In moveup()";

  // Получение номера первой выделенной строки
  int pos=getFirstSelectionPos();

  // Выясняется ссылка на таблицу конечных данных
  RecordTableData *table=recordSourceModel->getTableData();

  // Перемещение текущей записи вверх
  table->moveUp(pos);

  // Установка засветки на перемещенную запись
  setSelectionToPos(pos-1);

  // Сохранение дерева веток
  find_object<TreeScreen>("treeScreen")->saveKnowTree();
}


// Перемещение записи вниз
void RecordTableController::moveDn(void)
{
 qDebug() << "In movedn()";

 // Получение номера первой выделенной строки
 int pos=getFirstSelectionPos();

 // Выясняется ссылка на таблицу конечных данных
 RecordTableData *table=recordSourceModel->getTableData();

 // Перемещение текущей записи вниз
 table->moveDn(pos);

 // Установка засветки на перемещенную запись
 setSelectionToPos(pos+1);

 // Сохранение дерева веток
 find_object<TreeScreen>("treeScreen")->saveKnowTree();

}


// Клик по пункту "Сортировка" в контекстном меню
void RecordTableController::onSortClick(void)
{
  RecordTableScreen *parentPointer=qobject_cast<RecordTableScreen *>(parent());

  // Если сортировка еще не включена
  if( !recordTableView->isSortingEnabled() )
  {
    // Включается сортировка
    recordTableView->setSortingEnabled(true);

    recordProxyModel->setSortRole(Qt::DisplayRole);

    // Включается сортировка по нужному столбцу
    int n=parentPointer->actionSort->data().toInt();
    qDebug() << "Sort column number " << n;
    recordProxyModel->sort(n);

    // Треугольничек сортировки переставляется на нужный столбец
    horizontalHeader()->setSortIndicator(n, Qt::AscendingOrder);

    // Запрещается передвижение заголовков столбцов
    // так как после переноса неправильно устанавливается треугольничек сортировки, он остается на том же по счету столбце
    // horizontalHeader()->setSectionsMovable(false);
  }
  else
  {
    // Оменяется сортировка
    recordTableView->setSortingEnabled(false);
    recordProxyModel->setSortRole(Qt::InitialSortOrderRole);
    recordProxyModel->invalidate();

    // Разрешается передвижение заголовков столбцов
    // horizontalHeader()->setSectionsMovable(true);
  }

}


