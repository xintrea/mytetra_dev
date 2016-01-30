#ifndef ACTIONLOGSCREEN_H
#define ACTIONLOGSCREEN_H

#include <QWidget>
#include <QVBoxLayout>
#include <QAction>

// Виджет отображения лога действий пользователя

class ActionLogView;
class ActionLogController;
class QDialogButtonBox;

class ActionLogScreen : public QWidget
{
  Q_OBJECT
public:
  ActionLogScreen(QWidget *parent = 0);
  virtual ~ActionLogScreen();

protected:

  void setupUI(void);
  void setupSignals(void);
  void assembly(void);

  // Указатель на контроллер 
  ActionLogController *actionLogController;

  // Указатель на таблицу (представление)
  ActionLogView *actionLogView;

  // Экранные элементы
  QDialogButtonBox *buttonBox;
  QVBoxLayout *screenLayout;
};

#endif // ACTIONLOGSCREEN_H
