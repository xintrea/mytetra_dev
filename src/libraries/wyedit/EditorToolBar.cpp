#include <QDebug>

#include "main.h"
#include "EditorToolBar.h"
#include "Editor.h"


EditorToolBar::EditorToolBar(QWidget *parent) : QWidget(parent)
{
  isInit=false;
}


EditorToolBar::~EditorToolBar()
{

}


void EditorToolBar::initToolsLine1(QStringList toolsLine)
{
  if(isInit)
    criticalError("Method "+QString(__FUNCTION__)+" has running before init() only.");

  toolsListInLine1=toolsLine;
}


void EditorToolBar::initToolsLine2(QStringList toolsLine)
{
  if(isInit)
    criticalError("Method "+QString(__FUNCTION__)+" has running before init() only.");

  toolsListInLine2=toolsLine;
}


void EditorToolBar::initDisableToolList(QStringList toolNames)
{
  if(isInit)
    criticalError("Method "+QString(__FUNCTION__)+" has running before init() only.");

  disableToolList=toolNames;
}


// Инициализация панели инструментов
// Если mode=WYEDIT_DESKTOP_MODE - происходит обычная инициализация
// Если mode=WYEDIT_MOBILE_MODE - при инициализации в первую строку панели инструментов, слева, добавляется кнопка back
void EditorToolBar::init()
{
  isInit=true;

  setupButtons();
  assemblyButtons();
}


