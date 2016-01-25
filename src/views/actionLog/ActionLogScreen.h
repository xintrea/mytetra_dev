#ifndef ACTIONLOGSCREEN_H
#define ACTIONLOGSCREEN_H

#include <QWidget>
#include <QToolBar>
#include <QVBoxLayout>
#include <QAction>

class ActionLogView;
class ActionLogController;

// Виджет

class ActionLogScreen : public QWidget
{
  Q_OBJECT
public:
  ActionLogScreen(QWidget *parent = 0);
  virtual ~ActionLogScreen();

  void clear(void);

  QAction *actionCancel;

signals:

public slots:

protected:

  void setupActions(void);
  void setupUI(void);
  void setupSignals(void);
  void assembly(void);

  // Указатель на контроллер 
  ActionLogController *ActionLogController;

  // Указатель на таблицу (представление)
  ActionLogView *ActionLogView;

  // Экранные элементы
  QToolBar *toolsLine;
  QVBoxLayout *screenLayout;
};

#endif // ACTIONLOGSCREEN_H
