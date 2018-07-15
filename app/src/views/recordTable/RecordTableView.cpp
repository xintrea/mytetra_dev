#include <QObject>
#include <QMimeData>
#include <QDrag>
#include <QVector>
#include <QHeaderView>

#include "main.h"
#include "views/mainWindow/MainWindow.h"
#include "views/tree/TreeScreen.h"
#include "libraries/ClipboardRecords.h"
#include "RecordTableView.h"
#include "views/recordTable/RecordTableScreen.h"
#include "models/recordTable/RecordTableModel.h"
#include "models/recordTable/RecordTableProxyModel.h"
#include "models/appConfig/AppConfig.h"
#include "models/tree/KnowTreeModel.h"
#include "libraries/GlobalParameters.h"
#include "views/mainWindow/MainWindow.h"
#include "libraries/WindowSwitcher.h"
#include "controllers/recordTable/RecordTableController.h"


extern GlobalParameters globalParameters;
extern AppConfig mytetraConfig;


// Виджет, отображащий список записей в ветке


RecordTableView::RecordTableView(QWidget *parent) : QTableView(parent)
{
 // Изначально сортировка запрещена (заголовки столбцов не будут иметь треугольнички)
 this->setSortingEnabled(false);

 // Настройка области виджета для кинетической прокрутки
 setKineticScrollArea( qobject_cast<QAbstractItemView*>(this) );

 // Разрешение принимать жест QTapAndHoldGesture
 grabGesture(Qt::TapAndHoldGesture);
}


RecordTableView::~RecordTableView()
{

}


void RecordTableView::setController(RecordTableController *pController)
{
  controller=pController;
}


// Пришлось ввести метод init, так как инициализация невозможна без
// созданных в parent QAction, а создать в parent QAction можно только
// при наличии ссылки на данный объект
// Причина в том, что одни и те же QAction используются в двух местах -
// в RecordTableScreen и здесь в контекстном меню
void RecordTableView::init(void)
{
 qDebug() << "RecordTableView::init()";

 setupSignals();

 setSelectionMode(QAbstractItemView::SingleSelection); // Ранее было ExtendedSelection, но такой режим не подходит для Drag and Drop
 setSelectionBehavior(QAbstractItemView::SelectRows);

 restoreHeaderState();

 // Растягивание последней секции до размеров виджета
 horizontalHeader()->setStretchLastSection(true);

 // Заголовки не должны выглядеть нажатыми
 horizontalHeader()->setHighlightSections(false);

 horizontalHeader()->setMinimumSectionSize(16);

 // Горизонтальные заголовки делаются перемещяемыми
 #if QT_VERSION >= 0x040000 && QT_VERSION < 0x050000
 horizontalHeader()->setMovable(true);
 #endif
 #if QT_VERSION >= 0x050000 && QT_VERSION < 0x060000
 horizontalHeader()->setSectionsMovable(true);
 #endif

 // Установка высоты строки с принудительной стилизацией (если это необходимо),
 // так как стилизация через QSS для элементов QTableView полноценно не работает
 // У таблицы есть вертикальные заголовки, для каждой строки, в которых отображается номер строки.
 // При задании высоты вертикального заголовка, высота применяется и для всех ячеек в строке.
 verticalHeader()->setDefaultSectionSize ( verticalHeader()->minimumSectionSize () );
 int height=mytetraConfig.getUglyQssReplaceHeightForTableView();
 if(height!=0)
  verticalHeader()->setDefaultSectionSize( height );
 if(mytetraConfig.getInterfaceMode()=="mobile")
  verticalHeader()->setDefaultSectionSize( getCalculateIconSizePx() );

 setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

 restoreColumnWidth();

 // Разрешается перемещать секции заголовка таблицы
 enableMoveSection=true;

 // Нужно установить правила показа контекстного самодельного меню
 // чтобы оно могло вызываться
 assemblyContextMenu();
 setContextMenuPolicy(Qt::CustomContextMenu);
}


