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
#include "views/tree/KnowTreeView.h"
#include "models/recordTable/RecordTableModel.h"
#include "models/recordTable/RecordTableProxyModel.h"
#include "models/appConfig/AppConfig.h"
#include "models/tree/KnowTreeModel.h"
#include "libraries/GlobalParameters.h"
#include "views/mainWindow/MainWindow.h"
#include "libraries/WindowSwitcher.h"
#include "controllers/recordTable/RecordTableController.h"
#include "libraries/ShortcutManager.h"
#include "libraries/helpers/ObjectHelper.h"
#include "libraries/helpers/GestureHelper.h"
#include "libraries/helpers/CssHelper.h"


extern GlobalParameters globalParameters;
extern AppConfig mytetraConfig;
extern ShortcutManager shortcutManager;


// Виджет, отображащий список записей в ветке


RecordTableView::RecordTableView(QWidget *parent) : QTableView(parent)
{
 // Изначально сортировка запрещена (заголовки столбцов не будут иметь треугольнички)
 this->setSortingEnabled(false);

 // Настройка области виджета для кинетической прокрутки
 GestureHelper::setKineticScrollArea( qobject_cast<QAbstractItemView*>(this) );

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

 // Горизонтальные заголовки делаются перемещаемыми
 horizontalHeader()->setSectionsMovable(true);

 // Установка высоты строки с принудительной стилизацией (если это необходимо),
 // так как стилизация через QSS для элементов QTableView полноценно не работает
 // У таблицы есть вертикальные заголовки, для каждой строки, в которых отображается номер строки.
 // При задании высоты вертикального заголовка, высота применяется и для всех ячеек в строке.
 verticalHeader()->setDefaultSectionSize ( verticalHeader()->minimumSectionSize () );
 int height=mytetraConfig.getUglyQssReplaceHeightForTableView();
 if(height!=0)
  verticalHeader()->setDefaultSectionSize( height );
 if(mytetraConfig.getInterfaceMode()=="mobile")
  verticalHeader()->setDefaultSectionSize( static_cast<int>( CssHelper::getCalculateIconSizePx() ) );

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
 connect(this, &RecordTableView::customContextMenuRequested,
         this, &RecordTableView::onCustomContextMenuRequested);

 // Соединение сигнал-слот чтобы показать контекстное меню по долгому нажатию
 connect(this, &RecordTableView::tapAndHoldGestureFinished,
         this, &RecordTableView::onCustomContextMenuRequested);

 // Сигнал чтобы открыть на редактирование параметры записи при двойном клике
 connect(this, &RecordTableView::doubleClicked,
         this, &RecordTableView::editFieldContext);

 // Нажатие на запись (вызывается сразу как только нажалась кнопка, до отпускания)
 connect(this, &RecordTableView::pressed,
         this, &RecordTableView::onPressToRecord);

 // Клик по записи (вызывается после отпускания кнопки)
 connect(this, &RecordTableView::clicked,
         this, &RecordTableView::onClickToRecord);


 RecordTableScreen *parentPointer=qobject_cast<RecordTableScreen *>(parent());

 // Сигналы для обновления панели инструментов при изменении в selectionModel()
 connect(this->selectionModel(), &QItemSelectionModel::currentChanged,
         parentPointer,          &RecordTableScreen::toolsUpdate);
 connect(this->selectionModel(), &QItemSelectionModel::selectionChanged,
         parentPointer,          &RecordTableScreen::toolsUpdate);

 // Сигналы для обновления панели инструментов
 connect(this,          &RecordTableView::activated,
         parentPointer, &RecordTableScreen::toolsUpdate);
 connect(this,          &RecordTableView::clicked,
         parentPointer, &RecordTableScreen::toolsUpdate);
 connect(this,          &RecordTableView::doubleClicked,
         parentPointer, &RecordTableScreen::toolsUpdate);
 connect(this,          &RecordTableView::entered,
         parentPointer, &RecordTableScreen::toolsUpdate);
 connect(this,          &RecordTableView::pressed,
         parentPointer, &RecordTableScreen::toolsUpdate);
 connect(QApplication::clipboard(), &QClipboard::dataChanged,
         parentPointer,             &RecordTableScreen::toolsUpdate);

 connect(this->horizontalHeader(), &QHeaderView::sectionMoved,
         this,                     &RecordTableView::onSectionMoved);
 connect(this->horizontalHeader(), &QHeaderView::sectionResized,
         this,                     &RecordTableView::onSectionResized);

 connect(find_object<KnowTreeView>("knowTreeView"), &KnowTreeView::dropEventHandleCatch,
         this, &RecordTableView::onDropEventHandleCatch);
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


// Реакия на сдвиг засветки клавишами или мышкой
// Сигнал может приходить как просто при смене выбора записи,
// так и при движении мышкой в момент, когда есть кандидат на перемещение
// но само перемещение еще не началось, и при этом указатель мышки сместился
// на соседнюю запись
void RecordTableView::onSelectionChanged(const QItemSelection &selected,
                                         const QItemSelection &deselected )
{
    QModelIndex selectRecord;
    QModelIndex deselectRecord;

    if(!selected.indexes().isEmpty())
        selectRecord=selected.indexes().at(0);

    if(!deselected.indexes().isEmpty())
        deselectRecord=deselected.indexes().at(0);

    // Если есть кандидат на перетаскивание записи (само перетаскивание еще не активно)
    // но мышка с него "уехала" на другую запись, и Qt пытается снять
    // выбор с кандидата на перетаскивание
    if(startDragIndex.isValid() and startDragIndex==deselectRecord)
    {
        qDebug() << "Disable select new record if drag candidate is set";
        return; // Делать програмный клик нельзя, чтобы не сбить перетаскивание, если оно начнется
    }

    // Если идет процесс перетаскивания
    if(isDragHappeningNow)
    {
        qDebug() << "Disable select new record if drag in move";
        return; // Делать програмный клик нельзя, чтобы не сбить перетаскивание, если оно начнется
    }

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
    // Устранение неправильного поведения Qt
    if(isDragHappeningNow)
    {
        qDebug() << "Disable click to record if drag moving";
        return;
    }

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
    ShortcutManager::stringRepresentation mode=ShortcutManager::stringRepresentation::brackets;
    if(selectItem.data(RECORD_BLOCK_ROLE).toString()=="1") // Все время забываю, что у объекта QModelIndex есть метод data()
      parentPointer->actionBlock->setText(tr("Unblock note")+" "+shortcutManager.getKeySequenceAsText("note-block", mode));
    else
      parentPointer->actionBlock->setText(tr("Block note")+" "+shortcutManager.getKeySequenceAsText("note-block", mode));
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
  if(mode==GlobalParameters::AddNewRecordBehavior::ADD_TO_END ||
    ( mode==GlobalParameters::AddNewRecordBehavior::ADD_AFTER && pos>=(model()->rowCount()-1) ) )
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
// Событие приходит до того, как даже произойдет выделение строки под курсором мышки
void RecordTableView::mousePressEvent(QMouseEvent *event)
{
    // Если нажата левая кнопка мыши
    if(event->buttons() == Qt::LeftButton)
    {
        // Запоминаются координаты курсора
        mouseStartPos=event->pos();

        // Запоминается запись-кандидат на перенос
        // Определение индекса записи нельзя делать через currentIndex(),
        // т. к. событие приходит раньше выделения записи под курсором
        // Определять индекс нужно именно по координатам курсора
        if( this->indexAt( event->pos() ).isValid() )
        {
            startDragIndex=this->indexAt( event->pos() );
            qDebug() << "Candidate to drag: " << startDragIndex.data().toString()
                     << " Row: " << startDragIndex.row()
                     << " ID: " << startDragIndex.data(RECORD_ID_ROLE).toString();
        }
        else
        {
            startDragIndex=QModelIndex(); // Иначе запоминается невалидный индекс
        }
    }

    // При клике перетаскивание еще не начинается,
    // и кроме того флаг от предыдущего пертаскивания надо очистить
    isDragHappeningNow=false;

    QTableView::mousePressEvent(event);
}


// Реакция на движение мышкой
void RecordTableView::mouseMoveEvent(QMouseEvent *event)
{
    qDebug() << "mouseMoveEvent()";

    // При режиме множественного выбора реакции на движение
    // мышкой быть не должно (так как работает криво), только на клики
    if(selectionMode()==QAbstractItemView::ExtendedSelection and
       this->selectionModel()->selectedRows().size()>1)
    {
        return;
    }

    // Если идет процесс перетаскивания
    if(isDragHappeningNow)
    {
        // При перетаскивании, Qt иногда может выделять соседние записи
        if(startDragIndex.isValid() and startDragIndex!=this->indexAt( event->pos() ))
        {
            qDebug() << "Drag detect, hold start record";

            // Чтобы этого не происходило, выделяется запись, на которой началось перетаскивание
            this->selectRow( startDragIndex.row() );
        }
    }

    // Если при движении нажата левая кнопка мышки
    // и выделена ровно одна строка
    // (при включенном множественном выборе возможны случайные выдения и соседних строк)
    if(event->buttons() & Qt::LeftButton and
       this->selectionModel()->selectedRows().size()==1)
    {
        // Выясняется расстояние от места начала нажатия
        int distance=(event->pos() - mouseStartPos).manhattanLength();

        if(distance >= QApplication::startDragDistance())
        {
            startDrag(); // Начинается перетаскивание
        }
    }

    // При зажатых кнопках нельзя пробрасывать вызов родительского метода
    // так как внутри него метасистема Qt может сгенерировать событие setSelection() и вызван слот
    // selectionChanged() с выбором соседней строки, не той на которой был клик,
    // при быстром движении мышкой
    if( !( (event->buttons() & Qt::LeftButton) or (event->buttons() & Qt::RightButton) ) )
    {
        QTableView::mouseMoveEvent(event);
    }
}


// Реакция на отпускание клавиши мышки
// Данное событие почему-то не генерируется, если нажатие происходило на одной строке,
// а отпускание происходит на другой строке
void RecordTableView::mouseReleaseEvent(QMouseEvent *event)
{
    qDebug() << "Mouse release at RecordTableView...";

    QTableView::mouseReleaseEvent(event);

    qDebug() << "Mouse release at RecordTableView after calling parent event";

    // Если мышка отпускается до того, как завершилось перетаскивание (перетаскивание активно)
    if(isDragHappeningNow)
    {
        // Значит перетаскивания не произошло, и надо выделить строку,
        // на которой начиналось перетаскивание
        if( startDragIndex.isValid() )
        {
            qDebug() << "Candidate drag finish: " << startDragIndex.data().toString();

            this->selectRow( startDragIndex.row() );
        }

        isDragHappeningNow=false;
    }

    // Индекс перетаскиваемой записи очищается в любом случае
    startDragIndex=QModelIndex();
}


// Начало переноса записи
void RecordTableView::startDrag()
{
    if( !startDragIndex.isValid() )
    {
        return;
    }

    // Исправление особенности Qt
    // Выделяется строка, на которой был старт движения DragAndDrop,
    // так как метод startDrag() может сработать, когда выделена строка
    // с соседней записью, которую на самом деле не нужно перемещать
    this->selectRow( startDragIndex.row() );
    isDragHappeningNow=true; // Выставляется флаг что началось перетаскивание

    qDebug() << "Start record drag\n";

    // Перед переносом нужно запомнить текст последней редактируемой записи, чтобы не перенесся неотредактированный вариант
    find_object<MainWindow>("mainwindow")->saveTextarea();

    // Копирование выделенных строк в объект переноса
    QDrag *drag=new QDrag(this);
    drag->setMimeData( getSelectedRecords() );

    // Запуск операции перетаскивания объекта
    unsigned int result=drag->exec(Qt::MoveAction);

    // Если перетаскивание завершено
    // Это неточно, докуметация и М.Шлее не объясняют внятно, когда срабатывает этот код
    if(result==0)
    {
        // Сюда код дойдет после того, как перетаскивание будет закончено
        qDebug() << "May be, success drag and drop move record";

        // todo: Совершенно непонятно, где удалять объект drag.
        // Если удалять в этом месте, имеем сегфолт
        // delete drag;

        // В модели данных обнуляется оформление элемента, который (возможно) подсвечивался при Drag And Drop
        find_object<TreeScreen>("treeScreen")->knowTreeModel->setData(QModelIndex(), QVariant(false), Qt::UserRole);
    }
}


// Слот вызывается из KnowTreeView если произошол Drop
void RecordTableView::onDropEventHandleCatch()
{
    isDragHappeningNow=false;
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
 std::sort(itemsForCopy.begin(),itemsForCopy.end());

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


// Переопределенный слот (virtual protected slot)
void RecordTableView::selectionChanged(const QItemSelection &selected,
                                       const QItemSelection &deselected )
{
    qDebug() << "RecordTableView::selectionChanged()";

    // Отладка
    const auto indices = selected.indexes();
    for(const auto & index : indices)
    {
        qDebug() << "Select row: " << index.row();
    }

    this->onSelectionChanged(selected, deselected);

    // Для корректной работы надо вызвать слот базового класса
    QTableView::selectionChanged(selected, deselected);
}


// Слот, срабатывающий после того, как был передвинут горизонтальный заголовок
void RecordTableView::onSectionMoved( int logicalIndex, int oldVisualIndex, int newVisualIndex )
{
  Q_UNUSED(logicalIndex)

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
  Q_UNUSED(logicalIndex)
  Q_UNUSED(oldSize)
  Q_UNUSED(newSize)

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

