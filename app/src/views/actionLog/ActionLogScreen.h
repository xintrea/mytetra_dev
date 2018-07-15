#ifndef ACTIONLOGSCREEN_H
#define ACTIONLOGSCREEN_H

#include <QDialog>
#include <QVBoxLayout>
#include <QAction>
#include <QToolBar>

// Виджет отображения лога действий пользователя

class ActionLogView;
class ActionLogController;
class QDialogButtonBox;

class ActionLogScreen : public QDialog
{
  Q_OBJECT
public:
  ActionLogScreen(QWidget *parent = 0);
  virtual ~ActionLogScreen();

  // Действия, используемые как на тулбаре, так и в контекстном меню
  QAction *actionCopy;

protected:

  // Указатель на контроллер 
  ActionLogController *actionLogController;

  // Указатель на таблицу (представление)
  ActionLogView *actionLogView;

  // Экранные элементы
  QToolBar *toolBar;
  QDialogButtonBox *buttonBox;
  QVBoxLayout *screenLayout;

  void setupUI(void);
  void setupSignals(void);
  void setupActions(void);
  void assembly(void);

};

#endif // ACTIONLOGSCREEN_H
