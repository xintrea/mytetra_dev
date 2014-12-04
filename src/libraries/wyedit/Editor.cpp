#include <QtGui>
#include <QLayout>
#include <QMessageBox>
#include <QColor>
#include <QColorDialog>
#include <QStyle>
#include <QPalette>
#include <QStringList>
#include <QMenu>
#include <QObject>
#include <QtGlobal>
#include <QDateTime>
#include <QTextCursor>
#include <QDebug>
#include <QInputDialog>
#include <QFileDialog>
#include <QScrollBar>

#include "Editor.h"
#include "EditorConfig.h"
#include "EditorConfigDialog.h"
#include "EditorContextMenu.h"
#include "EditorTextArea.h"
#include "EditorSplitCellForm.h"
#include "indentslider/IndentSlider.h"
#include "EditorAbsTable.h"
#include "EditorAddTableForm.h"
#include "EditorImageProperties.h"
#include "EditorMultiLineInputDialog.h"

#include "../../main.h"

Editor::Editor(QWidget *parent) : QWidget(parent)
{
 initDataEnableAssembly=true;
 initDataConfigFileName="";
 initDataEnableRandomSeed=false;

 dirFileEmptyReaction=DIRFILEEMPTY_REACTION_SHOW_ERROR;

 save_callback_func=NULL;
 load_callback_func=NULL;
 back_callback_func=NULL;
}


Editor::~Editor(void)
{

}


const char *Editor::getVersion(void)
{
 static const char *lib_version=WYEDIT_VERSION;
 return(lib_version);
}


void Editor::initEnableAssembly(bool flag)
{
 initDataEnableAssembly=flag;
}


void Editor::initConfigFileName(QString name)
{
 initDataConfigFileName=name;
}


void Editor::initEnableRandomSeed(bool flag)
{
 initDataEnableRandomSeed=flag;
}


// Инициализация редактора
// Если mode=WYEDIT_DESKTOP_MODE - происходит обычная инициализация
// Если mode=WYEDIT_MOBILE_MODE - при инициализации в первую строку панели инструментов, слева, добавляется кнопка back
void Editor::init(int mode)
{
 viewMode=mode;

 // Создается объект поддержки конфигурирования редактора
 editorConfig=new EditorConfig(initDataConfigFileName, this);
 editorConfig->setObjectName("editorconfig");

 // Выясняется перечень кнопок на панели инструментов
 toolsListInLine1=(editorConfig->get_tools_line_1()).split(",");
 toolsListInLine2=(editorConfig->get_tools_line_2()).split(",");

 // В мобильном режиме добавляется кнопка back (если ее нет)
 if(viewMode==WYEDIT_MOBILE_MODE && !toolsListInLine1.contains("back"))
 {
   toolsListInLine1.prepend("separator");
   toolsListInLine1.prepend("back");
 }

 // В мобильном режиме добавляется кнопка find_in_base (если ее нет)
 if(viewMode==WYEDIT_MOBILE_MODE && !toolsListInLine1.contains("find_in_base"))
 {
   toolsListInLine1.append("separator");
   toolsListInLine1.append("find_in_base");
 }

 // Создается виджет поиска, обязательно нужно указать parent чтобы
 // могли применяться флаги окна.
 // Виджет будет постоянно включен параллельно с работой редактора.
 // Только будет либо виден, либо невиден.
 findDialog=new EditorFindDialog(this);
 findDialog->hide();

 // Создаётся контекстное меню
 editorContextMenu=new EditorContextMenu(this);

 setup_buttons();
 setup_editor_area();

 setup_signals();

 assembly_buttons();

 if(initDataEnableAssembly) assembly();

 currentFontFamily="";
 currentFontSize=0;
 currentFontColor="#000000";
 flagSetFontParametersEnabled=true;
 currentTextIndent=0;
 currentLeftIndent=0;
 currentRightIndent=0;
 buttonsSelectColor=QColor(125,170,240,150); // 92,134,198

 // Вначале редактор находится в обычном нераспахнутом состоянии
 expand_edit_area_flag=false;

 update_indentline_geometry();

 // Устанавливается состояние распахнуто или нет панель инструментов
 if(editorConfig->get_expand_tools_lines())
  switch_expand_tools_lines(1);
 else
  switch_expand_tools_lines(-1);

 if(initDataEnableRandomSeed)
  {
   QDateTime datetime=QDateTime::currentDateTime ();
   int seed=datetime.toTime_t();
   // qDebug() << "Random generator init " << seed;
   srand(seed);
   // qDebug() << "Random generator " << rand();
  }
}


void Editor::setup_signals(void)
{
 // Создание сигналов, генерируемых кнопками форматирования текста
 connect(bold,SIGNAL(clicked()),        this,SLOT(on_bold_clicked()));
 connect(italic,SIGNAL(clicked()),      this,SLOT(on_italic_clicked()));
 connect(underline,SIGNAL(clicked()),   this,SLOT(on_underline_clicked()));
 connect(monospace,SIGNAL(clicked()),   this,SLOT(on_monospace_clicked()));
 connect(code,SIGNAL(clicked()),        this,SLOT(on_code_clicked()));
 connect(clear,SIGNAL(clicked()),       this,SLOT(on_clear_clicked()));

 connect(numericList,SIGNAL(clicked()), this,SLOT(on_numericlist_clicked()));
 connect(dotList,SIGNAL(clicked()),     this,SLOT(on_dotlist_clicked()));
 
 connect(indentPlus,SIGNAL(clicked()),  this,SLOT(on_indentplus_clicked()));
 connect(indentMinus,SIGNAL(clicked()), this,SLOT(on_indentminus_clicked()));
 
 connect(alignLeft,SIGNAL(clicked()),   this,SLOT(on_alignleft_clicked()));
 connect(alignCenter,SIGNAL(clicked()), this,SLOT(on_aligncenter_clicked()));
 connect(alignRight,SIGNAL(clicked()),  this,SLOT(on_alignright_clicked()));
 connect(alignWidth,SIGNAL(clicked()),  this,SLOT(on_alignwidth_clicked()));
 
 connect(fontSelect,SIGNAL(currentFontChanged(const QFont &)), this,SLOT(on_fontselect_changed(const QFont &)));
 connect(fontSize,SIGNAL(currentIndexChanged(int)),            this,SLOT(on_fontsize_changed(int)));
 connect(fontColor,SIGNAL(clicked()),                          this,SLOT(on_fontcolor_clicked()));

 connect(showHtml,SIGNAL(clicked()),this,SLOT(on_showhtml_clicked()));
 connect(findText,SIGNAL(clicked()),this,SLOT(on_findtext_clicked()));
 connect(settings,SIGNAL(clicked()),this,SLOT(on_settings_clicked()));
 connect(showFormatting,SIGNAL(clicked()),this,SLOT(on_showformatting_clicked()));

 // Работа с таблицами
 connect(createTable,SIGNAL(clicked()),    this,SLOT(on_createtable_clicked()));
 connect(tableRemoveRow,SIGNAL(clicked()), this,SLOT(on_table_remove_row_clicked()));
 connect(tableRemoveCol,SIGNAL(clicked()), this,SLOT(on_table_remove_col_clicked()));
 connect(tableAddRow,SIGNAL(clicked()),    this,SLOT(on_table_add_row_clicked()));
 connect(tableAddCol,SIGNAL(clicked()),    this,SLOT(on_table_add_col_clicked()));
 connect(tableMergeCells,SIGNAL(clicked()),this,SLOT(on_table_merge_cells_clicked()));
 connect(tableSplitCell,SIGNAL(clicked()), this,SLOT(on_table_split_cell_clicked()));

 connect(insertImageFromFile, SIGNAL(clicked()), this, SLOT(on_insert_image_from_file_clicked()));
 connect(expandEditArea, SIGNAL(clicked()), this, SLOT(on_expand_edit_area_clicked()));
 connect(expandToolsLines, SIGNAL(clicked()), this, SLOT(on_expand_tools_lines_clicked()));
 connect(save, SIGNAL(clicked()), this, SLOT(on_save_clicked()));
 connect(back, SIGNAL(clicked()), this, SLOT(on_back_clicked()));
 connect(findInBase, SIGNAL(clicked()), this, SLOT(on_find_in_base_clicked()));

 connect(textArea,SIGNAL(cursorPositionChanged()), this,SLOT(on_cursor_position_changed()));
 connect(textArea,SIGNAL(selectionChanged()),      this,SLOT(on_selection_changed()));

 connect(indentSlider,SIGNAL(change_textindent_pos(int)), this,SLOT(on_indentline_change_textindent_pos(int)));
 connect(indentSlider,SIGNAL(change_leftindent_pos(int)), this,SLOT(on_indentline_change_leftindent_pos(int)));
 connect(indentSlider,SIGNAL(change_rightindent_pos(int)),this,SLOT(on_indentline_change_rightindent_pos(int)));
 connect(indentSlider,SIGNAL(mouse_release()),            this,SLOT(on_indentline_mouse_release()));

 connect(this,SIGNAL(send_set_textindent_pos(int)), indentSlider,SLOT(set_textindent_pos(int)));
 connect(this,SIGNAL(send_set_leftindent_pos(int)), indentSlider,SLOT(set_leftindent_pos(int)));
 connect(this,SIGNAL(send_set_rightindent_pos(int)),indentSlider,SLOT(set_rightindent_pos(int)));

 // Соединение сигнал-слот чтобы показать контекстное меню по правому клику в редакторе
 connect(textArea, SIGNAL(customContextMenuRequested(const QPoint &)),
         this, SLOT(on_customContextMenuRequested(const QPoint &)));

 // Соединение сигнал-слот чтобы показать контекстное меню по долгому нажатию
 connect(textArea, SIGNAL(tapAndHoldGestureFinished(const QPoint &)),
         this, SLOT(on_customContextMenuRequested(const QPoint &)));

 // connect(textArea->document(), SIGNAL(modificationChanged (bool )),
 //         this, SLOT(onModificationChanged(bool)));


 // Сигналы контекстного меню
 connect(editorContextMenu,SIGNAL(undo()),this,SLOT(on_undo()));
 connect(editorContextMenu,SIGNAL(redo()),this,SLOT(on_redo()));
 connect(editorContextMenu,SIGNAL(cut()),this,SLOT(on_cut()));
 connect(editorContextMenu,SIGNAL(copy()),this,SLOT(on_copy()));
 connect(editorContextMenu,SIGNAL(paste()),this,SLOT(on_paste()));
 connect(editorContextMenu,SIGNAL(selectAll()),this,SLOT(on_selectAll()));
 connect(editorContextMenu,SIGNAL(contextMenuEditImageProperties()),this,SLOT(on_context_menu_edit_image_properties()));

 // Вызов диалога поиска в тексте
 connect(findDialog, SIGNAL(find_text(const QString &, QTextDocument::FindFlags)),
         this, SLOT(on_findtext_signal_detect(const QString &, QTextDocument::FindFlags)) );
}


