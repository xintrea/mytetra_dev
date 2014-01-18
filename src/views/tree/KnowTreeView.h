#ifndef __KNOWTREEVIEW_H__
#define __KNOWTREEVIEW_H__

#include <QWidget>
#include <QTreeView>
#include <QDragEnterEvent>
#include <QDropEvent>


class KnowTreeView : public QTreeView
{
 Q_OBJECT

public:
 explicit KnowTreeView(QWidget *parent = 0);
 virtual ~KnowTreeView();

signals:

public slots:

protected:
 void dragEnterEvent(QDragEnterEvent *event);
 void dragMoveEvent(QDragMoveEvent *event);
 void dropEvent(QDropEvent *event);
};

#endif // __KNOWTREEVIEW_H__

