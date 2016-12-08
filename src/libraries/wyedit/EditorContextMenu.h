#ifndef _EDITORCONTEXTMENU_H_
#define _EDITORCONTEXTMENU_H_

#include <QMenu>

class EditorContextMenu : public QMenu
{
 Q_OBJECT

 public:
  EditorContextMenu(QWidget *parent=0);
  ~EditorContextMenu(void);

 void setImageProperties(bool isImageSelect); // Активирование добавочного пункта меню "Свойства картинки")
 void setGotoReference(bool isReference); // Активирование добавочного пункта меню "Перейти по ссылке")


 signals:
  void undo(void);
  void redo(void);
  void cut(void);
  void copy(void);
  void paste(void);
  void selectAll(void);
  void contextMenuEditImageProperties(void);
  void contextMenuGotoReference(void);

 private:
  QAction *actionUndo;
  QAction *actionRedo;
  QAction *actionCut;
  QAction *actionCopy;
  QAction *actionPaste;
  QAction *actionSelectAll;
  QAction *actionEditImageProperties;
  QAction *actionGotoReference;

  void setup_actions(void);
  void setup_signals(void);
  void setup_menu(void);
};

#endif // _EDITORCONTEXTMENU_H_
