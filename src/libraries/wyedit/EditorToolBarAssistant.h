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
                                  EditorTextArea *iTextArea,
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

  bool getFlagSetFontParametersEnabled();

  int getFontSizeByNum(int n);

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

  void updateIndentSliderGeometry(void);
  void changeIndentSliderVisible(bool);


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

  Editor *editor; // Указатель на объект редактора, с которым работает эта панель кнопок
  EditorTextArea *textArea; // Указатель на объект области редактирования, с которой работает эта панель кнопок

  int viewMode;

  QString currentFontFamily;
  int     currentFontSize;
  QString currentFontColor;

  bool flagSetFontParametersEnabled; // Флаг разрешения/запрета срабатывания слотов установки параметров шрифта

  void setupSignals();
};

#endif // EDITORTOOLBARASSISTANT_H