void RecordTableView::setupSignals(void)
{
 // Сигнал чтобы показать контекстное меню по правому клику на списке записей
 connect(this,SIGNAL(customContextMenuRequested(const QPoint &)),
         this,SLOT(onCustomContextMenuRequested(const QPoint &)));

 // Соединение сигнал-слот чтобы показать контекстное меню по долгому нажатию
 connect(this, SIGNAL(tapAndHoldGestureFinished(const QPoint &)),
         this, SLOT(onCustomContextMenuRequested(const QPoint &)));

 // Сигнал чтобы открыть на редактирование параметры записи при двойном клике
 connect(this, SIGNAL(doubleClicked(const QModelIndex &)),
         this, SLOT(editFieldContext(void)));

 // Перемещение курсора по строкам таблицы
 if(mytetraConfig.getInterfaceMode()=="desktop")
   connect(this, SIGNAL(listSelectionChanged ( const QItemSelection &, const QItemSelection &) ),
           this, SLOT(onSelectionChanged ( const QItemSelection &, const QItemSelection &) ));

 // Нажатие на запись (вызывается сразу как только нажалась кнопка, до отпускания)
 connect(this, SIGNAL( pressed(const QModelIndex &) ),
         this, SLOT( onPressToRecord(const QModelIndex &) ));

 // Клик по записи (вызывается после отпускания кнопки)
 connect(this, SIGNAL( clicked(const QModelIndex &) ),
         this, SLOT( onClickToRecord(const QModelIndex &) ));


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

 connect(this->horizontalHeader(), SIGNAL( sectionMoved( int, int, int ) ),
         this, SLOT( onSectionMoved( int, int, int ) ) );
 connect(this->horizontalHeader(), SIGNAL( sectionResized( int, int, int ) ),
         this, SLOT( onSectionResized( int, int, int ) ) );
}


void RecordTableView::restoreHeaderState( void )
{
  // Видимость горизонтальных заголовков
  if(mytetraConfig.getRecordTableShowHorizontalHeaders()==false)
    horizontalHeader()->hide();
  else
    horizontalHeader()->show();

  // Видимость вертикальных заголовков
  if(mytetraConfig.getRecordTableShowVerticalHeaders()==false)
    verticalHeader()->hide();
  else
    verticalHeader()->show();
}


// Обработчик сигнала listSelectionChanged
void RecordTableView::onSelectionChanged(const QItemSelection &selected,
                                         const QItemSelection &deselected )
{
  QModelIndex selectRecord;
  QModelIndex deselectRecord;

  if(!selected.indexes().isEmpty())
    selectRecord=selected.indexes().at(0);

  if(!deselected.indexes().isEmpty())
    deselectRecord=deselected.indexes().at(0);

  if(selectRecord.isValid())
    clickToRecord(selectRecord); // Программный клик по записи, куда переместился табличный курсор
}


// Слот активации записи. Принимает индекс Proxy модели
// Срабатывает сразу при клике (до отпускания мышки)
void RecordTableView::onPressToRecord(const QModelIndex &index)
{
  // В десктопном режиме запись становится видна из-за обрабоки сигнала listSelectionChanged,
  // Поэтому здесь нужно только отработать клик по иконке аттача в столбце hasAttach
  // (Так как если курсор уже стоит на строке, то клик по иконке аттача не вызывает listSelectionChanged, ибо смены строки нет)
  if(mytetraConfig.getInterfaceMode()=="desktop")
  {
    controller->switchMetaEditorToEditorOrAttach( index );
    return;
  }
}


// Слот клика по записи. Принимает индекс Proxy модели
// Срабатывает при завершении клика (после отпускания мышки)
void RecordTableView::onClickToRecord(const QModelIndex &index)
{
  // В мобильном режиме, как если засветка уже стоит на строке с записью, должна открыться запись
  // (а в десктопном режиме этого не должно происходить, потому что запись уже видна на экране из-за обрабоки сигнала listSelectionChanged)
  if(mytetraConfig.getInterfaceMode()=="mobile")
  {
    clickToRecord(index);
    return;
  }
}


