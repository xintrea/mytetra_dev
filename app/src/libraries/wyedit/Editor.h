#ifndef MYTETRA_EDITOR
#define MYTETRA_EDITOR

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
#include "formatters/TableFormatter.h"
#include "formatters/ImageFormatter.h"
#include "formatters/MathExpressionFormatter.h"
#include "formatters/ReferenceFormatter.h"

// Fix ugly Qt QSS bug
#include "libraries/MtComboBox.h"


// ----------------------------------------------------------
// WyEdit - визуальный редактор для MyTetra
// Волгодонск, 2010 г.
// Контакты: xintrea@gmail.com, www.webhamster.ru
// Данный исходный код распространяется под лицензией GPL v.3
// © Степанов С. М. 2010
// ----------------------------------------------------------


#define WYEDIT_VERSION "WyEdit v.1.10 / 20.01.2016"

class EditorConfig;
class EditorTextEdit;
class EditorContextMenu;
class EditorTextArea;
class EditorIndentSliderAssistant;
class EditorToolBarAssistant;
class Formatter;
class MetaEditor;
class EditorCursorPositionDetector;

class Editor : public QWidget
{
 Q_OBJECT

 friend class MetaEditor;

 friend class Formatter;
 friend class PlacementFormatter;
 friend class TypefaceFormatter;
 friend class ListFormatter;
 friend class TableFormatter;
 friend class ImageFormatter;
 friend class MathExpressionFormatter;
 friend class ReferenceFormatter;

 friend class EditorContextMenu;

public:
 Editor(QWidget *parent=nullptr);
 ~Editor(void);

 // Объект, хранящий настройки редактора
 EditorConfig  *editorConfig=nullptr;

 // Ассистент панели кнопок
 EditorToolBarAssistant *editorToolBarAssistant=nullptr; // todo: Переименовать в toolBarAssistant?

 // Ассистент виджета горизонтальной линейки отступов
 EditorIndentSliderAssistant  *indentSliderAssistant=nullptr;

 // Вертикальная группировалка линеек кнопок и области редактирования
 QVBoxLayout   *buttonsAndEditLayout=nullptr;

 // Контекстное меню редактора
 EditorContextMenu *editorContextMenu=nullptr;

 EditorCursorPositionDetector *cursorPositionDetector=nullptr;

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

 void setDirFileEmptyReaction(int mode);
 int  getDirFileEmptyReaction(void);

 int  getCursorPosition(void);
 void setCursorPosition(int n);

 int  getScrollBarPosition(void);
 void setScrollBarPosition(int n);

 QString smartFontFamily(QString fontName); // Умное преобразование имени шрифта
 int smartFontSize(int fontSize); // Умное преобразование размера шрифта

 void switchAttachIconExists(bool isExists);

 enum
 {
  SAVE_IMAGES_SIMPLE=0,       // Простое сохранение картинок, встречающихся в тексте
  SAVE_IMAGES_REMOVE_UNUSED=1 // Сохранение картинок, встречающихся в тексте, с удалением из каталога записи тех, которых в тексте нет
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

 void send_expand_edit_area(bool flag);

 void wyeditFindInBaseClicked();

 void updateIndentsliderToActualFormat();
 void updateIndentSliderGeometry();

 void updateAlignButtonHiglight(bool);

 void changeFontselectOnDisplay(QString fontName);
 void changeFontsizeOnDisplay(int n);

private slots:

 void onShowhtmlClicked(void);
 void onFindtextClicked(void);
 void onSettingsClicked(void);
 void onShowformattingClicked(bool);

 void onExpandEditAreaClicked(void);
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
 void onPasteAsPlainText(void);
 void onSelectAll(void);

 void onFindtextSignalDetect(const QString &text, QTextDocument::FindFlags flags);

 // Открытие контекстного меню
 void onCustomContextMenuRequested(const QPoint &pos);

 // void onModificationChanged(bool flag);

private:

 void setupSignals(void);
 void setupToolsSignals(void);
 void setupEditorToolBarAssistant(int mode, EditorTextArea *textArea, QStringList disableToolList);
 void setupIndentSliderAssistant(void);
 void setupEditorTextArea(void);
 void setupCursorPositionDetector(void);
 void setupFormatters(void);
 void assembly(void);

 // Устанавка размера табуляции для клавиши Tab
 void setTabSize();

 // Переопределение событий обработки клавиш
 // нужны для определения момента undo/redo
 virtual void keyPressEvent(QKeyEvent *event);
 virtual void keyReleaseEvent(QKeyEvent *event);

 // Область редактирования текста
 EditorTextArea *textArea=nullptr;

 // Форматировщики текста
 TypefaceFormatter       *typefaceFormatter=nullptr;
 PlacementFormatter      *placementFormatter=nullptr;
 ListFormatter           *listFormatter=nullptr;
 TableFormatter          *tableFormatter=nullptr;
 ImageFormatter          *imageFormatter=nullptr;
 MathExpressionFormatter *mathExpressionFormatter=nullptr;
 ReferenceFormatter      *referenceFormatter=nullptr;

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

 EditorFindDialog *findDialog; // Виджет поиска

 bool expandEditAreaFlag; // Распахнуто ли на максимум окно редактора

 // Указатели на переопределенные функции записи и чтения редактируемого текста
 void (*saveCallbackFunc)(QObject *editor, QString saveString)=nullptr;
 void (*loadCallbackFunc)(QObject *editor, QString &loadString)=nullptr;

 // Указатель на функцию переключения на предыдущее окно (для мобильного интерфейса)
 void (*backCallbackFunc)(void)=nullptr;

 // Указатель на функцию открытия присоединенных файлов
 void (*attachCallbackFunc)(void)=nullptr;

 // Поля для хранения произвольных данных
 // Обычно используются для запоминания нестандартного набора данных
 // в объекте редактора, и считываются из функции обратного вызова
 QMap<QString, QString> miscFields;

 int dirFileEmptyReaction;
};

#endif /* MYTETRA_EDITOR */
