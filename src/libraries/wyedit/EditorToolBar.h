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

  // Кнопки форматирования текста
  QToolButton   *bold=NULL;
  QToolButton   *italic=NULL;
  QToolButton   *underline=NULL;
  QToolButton   *monospace=NULL;
  QToolButton   *code=NULL;
  QToolButton   *clear=NULL;

  QToolButton   *numericList=NULL;
  QToolButton   *dotList=NULL;

  QToolButton   *indentPlus=NULL;
  QToolButton   *indentMinus=NULL;

  QToolButton   *alignLeft=NULL;
  QToolButton   *alignCenter=NULL;
  QToolButton   *alignRight=NULL;
  QToolButton   *alignWidth=NULL;

  QToolButton   *settings=NULL;

  QFontComboBox *fontSelect=NULL;
  MtComboBox    *fontSize=NULL;
  QToolButton   *fontColor=NULL;

  QToolButton   *showHtml=NULL;

  QToolButton   *findText=NULL;

  QToolButton   *showFormatting=NULL;

  QToolButton   *createTable=NULL;
  QToolButton   *tableRemoveRow=NULL;
  QToolButton   *tableRemoveCol=NULL;
  QToolButton   *tableAddRow=NULL;
  QToolButton   *tableAddCol=NULL;
  QToolButton   *tableMergeCells=NULL;
  QToolButton   *tableSplitCell=NULL;

  QToolButton   *insertImageFromFile=NULL;
  QToolButton   *expandEditArea=NULL;
  QToolButton   *expandToolsLines=NULL;
  QToolButton   *save=NULL;

  QToolButton   *back=NULL;
  QToolButton   *findInBase=NULL;

  QToolButton   *showText=NULL;

  QToolButton   *toAttach=NULL;
  QIcon         iconAttachExists; // Иконка, когда аттачи есть
  QIcon         iconAttachNotExists; // Иконка, когда аттачей нет

  // Линейки с кнопками форматирования текста
  QToolBar    *toolsLine1=NULL;
  QToolBar    *toolsLine2=NULL;

  // Область, содержащая линейки с кнопками форматирования
  QVBoxLayout *textformatButtonsLayout=NULL;

  void init();

  void initDisableToolList(QStringList toolNames); // Список кнопок, которые нельзя использовать - они будут невидны (не добавлены)

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

  void insertButtonToToolsLine(QString toolName, QToolBar *line);
  void hideAllToolsElements(void);

  void updateToolsLines(void);

};

#endif // EDITORTOOLBAR_H
