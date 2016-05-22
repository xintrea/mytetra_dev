#ifndef EDITORTOOLBAR_H
#define EDITORTOOLBAR_H

#include <QWidget>
#include <QToolButton>
#include <QFontComboBox>
#include <QSpinBox>
#include <QToolBar>
#include <QVBoxLayout>

// Fix ugly Qt QSS bug
#include "libraries/MtComboBox.h"

#define MINIMUM_ALLOWED_FONT_SIZE 5
#define MAXIMUM_ALLOWED_FONT_SIZE 100


class EditorToolBar : public QWidget
{
  Q_OBJECT

public:

  explicit EditorToolBar(QWidget *parent = 0);
  virtual ~EditorToolBar();

  // Область, содержащая линейки с кнопками форматирования
  QVBoxLayout textformatButtonsLayout;

  // Линейки с кнопками форматирования текста
  QToolBar    toolsLine1;
  QToolBar    toolsLine2;

  // Кнопки форматирования текста
  QToolButton   bold;
  QToolButton   italic;
  QToolButton   underline;
  QToolButton   monospace;
  QToolButton   code;
  QToolButton   clear; // Очистка начертания текста
  QToolButton   textOnly; // Полная очистка текста (оставить один текст)
  QToolButton   fixBreakSymbol;

  QToolButton   numericList;
  QToolButton   dotList;

  QToolButton   indentPlus;
  QToolButton   indentMinus;

  QToolButton   alignLeft;
  QToolButton   alignCenter;
  QToolButton   alignRight;
  QToolButton   alignWidth;

  QToolButton   settings;

  QFontComboBox fontSelect;
  MtComboBox    fontSize;
  QToolButton   fontColor;

  QToolButton   reference;

  QToolButton   showHtml;

  QToolButton   findText;

  QToolButton   showFormatting;

  QToolButton   createTable;
  QToolButton   tableRemoveRow;
  QToolButton   tableRemoveCol;
  QToolButton   tableAddRow;
  QToolButton   tableAddCol;
  QToolButton   tableMergeCells;
  QToolButton   tableSplitCell;
  QToolButton   tableProperties;

  QToolButton   insertImageFromFile;
  QToolButton   expandEditArea;
  QToolButton   expandToolsLines;
  QToolButton   save;

  QToolButton   back;
  QToolButton   findInBase;

  QToolButton   showText;

  QToolButton   toAttach;

  QIcon         iconAttachExists; // Иконка, когда аттачи есть
  QIcon         iconAttachNotExists; // Иконка, когда аттачей нет

  void init();

  void initDisableToolList(QStringList toolNames); // Список кнопок, которые нельзя использовать - они будут невидны (не добавлены)
  void setEnableModifyTextButton(bool state); // Установка или снятие доступности кнопок, модифицирующих текст

  void initToolsLine1(QStringList toolsLine);
  void initToolsLine2(QStringList toolsLine);

  void switchAttachIconExists(bool isExists); // Переключение иконки аттачей в режим есть аттачи или нет аттачей

signals:

public slots:


protected:

  bool isInit;

  QStringList toolsListInLine1;
  QStringList toolsListInLine2;

  // Список инструментов, которые ненужно подгружать
  QStringList disableToolList;

  void setupButtons(void);
  void assemblyButtons(void);

  void insertButtonToToolsLine(QString toolName, QToolBar &line);

  QList<QWidget *> getButtonWidgetList(void);
  void hideAllToolsElements(void);

  void updateToolsLines(void);

};

#endif // EDITORTOOLBAR_H