// Создание объектов кнопок для линейки форматирования текста
void EditorToolBar::setupButtons(void)
{
  // Для того, чтобы WyEdit нормально добавлял кнопки на панель согласно файлу editorconf.ini,
  // имена объектов кнопок должны начинаться на "editor_tb"

  // Кнопка Bold
  bold.setParent(this);
  bold.setCheckable(true);
  bold.setShortcut(QKeySequence(QKeySequence::Bold));
  bold.setStatusTip(tr("Bold (Ctrl+B)"));
  bold.setIcon(QIcon(":/resource/pic/edit_bold.svg"));
  bold.setObjectName("editor_tb_bold");

  // Кнопка Italic
  italic.setParent(this);
  italic.setCheckable(true);
  italic.setShortcut(QKeySequence(QKeySequence::Italic));
  italic.setStatusTip(tr("Italic (Ctrl+I)"));
  italic.setIcon(QIcon(":/resource/pic/edit_italic.svg"));
  italic.setObjectName("editor_tb_italic");

  // Кнопка Underline
  underline.setParent(this);
  underline.setCheckable(true);
  underline.setShortcut(QKeySequence(QKeySequence::Underline));
  underline.setStatusTip(tr("Underline (Ctrl+U)"));
  underline.setIcon(QIcon(":/resource/pic/edit_underline.svg"));
  underline.setObjectName("editor_tb_underline");

  // Кнопка Monospace
  monospace.setParent(this);
  monospace.setShortcut(QKeySequence(tr("Ctrl+T")));
  monospace.setStatusTip(tr("Monospace (Ctrl+T)"));
  monospace.setIcon(QIcon(":/resource/pic/edit_monospace.svg"));
  monospace.setObjectName("editor_tb_monospace");

  // Кнопка Code
  code.setParent(this);
  code.setShortcut(QKeySequence(tr("Ctrl+M")));
  code.setStatusTip(tr("Code (Ctrl+M). Select a whole paragraphs to format text as code."));
  code.setIcon(QIcon(":/resource/pic/edit_code.svg"));
  code.setObjectName("editor_tb_code");

  // Кнопка Clear
  clear.setParent(this);
  clear.setShortcut(QKeySequence(tr("Ctrl+K")));
  clear.setStatusTip(tr("Reset format (Ctrl+K). When selected whole paragraph both text and paragraph format is reset to default or just text format in other case."));
  clear.setIcon(QIcon(":/resource/pic/edit_clear.svg"));
  clear.setObjectName("editor_tb_clear");

  // Кнопка TextOnly - преобразование выделенного фрагмента в чистый текст
  textOnly.setParent(this);
  textOnly.setStatusTip(tr("Reset selected text to text only"));
  textOnly.setIcon(QIcon(":/resource/pic/edit_text_only.svg"));
  textOnly.setObjectName("editor_tb_text_only");

  // Кнопка замены символов мягкого переноса (тег <br/> заменяется на разрыв параграфа </p><p>)
  fixBreakSymbol.setParent(this);
  // fixBreakSymbol.setShortcut(QKeySequence(tr("Ctrl+K")));
  fixBreakSymbol.setStatusTip(tr("Replace soft carriage return to standard carriage return.")); // Замена символов мягкого перевода строк на обычный
  fixBreakSymbol.setIcon(QIcon(":/resource/pic/edit_fixBreakSymbol.svg"));
  fixBreakSymbol.setObjectName("editor_tb_fix_break_symbol");

  // Кнопка нумерованного списка
  numericList.setParent(this);
  numericList.setStatusTip(tr("Numeric list"));
  numericList.setIcon(QIcon(":/resource/pic/edit_listnumeric.svg"));
  numericList.setObjectName("editor_tb_numericlist");

  // Кнопка списка с точками
  dotList.setParent(this);
  dotList.setStatusTip(tr("Marked list"));
  dotList.setIcon(QIcon(":/resource/pic/edit_listdot.svg"));
  dotList.setObjectName("editor_tb_dotlist");


  // Кнопка увеличения отступа
  indentPlus.setParent(this);
  indentPlus.setStatusTip(tr("Increase indent"));
  indentPlus.setIcon(QIcon(":/resource/pic/edit_indentplus.svg"));
  indentPlus.setObjectName("editor_tb_indentplus");

  // Кнопка уменьшения отступа
  indentMinus.setParent(this);
  indentMinus.setStatusTip(tr("Decrease indent"));
  indentMinus.setIcon(QIcon(":/resource/pic/edit_indentminus.svg"));
  indentMinus.setObjectName("editor_tb_indentminus");


  // Кнопка выравнивания по левому краю
  alignLeft.setParent(this);
  alignLeft.setCheckable(true);
  alignLeft.setShortcut(QKeySequence(tr("Ctrl+L")));
  alignLeft.setStatusTip(tr("Align left (Ctrl+L)"));
  alignLeft.setIcon(QIcon(":/resource/pic/edit_alignleft.svg"));
  alignLeft.setObjectName("editor_tb_alignleft");

  // Кнопка выравнивания по центру
  alignCenter.setParent(this);
  alignCenter.setCheckable(true);
  alignCenter.setShortcut(QKeySequence(tr("Ctrl+E")));
  alignCenter.setStatusTip(tr("Align center (Ctrl+E)"));
  alignCenter.setIcon(QIcon(":/resource/pic/edit_aligncenter.svg"));
  alignCenter.setObjectName("editor_tb_aligncenter");

  // Кнопка выравнивания по правому краю
  alignRight.setParent(this);
  alignRight.setCheckable(true);
  alignRight.setShortcut(QKeySequence(tr("Ctrl+R")));
  alignRight.setStatusTip(tr("Align right (Ctrl+R)"));
  alignRight.setIcon(QIcon(":/resource/pic/edit_alignright.svg"));
  alignRight.setObjectName("editor_tb_alignright");

  // Кнопка выравнивания по ширине
  alignWidth.setParent(this);
  alignWidth.setCheckable(true);
  alignWidth.setShortcut(QKeySequence(tr("Ctrl+J")));
  alignWidth.setStatusTip(tr("Align width (Ctrl+J)"));
  alignWidth.setIcon(QIcon(":/resource/pic/edit_alignwidth.svg"));
  alignWidth.setObjectName("editor_tb_alignwidth");


  // Выбор шрифта
  fontSelect.setParent(this);
  fontSelect.setObjectName("editor_tb_fontselect");

  // Пустой пункт в конце списка шрифтов, используется для обозначения что в
  // выделенном тексте несколько шрифтов
  fontSelect.addItem(" ");
  fontSelect.setItemIcon((fontSelect.count())-1,QIcon(":/resource/pic/edit_font_many.svg"));


  // Выбор размера шрифта
  fontSize.setParent(this);
  for(int i=MINIMUM_ALLOWED_FONT_SIZE; i<=MAXIMUM_ALLOWED_FONT_SIZE; i++)
    fontSize.addItem(QString("%1").arg(i),i);
  fontSize.setCurrentIndex(fontSize.findData(10));
  fontSize.setMinimumContentsLength(3);
  fontSize.setEditable(true);
  QValidator *fontsizeValidator = new QIntValidator(MINIMUM_ALLOWED_FONT_SIZE, MAXIMUM_ALLOWED_FONT_SIZE, this);
  fontSize.setValidator(fontsizeValidator);
  fontSize.setObjectName("editor_tb_fontsize");

  // Пустой пункт в конце списка размеров шрифтов, используется для обозначения
  // что в выделенном тексте несколько размеров
  fontSize.addItem(" ",0);


  // Кнопка выбора цвета шрифта
  // fontColor.setShortcut(QKeySequence(tr("Ctrl+F")));
  fontColor.setParent(this);
  fontColor.setStatusTip(tr("Text color"));
  fontColor.setIcon(QIcon(":/resource/pic/edit_fontcolor.svg"));
  fontColor.setObjectName("editor_tb_fontcolor");


  // Кнопка вызова виджета поиска текста
  findText.setParent(this);
  findText.setShortcut(QKeySequence(tr("Ctrl+F")));
  findText.setStatusTip(tr("Find text (Ctrl+F)"));
  findText.setIcon(QIcon(":/resource/pic/edit_findtext.svg"));
  findText.setObjectName("editor_tb_findtext");

  // Кнопка вызова виджета конфигурирования редактора
  settings.setParent(this);
  settings.setStatusTip(tr("Editor settings"));
  settings.setIcon(QIcon(":/resource/pic/edit_settings.svg"));
  settings.setObjectName("editor_tb_settings");

  // Кнопка редактирования ссылки (URL)
  reference.setParent(this);
  reference.setStatusTip(tr("Edit reference (URL)"));
  reference.setIcon(QIcon(":/resource/pic/edit_reference.svg"));
  reference.setObjectName("editor_tb_reference");

  // Кнопка просмотра HTML кода
  showHtml.setParent(this);
  showHtml.setStatusTip(tr("Edit HTML code"));
  showHtml.setIcon(QIcon(":/resource/pic/edit_showhtml.svg"));
  showHtml.setObjectName("editor_tb_showhtml");

  // Кнопка включения отображения символов фарматирования
  showFormatting.setParent(this);
  showFormatting.setCheckable(true);
  showFormatting.setStatusTip(tr("Show special chars"));
  showFormatting.setIcon(QIcon(":/resource/pic/edit_showformatting.svg"));
  showFormatting.setObjectName("editor_tb_showformatting");

  // Кнопка добавления новой таблицы
  createTable.setParent(this);
  createTable.setStatusTip(tr("Create a new table"));
  createTable.setIcon(QIcon(":/resource/pic/edit_createtable.svg"));
  createTable.setObjectName("editor_tb_createtable");

  tableRemoveRow.setParent(this);
  tableRemoveRow.setStatusTip(tr("Remove row(s)"));
  tableRemoveRow.setIcon(QIcon(":/resource/pic/edit_table_remove_row.svg"));
  tableRemoveRow.setObjectName("editor_tb_table_remove_row");

  tableRemoveCol.setParent(this);
  tableRemoveCol.setStatusTip(tr("Remove column(s)"));
  tableRemoveCol.setIcon(QIcon(":/resource/pic/edit_table_remove_col.svg"));
  tableRemoveCol.setObjectName("editor_tb_table_remove_col");

  tableAddRow.setParent(this);
  tableAddRow.setStatusTip(tr("Add row(s)"));
  tableAddRow.setIcon(QIcon(":/resource/pic/edit_table_add_row.svg"));
  tableAddRow.setObjectName("editor_tb_table_add_row");

  tableAddCol.setParent(this);
  tableAddCol.setStatusTip(tr("Add column(s)"));
  tableAddCol.setIcon(QIcon(":/resource/pic/edit_table_add_col.svg"));
  tableAddCol.setObjectName("editor_tb_table_add_col");

  tableMergeCells.setParent(this);
  tableMergeCells.setStatusTip(tr("Merge cells"));
  tableMergeCells.setIcon(QIcon(":/resource/pic/edit_table_merge_cells.svg"));
  tableMergeCells.setObjectName("editor_tb_table_merge_cells");

  tableSplitCell.setParent(this);
  tableSplitCell.setStatusTip(tr("Split cell"));
  tableSplitCell.setIcon(QIcon(":/resource/pic/edit_table_split_cell.svg"));
  tableSplitCell.setObjectName("editor_tb_table_split_cell");

  tableProperties.setParent(this);
  tableProperties.setStatusTip(tr("Table properties"));
  tableProperties.setIcon(QIcon(":/resource/pic/edit_table_properties.svg"));
  tableProperties.setObjectName("editor_tb_table_properties");

  insertImageFromFile.setParent(this);
  insertImageFromFile.setStatusTip(tr("Insert image from file / edit image properties of selected image"));
  insertImageFromFile.setIcon(QIcon(":/resource/pic/edit_insert_image_from_file.svg"));
  insertImageFromFile.setObjectName("editor_tb_insert_image_from_file");

  expandEditArea.setParent(this);
  expandEditArea.setStatusTip(tr("Expand edit area"));
  expandEditArea.setIcon(QIcon(":/resource/pic/edit_expand_text_area.svg"));
  expandEditArea.setObjectName("editor_tb_expand_edit_area");

  expandToolsLines.setParent(this);
  expandToolsLines.setStatusTip(tr("Expand tools"));
  expandToolsLines.setIcon(QIcon(":/resource/pic/edit_expand_tools_lines.svg"));
  expandToolsLines.setObjectName("editor_tb_expand_tools_lines");

  save.setParent(this);
  save.setStatusTip(tr("Save (Ctrl+S)"));
  save.setShortcut(QKeySequence(tr("Ctrl+S")));
  save.setIcon(QIcon(":/resource/pic/edit_save.svg"));
  save.setObjectName("editor_tb_save");


  // Кнопка "назад", используется в мобильном интерфейсе
  back.setParent(this);
  back.setStatusTip(tr("Back"));
  back.setIcon(QIcon(":/resource/pic/mobile_back.svg"));
  back.setObjectName("editor_tb_back");

  // Кнопка "поиск по базе", используется в мобильном интерфейсе
  findInBase.setParent(this);
  findInBase.setStatusTip(tr("Find in base"));
  findInBase.setIcon(QIcon(":/resource/pic/find_in_base.svg"));
  findInBase.setObjectName("editor_tb_find_in_base");


  // Кнопка "показать текст" для просмотра текста в отдельном окне
  showText.setParent(this);
  showText.setStatusTip(tr("Show text in detached window"));
  showText.setIcon(QIcon(":/resource/pic/edit_show_text.svg"));
  showText.setObjectName("editor_tb_show_text");

  // Кнопка переключения на аттачи
  iconAttachNotExists = QIcon(":/resource/pic/attach.svg");
  iconAttachExists = QIcon(":/resource/pic/attach_exists.svg");
  toAttach.setParent(this);
  toAttach.setStatusTip(tr("Show attach files"));
  toAttach.setIcon(iconAttachNotExists);
  toAttach.setObjectName("editor_tb_attach");

  // Все только что созданные элементы скрываются
  hideAllToolsElements();
}


