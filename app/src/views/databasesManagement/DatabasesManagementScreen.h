#ifndef DATABASESMANAGEMENTSCREEN_H
#define DATABASESMANAGEMENTSCREEN_H

#include <QDialog>
#include <QVBoxLayout>
#include <QAction>
#include <QToolBar>

// Виджет отображения настроек каталогов известных баз данных


class DatabasesManagementTable;
class DatabasesManagementController;
class QDialogButtonBox;

class DatabasesManagementScreen : public QDialog
{
  Q_OBJECT

public:
  DatabasesManagementScreen(QWidget *parent = nullptr);
  virtual ~DatabasesManagementScreen();

  // Действия, используемые как на тулбаре, так и в контекстном меню
  QAction *actionCopy;

public slots:

  void setupShortcuts(void);

protected:

  // Указатель на контроллер 
  DatabasesManagementController *databasesManagementController;

  // Указатель на таблицу баз данных (представление)
  DatabasesManagementTable *databasesManagementTable;

  // Экранные элементы
  QToolBar *toolBar;
  QDialogButtonBox *buttonBox;
  QVBoxLayout *screenLayout;

  void setupUI(void);
  void setupSignals(void);
  void setupActions(void);
  void assembly(void);

};

#endif // DATABASESMANAGEMENTSCREEN_H
