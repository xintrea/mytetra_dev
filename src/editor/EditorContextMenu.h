#ifndef _EDITORCONTEXTMENU_H_
#define _EDITORCONTEXTMENU_H_

#include <QMenu>

class EditorContextMenu : public QMenu
{
 Q_OBJECT

 public:
  EditorContextMenu(QWidget *parent=0);
  ~EditorContextMenu(void);

 void set_edit_image_properties(bool is_image_select);

 signals:
  void undo(void);
  void redo(void);
  void cut(void);
  void copy(void);
  void paste(void);
  void selectAll(void);
  void contextMenuEditImageProperties(void);

 private:
  QAction *actionUndo;
  QAction *actionRedo;
  QAction *actionCut;
  QAction *actionCopy;
  QAction *actionPaste;
  QAction *actionSelectAll;
  QAction *actionEditImageProperties;

  void setup_actions(void);
  void setup_signals(void);
  void setup_menu(void);
};

#endif // _EDITORCONTEXTMENU_H_
