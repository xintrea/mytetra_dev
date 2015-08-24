#include <QHeaderView>

#include "AttachTableView.h"
#include "controllers/attachTable/AttachTableController.h"


AttachTableView::AttachTableView(QWidget *parent) : QTableView(parent)
{
  this->horizontalHeader()->setStretchLastSection( true );
  this->setSelectionBehavior(QAbstractItemView::SelectRows); // Выделяется вся строка
}


AttachTableView::~AttachTableView()
{

}


void AttachTableView::init(void)
{
  setupSignals();
}


void AttachTableView::setupSignals(void)
{
  // Сигнал чтобы открыть на просмотр/редактирование файл по двойному клику
  connect(this, SIGNAL(doubleClicked(const QModelIndex &)),
          controller, SLOT(onOpenFile(void)));
}


void AttachTableView::setController(AttachTableController *pController)
{
  controller=pController;
}


void AttachTableView::resizeEvent(QResizeEvent *event)
{
  QRect viewRect = this->rect();
  float columnWidth = (float) viewRect.width() * 0.8;
  this->setColumnWidth( 0, columnWidth );

  // Отрисовка родительского класса
  QTableView::resizeEvent(event);
}


// Получение номера первого выделенного элемента
int AttachTableView::getFirstSelectionPos(void)
{
 // Получение списка выделенных Item-элементов
 QModelIndexList selectItems=selectionModel()->selectedIndexes();

 if(selectItems.isEmpty())
  return -1; // Если ничего не выделено
 else
  return (selectItems.at(0)).row(); // Индекс первого выделенного элемента
}
