#include <QWidget>
#include <QRegion>
#include <QLayout>
#include <QString>
#include <QWidget>
#include <QVariant>
#include <QTableWidget>
#include <QtDebug>
#include <QHeaderView>
#include <QPaintEvent>
#include <QTableView>
#include <QStandardItemModel>
#include <QStandardItem>

#include "FindTableWidget.h"
#include "main.h"
#include "views/mainWindow/MainWindow.h"


FindTableWidget::FindTableWidget(QWidget *parent) : QWidget(parent)
{
 setupUI();
 setupModels();
 setupSignals();
 assembly();
 
 clearAll();
}


FindTableWidget::~FindTableWidget(void)
{

}


void FindTableWidget::setupUI(void)
{
 findTableView=new QTableView(this);
 findTableView->setObjectName("FindTableView");
 findTableView->setMinimumSize(1,1);
 findTableView->horizontalHeader()->hide();
 findTableView->verticalHeader()->setDefaultSectionSize ( findTableView->verticalHeader()->minimumSectionSize () );

 // Устанавливается режим что могут выделяться только строки 
 // а не отдельный item таблицы
 findTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
  
 // Устанавливается режим что редактирование невозможно
 findTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

 // Проверка отрисовки виджета
 // findtableview->viewport()->installEventFilter( this );
}


void FindTableWidget::setupModels(void)
{
 // Создается модель табличных данных
 findTableModel=new QStandardItemModel(this);

 // Модель привязывается к виду
 findTableView->setModel(findTableModel);
}


void FindTableWidget::setupSignals(void)
{

 // Вариант для QTableWidget
 // connect(findTableView, SIGNAL(cellActivated(int, int)), this, SLOT(selectCell(int, int)));


 connect(findTableView, SIGNAL( activated(const QModelIndex &) ), this, SLOT( selectCell(const QModelIndex &) ));

}
 
void FindTableWidget::assembly(void)
{
 QHBoxLayout *central_layout=new QHBoxLayout();
 
 central_layout->addWidget(findTableView);
 central_layout->setContentsMargins(0,0,0,0); // Границы убираются

 this->setLayout(central_layout);
}


void FindTableWidget::updateColumnsWidth(void)
{
 findTableView->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
 findTableView->hide();
 findTableView->show();
}


void FindTableWidget::clearAll(void)
{
 // Модель таблицы очищается
 findTableModel->setRowCount(0);
 findTableModel->setColumnCount(0);
 
  // В модели таблицы устанавливаются две колонки Path и Title
 findTableModel->setColumnCount(2);
 
 // В модели устанавливаются заголовки колонок
 QStringList list;
 list << tr("Title") << tr("Details");
 findTableModel->setHorizontalHeaderLabels(list);
 
 findTableView->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
 findTableView->horizontalHeader()->setStretchLastSection(true);
}


void FindTableWidget::addRow(QString title, QString branchName, QString tags, QStringList path, int numInRecordtable)
{
 int i=findTableModel->rowCount();

 findTableModel->insertRow(i);

 // Заголовок записи
 QStandardItem *item_title=new QStandardItem();
 item_title->setText(title);
 
 // В ячейке заголовка также хранится информация о пути к ветке
 // и номере записи в таблице конечных записей
 qDebug() << "Path to record" << path;
 item_title->setData(QVariant(path), Qt::UserRole);
 item_title->setData(QVariant(numInRecordtable), Qt::UserRole+1);
 
 // Информация о записи
 QStandardItem *item_info=new QStandardItem();
 QString info;
 tags=tags.trimmed();
 if(tags.length()>0)
  item_info->setText(branchName+" ("+tags+")");
 else 
  item_info->setText(branchName);
 
 findTableModel->setItem(i, 0, item_title);
 findTableModel->setItem(i, 1, item_info);
 
 qDebug() << "In findtablewidget add_row() row count " << findTableModel->rowCount();
}


bool FindTableWidget::eventFilter( QObject * o, QEvent * e )
{
  if ( o == findTableView->viewport() && e->type() == QEvent::Paint )
  {
    QPaintEvent * pe = static_cast<QPaintEvent*>( e );
    qDebug() << "Rect to paint:" << pe->rect();
    qDebug() << "Visible rect:" << (findTableView->viewport()->visibleRegion()).boundingRect();
    qDebug() << " ";
    return false;
  }
  return false;
}


// void FindTableWidget::selectCell(int row, int column)
void FindTableWidget::selectCell(const QModelIndex & index)
{
 QStandardItem *clickItem=findTableModel->itemFromIndex(index);
 QStandardItem *item=findTableModel->item(clickItem->row(), 0); // Данные находятся в самом левом столбце с индексом 0
 
 // Выясняется путь к ветке и номер в таблице конечных записей
 QStringList path=item->data(Qt::UserRole).toStringList();
 int n=item->data(Qt::UserRole+1).toInt();

 qDebug() << "Get path to record:" << path;
 
 find_object<MainWindow>("mainwindow")->setTreePosition(path);
 find_object<MainWindow>("mainwindow")->setRecordtablePosition(n);
}

