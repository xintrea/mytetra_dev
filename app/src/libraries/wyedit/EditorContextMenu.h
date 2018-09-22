#ifndef _EDITORCONTEXTMENU_H_
#define _EDITORCONTEXTMENU_H_

#include <QMenu>

class EditorContextMenu : public QMenu
{
 Q_OBJECT

 public:
  EditorContextMenu(QWidget *parent=nullptr);
  ~EditorContextMenu(void);

 void setImageProperties(bool flag); // Активирование добавочного пункта меню "Свойства картинки")
 void setEditMathExpression(bool flag); // Активирование добавочного пункта меню "Редактирование формулы")
 void setGotoReference(bool flag); // Активирование добавочного пункта меню "Перейти по ссылке")
 void setPasteAsPlainText(bool flag);

 signals:
  void undo(void);
  void redo(void);
  void cut(void);
  void copy(void);
  void paste(void);
  void pasteAsPlainText(void);
  void selectAll(void);
  void contextMenuEditImageProperties(void);
  void contextMenuEditMathExpression(void);
  void contextMenuGotoReference(void);

 private:
  QAction *actionUndo;
  QAction *actionRedo;
  QAction *actionCut;
  QAction *actionCopy;
  QAction *actionPaste;
  QAction *actionPasteAsPlainText;
  QAction *actionSelectAll;
  QAction *actionEditImageProperties;
  QAction *actionEditMathExpression;
  QAction *actionGotoReference;

  void setup_actions(void);
  void setup_signals(void);
  void setup_menu(void);
};

#endif // _EDITORCONTEXTMENU_H_