QList<QWidget *> EditorToolBar::getButtonWidgetList(void)
{
  QRegExp nameMask("editor_tb_.*");

  return this->findChildren<QWidget *>(nameMask); // QList<QWidget *> tb_tools_list=qFindChildren(qobject_cast<QObject *>(this),name_mask);
}


void EditorToolBar::hideAllToolsElements(void)
{
  QList<QWidget *> tbToolsList=getButtonWidgetList();

  for(int i=0;i<tbToolsList.size();++i)
    tbToolsList.at(i)->setVisible(false);
}


// Установка или снятие доступности кнопок, модифицирующих текст
void EditorToolBar::setEnableModifyTextButton(bool state)
{
  // Перечень суффиксов имен кнопок, которые не влияют на форматирование
  QStringList noModifyButton;
  noModifyButton << "settings" \
                 << "findtext" \
                 << "showformatting" \
                 << "expand_edit_area" \
                 << "expand_tools_lines" \
                 << "back" \
                 << "find_in_base"
                 << "show_text" \
                 << "attach";

  QList<QWidget *> tbToolsList=getButtonWidgetList();

  // Перебор всех кнопок
  for(int i=0; i<tbToolsList.size(); ++i)
    if( !noModifyButton.contains( tbToolsList.at(i)->objectName().replace("editor_tb_", "") ) ) // Если кнопка влияет на форматирование
      tbToolsList.at(i)->setEnabled(state);
}