// Действия при выборе строки таблицы конечных записей. Принимает индекс Proxy модели
void RecordTableView::clickToRecord(const QModelIndex &index)
{
  controller->clickToRecord(index);

  globalParameters.getWindowSwitcher()->switchFromRecordtableToRecord();
}


void RecordTableView::assemblyContextMenu(void)
{
  // Конструирование меню
  contextMenu=new QMenu(this);

  RecordTableScreen *parentPointer=qobject_cast<RecordTableScreen *>(parent());

  contextMenu->addAction(parentPointer->actionAddNewToEnd);
  contextMenu->addAction(parentPointer->actionAddNewBefore);
  contextMenu->addAction(parentPointer->actionAddNewAfter);
  contextMenu->addSeparator();
  contextMenu->addAction(parentPointer->actionEditField);
  contextMenu->addAction(parentPointer->actionBlock);
  contextMenu->addAction(parentPointer->actionDelete);
  contextMenu->addSeparator();
  contextMenu->addAction(parentPointer->actionCut);
  contextMenu->addAction(parentPointer->actionCopy);
  contextMenu->addAction(parentPointer->actionPaste);
  contextMenu->addSeparator();
  contextMenu->addAction(parentPointer->actionSwitchSelectionMode);
  contextMenu->addAction(parentPointer->actionSort);
  contextMenu->addAction(parentPointer->actionPrint);
  contextMenu->addAction(parentPointer->actionCopyRecordReference);
  contextMenu->addAction(parentPointer->actionSettings);
}


// Открытие контекстного меню в таблице конечных записей
void RecordTableView::onCustomContextMenuRequested(const QPoint &mousePos)
{
  qDebug() << "In on_customContextMenuRequested";

  RecordTableScreen *parentPointer=qobject_cast<RecordTableScreen *>(parent());

  // Установка надписи блокировки/разблокировки записи
  QModelIndex selectItem=currentIndex();

  if(!selectItem.isValid())
    parentPointer->actionBlock->setText(tr("Block/Unblock note"));
  else
  {
    if(selectItem.data(RECORD_BLOCK_ROLE).toString()=="1") // Все время забываю, что у объекта QModelIndex есть метод data()
      parentPointer->actionBlock->setText(tr("Unblock note"));
    else
      parentPointer->actionBlock->setText(tr("Block note"));
  }

  // Устанавливается надпись для режима выбора записей
  if(selectionMode()==QAbstractItemView::SingleSelection)
    parentPointer->actionSwitchSelectionMode->setText(tr("Set multiple selection"));
  else if(selectionMode()==QAbstractItemView::ExtendedSelection)
    parentPointer->actionSwitchSelectionMode->setText(tr("Set single selection"));

  // Устанавливается надпись для пункта сортировки
  if( !this->isSortingEnabled() )
    parentPointer->actionSort->setText(tr("Enable sorting"));
  else
    parentPointer->actionSort->setText(tr("Disable sorting"));

  // Запоминается номер колонки, по которой был произведен клик (номер колонки будет правильный, даже если записей мало и клик произошел под записями)
  int n = this->horizontalHeader()->logicalIndexAt(mousePos);
  qDebug() << "Click on column number " << n;
  parentPointer->actionSort->setData( n ); // Запоминается номер колонки в объект действия для сортировки


  // Включение отображения меню на экране
  contextMenu->exec( viewport()->mapToGlobal(mousePos) ); // Меню открывается в позиции клика мышкой
}


// Слот, срабатывающий при нажатии кнопки редактирования записи
void RecordTableView::editFieldContext(void)
{
 qDebug() << "In RecordTableView::editFieldContext";

 // Получение индекса выделенного элемента
 // QModelIndexList selectItems=selectionModel()->selectedIndexes();
 // QModelIndex index=selectItems.at(0);
 QModelIndex index=currentIndex();

 controller->editFieldContext(index);

 // Нужно перерисовать окно редактирования чтобы обновились инфополя
 // делается это путем "повторного" выбора текущего пункта
 clickToRecord(index); // Раньше было select()
}


