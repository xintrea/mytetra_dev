#ifndef _EDITOR_H_
#define _EDITOR_H_

#include <QBoxLayout>
#include <QToolButton>
#include <QFontComboBox>
#include <QSpinBox>
#include <QTextEdit>
#include <QWidget>
#include <QLabel>
#include <QTextCodec>
#include <QToolBar>
#include <QSlider>
#include <QStringList>

#include "EditorToolBar.h"
#include "EditorFindDialog.h"
#include "EditorShowText.h"
#include "formatters/Formatter.h"
#include "formatters/PlacementFormatter.h"
#include "formatters/TypefaceFormatter.h"
#include "formatters/ListFormatter.h"

// Fix ugly Qt QSS bug
#include "libraries/MtComboBox.h"


// ----------------------------------------------------------
// WyEdit - визуальный редактор для MyTetra
// Волгодонск, 2010 г.
// Контакты: xintrea@gmail.com, www.webhamster.ru
// Данный исходный код распространяется под лицензией GPL v.3
// © Степанов С. М. 2010
// ----------------------------------------------------------


#define WYEDIT_VERSION "WyEdit v.1.7 / 21.06.2015"

class EditorConfig;
class EditorTextEdit;
class EditorContextMenu;
class EditorTextArea;
class IndentSlider;
class Formatter;
class MetaEditor;

class Editor : public QWidget
{
 Q_OBJECT

 friend class MetaEditor;

 friend class Formatter;
 friend class PlacementFormatter;
 friend class TypefaceFormatter;
 friend class ListFormatter;

public:
 Editor(QWidget *parent=0);
 ~Editor(void);

 // Объект, хранящий настройки редактора
 EditorConfig  *editorConfig=NULL;

 // Кнопки редактора
 EditorToolBar *editorToolBar=NULL; // todo: Сделать защищенным?

 // Виджет горизонтальной линейки отступов
 IndentSlider  *indentSlider=NULL;

 // Вертикальная группировалка линеек кнопок и области редактирования
 QVBoxLayout   *buttonsAndEditLayout=NULL;

 // Контекстное меню редактора
 EditorContextMenu *editorContextMenu=NULL;

 const char *getVersion(void);

 void initEnableAssembly(bool flag);
 void initConfigFileName(QString name);
 void initEnableRandomSeed(bool flag);
 void initDisableToolList(QStringList toolNames);
 void init(int mode);

 // Методы работы с textarea
 void setTextarea(QString text);
 void setTextareaEditable(bool editable);
 QString getTextarea(void);
 QTextDocument *getTextareaDocument(void);
 void setTextareaModified(bool modify);
 bool getTextareaModified(void);

 // Абсолютный или относительный путь (т.е. директория),
 // куда будет сохраняться текст. Без завершающего слеша
 bool setWorkDirectory(QString dirName);
 QString getWorkDirectory(void);
 
 // Имя файла, куда должен сохраняться текст
 // Без пути, только имя
 void setFileName(QString fileName);
 QString getFileName(void);

 void saveTextarea();
 bool saveTextareaText();
 bool saveTextareaImages(int mode);
 bool loadTextarea();

 // Методы установки нестандартных процедур чтения и сохранения текста
 void setSaveCallback(void (*func)(QObject *editor, QString saveString));
 void setLoadCallback(void (*func)(QObject *editor, QString &loadString));

 // Метод установки функции переключения на предыдущее окно (для мобильного интерфейса)
 void setBackCallback(void (*func)(void));

 // Метод установки функции нажатия на кнопку Attach
 void setAttachCallback(void (*func)(void));

 // Методы установки и чтения произвольных нестандартных данных 
 // которые может хранить объект редактора
 void setMiscField(QString name, QString value);
 QString getMiscField(QString name);
 void clearAllMiscField(void);

 void updateIndentlineGeometry();

 void setDirFileEmptyReaction(int mode);
 int  getDirFileEmptyReaction(void);

 int  getCursorPosition(void);
 void setCursorPosition(int n);

 int  getScrollBarPosition(void);
 void setScrollBarPosition(int n);

 void switchAttachIconExists(bool isExists);

 enum
 {
  SAVE_IMAGES_SIMPLE=0,
  SAVE_IMAGES_REMOVE_UNUSED=1
 };

 enum
  {
   DIRFILEEMPTY_REACTION_SHOW_ERROR,
   DIRFILEEMPTY_REACTION_SUPPRESS_ERROR
  };

 enum
 {
  WYEDIT_DESKTOP_MODE=0,
  WYEDIT_MOBILE_MODE=1
 };

signals:

 // Сигналы установки отступов на линейке с движками
 // согласно текущему форматированию
 void send_set_textindent_pos(int i);
 void send_set_leftindent_pos(int i);
 void send_set_rightindent_pos(int i);

 void send_expand_edit_area(bool flag);

 void wyeditFindInBaseClicked();

public slots:

