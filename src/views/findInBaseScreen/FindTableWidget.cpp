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
#include <QStyledItemDelegate>

#include "FindTableWidget.h"
#include "main.h"
#include "views/mainWindow/MainWindow.h"
#include "views/record/MetaEditor.h"
#include "models/appConfig/AppConfig.h"

#define USER_ROLE_PATH      Qt::UserRole
#define USER_ROLE_RECORD_ID Qt::UserRole+1

extern AppConfig mytetraConfig;


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
  findTableView->setObjectName("findTableView");
  findTableView->setMinimumSize(1,1);
  findTableView->horizontalHeader()->hide();

  // Установка высоты строки с принудительной стилизацией (если это необходимо),
  // так как стилизация через QSS для элементов QTableView полноценно не работает
  // У таблицы есть вертикальные заголовки, для каждой строки, в которых отображается номер строки.
  // При задании высоты вертикального заголовка, высота применяется и для всех ячеек в строке.
  findTableView->verticalHeader()->setDefaultSectionSize ( findTableView->verticalHeader()->minimumSectionSize () );
  int height=mytetraConfig.getUglyQssReplaceHeightForTableView();
  if(height!=0)
    findTableView->verticalHeader()->setDefaultSectionSize( height );
  if(mytetraConfig.getInterfaceMode()=="mobile")
    findTableView->verticalHeader()->setDefaultSectionSize( getCalculateIconSizePx() );

  // Устанавливается режим что могут выделяться только строки
  // а не отдельный item таблицы
  findTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
  
  // Устанавливается режим что редактирование невозможно
  findTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

  // Настройка области виджета для кинетической прокрутки
  setKineticScrollArea( qobject_cast<QAbstractItemView*>(findTableView) );
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


void FindTableWidget::addRow(QString title, QString branchName, QString tags, QStringList path, QString recordId)
{
  int i=findTableModel->rowCount();

  findTableModel->insertRow(i);

  // Принудительная стилизация, так как стилизация через QSS для элементов QTableView полноценно не работает
  // int height=mytetraconfig.getUglyQssReplaceHeightForTableView();
  // if(height!=0)
  //  findTableView->setRowHeight(i, height);

  // Заголовок (название) записи
  QStandardItem *item_title=new QStandardItem();
  item_title->setText(title);

  // В ячейке заголовка также хранится информация о пути к ветке
  // и номере записи в таблице конечных записей
  qDebug() << "Path to record" << path;
  item_title->setData(QVariant(path), USER_ROLE_PATH);
  item_title->setData(QVariant(recordId), USER_ROLE_RECORD_ID);

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


int FindTableWidget::getRowCount()
{
  return findTableModel->rowCount();
}


void FindTableWidget::paintEvent(QPaintEvent *event)
{
  QWidget::paintEvent(event);

  if(overdrawMessage.length()>0)
  {
    QPainter painter(this);
    painter.setPen( QApplication::palette().color(QPalette::ToolTipText) );
    painter.drawText(rect(), Qt::AlignCenter, overdrawMessage);
  }
}


void FindTableWidget::setOverdrawMessage(const QString iOverdrawMessage)
{
  overdrawMessage=iOverdrawMessage;

  if(overdrawMessage.length()>0)
    findTableView->hide(); // Скрывается виджет таблицы, потому что он перекрывает выводимую надпись
  else
    findTableView->show();

  // Обновляется внешний вид виджета
  update();
}


// void FindTableWidget::selectCell(int row, int column)
void FindTableWidget::selectCell(const QModelIndex & index)
{
  QStandardItem *clickItem=findTableModel->itemFromIndex(index);
  QStandardItem *item=findTableModel->item(clickItem->row(), 0); // Данные находятся в самом левом столбце с индексом 0

  // Выясняется путь к ветке и номер в таблице конечных записей
  QStringList path=item->data(USER_ROLE_PATH).toStringList();
  QString recordId=item->data(USER_ROLE_RECORD_ID).toString();

  qDebug() << "Get path to record:" << path;

  // Редактор переводится в режим отображения текста записи (а не приаттаченных файлов)
  MetaEditor *edView=find_object<MetaEditor>("editorScreen");
  edView->switchToEditorLayout();

  find_object<MainWindow>("mainwindow")->setTreePosition(path);
  find_object<MainWindow>("mainwindow")->setRecordtablePositionById(recordId);
}