// Создание объектов кнопок для линейки форматирования текста
void Editor::setup_buttons(void)
{
 // Кнопка Bold
 bold = new QToolButton(this);
 bold->setShortcut(QKeySequence(QKeySequence::Bold));
 bold->setStatusTip(tr("Bold (Ctrl+B)"));
 bold->setIcon(QIcon(":/resource/pic/edit_bold.svg"));
 bold->setObjectName("editor_tb_bold");

 // Кнопка Italic
 italic = new QToolButton(this);
 italic->setShortcut(QKeySequence(QKeySequence::Italic));
 italic->setStatusTip(tr("Italic (Ctrl+I)"));
 italic->setIcon(QIcon(":/resource/pic/edit_italic.svg"));
 italic->setObjectName("editor_tb_italic");

 // Кнопка Underline
 underline = new QToolButton(this);
 underline->setShortcut(QKeySequence(QKeySequence::Underline));
 underline->setStatusTip(tr("Underline (Ctrl+U)"));
 underline->setIcon(QIcon(":/resource/pic/edit_underline.svg"));
 underline->setObjectName("editor_tb_underline");

 // Кнопка Monospace
 monospace = new QToolButton(this);
 monospace->setShortcut(QKeySequence(tr("Ctrl+T")));
 monospace->setStatusTip(tr("Monospace (Ctrl+T)"));
 monospace->setIcon(QIcon(":/resource/pic/edit_monospace.svg"));
 monospace->setObjectName("editor_tb_monospace");

 // Кнопка Code
 code = new QToolButton(this);
 code->setShortcut(QKeySequence(tr("Ctrl+M")));
 code->setStatusTip(tr("Code (Ctrl+M). Select a whole paragraphs to format text as code."));
 code->setIcon(QIcon(":/resource/pic/edit_code.svg"));
 code->setObjectName("editor_tb_code");

 // Кнопка Clear
 clear = new QToolButton(this);
 clear->setShortcut(QKeySequence(tr("Ctrl+K")));
 clear->setStatusTip(tr("Reset format (Ctrl+K). When selected whole paragraph both text and paragraph format is reset to default or just text format in other case."));
 clear->setIcon(QIcon(":/resource/pic/edit_clear.svg"));
 clear->setObjectName("editor_tb_clear");


 // Кнопка нумерованного списка
 numericList = new QToolButton(this);
 numericList->setStatusTip(tr("Numeric list"));
 numericList->setIcon(QIcon(":/resource/pic/edit_listnumeric.svg"));
 numericList->setObjectName("editor_tb_numericlist");

 // Кнопка списка с точками
 dotList = new QToolButton(this);
 dotList->setStatusTip(tr("Marked list"));
 dotList->setIcon(QIcon(":/resource/pic/edit_listdot.svg"));
 dotList->setObjectName("editor_tb_dotlist");

 
 // Кнопка увеличения отступа
 indentPlus = new QToolButton(this);
 indentPlus->setStatusTip(tr("Increase indent"));
 indentPlus->setIcon(QIcon(":/resource/pic/edit_indentplus.svg"));
 indentPlus->setObjectName("editor_tb_indentplus");

 // Кнопка уменьшения отступа
 indentMinus = new QToolButton(this);
 indentMinus->setStatusTip(tr("Decrease indent"));
 indentMinus->setIcon(QIcon(":/resource/pic/edit_indentminus.svg"));
 indentMinus->setObjectName("editor_tb_indentminus");

 
 // Кнопка выравнивания по левому краю
 alignLeft = new QToolButton(this);
 alignLeft->setShortcut(QKeySequence(tr("Ctrl+L")));
 alignLeft->setStatusTip(tr("Align left (Ctrl+L)"));
 alignLeft->setIcon(QIcon(":/resource/pic/edit_alignleft.svg"));
 alignLeft->setObjectName("editor_tb_alignleft");
 
 // Кнопка выравнивания по центру
 alignCenter = new QToolButton(this);
 alignCenter->setShortcut(QKeySequence(tr("Ctrl+E")));
 alignCenter->setStatusTip(tr("Align center (Ctrl+E)"));
 alignCenter->setIcon(QIcon(":/resource/pic/edit_aligncenter.svg"));
 alignCenter->setObjectName("editor_tb_aligncenter");

 // Кнопка выравнивания по правому краю
 alignRight = new QToolButton(this);
 alignRight->setShortcut(QKeySequence(tr("Ctrl+R")));
 alignRight->setStatusTip(tr("Align right (Ctrl+R)"));
 alignRight->setIcon(QIcon(":/resource/pic/edit_alignright.svg"));
 alignRight->setObjectName("editor_tb_alignright");

 // Кнопка выравнивания по ширине
 alignWidth = new QToolButton(this);
 alignWidth->setShortcut(QKeySequence(tr("Ctrl+J")));
 alignWidth->setStatusTip(tr("Align width (Ctrl+J)"));
 alignWidth->setIcon(QIcon(":/resource/pic/edit_alignwidth.svg"));
 alignWidth->setObjectName("editor_tb_alignwidth");

 
 // Выбор шрифта
 fontSelect = new QFontComboBox(this);
 fontSelect->setObjectName("editor_tb_fontselect");

 // Пустой пункт в конце списка шрифтов, используется для обозначения что в 
 // выделенном тексте несколько шрифтов
 fontSelect->addItem(" ");
 fontSelect->setItemIcon((fontSelect->count())-1,QIcon(":/resource/pic/edit_font_many.svg"));


 // Выбор размера шрифта
 fontSize=new MtComboBox(this);
 for(int i=MINIMUM_ALLOWED_FONT_SIZE; i<=MAXIMUM_ALLOWED_FONT_SIZE; i++) 
  fontSize->addItem(QString("%1").arg(i),i);
 fontSize->setCurrentIndex(fontSize->findData(10));
 fontSize->setMinimumContentsLength(3);
 fontSize->setEditable(true);
 QValidator *fontsizeValidator = new QIntValidator(MINIMUM_ALLOWED_FONT_SIZE, MAXIMUM_ALLOWED_FONT_SIZE, this);
 fontSize->setValidator(fontsizeValidator);
 fontSize->setObjectName("editor_tb_fontsize");

 // Пустой пункт в конце списка размеров шрифтов, используется для обозначения
 // что в выделенном тексте несколько размеров
 fontSize->addItem(" ",0);


 // Кнопка выбора цвета шрифта
 fontColor=new QToolButton(this);
 // fontColor->setShortcut(QKeySequence(tr("Ctrl+F")));
 fontColor->setStatusTip(tr("Text color"));
 fontColor->setIcon(QIcon(":/resource/pic/edit_fontcolor.svg"));
 fontColor->setObjectName("editor_tb_fontcolor");


 // Кнопка вызова виджета поиска текста
 findText=new QToolButton(this);
 findText->setShortcut(QKeySequence(tr("Ctrl+F")));
 findText->setStatusTip(tr("Find text (Ctrl+F)"));
 findText->setIcon(QIcon(":/resource/pic/edit_findtext.svg"));
 findText->setObjectName("editor_tb_findtext");

 // Кнопка вызова виджета конфигурирования редактора
 settings = new QToolButton(this);
 settings->setStatusTip(tr("Editor settings"));
 settings->setIcon(QIcon(":/resource/pic/edit_settings.svg"));
 settings->setObjectName("editor_tb_settings");

 // Кнопка просмотра HTML кода
 showHtml = new QToolButton(this);
 showHtml->setStatusTip(tr("Edit HTML code"));
 showHtml->setIcon(QIcon(":/resource/pic/edit_showhtml.svg"));
 showHtml->setObjectName("editor_tb_showhtml");

 // Кнопка включения отображения символов фарматирования
 showFormatting = new QToolButton(this);
 showFormatting->setStatusTip(tr("Show special chars"));
 showFormatting->setIcon(QIcon(":/resource/pic/edit_showformatting.svg"));
 showFormatting->setObjectName("editor_tb_showformatting");

 // Кнопка добавления новой таблицы
 createTable = new QToolButton(this);
 createTable->setStatusTip(tr("Create a new table"));
 createTable->setIcon(QIcon(":/resource/pic/edit_createtable.svg"));
 createTable->setObjectName("editor_tb_createtable");

 tableRemoveRow = new QToolButton(this);
 tableRemoveRow->setStatusTip(tr("Remove row(s)"));
 tableRemoveRow->setIcon(QIcon(":/resource/pic/edit_table_remove_row.svg"));
 tableRemoveRow->setObjectName("editor_tb_table_remove_row");

 tableRemoveCol = new QToolButton(this);
 tableRemoveCol->setStatusTip(tr("Remove column(s)"));
 tableRemoveCol->setIcon(QIcon(":/resource/pic/edit_table_remove_col.svg"));
 tableRemoveCol->setObjectName("editor_tb_table_remove_col");

 tableAddRow = new QToolButton(this);
 tableAddRow->setStatusTip(tr("Add row(s)"));
 tableAddRow->setIcon(QIcon(":/resource/pic/edit_table_add_row.svg"));
 tableAddRow->setObjectName("editor_tb_table_add_row");

 tableAddCol = new QToolButton(this);
 tableAddCol->setStatusTip(tr("Add column(s)"));
 tableAddCol->setIcon(QIcon(":/resource/pic/edit_table_add_col.svg"));
 tableAddCol->setObjectName("editor_tb_table_add_col");

 tableMergeCells = new QToolButton(this);
 tableMergeCells->setStatusTip(tr("Merge cells"));
 tableMergeCells->setIcon(QIcon(":/resource/pic/edit_table_merge_cells.svg"));
 tableMergeCells->setObjectName("editor_tb_table_merge_cells");

 tableSplitCell = new QToolButton(this);
 tableSplitCell->setStatusTip(tr("Split cell"));
 tableSplitCell->setIcon(QIcon(":/resource/pic/edit_table_split_cell.svg"));
 tableSplitCell->setObjectName("editor_tb_table_split_cell");

 insertImageFromFile = new QToolButton(this);
 insertImageFromFile->setStatusTip(tr("Insert image from file / edit image properties of selected image"));
 insertImageFromFile->setIcon(QIcon(":/resource/pic/edit_insert_image_from_file.svg"));
 insertImageFromFile->setObjectName("editor_tb_insert_image_from_file");

 expandEditArea = new QToolButton(this);
 expandEditArea->setStatusTip(tr("Expand edit area"));
 expandEditArea->setIcon(QIcon(":/resource/pic/edit_expand_text_area.svg"));
 expandEditArea->setObjectName("editor_tb_expand_edit_area");

 expandToolsLines = new QToolButton(this);
 expandToolsLines->setStatusTip(tr("Expand tools"));
 expandToolsLines->setIcon(QIcon(":/resource/pic/edit_expand_tools_lines.svg"));
 expandToolsLines->setObjectName("editor_tb_expand_tools_lines");

 save = new QToolButton(this);
 save->setStatusTip(tr("Save (Ctrl+S)"));
 save->setShortcut(QKeySequence(tr("Ctrl+S")));
 save->setIcon(QIcon(":/resource/pic/edit_save.svg"));
 save->setObjectName("editor_tb_save");


 // Кнопка "назад", используется в мобильном интерфейсе
 back = new QToolButton(this);
 back->setStatusTip(tr("Back"));
 back->setIcon(QIcon(":/resource/pic/mobile_back.svg"));
 back->setObjectName("editor_tb_back");

 // Кнопка "поиск по базе", используется в мобильном интерфейсе
 findInBase = new QToolButton(this);
 findInBase->setStatusTip(tr("Find in base"));
 findInBase->setIcon(QIcon(":/resource/pic/find_in_base.svg"));
 findInBase->setObjectName("editor_tb_find_in_base");


 // Виджет настройки отступов
 indentSlider=new IndentSlider(this->width(), 16, this);
 indentSlider->setObjectName("editor_tb_indentslider");


 // Все только что созданные элементы скрываются
 hide_all_tools_elements();
}


void Editor::hide_all_tools_elements(void)
{
  QRegExp nameMask("editor_tb_.*");

  // QList<QWidget *> tb_tools_list=qFindChildren(qobject_cast<QObject *>(this),name_mask);
  QList<QWidget *> tbToolsList=this->findChildren<QWidget *>(nameMask);

  for(int i=0;i<tbToolsList.size();++i)
    tbToolsList.at(i)->setVisible(false);
}


