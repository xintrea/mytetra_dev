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

#include "EditorFindDialog.h"

// Fix ugly Qt QSS bug
#include "libraries/MtComboBox.h"


// ----------------------------------------------------------
// WyEdit - встраиваемый визуальный редактор
// Волгодонск, 2010 г.
// Контакты: xintrea@gmail.com, www.webhamster.ru
// Данный исходный код распространяется под лицензией GPL v.3
// © Степанов С. М. 2010
// ----------------------------------------------------------

// 1.0 - Первая рабочая версия
//     - Данная версия использовалась в MyTetra 1.25

// 1.1 - Основана на версии 1.0
//     - Добавлена кнопка распахивания окна редактора и кнопка
//       сворачивания/разворачивания панели инструментов
//     - Доработан режим форматирования в код
//     - Усилена очистка при нажатии кнопки [C]
//     - Исправлено несколько проблемм, онаруженных пользователями
//     - Данная версия использовалась в MyTetra 1.28

// 1.2 - Основана на версии 1.1
//     - Переделан прототип конструктора
//     - Переделан механизм инициализации объекта редактора,
//       после создания объекта, необходимо установить начальные свойства
//       с помощью методов
//       initEnableAssembly()
//       initConfigFileName()
//       initEnableRandomSeed()
//       и затем вызвать метод init()
//     - Добавлен метод getVersion()

// 1.3 - Основана на версии 1.2
//     - Сделаны методы для установки функций обратного вызова в редакторе,
//       они нужны для замены стандартной работы с файлами
//       set_save_callback()
//       set_load_callback()
//     - В редакторе сделано хранение произвольных полей данных,
//       через них функции обратного вызова могут узнавать, в каких
//       режимах им надо работать. Для этого добавлены методы
//       setMiscField()
//       getMiscField()

// 1.4 - Основана на версии 1.3
//     - Добавлен метод setDisableToolList(), в котором можно указать имена
//       инструментов, которые не нужно подгружать на панели
//       инструментов редактора

// 1.5 - Основана на версии 1.4
//     - Добавлен слот onModificationChanged(), чтобы правильно
//       устанавливался флаг изменений после использования кнопки
//       сохранения.
//       Его использование закомментировано, так как выяснено, 
//       что флаг правильно устанавливается в документе объекта
//       QTextEdit автоматически


#define WYEDIT_VERSION "WyEdit v.1.5 / 07.07.2011"


#define MINIMUM_ALLOWED_FONT_SIZE 5
#define MAXIMUM_ALLOWED_FONT_SIZE 100

class EditorConfig;
class EditorTextEdit;
class EditorContextMenu;
class EditorTextArea;
class IndentSlider;

class Editor : public QWidget
{
 Q_OBJECT

public:
 Editor(QWidget *parent=0);
 ~Editor(void);

 EditorConfig  *editorConfig;

 // Кнопки форматирования текста
 QToolButton   *bold;
 QToolButton   *italic;
 QToolButton   *underline;
 QToolButton   *monospace;
 QToolButton   *code;
 QToolButton   *clear;

 QToolButton   *numericList;
 QToolButton   *dotList;
 
 QToolButton   *indentPlus;
 QToolButton   *indentMinus;
 
 QToolButton   *alignLeft;
 QToolButton   *alignCenter;
 QToolButton   *alignRight;
 QToolButton   *alignWidth;

 QToolButton   *settings;
 
 QFontComboBox *fontSelect;
 MtComboBox     *fontSize;
 QToolButton   *fontColor;
 
 QToolButton   *showHtml;
 QLabel        *infoArea;

 QToolButton   *findText;

 QToolButton   *showFormatting;

 QToolButton   *createTable;
 QToolButton   *tableRemoveRow;
 QToolButton   *tableRemoveCol;
 QToolButton   *tableAddRow;
 QToolButton   *tableAddCol;
 QToolButton   *tableMergeCells;
 QToolButton   *tableSplitCell;

 QToolButton   *insertImageFromFile;
 QToolButton   *expandEditArea;
 QToolButton   *expandToolsLines;
 QToolButton   *save;

 IndentSlider  *indentSlider;

 // Горизонтальная линейка, содержащая кнопки форматирования
 QVBoxLayout *textformatButtonsLayout;
 QToolBar    *toolsLine1;
 QToolBar    *toolsLine2;
 void insert_button_to_tools_line(QString toolName, QToolBar *line);

 // Вертикальная группировалка линеек кнопок и области редактирования
 QVBoxLayout   *buttonsAndEditLayout;

 // Контекстное меню редактора
 EditorContextMenu *editorContextMenu;

 const char *getVersion(void);

 void initEnableAssembly(bool flag);
 void initConfigFileName(QString name);
 void initEnableRandomSeed(bool flag);
 void init(void);

 // Методы работы с textarea
 void set_textarea(QString text);
 void set_textarea_editable(bool editable);
 QString get_textarea(void);
 QTextDocument *get_textarea_document(void);
 void set_textarea_modified(bool modify);
 bool get_textarea_modified(void);

 // Абсолютный или относительный путь (т.е. директория),
 // куда будет сохраняться текст. Без завершающего слеша
 bool set_work_directory(QString dirName);
 QString get_work_directory(void);
 
 // Имя файла, куда должен сохраняться текст
 // Без пути, только имя
 void set_file_name(QString fileName);
 QString get_file_name(void);

