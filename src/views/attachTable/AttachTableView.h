#ifndef __ATTACHTABLEVIEW_H__
#define __ATTACHTABLEVIEW_H__

#include <QObject>
#include <QWidget>
#include <QTableView>


// Отображение таблицы с файлами (только таблица)

class AttachTableView : public QTableView
{
  Q_OBJECT

public:
  AttachTableView(QWidget *parent=0);
  virtual ~AttachTableView();

protected:
  AttachTableController *controller;

};

#endif // __ATTACHTABLEVIEW_H__