// Сборка линейки редактирования текста в горизонтальную линейку
void EditorToolBar::assemblyButtons(void)
{
  // Заполнение горизонтальных линеек с кнопками форматирования текста
  updateToolsLines();

  /*
 QSize toolIconSize(16, 16);
 toolsLine1->setIconSize(toolIconSize);
 toolsLine2->setIconSize(toolIconSize);
 */

  // Горизонтальные линейки собираются
  textformatButtonsLayout.addWidget( &toolsLine1 );
  textformatButtonsLayout.addWidget( &toolsLine2 );

  // Полученый набор элементов устанавливается для текущего виджета
  setLayout( &textformatButtonsLayout );

  // Границы убираются, так как данный объект будет использоваться как виджет
  QLayout *lt;
  lt=layout();
  lt->setContentsMargins(0,0,0,0);
}


void EditorToolBar::updateToolsLines(void)
{
  for(int i=0;i<toolsListInLine1.size();++i)
  {
    QString b=toolsListInLine1.at(i).trimmed();
    insertButtonToToolsLine(b,toolsLine1);
  }

  for(int i=0;i<toolsListInLine2.size();++i)
  {
    QString b=toolsListInLine2.at(i).trimmed();
    insertButtonToToolsLine(b,toolsLine2);
  }
}