// Создание объекта области редактирования
void Editor::setup_editor_area(void)
{
 // Создается область редактирования
 textArea=new EditorTextArea(this);
 textArea->setObjectName("textArea");
 textArea->setContextMenuPolicy(Qt::CustomContextMenu); // Меню определяется в программе

 QFont font;
 font.fromString(editorConfig->get_default_font());
 font.setPointSize(editorConfig->get_default_font_size());
 textArea->selectAll();
 textArea->setCurrentFont(font);
 textArea->setFont(font);
}


// В линейку кнопок вставляется нужный инструмент
// Метод принимает имя инструмента
// и указатель на линейку
void Editor::insert_button_to_tools_line(QString toolName, QToolBar *line)
{
 qDebug() << "Editor::insert_button_to_tools_line() disableToolList : " << disableToolList;

 if(toolName=="separator")
  {
   line->addSeparator();
  }
 else
  {
   QString name("editor_tb_"+toolName);
   QWidget *tool=this->findChild<QWidget *>(name);

   if(tool!=NULL)
    {
     // Если данный инструмент не содержится в списке заблокированных
     if(!disableToolList.contains(toolName))
      {
       tool->setVisible(true);

       line->addWidget(tool); // Инструмент добавляется на панель инструментов

       QToolButton *tb=qobject_cast<QToolButton*>(tool);
       if(tb!=0) tb->setAutoRaise(false);
      }
     else
      {
       QToolButton *tb=qobject_cast<QToolButton*>(tool);
       if(tb!=0)
        tb->setEnabled(false);
      }
    }
   else
    critical_error("WyEdit: Can not find editor tool with name '"+toolName+"'. Please check editor *.ini file");
  }
}


// Сборка линейки редактирования текста в горизонтальную линейку
void Editor::assembly_buttons(void)
{
 // Заполнение горизонтальных линеек с кнопками форматирования текста
 toolsLine1=new QToolBar();
 toolsLine2=new QToolBar();

 updateToolsLines();

 /*
 QSize toolIconSize(16, 16);
 toolsLine1->setIconSize(toolIconSize);
 toolsLine2->setIconSize(toolIconSize);
 */

 // Горизонтальные линейки собираются
 textformatButtonsLayout=new QVBoxLayout();
 textformatButtonsLayout->addWidget(toolsLine1);
 textformatButtonsLayout->addWidget(toolsLine2);

 // Виджет настройки отступов виден только в desktop интерфейсе
 if(viewMode==WYEDIT_DESKTOP_MODE)
 {
   indentSlider->setVisible(true);
   textformatButtonsLayout->addWidget(indentSlider);
 }
 else
   indentSlider->setVisible(false);

}


void Editor::updateToolsLines(void)
{
  for(int i=0;i<toolsListInLine1.size();++i)
  {
    QString b=toolsListInLine1.at(i).trimmed();
    insert_button_to_tools_line(b,toolsLine1);
  }

  for(int i=0;i<toolsListInLine2.size();++i)
  {
    QString b=toolsListInLine2.at(i).trimmed();
    insert_button_to_tools_line(b,toolsLine2);
  }
}


// Сборка редактора и его активизация
void Editor::assembly(void)
{
 // Создается вертикальная область, которая должна объединить
 // линейку кнопок редактирования и область редактирования текста
 buttonsAndEditLayout=new QVBoxLayout(this);
 buttonsAndEditLayout->setObjectName("buttons_and_edit_layout");

 // Добавляются линейки кнопок
 buttonsAndEditLayout->addLayout(textformatButtonsLayout);

 // Добавляется область редактирования
 buttonsAndEditLayout->addWidget(textArea);

 // Полученый набор элементов устанавливается для текущего окна
 setLayout(buttonsAndEditLayout);

 // Границы убираются, так как данный объект будет использоваться как виджет
 QLayout *lt;
 lt=layout();
 lt->setContentsMargins(0,2,0,0);
}


// Синхронизация размеров линейки отступов относительно области редактирования
void Editor::update_indentline_geometry()
{
 // Синхронизируется ширина виджета линейки отступов
 indentSlider->set_widget_width(textArea->width());

 // Синхронизируется геометрия линейки отступов
 int leftPos=textArea->get_indent_started_left();
 int rightPos=textArea->get_indent_started_right();
 indentSlider->set_indentline_left_pos(leftPos);
 indentSlider->set_indentline_right_pos(rightPos);

 indentSlider->update();
}


// Установка текста области редактирования
void Editor::set_textarea(QString text)
{
 textArea->setHtml(text);

 // Очищается URL документа, так как документ создается "из ничего"
 textArea->document()->setMetaInformation( QTextDocument::DocumentUrl, "" );
}


// Установка запрета или разрешения редактирования
void Editor::set_textarea_editable(bool editable)
{
 if(editable==true)
 {
  // Если редактирование разрешено
  textArea->setTextInteractionFlags(Qt::TextEditorInteraction);
  textArea->setPalette(qApp->palette());
 }
 else 
 {
  // Если редактирование запрещено
  textArea->setTextInteractionFlags(Qt::TextBrowserInteraction);

  QPalette pal=qApp->palette();
  QColor inactiveColor;
  
  inactiveColor=qApp->palette().color(QPalette::Disabled, QPalette::Window);
  pal.setColor(QPalette::Normal, QPalette::Base, inactiveColor);
  textArea->setPalette(pal);
  
  // QPalette plt=standart_style->standardPalette();
 }
}

 
// Получение текста области редактирования в формате HTML
QString Editor::get_textarea(void)
{
 return textArea->document()->toHtml("UTF-8");
}



QTextDocument *Editor::get_textarea_document(void)
{
 return textArea->document();
}


bool Editor::set_work_directory(QString dirName)
{
 QDir directory(dirName);

 if(directory.exists())
  {
   workDirectory=dirName;
   return true;
  }
 else
  {
   critical_error("WyEdit: Can not set work directory to "+dirName+". Directory not exists.");
   return false;
  }
}


QString Editor::get_work_directory(void)
{
 return workDirectory;
}


void Editor::set_file_name(QString fileName)
{
 workFileName=fileName;
}


QString Editor::get_file_name(void)
{
 return workFileName;
}


// Сохранение HTML кода документа в файл
bool Editor::save_textarea_text()
{
 if(workFileName.length()==0)
  {
   critical_error("WyEdit: Save function. Not setted work file name for editor.");
   return false;
  }

 QString fullFileName=workDirectory+"/"+workFileName;

 // Создание нового файла записи
 QFile wfile(fullFileName);

 if (!wfile.open(QIODevice::WriteOnly | QIODevice::Text))
  {
   critical_error("WyEdit: Cant open file "+fullFileName+" for write.");
   return false;
  }

 qDebug() << "Write to file " << workFileName;

 QTextStream out(&wfile);
 QString content=textArea->document()->toHtml("UTF-8");
 out.setCodec("UTF-8");
 out << content;

 return true;
}


// Сохранение картинок, которые присутствуют в документе,
// в указанную директорию
bool Editor::save_textarea_images(int mode=SAVE_IMAGES_SIMPLE)
{
 qDebug() << "Save images...\n" ;
 qDebug() << "Block count" << textArea->document()->blockCount() << "\n";

 if(workDirectory.length()==0)
  {
   critical_error("WyEdit: Save images function. Not setted work directory.");
   return false;
  }

 // Перебираются блоки документа и находятся блоки с картинками
 QStringList imagesNames; // В список сохраняются имена найденных картинок
 QTextBlock textBlock = textArea->document()->begin();
 while(textBlock.isValid())
  {
   QTextBlock::iterator it;

   for(it = textBlock.begin(); !(it.atEnd()); ++it)
    {
     QTextFragment currentFragment = it.fragment();
     if(currentFragment.isValid())
      {
       if(currentFragment.charFormat().isImageFormat())
        {
         // Найден блок с картинкой

         // Выясняется формат картинки
         QTextImageFormat imgFmt = currentFragment.charFormat().toImageFormat();

         // Из формата выясняется имя картинки
         QString imageName=imgFmt.name();
         imagesNames << imageName;
         qDebug() << "Image " << imageName << "\n"; // имя файла

         // Если файла картинки не существует
         QString imageFileName=workDirectory+"/"+imageName;
         QFileInfo tryFile(imageFileName);
         if(tryFile.exists()==false)
          {
           qDebug() << "Save image data to file " << imageFileName;

           // Из ресурсов вытягивается картинка
           QVariant imageData=textArea->document()->resource(QTextDocument::ImageResource, QUrl(imageName));

           QImage picture=imageData.value<QImage>();

           picture.save(imageFileName, "png");
          }
        }
      }
    }
   textBlock = textBlock.next();
  }


 // Если при сохранении все лишние картинки должны удалиться в каталоге
 if(mode==SAVE_IMAGES_REMOVE_UNUSED)
  {
   // Выясняется список файлов в директории
   QDir directory(workDirectory);
   QStringList imageInDirectory=directory.entryList(QDir::Files);

   // Перебираются файлы в директории
   foreach(QString fileName, imageInDirectory)
    if(fileName.contains(QRegExp("\\.png$"))) // Обрабатыватся только *.png файлы
     if(!imagesNames.contains(fileName))
     {
      // Если в списке картинок нет текущего png файла,
      // значит этот файл лишний и он удаляется
      QFile currentFile(workDirectory+"/"+fileName);
      currentFile.remove();
     }
  }

 qDebug() << "Save images finish\n" ;

 return true;
}


void Editor::save_textarea(void)
{
 qDebug() << "Save textarea...";
 
 // Если запись была открыта на просмотр и изменена
 if(get_work_directory().length()!=0 &&
    get_file_name().length()!=0 &&
    get_textarea_modified()==true)
  {
   // Перенос текущего файла записи в корзину
   qDebug() << "Try remove file " << get_file_name() << " from directory " << get_work_directory();
   if( QFileInfo( get_work_directory()+"/"+get_file_name() ).exists() )
    {
     qDebug() << "File exists. Remove it.";
     remove_file_to_trash(get_work_directory()+"/"+get_file_name());
    }
   else
    qDebug() << "Cant remove file. File not exists.";

   // Если происходит прямая работа с файлом текста
   if(load_callback_func==NULL)
    {
     // Сохранение текста записи в файл
     save_textarea_text();

     // Сохранение картинок, которые есть в записи
     save_textarea_images(Editor::SAVE_IMAGES_REMOVE_UNUSED);
    }
   else
    {
     // Иначе задана функция обратного вызова для записи текста и картинок

     QString content=textArea->document()->toHtml("UTF-8");
     save_callback_func(qobject_cast<QObject *>(this), content);
    }

   // Так как произошло сохранение,
   // отмечается что новый текст небыл еще изменен
   set_textarea_modified(false);
  }
}


bool Editor::load_textarea()
{
 // Если не заданы директория или файл
 // Но установлен режим подавления ошибки
 if(workFileName.length()==0 || workDirectory.length()==0)
  if(dirFileEmptyReaction==DIRFILEEMPTY_REACTION_SUPPRESS_ERROR)
   {
    // Никаких действий с областью редактирования не делается
    // скорее всего она останется просто пустой
    return true;
   }


 if(workFileName.length()==0)
  {
   critical_error("WyEdit: Load function. Not setted work file name for editor.");
   return false;
  }

 if(workDirectory.length()==0)
  {
   critical_error("WyEdit: Not setted work directory for editor.");
   return false;
  }

 QString fileName=workDirectory+"/"+workFileName;

 QString content;

 // Если происходит прямая работа с файлом текста
 if(load_callback_func==NULL)
  {
   // Создается объект файла с нужным именем
   QFile f(fileName);

   // Если нужный файл не существует
   if(!f.exists())
    {
     critical_error("WyEdit: File "+fileName+" not found");
     return false;
    }

   // Если нужный файл недоступен для чтения
   if(!f.open(QIODevice::ReadOnly))
    {
     critical_error("WyEdit: File "+fileName+" not readable. Check permission.");
     return false;
    }

   content=QString::fromUtf8(f.readAll());
  }
 else
  {
   // Иначе задана функция обратного вызова для установки начального текста
   QString resultString;
   load_callback_func(qobject_cast<QObject *>(this), resultString);
   content=resultString;
  }

 // Устанавливается URL документа, с помощью него будут высчитываться
 // относительные ссылки при загрузке картинок
 textArea->document()->setMetaInformation( QTextDocument::DocumentUrl, "file:"+fileName );

 qDebug() << "Editor::load_textarea() Set document URL to " << fileName;

 // Текст из файла вставляется в область редактирования
 // QString content=QString::fromUtf8(f.readAll());
 // qDebug() << "Load content:";
 // qDebug() << content;
 textArea->setHtml( content );

 // qDebug() << "Set content:";
 // qDebug() << textArea->toHtml();

 return true;
}


