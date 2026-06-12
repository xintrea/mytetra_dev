#ifndef BOOKMARKSDOCKWIDGET_H
#define BOOKMARKSDOCKWIDGET_H

#include <QDockWidget>
#include <QVariantMap>

class QAction;
class QListWidget;
class QListWidgetItem;
class QPoint;

class BookmarksDockWidget : public QDockWidget
{
  Q_OBJECT

public:
  explicit BookmarksDockWidget(QWidget *parent=nullptr);

public slots:
  void refresh();

private slots:
  void openBookmark(QListWidgetItem *item);
  void removeSelectedBookmark();
  void moveSelectedBookmarkUp();
  void moveSelectedBookmarkDown();
  void showContextMenu(const QPoint &pos);
  void updateActions();

private:
  QVariantMap bookmarkData(QListWidgetItem *item) const;
  void moveSelectedBookmark(int direction);

  QListWidget *listWidget;
  QAction *moveUpAction;
  QAction *moveDownAction;
  QAction *removeBookmarkAction;
};

#endif
