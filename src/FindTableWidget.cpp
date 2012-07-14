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

#include "FindTableWidget.h"
#include "main.h"
#include "MainWindow.h"

FindTableWidget::FindTableWidget(QWidget *parent) : QWidget(parent)
{
 setupUI();
 setupSignals();
 assembly();
 
 clearAll();
}


FindTableWidget::~FindTableWidget(void)
{

}

void FindTableWidget::setupUI(void)
{
 findTableView=new QTableWidget();
 findTableView->setObjectName("findtableview");
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


void FindTableWidget::setupSignals(void)
{

 connect(findTableView, SIGNAL(cellActivated(int, int)),
         this, SLOT(selectCell(int, int)));

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
 // Таблица очищается
 findTableView->setRowCount(0);
 findTableView->setColumnCount(0);
 
  // Устанавливается в таблице две колонки Path и Title
 findTableView->setColumnCount(2);
 
 // Устанавливаются заголовки колонок
 QStringList list;
 list << tr("Title") << tr("Details");
 findTableView->setHorizontalHeaderLabels(list);
 
 findTableView->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
 findTableView->horizontalHeader()->setStretchLastSection(true);
}


void FindTableWidget::addRow(QString title, QString branchName, QString tags, QStringList path, int numInRecordtable)
{
 int i=findTableView->rowCount();

 findTableView->insertRow(i);

 // Заголовок записи
 QTableWidgetItem *item_title=new QTableWidgetItem();
 item_title->setText(title);
 
 // В ячейке заголовка также хранится информация о пути к ветке
 // и номере записи в таблице конечных записей
 item_title->setData(Qt::UserRole  ,path);
 item_title->setData(Qt::UserRole+1,numInRecordtable);
 
 // Информация о записи
 QTableWidgetItem *item_info=new QTableWidgetItem();
 QString info;
 tags=tags.trimmed();
 if(tags.length()>0)
  item_info->setText(branchName+" ("+tags+")");
 else 
  item_info->setText(branchName);
 
 findTableView->setItem(i,0,item_title);
 findTableView->setItem(i,1,item_info);
 
 qDebug() << "In findtablewidget add_row() row count " << findTableView->rowCount();
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


void FindTableWidget::selectCell(int row, int column)
{
 Q_UNUSED(column);

 QTableWidgetItem *item=findTableView->item(row,0);
 
 // Выясняется путь к ветке и номер в таблице конечных записей
 QStringList path=item->data(Qt::UserRole).toStringList();
 int n=item->data(Qt::UserRole+1).toInt();
 
 find_object<MainWindow>("mainwindow")->setTreePosition(path);
 find_object<MainWindow>("mainwindow")->setRecordtablePosition(n);
}