void Editor::set_textarea_modified(bool modify)
{
 qDebug() << "Editor::set_textarea_modified() :" << modify;
 textArea->document()->setModified(modify);
}


bool Editor::get_textarea_modified(void)
{
 qDebug() << "Editor::get_textarea_modified() :" << textArea->document()->isModified();
 return textArea->document()->isModified();
}


/////////////////////////////////////////////////
// Форматирование текста
/////////////////////////////////////////////////

// Форматирование Bold
void Editor::on_bold_clicked(void)
{
 // Если выделение есть
 if(textArea->textCursor().hasSelection())
  {
   // Обычное форматирование
   if(textArea->fontWeight() != QFont::Bold)
    textArea->setFontWeight(QFont::Bold); // Bold
   else
    textArea->setFontWeight(0); // Remove Bold
  }
 else
  {
   // Иначе надо выделить дополнительным курсором слово на
   // котором стоит курсор
   QTextCursor cursor=textArea->textCursor();
   cursor.select(QTextCursor::WordUnderCursor);

   // К выделению применяется форматирование Bold
   QTextCharFormat format;
   if(cursor.charFormat().fontWeight() != QFont::Bold)
    format.setFontWeight(QFont::Bold);
   else
    format.setFontWeight(0);
   cursor.mergeCharFormat(format);
  }

 update_outline_button_higlight();
}

// Форматирование Italic
void Editor::on_italic_clicked(void)
{
 // Если выделение есть
 if(textArea->textCursor().hasSelection())
  {
   // Обычное форматирование
   if(!textArea->fontItalic())
    textArea->setFontItalic(true);
   else
    textArea->setFontItalic(false);
  }
 else
  {
   // Иначе надо выделить дополнительным курсором слово на
   // котором стоит курсор
   QTextCursor cursor=textArea->textCursor();
   cursor.select(QTextCursor::WordUnderCursor);

   // К выделению применяется форматирование Italic
   QTextCharFormat format;
   if(!cursor.charFormat().fontItalic())
    format.setFontItalic(true);
   else
    format.setFontItalic(false);
   cursor.mergeCharFormat(format);
  }

 update_outline_button_higlight();
}

// Форматирование подчеркивания
void Editor::on_underline_clicked(void)
{
 // Если выделение есть
 if(textArea->textCursor().hasSelection())
  {
   // Обычное форматирование
   if(!textArea->fontUnderline())
    textArea->setFontUnderline(true);
   else
    textArea->setFontUnderline(false);
  }
 else
  {
   // Иначе надо выделить дополнительным курсором слово на
   // котором стоит курсор
   QTextCursor cursor=textArea->textCursor();
   cursor.select(QTextCursor::WordUnderCursor);

   // К выделению применяется форматирование Underline
   QTextCharFormat format;
   if(!cursor.charFormat().fontUnderline())
    format.setFontUnderline(true);
   else
    format.setFontUnderline(false);
   cursor.mergeCharFormat(format);
  }

 update_outline_button_higlight();
}


// Форматирование моноширинным шрифтом
void Editor::on_monospace_clicked(void)
{
 textArea->textCursor().beginEditBlock();

 // Устанавливается шрифт
 QFont font;
 font.fromString(editorConfig->get_monospace_font());
 textArea->setFontFamily(font.family());
 
 // Новый установленный шрифт показывается в выпадающем списке шрифтов
 set_fontselect_on_display(font.family());

 // Если в настройках разрешена автоматическая установка нужного размера
 // моноширинного шрифта
 if(editorConfig->get_monospace_font_size_apply()==true)
 {
  // Устанавливается нужный размер
  textArea->setFontPointSize(editorConfig->get_monospace_font_size());

  // В выпадающем списке размеров выставляется установленный размер
  set_fontsize_on_display(editorConfig->get_monospace_font_size());
 }

 textArea->textCursor().endEditBlock();
}


// Выбран ли блок текста (т.е. находятся ли начало и конец выделения
// точно на границах блока)
bool Editor::is_block_select(void)
{
 // Выясняются позиции начала и конца выделения
 int start=textArea->textCursor().selectionStart();
 int stop =textArea->textCursor().selectionEnd();

 // Высталяются флаги, находится ли начало и конец выделения на границах блока.
 // Это определяется с помощью дополнительного курсора
 int flagStartBlockEdge=0;
 int flagStopBlockEdge=0;
 QTextCursor cursor=textArea->textCursor();
 cursor.setPosition(start);
 if(cursor.atBlockStart()==true || cursor.atBlockEnd()==true)
  flagStartBlockEdge=1;
 cursor.setPosition(stop);
 if(cursor.atBlockStart()==true || cursor.atBlockEnd()==true)
  flagStopBlockEdge=1;

 if(flagStartBlockEdge==1 && flagStopBlockEdge==1) return true;
 else return false;
}


// Стоит ли курсор на пустой строке (т.е. в строке есть только символ
// перевода на новую строку)
bool Editor::is_cursor_on_empty_line(void)
{
 if(!textArea->textCursor().atBlockStart()) return false;

 // Создаётся дополнительный курсор
 QTextCursor cursor=textArea->textCursor();

 // Выделяется анализируемая строка
 cursor.select(QTextCursor::LineUnderCursor);

 // Текст анализируемой строки
 QString selectedText=cursor.selectedText();

 if(selectedText.size()>0)return false;
 else return true;
}


bool Editor::is_cursor_on_space_line(void)
{
 // Если есть выделение, функция работать не должна
 if(textArea->textCursor().hasSelection()) return false;

 // Создаётся дополнительный курсор
 QTextCursor cursor=textArea->textCursor();

 // Выделяется анализируемая строка
 cursor.select(QTextCursor::LineUnderCursor);

 // Текст анализируемой строки
 QString selectedText=cursor.selectedText();

 if(selectedText.simplified().size()>0)return false;
 else return true;
}


// Форматирование в качестве кода
void Editor::on_code_clicked(void)
{
 // Если выделения нет
 if(!textArea->textCursor().hasSelection()) return;

 bool enableIndent;

 // Проверяется, выбран ли четко блок (блоки) текста
 // Блок - это текст между <p>...</p>
 if(!is_block_select())
  {
   // Если выбран не четко блок (блоки)

   // Нужно определить, находится ли выделение в пределах одного блока
   // Если выделение вылазит за пределы одного блока, форматировать нельзя

   // Выясняются позиции начала и конца блока
   QTextBlock currentBlock = textArea->textCursor().block();
   int blockStart=currentBlock.position();
   int blockStop=currentBlock.position()+currentBlock.length();

   // Выясняются позиции начала и конца выделения
   int selectStart=textArea->textCursor().selectionStart();
   int selectStop =textArea->textCursor().selectionEnd();

   qDebug() << "Code format action, block " << blockStart << blockStop << " selection " << selectStart << selectStop;

   if(blockStart<=selectStart && blockStop>=selectStop)
    enableIndent=false; // Выбран кусок текста в пределах блока
   else
    return;
  }
 else
  enableIndent=true; // Выбран четко блок (блоки) текста, нужно делать отступ

 textArea->textCursor().beginEditBlock();

 // Устанавливается шрифт
 QFont font;
 font.fromString(editorConfig->get_code_font());
 textArea->setFontFamily(font.family());

 // Новый установленный шрифт показывается в выпадающем списке шрифтов
 set_fontselect_on_display(font.family());

 // Если в настройках разрешена автоматическая установка нужного размера
 if(editorConfig->get_code_font_size_apply()==true)
 {
  // Устанавливается нужный размер
  textArea->setFontPointSize(editorConfig->get_code_font_size());

  // В выпадающем списке размеров выставляется установленный размер
  set_fontsize_on_display(editorConfig->get_code_font_size());
 }

 // Устанавливается цвет текста
 QColor codeColor(editorConfig->get_code_font_color ());
 textArea->setTextColor(codeColor);

 // Если в настройках разрешена автоматическая установка нужного отступа
 // И выбран четко блок (блоки)
 if(editorConfig->get_code_indent_size_apply()==true &&
    enableIndent==true)
  {
   // Создание форматирования
   QTextBlockFormat indentFormatting;
   indentFormatting.setLeftMargin(editorConfig->get_code_indent_size());
   indentFormatting.setAlignment(Qt::AlignLeft); // Выравнивание по левому краю

   // Форматирование для добавления отступа
   textArea->textCursor().mergeBlockFormat(indentFormatting);

   update_indentslider_to_actual_format();
  }

 textArea->textCursor().endEditBlock();
}


// Очистка форматирования, т.е. установка стандартного шрифта,
// размера и убирание утолщения, наклона, подчеркивания
void Editor::on_clear_clicked(void)
{
 // Проверка скрытия кнопок
 /*
 QToolButton *tool=this->findChild<QToolButton *>("editor_tb_save");
 tool->hide();
 tool->setVisible(false);
 tool->setEnabled(false);
 */

 // Очистка возможна только если что-то выделено
 // Или курсор стоит на пустой строке с одним символом перевода строки
 // Или курсор стоит на строке, в которой нет текста
 bool flag_cursor_on_empty_line=is_cursor_on_empty_line();
 bool flag_cursor_on_space_line=is_cursor_on_space_line();
 if(!(textArea->textCursor().hasSelection() ||
      flag_cursor_on_empty_line ||
      flag_cursor_on_space_line)) return;

 textArea->textCursor().beginEditBlock();

 if(flag_cursor_on_space_line)
  (textArea->textCursor()).select(QTextCursor::LineUnderCursor);

 // Создается стандартный шрифт
 QFont font;
 font.fromString( editorConfig->get_default_font() ); // Стандартное начертание
 font.setPointSize( editorConfig->get_default_font_size() ); // Стандартный размер

 /*
 font.setBold(false); // Толщина обычная
 font.setWeight(QFont::Normal); // Толщина обычная
 font.setItalic(false); // Наклона нет
 font.setUnderline(false); // Подчеркивания нет
 */

 // Применяется стандартный шрифт
 textArea->setCurrentFont(font);

 // Новый установленный шрифт показывается в выпадающем списке шрифтов
 set_fontselect_on_display(font.family());

 // В выпадающем списке размеров выставляется установленный размер
 set_fontsize_on_display(editorConfig->get_default_font_size());


 // Если выделен блок
 // или курсор на пустой линии
 // или курсор на линии на которой нет символов
 if(is_block_select() ||
    flag_cursor_on_empty_line ||
    flag_cursor_on_space_line)
 {
   QTextBlockFormat formt;
   formt.setLeftMargin(0); // Убирается отступ
   formt.setAlignment(Qt::AlignLeft); // Выравнивание по левому краю

   // Применение форматирование
   textArea->textCursor().setBlockFormat(formt);
 }

 // Если была работа со строкой, в которой нет символов,
 // курсор переносится на начало строки, чтобы не путать пользователя
 if(flag_cursor_on_space_line)
  textArea->moveCursor(QTextCursor::StartOfLine);


 // Удаление какого-либо форматирования стилем
 QString htmlCode=textArea->textCursor().selection().toHtml();
 qDebug() << "Before remove style: " << htmlCode;

 // В регулярных выражениях Qt кванторы по-умолчанию жадные (greedy)
 // Поэтому напрямую регвыру указывается что кванторы должны быть ленивые
 QRegExp replace_expression("style=\".*\"");
 replace_expression.setMinimal(true);

 htmlCode.replace(replace_expression, "");
 qDebug() << "After remove style: " << htmlCode;

 /*
 QRegExp reg_header("^.*<body>");
 reg_header.setMinimal(true);

 QRegExp reg_footer("</body>.*$");
 reg_footer.setMinimal(true);

 QRegExp reg_span_open("<span.?>");
 reg_span_open.setMinimal(true);

 QRegExp reg_span_close("</span>");
 reg_span_close.setMinimal(true);

 htmlCode.replace(reg_header, "");
 htmlCode.replace(reg_footer, "");
 htmlCode.replace(reg_span_open, "");
 htmlCode.replace(reg_span_close, "");
 qDebug() << "After remove header and footer: " << htmlCode;
 */

 QString currStyleSheet=textArea->document()->defaultStyleSheet();
 textArea->document()->setDefaultStyleSheet(" ");

 textArea->textCursor().removeSelectedText();
 textArea->textCursor().insertHtml(htmlCode);

 textArea->document()->setDefaultStyleSheet(currStyleSheet);


 textArea->textCursor().endEditBlock();

 // Вызывается метод, как будто переместился курсор с выделением, чтобы
 // обновились состояния подсветок кнопок форматирования
 on_selection_changed();

 update_indentslider_to_actual_format();
}