// Получение номера первого выделенного элемента
int RecordTableView::getFirstSelectionPos(void)
{
 // Получение списка выделенных Item-элементов
 QModelIndexList selectItems=selectionModel()->selectedIndexes();

 if(selectItems.isEmpty())
  return -1; // Если ничего не выделено
 else
  return (selectItems.at(0)).row(); // Номер первого выделенного элемента
}


// Получение ID первого выделенного элемента
QString RecordTableView::getFirstSelectionId(void)
{
  // Получение списка выделенных Item-элементов
  QModelIndexList selectItems=selectionModel()->selectedIndexes();

  if(selectItems.isEmpty())
    return ""; // Если ничего не выделено

  return selectItems.at(0).data(RECORD_ID_ROLE).toString();
}


// Получение модельного индекса первого выделенного элемента в Proxy модели
QModelIndex RecordTableView::getFirstSelectionProxyIndex(void)
{
  int pos=getFirstSelectionPos();

  if(pos==-1)
    return QModelIndex();

  // QModelIndex index = recordProxyModel->index( pos, 0 );
  QModelIndex index = controller->convertPosToProxyIndex(pos);

  return index;
}


// Получение модельного индекса первого выделенного элемента в Source модели
QModelIndex RecordTableView::getFirstSelectionSourceIndex(void)
{
  QModelIndex proxyIndex=getFirstSelectionProxyIndex();

  if(!proxyIndex.isValid())
    return QModelIndex();

  // QModelIndex index = recordProxyModel->mapToSource( proxyIndex );
  QModelIndex index = controller->convertProxyIndexToSourceIndex(proxyIndex);

  return index;
}


bool RecordTableView::isSelectedSetToTop(void)
{
 if(getFirstSelectionPos()==0)return true;
 else return false;
}


bool RecordTableView::isSelectedSetToBottom(void)
{
  if(getFirstSelectionPos()==model()->rowCount()-1)
    return true;
  else
    return false;
}


// Установка засветки в нужную строку на экране
void RecordTableView::setSelectionToPos(int iPos)
{
  QModelIndex index=controller->convertPosToProxyIndex(iPos); // Модельный индекс в Proxy модели
  int pos=index.row();

  // todo: Если это условие ни разу не сработает, значит преобразование ipos - pos надо просто убрать
  if(pos!=iPos)
  {
    QMessageBox msgBox;
    msgBox.setText("In RecordTableView::setSelectionToPos() input pos not equal model pos");
    msgBox.exec();
  }

  int rowCount=controller->getRowCount();
  if(pos>(rowCount-1))
   return;

  // Простой механизм выбора строки. Похоже, что его использовать не получится
  selectRow(pos);

  // Установка засветки на нужный индекс
  // selectionModel()->setCurrentIndex(selIdx, QItemSelectionModel::ClearAndSelect);

  // В мобильной версии реакции на выбор записи нет (не обрабатывается сигнал смены строки в модели выбора)
  // Поэтому по записи должен быть сделан виртуальный клик, чтобы заполнилась таблица конечных записей
  if(mytetraConfig.getInterfaceMode()=="mobile")
    emit this->clicked(index); // QModelIndex selIdx=recordSourceModel->index(pos, 0);

  scrollTo( currentIndex() ); // QAbstractItemView::PositionAtCenter
}


// mode - режим в котором добавлялась новая запись
// pos - позиция новой записи в размерности Source модели
void RecordTableView::moveCursorToNewRecord(int mode, int pos)
{
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
    ( mode==ADD_NEW_RECORD_AFTER && pos>=(model()->rowCount()-1) ) )
    scrollToBottom();

  int proxyPos=controller->convertPosToProxyIndex(pos).row();

  selectRow(proxyPos);
}


void RecordTableView::switchSelectionMode()
{
  if(selectionMode()==QAbstractItemView::SingleSelection)
  {
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    return;
  }

  if(selectionMode()==QAbstractItemView::ExtendedSelection)
  {
    setSelectionMode(QAbstractItemView::SingleSelection);
    return;
  }
}


