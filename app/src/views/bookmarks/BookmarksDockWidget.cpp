#include <QAction>
#include <QListWidget>
#include <QMenu>
#include <QMessageBox>
#include <QSize>
#include <QToolBar>
#include <QVBoxLayout>

#include "BookmarksDockWidget.h"

#include "controllers/recordTable/RecordTableController.h"
#include "libraries/GlobalParameters.h"
#include "libraries/helpers/ObjectHelper.h"
#include "models/recordTable/Record.h"
#include "models/tree/KnowTreeModel.h"
#include "models/tree/TreeItem.h"
#include "views/mainWindow/MainWindow.h"
#include "views/tree/TreeScreen.h"

extern GlobalParameters globalParameters;

BookmarksDockWidget::BookmarksDockWidget(QWidget *parent)
  : QDockWidget(tr("Bookmarks"), parent)
{
  setObjectName("bookmarksDockWidget");

  QWidget *contentWidget=new QWidget(this);
  QVBoxLayout *layout=new QVBoxLayout(contentWidget);
  layout->setContentsMargins(0, 0, 0, 0);

  QToolBar *toolBar=new QToolBar(contentWidget);
  toolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
  toolBar->setIconSize(QSize(24, 24));
  layout->addWidget(toolBar);

  listWidget=new QListWidget(contentWidget);
  listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
  layout->addWidget(listWidget);
  setWidget(contentWidget);

  moveUpAction=new QAction(tr("Move up"), this);
  moveUpAction->setIcon(QIcon(":/resource/pic/move_up.svg"));
  moveUpAction->setToolTip(tr("Move up"));
  moveDownAction=new QAction(tr("Move down"), this);
  moveDownAction->setIcon(QIcon(":/resource/pic/move_dn.svg"));
  moveDownAction->setToolTip(tr("Move down"));

  removeBookmarkAction=new QAction(tr("Remove from bookmarks"), this);
  removeBookmarkAction->setIcon(
      QIcon(":/resource/icons/BlueSphere/color_bluesphere_button_cancel.svg"));
  removeBookmarkAction->setToolTip(tr("Remove from bookmarks"));

  toolBar->addAction(moveUpAction);
  toolBar->addAction(moveDownAction);
  toolBar->addAction(removeBookmarkAction);

  connect(listWidget, &QListWidget::itemDoubleClicked,
          this, &BookmarksDockWidget::openBookmark);
  connect(listWidget, &QListWidget::currentRowChanged,
          this, &BookmarksDockWidget::updateActions);
  connect(listWidget, &QListWidget::customContextMenuRequested,
          this, &BookmarksDockWidget::showContextMenu);
  connect(moveUpAction, &QAction::triggered,
          this, &BookmarksDockWidget::moveSelectedBookmarkUp);
  connect(moveDownAction, &QAction::triggered,
          this, &BookmarksDockWidget::moveSelectedBookmarkDown);
  connect(removeBookmarkAction, &QAction::triggered,
          this, &BookmarksDockWidget::removeSelectedBookmark);

  updateActions();
}


void BookmarksDockWidget::refresh()
{
  const QString selectedRecordId=
      bookmarkData(listWidget->currentItem()).value("recordId").toString();
  listWidget->clear();

  TreeScreen *treeScreen=find_object<TreeScreen>("treeScreen");
  if(treeScreen==nullptr || treeScreen->knowTreeModel==nullptr)
    return;

  const QList<BookmarkedRecord> bookmarks=treeScreen->knowTreeModel->getBookmarkedRecords();
  for(const BookmarkedRecord &bookmark : bookmarks)
  {
    Record *record=treeScreen->knowTreeModel->getRecord(bookmark.recordId);
    if(record==nullptr)
      continue;

    QString title;
    if(record->getField("crypt")=="1" && globalParameters.getCryptKey().isEmpty())
      title=tr("Encrypted note");
    else
      title=record->getField("name");

    if(title.isEmpty())
      title=tr("Untitled note");

    QListWidgetItem *item=new QListWidgetItem(
          QIcon(":/resource/icons/bookmark_active.svg"), title, listWidget);

    QVariantMap ids;
    ids.insert("recordId", bookmark.recordId);
    ids.insert("parentNodeId", bookmark.parentNodeId);
    item->setData(Qt::UserRole, ids);

    if(bookmark.recordId==selectedRecordId)
      listWidget->setCurrentItem(item);
  }

  updateActions();
}