void Editor::format_to_list(QTextListFormat::Style setFormat)
{
 // Если выделения нет
 if(!textArea->textCursor().hasSelection()) return;

 // Форматирование в список возможно только если выделен блок
 if(!is_block_select()) return;

 // Если строки выбраны
 if(textArea->textCursor().hasSelection())
  {
   if(textArea->textCursor().currentList()==0 ||
      textArea->textCursor().currentList()->format().style()!=setFormat)
    {
     // Cтроки еще не отформатированы в данный вид списка, 
     // надо отформатировать в список

     qDebug() << "Formatting to list";

     // Применение форматирования
     textArea->textCursor().createList(setFormat);
    } 
   else
    {
     // Строки уже были отформатированы в список, надо форматирование
     // сбросить в стандартное
     
     qDebug() << "Remove list formatting";

     // Выяснение текущего отступа
     int currentIndent;
     currentIndent=(int) textArea->textCursor().blockFormat().leftMargin();

     // Создание форматирования
     QTextBlockFormat indentFormatting;
     indentFormatting.setLeftMargin(currentIndent);

     // Форматирование
     textArea->textCursor().setBlockFormat(indentFormatting);
     
     // Создание форматирования по умолчанию чтобы убрать форматирование в список
     // QTextBlockFormat formatting;
     // formatting.setAlignment(Qt::AlignLeft);

     // Форматирование
     // textarea->textCursor().setBlockFormat(formatting);
    }
  }
 else
  {
   // Вставляется первый пустой пункт
   textArea->textCursor().insertList(setFormat);
  }

 // Выравнивание прокрутки чтоб курсор был виден если он уехал вниз
 textArea->ensureCursorVisible();
}


// Форматирование в нумерованный список
void Editor::on_numericlist_clicked(void)
{
 format_to_list(QTextListFormat::ListDecimal);
}


// Форматирование в список с точечками
void Editor::on_dotlist_clicked(void)
{
 format_to_list(QTextListFormat::ListDisc);
}


// Добавление отступа
void Editor::on_indentplus_clicked(void)
{
 // Работа с отступом возможна только если выделен блок
 // Или курсор стоит в начале строки
 // Закомментировано. Теперь отступ можно добавлять в любой момент
 /*
 int start=textarea->textCursor().selectionStart();
 QTextCursor cursor=textarea->textCursor();
 cursor.setPosition(start);
 if( !(is_block_select() || cursor.atBlockStart()) ) return;
 */

 int currentIndent;

 // Выяснение текущего отступа
 currentIndent=(int) textArea->textCursor().blockFormat().leftMargin();

 // Создание форматирования
 QTextBlockFormat indentFormatting;
 indentFormatting.setLeftMargin(currentIndent+10); // setTextIndent(10);

 // Форматирование
 textArea->textCursor().mergeBlockFormat(indentFormatting);

 update_indentslider_to_actual_format();
}


// Убирание отступа
void Editor::on_indentminus_clicked(void)
{
 int currentIndent, indentForSet;

 // Выяснение текущего отступа
 currentIndent=(int) textArea->textCursor().blockFormat().leftMargin();

 if((currentIndent-10)>0)indentForSet=currentIndent-10;
 else indentForSet=0;

 // Создание форматирования
 QTextBlockFormat indentFormatting;
 indentFormatting.setLeftMargin(indentForSet);

 // Форматирование
 textArea->textCursor().mergeBlockFormat(indentFormatting);

 update_indentslider_to_actual_format();
}


// Форматирование по левому краю
void Editor::on_alignleft_clicked(void)
{
 align_text(Qt::AlignLeft);
}


// Форматирование по центру
void Editor::on_aligncenter_clicked(void)
{
 align_text(Qt::AlignHCenter);
}


// Форматирование по правому краю
void Editor::on_alignright_clicked(void)
{
 align_text(Qt::AlignRight);
}


// Форматирование по ширине
void Editor::on_alignwidth_clicked(void)
{
 align_text(Qt::AlignJustify);
}


// Выравнивание текста, вспомогательный метод
void Editor::align_text(Qt::AlignmentFlag mode)
{
 // Создание форматирования
 QTextBlockFormat formatting;
 formatting.setAlignment(mode);

 // Форматирование
 textArea->textCursor().mergeBlockFormat(formatting);

 update_align_button_higlight(true);
}


// Слот, срабатывающий при изменении шрифта в списке шрифтов
void Editor::on_fontselect_changed(const QFont &font)
{
 if(flagSetFontParametersEnabled==false) return;

 textArea->setFontFamily(font.family());

 currentFontFamily=font.family();

 // Курсор после выбора возвращается в область редактирования
 textArea->setFocus();
}


// Метод только меняет значение, показываемое списком шрифтов
void Editor::set_fontselect_on_display(QString fontName)
{
 flagSetFontParametersEnabled=false;

 if(fontName.size()>0)
  fontSelect->setCurrentIndex(fontSelect->findText(fontName));
 else
  fontSelect->setCurrentIndex(fontSelect->count()-1);

 currentFontFamily=fontName;

 flagSetFontParametersEnabled=true;
}


// Слот, срабатывающий когда изменен размер шрифта через список размеров
void Editor::on_fontsize_changed(int i)
{
 if(flagSetFontParametersEnabled==false) return;

 int n=(fontSize->itemData(i)).toInt();
 
 if(n<MINIMUM_ALLOWED_FONT_SIZE) return;
 if(n>MAXIMUM_ALLOWED_FONT_SIZE) return;
 
 textArea->setFontPointSize(n);

 currentFontSize=n;

 // Курсор после выбора возвращается в область редактирования
 textArea->setFocus();
}


// Метод только меняет значение, показываемое списком размеров шрифта
void Editor::set_fontsize_on_display(int n)
{
 flagSetFontParametersEnabled=false;

 fontSize->setCurrentIndex(fontSize->findData(n));
 currentFontSize=n;

 flagSetFontParametersEnabled=true;
}


// Слот, срабатыващий при нажатии на кнопку выбора цвета текста
void Editor::on_fontcolor_clicked()
{
 // Текущий цвет возле курсора
 QColor currentColor=textArea->textColor();

 // Диалог запроса цвета
 QColor selectedColor=QColorDialog::getColor(currentColor, this);

 // Если цвет выбран, и он правильный
 if(selectedColor.isValid())
  {
   // Меняется цвет кнопки
   fontColor->setPalette(QPalette(selectedColor));

   // Если выделение есть
   if(textArea->textCursor().hasSelection())
    textArea->setTextColor(selectedColor); // Меняется цвет текста
   else
    {
     // Иначе надо выделить дополнительным курсором слово на
     // котором стоит курсор
     QTextCursor cursor=textArea->textCursor();
     cursor.select(QTextCursor::WordUnderCursor);

     QTextCharFormat format;
     format.setForeground(selectedColor);

     cursor.mergeCharFormat(format);
    }
  }
}


// Слот вызывается при каждом движении курсора в момент выделения текста
void Editor::on_selection_changed(void)
{
 // Если выделения нет
 if(!textArea->textCursor().hasSelection()) return;

 // qDebug() << "Selection text " << textarea->textCursor().selection().toHtml();

 // Позиции начала и конца выделения
 int start=textArea->textCursor().selectionStart();
 int stop =textArea->textCursor().selectionEnd();
 // qDebug() << "Selection start " << start << " stop " << stop;

 // Создается дополнительный курсор, который будет пробегать по выделенному тексту
 QTextCursor cursor=textArea->textCursor();
 cursor.setPosition(start);

 // Выравнивание относится к форматированию строк, начальное состояние
 // берётся из начального положения курсора
 int startAlign=cursor.blockFormat().alignment();

 // Курсор сдвигается на одну позицию вперёд
 cursor.movePosition(QTextCursor::NextCharacter);

 // Для анализа форматирования символов надо начинать
 // с позиции, следующей справа от начала выделения
 QString startFontFamily=cursor.charFormat().fontFamily(); // Шрифт
 qreal startSize=cursor.charFormat().fontPointSize(); // Размер шрифта
 bool startBold=false;
 if(cursor.charFormat().fontWeight()==QFont::Bold) startBold=true; // Тощина
 bool startItalic=cursor.charFormat().fontItalic(); // Наклон
 bool startUnderline=cursor.charFormat().fontUnderline(); // Подчеркивание

 int differentFontFlag=0;
 int differentSizeFlag=0;
 int differentBoldFlag=0;
 int differentItalicFlag=0;
 int differentUnderlineFlag=0;
 int differentAlignFlag=0;

 // Пробегаем по выделенному куску текста дополнительным курсором
 // и выясняем, есть ли разные шрифты, разные размеры,
 // разные начертания символов, разное выравнивание в выделенном тексте
 while(cursor.position()<=stop) 
  {
   if(differentFontFlag==0 && startFontFamily!=cursor.charFormat().fontFamily())
    differentFontFlag=1;

   if(differentSizeFlag==0 && startSize!=cursor.charFormat().fontPointSize())
    differentSizeFlag=1;

   if(differentBoldFlag==0)
    {
     int b=cursor.charFormat().fontWeight();
     if(startBold==false && b==QFont::Bold)differentBoldFlag=1;
     else if(startBold==true && b==QFont::Normal)differentBoldFlag=1;
    }

   if(differentItalicFlag==0 && startItalic!=cursor.charFormat().fontItalic())
    differentItalicFlag=1;

   if(differentUnderlineFlag==0 && startUnderline!=cursor.charFormat().fontUnderline())
    differentUnderlineFlag=1;

   if(differentAlignFlag==0 && startAlign!=cursor.blockFormat().alignment())
    differentAlignFlag=1;

   // Курсор передвигается на одну позицию вперед
   // Если дальше двигаться некуда (конец документа) цикл досрочно завершается
   if(cursor.movePosition(QTextCursor::NextCharacter)==false) break;
  }


 // Список выбора шрифта начинает указывать на нужный шрифт
 if(differentFontFlag==0)
  set_fontselect_on_display(startFontFamily); // Если всё выделение одним шрифтом
 else 
  set_fontselect_on_display("");

 // Список выбора размера начинает указывать на нужный размер
 if(differentSizeFlag==0)
  set_fontsize_on_display((int)startSize); // Если всё отформатировано одним размером
 else
  set_fontsize_on_display(0); // В выделении есть разные размеры

 // Кнопка Bold выключается, если есть разное Bold форматирование
 // и включается, если форматирование одинаковое,
 // и выделение начиналось с Bold
 if(differentBoldFlag==1)
  set_outline_button_higlight(BT_BOLD,false);
 else
  if(startBold==true)
   set_outline_button_higlight(BT_BOLD,true);

 // Кнопка Italic
 if(differentItalicFlag==1)
  set_outline_button_higlight(BT_ITALIC,false);
 else
  if(startItalic==true)
   set_outline_button_higlight(BT_ITALIC,true);

 // Кнопка Underline
 if(differentUnderlineFlag==1)
  set_outline_button_higlight(BT_UNDERLINE,false);
 else
  if(startUnderline==true)
   set_outline_button_higlight(BT_UNDERLINE,true);

 // Кнопки выравнивания
 if(differentAlignFlag==1)
  update_align_button_higlight(false);
 else
  update_align_button_higlight(true);
}


