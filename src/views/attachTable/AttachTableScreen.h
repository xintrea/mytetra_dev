#ifndef ATTACHTABLESCREEN_H
#define ATTACHTABLESCREEN_H

#include <QWidget>

class AttachTableView;

class AttachTableScreen : public QWidget
{
  Q_OBJECT
public:
  AttachTableScreen(QWidget *parent = 0);
  virtual ~AttachTableScreen();

signals:

public slots:

protected:
  QToolBar *toolsLine;

  QVBoxLayout *screenLayout;

  QAction *actionAdd;
  QAction *actionEdit;
  QAction *actionDelete;

  // Таблица приаттаченных файлов
  AttachTableView *attachTableView;
};

#endif // ATTACHTABLESCREEN_H