// Обработчик событий, нужен только для QTapAndHoldGesture (долгое нажатие)
bool RecordTableView::event(QEvent *event)
{
  if (event->type() == QEvent::Gesture)
  {
    qDebug() << "In gesture event(): " << event << " Event type: " << event->type();
    return gestureEvent(static_cast<QGestureEvent*>(event));
  }

  return QTableView::event(event);
}


// Обработчик жестов
// Вызывается из обработчика событий
bool RecordTableView::gestureEvent(QGestureEvent *event)
{
  qDebug() << "In gestureEvent()" << event;

  if (QGesture *gesture = event->gesture(Qt::TapAndHoldGesture))
    tapAndHoldGestureTriggered(static_cast<QTapAndHoldGesture *>(gesture));

  return true;
}


// Обработчик жеста TapAndHoldGesture
// Вызывается из обработчика жестов
void RecordTableView::tapAndHoldGestureTriggered(QTapAndHoldGesture *gesture)
{
  qDebug() << "In tapAndHoldGestureTriggered()" << gesture;

  if(gesture->state()==Qt::GestureFinished)
    if(globalParameters.getTargetOs()=="android")
      emit tapAndHoldGestureFinished( mapFromGlobal(gesture->position().toPoint()) );
}


// Реакция на нажатие кнопок мышки
void RecordTableView::mousePressEvent(QMouseEvent *event)
{
 // Если нажата левая кнопка мыши
 if(event->buttons() == Qt::LeftButton)
  {
   mouseStartPos=event->pos();
  }

 QTableView::mousePressEvent(event);
}


// Реакция на движение мышкой
void RecordTableView::mouseMoveEvent(QMouseEvent *event)
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

 QTableView::mouseMoveEvent(event);
}


// Реакция на отпускание клавиши мышки
void RecordTableView::mouseReleaseEvent(QMouseEvent *event)
{
 QTableView::mouseReleaseEvent(event);
}


// Начало переноса записи
void RecordTableView::startDrag()
{
 qDebug() << "Start record drag\n";

 // Если действительно выбрана строка
 if( currentIndex().isValid() )
  {
   // Перед переносом нужно запомнить текст последней редактируемой записи, чтобы не перенесся неотредактированный вариант
   find_object<MainWindow>("mainwindow")->saveTextarea();

   // Копирование выделенных строк в объект переноса
   QDrag *drag=new QDrag(this);
   drag->setMimeData( getSelectedRecords() );

   // Запуск операции перетаскивания объекта
   unsigned int result=drag->exec(Qt::MoveAction);

   // Если перетаскивание завершено
   if(result==0)
    {
     // Сюда код дойдет после того, как перетаскивание будет закончено
     qDebug() << "Success drag and drop move record";

     // todo: Совершенно непонятно, где удалять объект drag.
     // Если удалять в этом месте, имеем сегфолт
     // delete drag;

     // В модели данных обнуляется оформление элемента, который (возможно) подсвечивался при Drag And Drop
     find_object<TreeScreen>("treeScreen")->knowTreeModel->setData(QModelIndex(), QVariant(false), Qt::UserRole);
    }
  }
}


ClipboardRecords *RecordTableView::getSelectedRecords(void)
{
 // Получение списка Item-элементов, подлежащих копированию
 QModelIndexList itemsForCopy=selectionModel()->selectedIndexes();

 // В списке должны остаться только элементы столбца 0
 // (так как ранее одна строка была одним элементом списка,
 // а теперь используется таблица, и при одной выделенной строке
 // выделено несколько элементов)
 QMutableListIterator<QModelIndex> iterator(itemsForCopy);
 while (iterator.hasNext())
 {
  iterator.next();

  QModelIndex index=iterator.value();
  if(index.column()>0)
    iterator.remove();
 }


 // Список возвращается в произвольном порядке, не в таком как на экране
 // поэтому его нужно отсортировать по QModelIndex
 qSort(itemsForCopy.begin(),itemsForCopy.end());

 qDebug() << "Get selected records";
 for(int i=0; i<itemsForCopy.size(); ++i)
   qDebug() << itemsForCopy.at(i).data().toString();


 // Объект с данными для заполнения буфера обмена
 ClipboardRecords *clipboardRecords=new ClipboardRecords();
 clipboardRecords->clear();

 // Объект заполняется выбранными записями
 controller->addRecordsToClipboard(clipboardRecords, itemsForCopy);

 return clipboardRecords;
}