void Editor::update_tool_line_to_actual_format(void)
{
 // Список должен показывать текущий шрифт позиции, где находится курсор
 if(currentFontFamily!=textArea->fontFamily())
  set_fontselect_on_display(textArea->fontFamily());

 // Размер
 if(currentFontSize!=(int)textArea->fontPointSize())
  set_fontsize_on_display((int)textArea->fontPointSize());

 // Кнопки форматирования начертания
 update_outline_button_higlight();

 // Кнопки выравнивания
 update_align_button_higlight(true);
}


void Editor::update_indentslider_to_actual_format(void)
{
 int i;

 i=(int)textArea->textCursor().blockFormat().textIndent();
 if(currentTextIndent!=i)
  {
   emit send_set_textindent_pos(i);
   currentTextIndent=i;
  }

 i=(int)textArea->textCursor().blockFormat().leftMargin();
 if(currentLeftIndent!=i)
  {
   emit send_set_leftindent_pos(i);
   currentLeftIndent=i;
  }

 i=(int)textArea->textCursor().blockFormat().rightMargin();
 if(currentRightIndent!=i)
  {
   emit send_set_rightindent_pos(i);
   currentRightIndent=i;
  }
}


// Слот вызывается при каждом перемещении курсора
void Editor::on_cursor_position_changed(void)
{
 // Если одновременно идет режим выделения 
 // то обслуживание текущего шрифта и размера идет в on_selection_changed()
 if(textArea->textCursor().hasSelection()) return;

 update_tool_line_to_actual_format();

 update_indentslider_to_actual_format();
}


bool Editor::is_key_for_tool_line_update(QKeyEvent *event)
{
 if(event->modifiers().testFlag(Qt::ControlModifier) ||
    event->modifiers().testFlag(Qt::AltModifier) ||
    event->modifiers().testFlag(Qt::MetaModifier) ||
    event->key()==Qt::Key_F1 ||
    event->key()==Qt::Key_F2 ||
    event->key()==Qt::Key_F3 ||
    event->key()==Qt::Key_F4 ||
    event->key()==Qt::Key_F5 ||
    event->key()==Qt::Key_F6 ||
    event->key()==Qt::Key_F7 ||
    event->key()==Qt::Key_F8 ||
    event->key()==Qt::Key_F9 ||
    event->key()==Qt::Key_F10 ||
    event->key()==Qt::Key_F11 ||
    event->key()==Qt::Key_F12)
  return true;
 else
  return false;
}


// Cлот отлавливает нажатия клавиш
void Editor::keyPressEvent(QKeyEvent *event)
{
 if(is_key_for_tool_line_update(event)) update_tool_line_to_actual_format();
}


// Cлот отлавливает отжатия клавиш
void Editor::keyReleaseEvent(QKeyEvent * event)
{
 if(is_key_for_tool_line_update(event)) update_tool_line_to_actual_format();
}


void Editor::on_undo(void)
{
 qDebug() << "Undo slot normal running ";
 textArea->undo();
 update_tool_line_to_actual_format(); // Обновляется панель с кнопками
}


void Editor::on_redo(void)
{
 qDebug() << "Redo slot normal running ";
 textArea->redo();
 update_tool_line_to_actual_format(); // Обновляется панель с кнопками
}


void Editor::on_cut(void)
{
 textArea->cut();
 update_tool_line_to_actual_format(); // Обновляется панель с кнопками
}


void Editor::on_copy(void)
{
 // Если выбрана только картинка или курсор стоит на позиции картинки
 if(is_image_select() || is_cursor_on_image())
  {
   QTextImageFormat imageFormat;

   if(is_image_select()) 
    imageFormat = image_format_on_select();

   if(is_cursor_on_image())
    imageFormat = image_format_on_cursor();

   // Из формата выясняется имя картинки
   QString imageName=imageFormat.name();

   // Из ресурсов вытягивается картинка
   QVariant imageData=textArea->document()->resource(QTextDocument::ImageResource, QUrl(imageName));
   QImage image=imageData.value<QImage>();

   // Создается ссылка на буфер обмена
   QClipboard *clipboard=QApplication::clipboard();

   // Копирвание картинки в буфер обмена
   clipboard->setImage(image);
  }
 else  
  textArea->copy(); // Обычное копирование

 update_tool_line_to_actual_format(); // Обновляется панель с кнопками
}


void Editor::on_paste(void)
{
 textArea->paste();
 update_tool_line_to_actual_format(); // Обновляется панель с кнопками
}


void Editor::on_selectAll(void)
{
 textArea->selectAll();
 update_tool_line_to_actual_format(); // Обновляется панель с кнопками
}


// Обновление подсветки кнопок выравнивания текста
// Если параметр activate=false, все кнопки будут выставлены в неактивные
// Если параметр activate=true, будет подсвечена кнопка, соответсвующая
// текущему форматированию
void Editor::update_align_button_higlight(bool activate)
{
 QPalette palActive, palInactive;
 palActive.setColor(QPalette::Normal, QPalette::Button, buttonsSelectColor);
 palActive.setColor(QPalette::Normal, QPalette::Window, buttonsSelectColor);

 alignLeft->setPalette(palInactive);
 alignCenter->setPalette(palInactive);
 alignRight->setPalette(palInactive);
 alignWidth->setPalette(palInactive);

 if(activate==false)return;

 if(textArea->alignment()==Qt::AlignLeft)         alignLeft->setPalette(palActive);
 else if(textArea->alignment()==Qt::AlignHCenter) alignCenter->setPalette(palActive);
 else if(textArea->alignment()==Qt::AlignRight)   alignRight->setPalette(palActive);
 else if(textArea->alignment()==Qt::AlignJustify) alignWidth->setPalette(palActive);
}


// Обновление подсветки клавиш начертания текста
void Editor::update_outline_button_higlight(void)
{
 QPalette palActive, palInactive;
 palActive.setColor(QPalette::Normal, QPalette::Button, buttonsSelectColor);
 palActive.setColor(QPalette::Normal, QPalette::Window, buttonsSelectColor);

 bold->setPalette(palInactive);
 italic->setPalette(palInactive);
 underline->setPalette(palInactive);

 if(textArea->fontWeight()==QFont::Bold) bold->setPalette(palActive);
 if(textArea->fontItalic()==true)        italic->setPalette(palActive);
 if(textArea->fontUnderline()==true)     underline->setPalette(palActive);
}


void Editor::set_outline_button_higlight(int button, bool active)
{
 QPalette palActive, palInactive;
 palActive.setColor(QPalette::Normal, QPalette::Button, buttonsSelectColor);
 palActive.setColor(QPalette::Normal, QPalette::Window, buttonsSelectColor);

 if(button==BT_BOLD)
 {
  if(active==false) bold->setPalette(palInactive);
  else              bold->setPalette(palActive);
  return;
 }

 if(button==BT_ITALIC)
 {
  if(active==false) italic->setPalette(palInactive);
  else              italic->setPalette(palActive);
  return;
 }

 if(button==BT_UNDERLINE)
 {
  if(active==false) underline->setPalette(palInactive);
  else              underline->setPalette(palActive);
  return;
 }
}


// Показывание окна с исходным текстом HTML
void Editor::on_showhtml_clicked(void)
{
 // info_window(textArea->toHtml());

 EditorMultiLineInputDialog dialog(this);

 dialog.set_text(textArea->toHtml());
 dialog.set_window_title(tr("Edit HTML source"));

 if(dialog.exec()!=QDialog::Accepted)
  return;

 textArea->setHtml(dialog.get_text());

 // Если в диалоговом окне был иземен HTML код
 if(dialog.isModified()) 
  textArea->document()->setModified(true);
}


void Editor::on_findtext_clicked(void)
{
 findDialog->show();
 findDialog->activateWindow();
}


// Слот, принимающий данные от окна поиска текста
void Editor::on_findtext_signal_detect(const QString &text, QTextDocument::FindFlags flags)
{
 qDebug() << "Find text " << text << " with flags " << flags;

 if(!textArea->find(text, flags))
 {
  findDialog->hide();
  QMessageBox::information(this,
                           tr("Search result"),
                           tr("String '<b>")+text+tr("</b>' not found"),
                           QMessageBox::Close);
 }
}

 
// Открытие контекстного меню в области редактирования
void Editor::on_customContextMenuRequested(const QPoint &pos)
{
  qDebug() << "In Editor on_customContextMenuRequested";

  // Конструирование меню
  // editor_context_menu=textarea->createStandardContextMenu();

  // Включение отображения меню на экране
  // menu.exec(event->globalPos());

  // Если выбрана картинка
  // Или нет выделения, но курсор находится на позиции картинки
  if(is_image_select() ||
     is_cursor_on_image())
   editorContextMenu->set_edit_image_properties( true );
  else
   editorContextMenu->set_edit_image_properties( false );

  // Контекстное меню запускается
  editorContextMenu->exec(textArea->viewport()->mapToGlobal(pos));
}


void Editor::on_settings_clicked(void)
{
 // Создается окно настроек, после выхода из этой функции окно удалится
 EditorConfigDialog dialog;
 dialog.show();
}


// Действия при нажатии кнопки отображения символов редактирования
void Editor::on_showformatting_clicked(void)
{
 QPalette palActive, palInactive;
 palActive.setColor(QPalette::Normal, QPalette::Button, buttonsSelectColor);
 palActive.setColor(QPalette::Normal, QPalette::Window, buttonsSelectColor);

 if(textArea->get_showformatting()==false)
  {
   textArea->set_showformatting(true);
   showFormatting->setPalette(palActive);
  }
 else
  {
   textArea->set_showformatting(false);
   showFormatting->setPalette(palInactive);
  }
}


// Действия при перемещении движка абзацного отступа
void Editor::on_indentline_change_textindent_pos(int i)
{
 // Создание форматирования
 QTextBlockFormat indentFormatting;
 indentFormatting.setTextIndent(i);

 // Форматирование для добавления отступа
 textArea->textCursor().mergeBlockFormat(indentFormatting);

 // Редактор запоминает отступ, чтобы было с чем сравнивать
 // при перемещении курсора со строки на строку
 currentTextIndent=i;

 textArea->show_indetedge(true);
 textArea->set_indentedge_pos(indentSlider->get_current_indentedge_pos()-textArea->lineWidth()-1);
}


