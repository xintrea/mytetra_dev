#ifndef RECORDLISTSCREEN_H_
#define RECORDLISTSCREEN_H_

#include <QWidget>
#include <QListView>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolBar>
#include <QStringList>
#include <QItemSelection>


class RecordListScreen : public QListView
{
 Q_OBJECT

public:
 RecordListScreen(QWidget *parent=0);
 virtual ~RecordListScreen();

signals:

 void listSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

protected slots:

 // Слот, который автоматически срабатыват при изменении selection в списке
 // Этот слот нигде не надо прописывать через connect(), так как он
 // является переопределенным, так как его тип virtual protected slot
 virtual void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

protected:

 QPoint mouseStartPos;
 bool isStartDrag;
 void mousePressEvent(QMouseEvent *event);
 void mouseMoveEvent(QMouseEvent *event);
 void mouseReleaseEvent(QMouseEvent *event);
 void startDrag();

};

#endif /* RECORDLISTSCREEN_H_ */