// В линейку кнопок вставляется нужный инструмент
// Метод принимает имя инструмента
// и указатель на линейку
void EditorToolBar::insertButtonToToolsLine(QString toolName, QToolBar &line)
{
  // qDebug() << "EditorToolBar::insert_button_to_tools_line() disableToolList : " << disableToolList;

  if(toolName=="separator")
  {
    line.addSeparator();
  }
  else
  {
    QString name("editor_tb_"+toolName);

    QWidget *tool=qobject_cast<QWidget *>(this->findChild<QObject *>(name));

    if(tool!=NULL)
    {
      // Если данный инструмент не содержится в списке заблокированных
      if(!disableToolList.contains(toolName))
      {
        tool->setVisible(true);

        line.addWidget(tool); // Инструмент добавляется на панель инструментов

        QToolButton *tb=qobject_cast<QToolButton*>(tool);
        if(tb!=0)
          tb->setAutoRaise(false);
      }
      else
      {
        QToolButton *tb=qobject_cast<QToolButton*>(tool);
        if(tb!=0)
          tb->setEnabled(false);
      }
    }
    else
      criticalError("WyEdit: Can not find editor tool with name '"+toolName+"'. Please check editor *.ini file");
  }
}


void EditorToolBar::switchAttachIconExists(bool isExists)
{
  if(isExists)
    toAttach.setIcon( iconAttachExists );
  else
    toAttach.setIcon( iconAttachNotExists );
}