// Действия при перемещении движка левого отступа
void Editor::on_indentline_change_leftindent_pos(int i)
{
 // Создание форматирования
 QTextBlockFormat indentFormatting;
 indentFormatting.setLeftMargin(i);

 // Форматирование для добавления отступа
 textArea->textCursor().mergeBlockFormat(indentFormatting);

 // Редактор запоминает отступ, чтобы было с чем сравнивать
 // при перемещении курсора со строки на строку
 currentLeftIndent=i;

 textArea->show_indetedge(true);
 textArea->set_indentedge_pos(indentSlider->get_current_indentedge_pos()-textArea->lineWidth()-1);
}

// Действия при перемещении движка правого отступа
void Editor::on_indentline_change_rightindent_pos(int i)
{
 // Создание форматирования
 QTextBlockFormat indentFormatting;
 indentFormatting.setRightMargin(i);

 // Форматирование для добавления отступа
 textArea->textCursor().mergeBlockFormat(indentFormatting);

 // Редактор запоминает отступ, чтобы было с чем сравнивать
 // при перемещении курсора со строки на строку
 currentRightIndent=i;

 textArea->show_indetedge(true);
 textArea->set_indentedge_pos(indentSlider->get_current_indentedge_pos()-textArea->lineWidth()-1);
}


// Действия в момент отпускания кнопки мышки над линейкой отступов
void Editor::on_indentline_mouse_release(void)
{
 textArea->show_indetedge(false); // Скрывается вертикальная линия
 textArea->set_indentedge_pos(0); // Координата вертикальной линии обнуляется
}


// Действия при нажатии кнопки создания новой таблицы
void Editor::on_createtable_clicked(void)
{
 // Создается и запускается диалог создания новой таблицы
 EditorAddTableForm dialog;
 if(dialog.exec()!=QDialog::Accepted) return;

 // Выясняются введенные в диалоге данные
 int tableColumns=dialog.get_columns();
 int tableRows=dialog.get_rows();
 int tableWidth=dialog.get_width();

 // Целочислительный формат ширины таблицы преобразуется в проценты
 QTextLength tableWidthInPercent(QTextLength::PercentageLength, tableWidth);

 // Создается форматирование таблицы
 QTextTableFormat tableFormat;
 tableFormat.setWidth(tableWidthInPercent);
 tableFormat.setAlignment(Qt::AlignHCenter);
 tableFormat.setBorder(1);
 tableFormat.setBorderStyle(QTextFrameFormat::BorderStyle_Solid);
 tableFormat.setPadding(0);
 tableFormat.setCellPadding(0);
 tableFormat.setCellSpacing(-1);

 // Добавляется таблица с нужными размерами и форматированием
 // QTextTable *table=textarea->textCursor().insertTable(table_rows, table_columns, table_format);
 textArea->textCursor().insertTable(tableRows, tableColumns, tableFormat);

 return;

 /*
 // Вставка новой таблицы через генериацию HTML кода
 // Если данные введены нормально
 if(table_vnum > 0 && table_hnum > 0 && table_width > 0)
  {
   QStringList tab;
   tab.clear();

   // QColor table_color;
   // table_color=qApp->palette().color(QPalette::Normal, QPalette::Base);
   // qDebug() << "Table background color " << table_color.name();
   // tab.append(QString("<table border=\"1\" align=\"center\" width=\"%1%\" cellspacing=\"0\" cellpadding=\"0\" bgcolor=\"%2%\">").arg(table_width).arg(table_color.name()));

   tab.append(QString("<table style=\"border-style:solid; border-right:none; border-bottom:none;\" border=\"1\" align=\"center\" width=\"%1%\" cellspacing=\"0\" cellpadding=\"3\">").arg(table_width));

   for (int i=0;i<table_hnum;i++)
    {
     tab.append(QString("<tr>"));
     for (int o=0;o<table_vnum;o++)
      {
       tab.append(QString("<td style=\"border-style:solid; border-right:none; border-bottom:none;\"><p></p></td>"));
      }
     tab.append(QString("</tr>"));
    }

   tab.append(QString("</table>"));

   QString table_text = tab.join("\n");
   QTextDocumentFragment fragment = QTextDocumentFragment::fromHtml(table_text);
   textarea->textCursor().insertFragment(fragment);
  }
 */

 /*
 // Вставка кода таблицы из файла
 QFile file("table.txt");
 if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
  critical_error("Can not open table file");

 QString tablecode;
 while(!file.atEnd())
 {
  QString line=file.readLine();
  tablecode=tablecode+line+"\n";
 }

 file.close();

 QTextDocumentFragment fragment = QTextDocumentFragment::fromHtml(tablecode);
 textarea->textCursor().insertFragment(fragment);

 return;
 */

}


// Удаление строк из таблицы
void Editor::on_table_remove_row_clicked(void)
{
 QTextCursor cursor(textArea->textCursor());
 QTextTable *table=cursor.currentTable();
 if(table!=0)
  {
   QTextTableCell cell=table->cellAt(cursor);
   int cellRowCursor=cell.row(); // Текущий номер строки (счет с нуля)
   table->removeRows(cellRowCursor,1); // Удаляются строки таблицы
  }
}


// Удаление столбцов из таблицы
void Editor::on_table_remove_col_clicked(void)
{
 QTextCursor cursor(textArea->textCursor());
 QTextTable *table=cursor.currentTable();
 if(table!=0)
  {
   QTextTableCell cell=table->cellAt(cursor);
   int cellColCursor=cell.column(); // Текущий номер столбца (счет с нуля)
   table->removeColumns(cellColCursor,1); // Удаляются столбцы таблицы
  }
}


// Добавление строк в таблицу
void Editor::on_table_add_row_clicked(void)
{
 QTextCursor cursor(textArea->textCursor());
 QTextTable *table = cursor.currentTable();
 if(table)
  {
   QTextTableCell cell=table->cellAt(cursor); // Выясняется текущая ячейка
   int cellRowCursor=cell.row(); // Текущий номер строки (счет с нуля)

   bool ok=false;
   int addNum=QInputDialog::getInt(this, tr("Append rows to table"),tr("Append rows:"), 1, 1, 100, 1, &ok);

   if(ok && addNum > 0)
    table->insertRows(cellRowCursor + 1,addNum);
  }
}


// Добавление столбцов в таблицу
void Editor::on_table_add_col_clicked(void)
{
 QTextCursor cursor(textArea->textCursor());
 QTextTable *table = cursor.currentTable();
 if(table)
  {
   QTextTableCell cell=table->cellAt(cursor); // Выясняется текущая ячейка
   int cellColCursor=cell.column(); // Текущий номер столбца (счет с нуля)

   bool ok=false;
   int addNum=QInputDialog::getInt(this, tr("Append columns to table"),tr("Append columns:"), 1, 1, 100, 1, &ok);

   if(ok && addNum > 0)
    table->insertColumns(cellColCursor + 1,addNum);
  }
}


// Объединение ячеек таблицы
void Editor::on_table_merge_cells_clicked(void)
{
 QTextCursor cursor(textArea->textCursor());
 QTextTable *table=cursor.currentTable();
 if(table)
  table->mergeCells(cursor);
}


void Editor::on_table_split_cell_clicked(void)
{
 QTextCursor cursor(textArea->textCursor());
 QTextTable *table=cursor.currentTable();

 // Если курсор находится где-то внутри таблицы
 if(table)
  {
   // Выясняется, выделены ячейки или нет
   int *currX=new int;
   int *currY=new int;
   int *currWidthX=new int;
   int *currWidthY=new int;
   cursor.selectedTableCells(currX,currY,currWidthX,currWidthY);

   // Если ячейки выделены, и захвачено для выделения более одной ячейки
   if(*currWidthX>1 || *currWidthY>1)
    {
     // Работа возможна только с одной ячейкой
     QMessageBox msgBox;
     msgBox.setWindowTitle(tr("Split cell"));
     msgBox.setText(tr("Select a single cell."));
     msgBox.exec();
     return;
    }

   QTextTableCell cell=table->cellAt(cursor); // Выясняется текущая ячейка
   int cellColCursor=cell.column(); // Текущий номер столбца (счет с нуля)
   int cellRowCursor=cell.row(); // Текущий номер строки (счет с нуля)
   qDebug() << "Current cell is " << cellColCursor << cellRowCursor;

   // Курсором выделяется таблица
   cursor.setPosition(table->firstCursorPosition().position() - 1, QTextCursor::MoveAnchor);
   cursor.setPosition(table->lastCursorPosition().position() + 1, QTextCursor::KeepAnchor);

   // Выделенный фрагмент выделяется в отдельный объект
   QTextDocumentFragment fragment(cursor);

   EditorAbsTable current_table(table->columns(), table->rows(), fragment.toHtml());

   qDebug() << "This cell colspan " << current_table.get_cell_colspan(cellColCursor,cellRowCursor) << " rowspan " << current_table.get_cell_rowspan(cellColCursor,cellRowCursor);


   // Выясняется, на сколько сегментов можно разбивать ячейку
   int hSplitTo;
   int vSplitTo;
   int hCurrMerge=current_table.get_cell_colspan(cellColCursor,cellRowCursor);
   int vCurrMerge=current_table.get_cell_rowspan(cellColCursor,cellRowCursor);

   if(hCurrMerge==0)hCurrMerge=1;
   if(vCurrMerge==0)vCurrMerge=1;

   if(hCurrMerge==1) hSplitTo=0; // Можно разбивать на любое число
   else              hSplitTo=hCurrMerge;

   if(vCurrMerge==1) vSplitTo=0; // Можно разбивать на любое число
   else              vSplitTo=vCurrMerge;


   // Создается диалог запроса на сколько сегментов надо разбивать ячейку
   EditorSplitCellForm dialog(hSplitTo, vSplitTo);
   if(dialog.exec()==QDialog::Accepted)
    {
     int hSplit=dialog.get_num_split_h(); // Эти значения всегда 1 или больше
     int vSplit=dialog.get_num_split_v();

     // Разбивка по горизонтали
     if(hSplit>1)
      {
       if(hCurrMerge==1)
        current_table.split_single_cell_by_horisontal(cellColCursor,cellRowCursor,hSplit);
       else
        current_table.split_merged_cell_by_horisontal(cellColCursor,cellRowCursor);
      }

     // Разбивка по вертикали
     if(vSplit>1)
      {
       if(vCurrMerge==1)
        current_table.split_single_cell_by_vertical(cellColCursor,
                                                    cellColCursor+hSplit-1,
                                                    cellRowCursor,
                                                    vSplit);
       else
        current_table.split_merged_cell_by_vertical(cellColCursor,
                                                    cellColCursor+hSplit-1,
                                                    cellRowCursor);
      }

     qDebug() << "New table HTML text " << current_table.get_table();

     cursor.beginEditBlock();
     cursor.deleteChar();
     cursor.insertHtml(current_table.get_table());
     cursor.endEditBlock();

     return;
    }
   else
    {

    }


  } // Закончилось условие что курсор находится где-то внутри таблицы

}


void Editor::on_context_menu_edit_image_properties()
{
 // Если выделена картинка
 if(is_image_select() ||
    is_cursor_on_image())
  {
   qDebug() << "Image selected";

   edit_image_properties();
  }
}