// Переопределенный сигнал (virtual protected slot)
void RecordTableView::selectionChanged(const QItemSelection &selected,
                                       const QItemSelection &deselected )
{
 // qDebug() << "RecordTableView::selectionChanged()";

 emit listSelectionChanged(selected, deselected);

 // Для корректной работы надо вызвать сигнал базового класса
 QTableView::selectionChanged(selected, deselected);
}


// Слот, срабатывающий после того, как был передвинут горизонтальный заголовок
void RecordTableView::onSectionMoved( int logicalIndex, int oldVisualIndex, int newVisualIndex )
{
  Q_UNUSED(logicalIndex);

  if(!enableMoveSection)
    return;

  // Если была включена сортировка
  /*
  if( this->isSortingEnabled() )
    if( horizontalHeader()->sortIndicatorSection())
      horizontalHeader()->setSortIndicator(n, Qt::AscendingOrder); // Треугольничек сортировки переставляется на нужный столбец
  */

  // Запоминается ширина столбцов
  int oldVisualWidth=horizontalHeader()->sectionSize(oldVisualIndex);
  int newVisualWidth=horizontalHeader()->sectionSize(newVisualIndex);

  // В настройках последовательность полей меняется
  QStringList showFields=mytetraConfig.getRecordTableShowFields();
  showFields.move(oldVisualIndex, newVisualIndex);
  mytetraConfig.setRecordTableShowFields(showFields);

  qDebug() << "New show field sequence" << showFields;

  enableMoveSection=false;

  // Перемещение в данном представлении сбрасывается, так как модель берет последовательность полей из настроек
  // После это кода logicalIindex=visualIndex для всех полей
  for(int logicalIdx=0; logicalIdx<showFields.size(); logicalIdx++)
  {
    int visualIdx=horizontalHeader()->visualIndex( logicalIdx );
    if( visualIdx != logicalIdx )
      horizontalHeader()->moveSection(visualIdx, logicalIdx); // Этот вызов запустит срабатывание этого же слота sectionMoved(), поэтому нужен enableMoveSection
  }

  enableMoveSection=true;

  horizontalHeader()->reset();

  // Устанавливается ширина столбцов после перемещения
  horizontalHeader()->resizeSection(oldVisualIndex, newVisualWidth);
  horizontalHeader()->resizeSection(newVisualIndex, oldVisualWidth);

  saveColumnWidth();
}


void RecordTableView::onSectionResized( int logicalIndex, int oldSize, int newSize )
{
  Q_UNUSED(logicalIndex);
  Q_UNUSED(oldSize);
  Q_UNUSED(newSize);

  saveColumnWidth();
}


// Сохранение ширины колонок в конфигфайл
void RecordTableView::saveColumnWidth(void)
{
  // Выясняется количество полей
  int count=mytetraConfig.getRecordTableShowFields().size();

  QStringList columnWidthList;

  for(int i=0; i<count; i++)
  {
    QString width;
    width.setNum( columnWidth(i) );
    columnWidthList << width;
  }

  mytetraConfig.setRecordTableFieldsWidth( columnWidthList );

  // qDebug() << "Save column width " << columnWidthList;
}


// Восстановление ширины колонок из конфигфайла
void RecordTableView::restoreColumnWidth(void)
{
  QStringList columnWidthList=mytetraConfig.getRecordTableFieldsWidth();

  // qDebug() << "Restore column width " << columnWidthList;

  // Восстанавливается ширина всех колонок без последней
  // Чтобы последняя растягивалась по месту
  for(int i=0; i<columnWidthList.size()-1; i++)
    setColumnWidth( i, columnWidthList[i].toInt() );
}


void RecordTableView::updateRow(int rowNum)
{
  int columnCount=model()->columnCount();

  for(int i=0; i<columnCount; ++i)
  {
    QModelIndex index=model()->index(rowNum, i);
    update(index);
  }
}

