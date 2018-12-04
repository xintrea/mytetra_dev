#ifndef _EDITORCONTEXTMENU_H_
#define _EDITORCONTEXTMENU_H_

#include <QMenu>

class EditorContextMenu : public QMenu
{
 Q_OBJECT

 public:
  EditorContextMenu(QWidget *parent=nullptr);
  ~EditorContextMenu(void);

 void update(void);

 QList<QAction *> getActionsList(void);

 void setImageProperties(bool flag); // Активирование добавочного пункта меню "Свойства картинки")
 void setEditMathExpression(bool flag); // Активирование добавочного пункта меню "Редактирование формулы")
 void setGotoReference(bool flag); // Активирование добавочного пункта меню "Перейти по ссылке")
 void setPasteAsPlainText(bool flag);
 void setFormatToLowerCase(bool flag); // Активирование добавочного пункта меню "Строчные"
 void setFormatToUpperCase(bool flag); // Активирование добавочного пункта меню "ПРОПИСНЫЕ"

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

  void lowercase(void);
  void uppercase(void);

 private:
  QAction *actionUndo;
  QAction *actionRedo;
  QAction *actionCut;
  QAction *actionCopy;
  QAction *actionPaste;
  QAction *actionPasteAsPlainText;
  QAction *actionSelectAll;
  QAction *actionLowercase; // строчный
  QAction *actionUppercase; // ПРОПИСНЫЕ

  QAction *actionEditImageProperties;
  QAction *actionEditMathExpression;
  QAction *actionGotoReference;

  void setupActions(void);
  void setupShortcuts(void);
  void setupSignals(void);
  void setupMenu(void);

protected slots:

  void onActionUndo(void);
  void onActionRedo(void);
  void onActionCut(void);
  void onActionCopy(void);
  void onActionPaste(void);
  void onActionPasteAsPlainText(void);
  void onActionSelectAll(void);
  void onActionContextMenuEditImageProperties(void);
  void onActionContextMenuEditMathExpression(void);
  void onActionContextMenuGotoReference(void);

  void onActionLowercase();
  void onActionUppercase();

};

#endif // _EDITORCONTEXTMENU_H_