 void save_textarea();
 bool save_textarea_text();
 bool save_textarea_images(int mode);
 bool load_textarea();

 // Методы установки нестандартных процедур чтения и сохранения текста
 void set_save_callback(void (*func)(QObject *editor, QString saveString));
 void set_load_callback(void (*func)(QObject *editor, QString &loadString));

 // Методы установки и чтения произвольных нестандартных данных 
 // которые может хранить объект редактора
 void setMiscField(QString name, QString value);
 QString getMiscField(QString name);

 void update_indentline_geometry();

 void setDirFileEmptyReaction(int mode);
 int  getDirFileEmptyReaction(void);

 // Метод позволяющий управлять доступностью инструментов редактирования
 void setDisableToolList(QStringList toolNames);

 int  getCursorPosition(void);
 void setCursorPosition(int n);

 int  getScrollBarPosition(void);
 void setScrollBarPosition(int n);

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

signals:

 // Сигналы установки отступов на линейке с движками
 // согласно текущему форматированию
 void send_set_textindent_pos(int i);
 void send_set_leftindent_pos(int i);
 void send_set_rightindent_pos(int i);

 void send_expand_edit_area(bool flag);

private slots:

 // Действия в области редактирования
 void on_bold_clicked(void);
 void on_italic_clicked(void);
 void on_underline_clicked(void);
 void on_monospace_clicked(void);
 void on_code_clicked(void);
 void on_clear_clicked(void);

 void on_numericlist_clicked(void);
 void on_dotlist_clicked(void);
 void on_indentplus_clicked(void);
 void on_indentminus_clicked(void);
 
 void on_alignleft_clicked(void);
 void on_aligncenter_clicked(void);
 void on_alignright_clicked(void);
 void on_alignwidth_clicked(void);
  
 void on_fontselect_changed(const QFont &font);
 void on_fontsize_changed(int i);
 void on_fontcolor_clicked();

 void set_fontselect_on_display(QString fontName);
 void set_fontsize_on_display(int n);

 void on_showhtml_clicked(void);
 void on_findtext_clicked(void);
 void on_settings_clicked(void);
 void on_showformatting_clicked(void);

 void on_createtable_clicked(void);
 void on_table_remove_row_clicked(void);
 void on_table_remove_col_clicked(void);
 void on_table_add_row_clicked(void);
 void on_table_add_col_clicked(void);
 void on_table_merge_cells_clicked(void);
 void on_table_split_cell_clicked(void);

 void on_insert_image_from_file_clicked(void);
 void on_expand_edit_area_clicked(void);
 void on_expand_tools_lines_clicked(void);
 void on_save_clicked(void);

 void on_cursor_position_changed(void); // Слот, контролирущий перемещение курсора
 void on_selection_changed(void);
 void on_undo(void);
 void on_redo(void);
 void on_cut(void);
 void on_copy(void);
 void on_paste(void);
 void on_selectAll(void);

 void on_findtext_signal_detect(const QString &text, QTextDocument::FindFlags flags);

 // Слоты обработки перемещения движков настройки отступов
 void on_indentline_change_textindent_pos(int i);
 void on_indentline_change_leftindent_pos(int i);
 void on_indentline_change_rightindent_pos(int i);
 void on_indentline_mouse_release(void);

 // Открытие контекстного меню
 void on_customContextMenuRequested(const QPoint &pos);

 void on_context_menu_edit_image_properties(void);

 // void onModificationChanged(bool flag);

protected:

 // Область редактирования текста
 EditorTextArea *textArea;


private:

 bool    initDataEnableAssembly;
 QString initDataConfigFileName;
 bool    initDataEnableRandomSeed;

 // Рабочая директория редактора и файл текста
 // Используется при сохранении текста на диск
 QString workDirectory;
 QString workFileName;

 void setup_signals(void);
 void setup_buttons(void);
 void setup_editor_area(void);
 void assembly_buttons(void);
 void assembly(void);
 void hide_all_tools_elements(void);
 void format_to_list(QTextListFormat::Style setFormat);
 void align_text(Qt::AlignmentFlag mode);

 bool is_block_select(void);
 bool is_cursor_on_empty_line(void);
 bool is_cursor_on_space_line(void);
 bool is_image_select(void);
 bool is_cursor_on_image(void);

 QTextImageFormat image_format_on_select(void);
 QTextImageFormat image_format_on_cursor(void);

 void edit_image_properties(void);

 void update_tool_line_to_actual_format(void);
 void update_indentslider_to_actual_format(void);
 void update_align_button_higlight(bool activate);
 void update_outline_button_higlight(void);
 void set_outline_button_higlight(int button, bool active);
 bool is_key_for_tool_line_update(QKeyEvent *event);

 // Метод, переключающий состояние видимости полной панели инструментов
 // Если вызывается без параметра, метод сам переключает
 // Параметр 1 - включить полную видимость
 // Параметр -1 - выключить полную видимость
 void switch_expand_tools_lines(int flag=0);

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

 // Поля для хранения произвольных данных
 // Обычно используются для запоминания нестандартного набора данных
 // в объекте редактора, и считываются из функции обратного вызова
 QMap<QString, QString> miscFields;

 int dirFileEmptyReaction;

 // Список инструментов, которые ненужно подгружать
 QStringList disableToolList;

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