 void onUpdateIndentlineGeometrySlot(void);

private slots:

 void setFontselectOnDisplay(QString fontName);
 void setFontsizeOnDisplay(int n);

 void onShowhtmlClicked(void);
 void onFindtextClicked(void);
 void onSettingsClicked(void);
 void onShowformattingClicked(void);

 void onCreatetableClicked(void);
 void onTableRemoveRowClicked(void);
 void onTableRemoveColClicked(void);
 void onTableAddRowClicked(void);
 void onTableAddColClicked(void);
 void onTableMergeCellsClicked(void);
 void onTableSplitCellClicked(void);

 void onInsertImageFromFileClicked(void);
 void onExpandEditAreaClicked(void);
 void onExpandToolsLinesClicked(void);
 void onSaveClicked(void);
 void onBackClicked(void);
 void onFindInBaseClicked(void);
 void onShowTextClicked(void);
 void onToAttachClicked(void);

 void onCursorPositionChanged(void); // Слот, контролирущий перемещение курсора
 void onSelectionChanged(void);
 void onUndo(void);
 void onRedo(void);
 void onCut(void);
 void onCopy(void);
 void onPaste(void);
 void onSelectAll(void);

 void onFindtextSignalDetect(const QString &text, QTextDocument::FindFlags flags);

 // Слоты обработки перемещения движков настройки отступов
 void onIndentlineChangeTextindentPos(int i);
 void onIndentlineChangeLeftindentPos(int i);
 void onIndentlineChangeRightindentPos(int i);
 void onIndentlineMouseRelease(void);

 // Открытие контекстного меню
 void onCustomContextMenuRequested(const QPoint &pos);

 void onContextMenuEditImageProperties(void);

 // void onModificationChanged(bool flag);

private:

 // Область редактирования текста
 EditorTextArea *textArea;

 // Форматировщики текста
 TypefaceFormatter  *typefaceFormatter;
 PlacementFormatter *placementFormatter;
 ListFormatter      *listFormatter;

 bool isInit;

 bool        initDataEnableAssembly;
 QString     initDataConfigFileName;
 bool        initDataEnableRandomSeed;
 QStringList initDataDisableToolList;

 // Рабочая директория редактора и файл текста
 // Используется при сохранении текста на диск
 QString workDirectory;
 QString workFileName;

 int viewMode; // Режим отображения редактора - WYEDIT_DESKTOP_MODE или WYEDIT_MOBILE_MODE

 void setupSignals(void);
 void setupEditorToolBar(void);
 void setupIndentSlider(void);
 void setupEditorArea(void);
 void setupFormatters(void);
 void assembly(void);

 bool isBlockSelect(void);
 bool isCursorOnEmptyLine(void);
 bool isCursorOnSpaceLine(void);
 bool isImageSelect(void);
 bool isCursorOnImage(void);

 QTextImageFormat imageFormatOnSelect(void);
 QTextImageFormat imageFormatOnCursor(void);

 void editImageProperties(void);

 void updateToolLineToActualFormat(void);
 void updateIndentsliderToActualFormat(void);
 void updateAlignButtonHiglight(bool activate);
 void updateOutlineButtonHiglight(void);
 void setOutlineButtonHiglight(int button, bool active);
 bool isKeyForToolLineUpdate(QKeyEvent *event);

 // Метод, переключающий состояние видимости полной панели инструментов
 // Если вызывается без параметра, метод сам переключает
 // Параметр 1 - включить полную видимость
 // Параметр -1 - выключить полную видимость
 void switchExpandToolsLines(int flag=0);

 // Переопределяется слот обработки клавиш
 // нужен для определения момента undo/redo
 virtual void keyPressEvent(QKeyEvent * event);
 virtual void keyReleaseEvent(QKeyEvent * event);

 QString currentFontFamily;
 int     currentFontSize;
 QString currentFontColor;
 bool    flagSetFontParametersEnabled;
 int     currentTextIndent;
 int     currentLeftIndent;
 int     currentRightIndent;

 EditorFindDialog *findDialog; // Виджет поиска

 QColor buttonsSelectColor; // Цвет рамки выделенных кнопок

 bool expand_edit_area_flag; // Распахнуто ли на максимум окно редактора

 // Указатели на переопределенные функции записи и чтения редактируемого текста
 void (*save_callback_func)(QObject *editor, QString saveString);
 void (*load_callback_func)(QObject *editor, QString &loadString);

 // Указатель на функцию переключения на предыдущее окно (для мобильного интерфейса)
 void (*back_callback_func)(void);

 // Указатель на функцию открытия присоединенных файлов
 void (*attach_callback_func)(void);

 // Поля для хранения произвольных данных
 // Обычно используются для запоминания нестандартного набора данных
 // в объекте редактора, и считываются из функции обратного вызова
 QMap<QString, QString> miscFields;

 int dirFileEmptyReaction;

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
};

#endif /* _EDITOR_H_ */
