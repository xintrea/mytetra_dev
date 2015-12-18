#ifndef EDITORTOOLBARASSISTANT_H
#define EDITORTOOLBARASSISTANT_H

#include <QObject>

class Editor;
class EditorToolBar;
class EditorTextArea;

class EditorToolBarAssistant : public QObject
{
  Q_OBJECT
public:
  explicit EditorToolBarAssistant(QObject *parent, int iViewMode, QStringList iDisableToolList);
  EditorToolBarAssistant();

  EditorToolBar *getToolBar(void);

  void setFontselectOnDisplay(QString fontName);
  void setFontsizeOnDisplay(int n);

  // Метод, переключающий состояние видимости развернутой/свернутой панели инструментов
  // Если вызывается без параметра, метод сам переключает
  // Параметр 1 - включить полную видимость
  // Параметр -1 - выключить полную видимость
  void switchExpandToolsLines(int flag=0);

  void updateToActualFormat(void);
  void updateOutlineButtonHiglight(void);
  void setOutlineButtonHiglight(int button, bool active);
  void setShowFormattingButtonHiglight(bool active);
  bool isKeyForToolLineUpdate(QKeyEvent *event);

  void switchAttachIconExists(bool isExists);

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

protected:

  Editor *editor;
  EditorToolBar *toolBar;

  int viewMode;

  QString currentFontFamily;
  int     currentFontSize;
  QString currentFontColor;
  QColor  buttonsSelectColor; // Цвет рамки выделенных кнопок

  void setupSignals();
};

#endif // EDITORTOOLBARASSISTANT_H
