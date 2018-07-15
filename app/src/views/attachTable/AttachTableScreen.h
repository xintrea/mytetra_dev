#ifndef ATTACHTABLESCREEN_H
#define ATTACHTABLESCREEN_H

#include <QWidget>
#include <QToolBar>
#include <QVBoxLayout>
#include <QAction>

class AttachTableView;
class AttachTableController;


// Виджет таблицы приаттаченных файлов (с кнопками)

class AttachTableScreen : public QWidget
{
  Q_OBJECT
public:
  AttachTableScreen(QWidget *parent = 0);
  virtual ~AttachTableScreen();

  void clear(void);

  QAction *actionAddAttach; // Добавление локального файла
  QAction *actionAddAttachFromUrl; // Добавление файла по интернет-Url
  QAction *actionAddLink; // Добавление линка на локальный файл
  QAction *actionEditFileName;
  QAction *actionDeleteAttach;
  QAction *actionOpenAttach;
  QAction *actionSaveAsAttach;
  QAction *actionShowAttachInfo;

  void setReadOnly(bool state);

signals:

public slots:

protected:

  void setupActions(void);
  void setupUI(void);
  void setupSignals(void);
  void assembly(void);


  // Указатель на контроллер таблицы приаттаченных файлов
  AttachTableController *attachTableController;

  // Указатель на таблицу приаттаченных файлов (представление)
  AttachTableView *attachTableView;

  // Экранные элементы
  QToolBar *toolsLine;
  QVBoxLayout *screenLayout;

  QAction *actionSwitchToEditor;

};

#endif // ATTACHTABLESCREEN_H
