#ifndef EDITORSHOWTEXTCONTEXTMENU_H
#define EDITORSHOWTEXTCONTEXTMENU_H

#include <QMenu>
#include <QTextEdit>
#include <QSharedPointer>


class EditorShowTextContextMenu : public QMenu
{
 Q_OBJECT

public:
  EditorShowTextContextMenu(QWidget *parent=nullptr);
  ~EditorShowTextContextMenu(void);

 void update(void);


signals:

  void copy(void);
  void selectAll(void);
  void gotoNote(void);

private:

  QAction *actionCopy;
  QAction *actionSelectAll;
  QAction *actionGotoNote;

  void setupActions(void);
  void setupSignals(void);
  void setupMenu(void);

protected slots:

  void onActionCopy(void);
  void onActionSelectAll(void);
  void onActionGotoNote(void);
};

#endif // EDITORSHOWTEXTCONTEXTMENU_H