// Метод, определяющий, выбрана ли только одна картинка
bool Editor::is_image_select(void)
{
 // Происходит анализ, выделена ли картинка
 bool is_image_select_flag=false;

 // Блок, в пределах которого находится курсор
 QTextBlock currentBlock = textArea->textCursor().block();
 QTextBlock::iterator it;
 QTextFragment fragment;

 // Если есть выделение
 if(textArea->textCursor().hasSelection())
  {
   // Перебиратся фрагметы блока
   for(it = currentBlock.begin(); !(it.atEnd()); ++it)
    {
     fragment = it.fragment();

     // Если фрагмент содержит изображение
     if(fragment.isValid())
      if(fragment.charFormat().isImageFormat ())
       {
        int fragmentStart=fragment.position();
        int fragmentEnd=fragmentStart+fragment.length();
        int selectionStart=textArea->textCursor().selectionStart();
        int selectionEnd=textArea->textCursor().selectionEnd();

        // Если начало и конец фрагмента совпадает с координатами выделения
        // Проверяется и случай, когда блок выделен в обратную сторону
        if( (fragmentStart==selectionStart && fragmentEnd==selectionEnd) ||
            (fragmentStart==selectionEnd && fragmentEnd==selectionStart) )
         {
          is_image_select_flag=true;
          break;
         }
       }
    }
  }

 return is_image_select_flag;
}


// Формат картинки, которая выделена (если выделена единственная картинка)
QTextImageFormat Editor::image_format_on_select(void)
{
 // Блок, в пределах которого находится курсор
 QTextBlock currentBlock = textArea->textCursor().block();
 QTextBlock::iterator it;
 QTextFragment fragment;

 // Если есть выделение
 if(textArea->textCursor().hasSelection())
  {
   // Перебиратся фрагметы блока
   for(it = currentBlock.begin(); !(it.atEnd()); ++it)
    {
     fragment = it.fragment();

     // Если фрагмент содержит изображение
     if(fragment.isValid())
      if(fragment.charFormat().isImageFormat ())
       {
        int fragmentStart=fragment.position();
        int fragmentEnd=fragmentStart+fragment.length();
        int selectionStart=textArea->textCursor().selectionStart();
        int selectionEnd=textArea->textCursor().selectionEnd();

        // Если начало и конец фрагмента совпадает с координатами выделения
        // Проверяется и случай, когда блок выделен в обратную сторону
        if( (fragmentStart==selectionStart && fragmentEnd==selectionEnd) ||
            (fragmentStart==selectionEnd && fragmentEnd==selectionStart) )
         {
          QTextImageFormat imageFormat=fragment.charFormat().toImageFormat();

          return imageFormat;
         }
       }
    }
  }

 return QTextImageFormat();
}


// Проверка, находится ли курсор на позиции, где находится картинка
bool Editor::is_cursor_on_image(void)
{
 // Проверка срабатывает только если нет выделения
 if(textArea->textCursor().hasSelection()==false)
  {
   QTextImageFormat imageFormat = textArea->textCursor().charFormat().toImageFormat();

   if(imageFormat.isValid())
    return true;
  }

 return false;
}


// Формат картинки на которой находится курсор
QTextImageFormat Editor::image_format_on_cursor(void)
{
 // Проверка срабатывает только если нет выделения
 if(textArea->textCursor().hasSelection()==false)
  {
   QTextImageFormat imageFormat = textArea->textCursor().charFormat().toImageFormat();

   if(imageFormat.isValid())
    return imageFormat;
  }

 return QTextImageFormat();
}


// Обработка клавиши добавления картинки
void Editor::on_insert_image_from_file_clicked(void)
{

 // Если выделена картинка
 if(is_image_select() ||
    is_cursor_on_image())
  {
   qDebug() << "Image selected";

   edit_image_properties();
  }
 else
  {
   // Иначе картинка не выделена, и срабатывает режим добавления
   // картинки из файла

   // Диалог выбора файлов катинок
   QFileDialog imageSelectDialog(this);
   imageSelectDialog.setFileMode(QFileDialog::ExistingFiles); // QFileDialog::ExistingFile
   imageSelectDialog.setNameFilter("*.png *.jpg *.gif");
   imageSelectDialog.setWindowTitle(tr("Insert image"));
   imageSelectDialog.setDirectory(QDir::homePath());
   imageSelectDialog.exec();

   // Выясняется список выбранных файлов
   QStringList files=imageSelectDialog.selectedFiles();

   // Если ни один файл не выбран
   if(files.size()==0) return;

   // Перебираются файлы выбранных картинок
   for(int i=0; i<files.size(); ++i)
    {
     // Текущее имя файла
     QString currFileName=files.at(i);

     // Картинка загружается из файла
     QImage image = QImageReader(currFileName).read();

     // Внутреннее имя картинки
     QString imageName="image"+QString::number(rand())+".png";

     // Картинка добавляется в хранилище документа
     textArea->document()->addResource(QTextDocument::ImageResource, QUrl(imageName), image );

     // Создается описание форматированной картинки
     // QTextImageFormat imageFormat;
     // imageFormat.setName(link.toString());

     // Картинка вставляется в текст
     QTextCursor cursor=textArea->textCursor();
     cursor.insertImage(imageName);
    } // Закончился цикл перебора файлов картинок
  } // Завершилось условие что картинка не выбрана и нужно добавлять из файла

}


void Editor::edit_image_properties(void)
{
 // Данные обрабатываемой картинки
 QTextImageFormat imageFormat;

 // Если выбрано изображение
 if(is_image_select())
  imageFormat=image_format_on_select();

 // Если изображение не выбрано, но курсор находится в позиции изображения
 if(is_cursor_on_image())
  imageFormat=image_format_on_cursor();
  

 // Выясняется имя картинки в ресурсах документа
 QString imageName=imageFormat.name();

 // По имени из ресурсов вытягивается кратинка
 QUrl urlName(imageName);
 QVariant imageData=textArea->document()->resource(QTextDocument::ImageResource, urlName);
 QImage image=imageData.value<QImage>();

 // Выяснятся реальные размеры картики
 int realImageWidth=image.width();
 int realImageHeight=image.height();

 qDebug() << "Real image width " << realImageWidth << " height " << realImageHeight;
 qDebug() << "Format image width " << imageFormat.width() << " height " << imageFormat.height();

 // Создается и запускается диалог запроса размеров картинки
 EditorImageProperties dialog;
 dialog.set_info(tr("Real image size ") +
                 QString::number(realImageWidth) +
                 " x " +
                 QString::number(realImageHeight) +
                 tr(" pixels"));
 dialog.set_real_width(realImageWidth);
 dialog.set_real_height(realImageHeight);

 // Если в форматировании картинки не задан размер картинки
 if(imageFormat.width()==0 && imageFormat.height()==0)
  {
   // В окне настройки стартовый размер задается как размер картинки
   dialog.set_width(realImageWidth);
   dialog.set_height(realImageHeight);
  }
 else
  {
   // В окне настройки стартовый размер задается согласно форматированию
   dialog.set_width(imageFormat.width());
   dialog.set_height(imageFormat.height());
  }

 dialog.update_percent();


 // Запуск диалога на выполнение
 if(dialog.exec()!=QDialog::Accepted)
  return;

 imageFormat.setWidth(dialog.get_width());
 imageFormat.setHeight(dialog.get_height());

 // Если в новом формате картинки нет никаких ошибок
 if(imageFormat.isValid())
  {

   // Если выбрано изображение
   if(is_image_select())
    {
     QTextFragment fragment;

     // Блок, в пределах которого находится курсор
     QTextBlock currentBlock = textArea->textCursor().block();
     QTextBlock::iterator it;

     // Перебиратся фрагметы блока
     // Так как известно, что картинка выделена, поиск фрагмента будет успешным
     for(it = currentBlock.begin(); !(it.atEnd()); ++it)
      {
       fragment = it.fragment();

       // Если фрагмент содержит изображение
       if(fragment.isValid() &&
          fragment.charFormat().isImageFormat())
        break; // Переменная fragment содержит только картинку
      }


     QTextCursor helper = textArea->textCursor();

     helper.setPosition(fragment.position());
     helper.setPosition(fragment.position() + fragment.length(),
                                            QTextCursor::KeepAnchor);
     helper.setCharFormat(imageFormat);
    }

   
   // Если изображение не выбрано, но курсор находится в позиции изображения
   if(is_cursor_on_image())
    {
     int cursorPosition=textArea->textCursor().position();

     QTextCursor helper=textArea->textCursor();

     helper.setPosition(cursorPosition);

     if(textArea->textCursor().atBlockStart())
      helper.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
     else
      helper.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);

     helper.setCharFormat(imageFormat);
    }
  }

}


void Editor::on_expand_edit_area_clicked(void)
{
 if(expand_edit_area_flag==false)
  {
   expand_edit_area_flag=true;
   emit send_expand_edit_area(expand_edit_area_flag);
  }
 else
  {
   expand_edit_area_flag=false;
   emit send_expand_edit_area(expand_edit_area_flag);
  }
}


void Editor::on_expand_tools_lines_clicked(void)
{
 switch_expand_tools_lines();
}


// Метод, переключающий состояние видимости полной панели инструментов
// Если вызывается без параметра (по умолчанию 0), метод сам переключает
// Параметр 1 - включить полную видимость
// Параметр -1 - выключить полную видимость
void Editor::switch_expand_tools_lines(int flag)
{
 bool setFlag=true;

 // Если метод был вызван без параметра
 if(flag==0)
  {
   bool is_expand=editorConfig->get_expand_tools_lines();

   if(is_expand) setFlag=false; // Если панель инструментов распахнута, надо сомкнуть
   else setFlag=true; // Иначе распахнуть
  }
 else
  {
   // Иначе метод вызывался с каким-то параметром
   if(flag==1) setFlag=true;
   if(flag==-1) setFlag=false;
  }


 // Панели распахиваются/смыкаются (кроме первой линии инструментов)
 toolsLine2->setVisible(setFlag);
 if(viewMode==WYEDIT_DESKTOP_MODE)
   indentSlider->setVisible(setFlag);

 // Запоминается новое состояние
 editorConfig->set_expand_tools_lines(setFlag);

 // На всякий случай обновляется геометрия расположения движков на слайд-панели
 update_indentline_geometry();
}


void Editor::on_save_clicked(void)
{
  save_textarea();
}


void Editor::on_back_clicked(void)
{
  // back_callback_func(qobject_cast<QObject *>(this));
 back_callback_func();
}


void Editor::on_find_in_base_clicked(void)
{
 emit wyeditFindInBaseClicked();
}


void Editor::set_save_callback(void (*func)(QObject *editor, QString saveString))
{
  save_callback_func=func;
}


void Editor::set_back_callback(void (*func)(void))
{
  back_callback_func=func;
}


void Editor::set_load_callback(void (*func)(QObject *editor, QString &String))
{
  load_callback_func=func;
}


void Editor::setMiscField(QString name, QString value)
{
  miscFields[name]=value;
}


QString Editor::getMiscField(QString name)
{
  if(miscFields.contains(name))
    return miscFields[name];
  else
    return QString();
}


void Editor::setDirFileEmptyReaction(int mode)
{
  // Проверяется допустимость переданного значения
  if(mode==DIRFILEEMPTY_REACTION_SHOW_ERROR ||
     mode==DIRFILEEMPTY_REACTION_SUPPRESS_ERROR)
    dirFileEmptyReaction=mode;
  else
    critical_error("Editor::setDirFileEmptyReaction() : Unsupport mode "+QString::number(mode));
}


int Editor::getDirFileEmptyReaction(void)
{
  return dirFileEmptyReaction;
}


// Метод позволяющий управлять доступностью инcтрументов редактирования
void Editor::setDisableToolList(QStringList toolNames)
{
  qDebug() << "Editor::setDisableToolList() : " << toolNames;
  disableToolList=toolNames;
}


int Editor::getCursorPosition(void)
{
  return textArea->textCursor().position();
}


void Editor::setCursorPosition(int n)
{
  QTextCursor cursor=textArea->textCursor();

  cursor.setPosition(n);

  textArea->setTextCursor(cursor);
}


int Editor::getScrollBarPosition(void)
{
  return textArea->verticalScrollBar()->value();
}


void Editor::setScrollBarPosition(int n)
{
  textArea->verticalScrollBar()->setValue(n);
}