void BookmarksDockWidget::openBookmark(QListWidgetItem *item)
{
  const QVariantMap ids=bookmarkData(item);
  const QString recordId=ids.value("recordId").toString();
  const QString parentNodeId=ids.value("parentNodeId").toString();

  TreeScreen *treeScreen=find_object<TreeScreen>("treeScreen");
  MainWindow *mainWindow=find_object<MainWindow>("mainwindow");
  if(treeScreen==nullptr || mainWindow==nullptr || treeScreen->knowTreeModel==nullptr)
    return;

  TreeItem *parentItem=treeScreen->knowTreeModel->getItemById(parentNodeId);
  if(parentItem==nullptr || !treeScreen->knowTreeModel->isRecordIdExists(recordId))
  {
    refresh();
    return;
  }

  mainWindow->setTreeAndRecordtablePositions(parentItem->getPath(), recordId);
}


void BookmarksDockWidget::removeSelectedBookmark()
{
  const QVariantMap ids=bookmarkData(listWidget->currentItem());
  const QString recordId=ids.value("recordId").toString();
  if(recordId.isEmpty())
    return;

  RecordTableController *controller=find_object<RecordTableController>("recordTableController");
  if(controller==nullptr)
    return;

  const BookmarkChangeResult result=controller->setBookmark(recordId, false);
  if(result!=BookmarkChangeResult::Success)
    QMessageBox::warning(this, tr("Bookmarks"), tr("The bookmark could not be removed."));
}


void BookmarksDockWidget::moveSelectedBookmarkUp()
{
  moveSelectedBookmark(-1);
}


void BookmarksDockWidget::moveSelectedBookmarkDown()
{
  moveSelectedBookmark(1);
}


void BookmarksDockWidget::moveSelectedBookmark(int direction)
{
  const QString recordId=
      bookmarkData(listWidget->currentItem()).value("recordId").toString();
  if(recordId.isEmpty())
    return;

  RecordTableController *controller=find_object<RecordTableController>("recordTableController");
  if(controller==nullptr)
    return;

  const BookmarkChangeResult result=controller->moveBookmark(recordId, direction);
  if(result!=BookmarkChangeResult::Success)
    QMessageBox::warning(this, tr("Bookmarks"), tr("The bookmark could not be moved."));
}


void BookmarksDockWidget::showContextMenu(const QPoint &pos)
{
  QListWidgetItem *item=listWidget->itemAt(pos);
  if(item==nullptr)
    return;

  listWidget->setCurrentItem(item);

  QMenu menu(this);
  menu.addAction(moveUpAction);
  menu.addAction(moveDownAction);
  menu.addSeparator();
  menu.addAction(removeBookmarkAction);
  menu.exec(listWidget->viewport()->mapToGlobal(pos));
}


void BookmarksDockWidget::updateActions()
{
  const int currentRow=listWidget->currentRow();
  const int count=listWidget->count();
  const bool hasSelection=currentRow>=0 && currentRow<count;

  moveUpAction->setEnabled(hasSelection && currentRow>0);
  moveDownAction->setEnabled(hasSelection && currentRow<count-1);
  removeBookmarkAction->setEnabled(hasSelection);
}


QVariantMap BookmarksDockWidget::bookmarkData(QListWidgetItem *item) const
{
  if(item==nullptr)
    return QVariantMap();

  return item->data(Qt::UserRole).toMap();
}
