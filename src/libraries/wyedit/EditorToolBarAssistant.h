#ifndef EDITORTOOLBARASSISTANT_H
#define EDITORTOOLBARASSISTANT_H

#include <QWidget>
#include "EditorToolBar.h"

class Editor;
class EditorTextArea;

class EditorToolBarAssistant : public EditorToolBar
{
  Q_OBJECT

public:
  explicit EditorToolBarAssistant(QWidget *parent,
                                  int iViewMode,
                                  EditorTextArea *textArea,
                                  QStringList iDisableToolList);
  ~EditorToolBarAssistant();

  // Метод, переключающий состояние видимости развернутой/свернутой панели инструментов
  // Если вызывается без параметра, метод сам переключает
  // Параметр 1 - включить полную видимость
  // Параметр -1 - выключить полную видимость
  void switchExpandToolsLines(int flag=0);

  void updateToActualFormat(void);
  void setOutlineButtonHiglight(int button, bool active);
  void setShowFormattingButtonHiglight(bool active);
  bool isKeyForToolLineUpdate(QKeyEvent *event);

  enum
   {
    BT_BOLD,
    BT_ITALIC,
    BT_UNDERLINE,
    BT_ALIGN_LEFT,
    BT_ALIGN_CENTER,
    BT_ALIGN_RIGHT,
    BT_ALIGN_WIDTH
   };

signals:

public slots:

  void onExpandToolsLinesClicked(void);
  void onUpdateAlignButtonHiglight(bool activate);
  void onUpdateOutlineButtonHiglight(void);
  void onChangeFontselectOnDisplay(QString fontName);
  void onChangeFontsizeOnDisplay(int n);
  void onChangeFontFamily(QString fontFamily);
  void onChangeFontPointSize(int n);
  void onChangeFontcolor(QColor color);

protected:

  Editor *editor;
  EditorTextArea *textArea;

  int viewMode;

  QString currentFontFamily;
  int     currentFontSize;
  QString currentFontColor;
  QColor  buttonsSelectColor; // Цвет рамки выделенных кнопок

  void setupSignals();
};

#endif // EDITORTOOLBARASSISTANT_H
