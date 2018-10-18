#include <QDebug>

#include "main.h"
#include "EditorToolBar.h"
#include "Editor.h"
#include "libraries/ShortcutManager.h"

extern ShortcutManager shortcutManager;

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
  setupShortcuts();
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
  bold.setIcon(QIcon(":/resource/pic/edit_bold.svg"));
  bold.setObjectName("editor_tb_bold");

  // Кнопка Italic
  italic.setParent(this);
  italic.setCheckable(true);
  italic.setIcon(QIcon(":/resource/pic/edit_italic.svg"));
  italic.setObjectName("editor_tb_italic");

  // Кнопка Underline
  underline.setParent(this);
  underline.setCheckable(true);
  underline.setIcon(QIcon(":/resource/pic/edit_underline.svg"));
  underline.setObjectName("editor_tb_underline");

  // Кнопка Monospace
  monospace.setParent(this);
  monospace.setIcon(QIcon(":/resource/pic/edit_monospace.svg"));
  monospace.setObjectName("editor_tb_monospace");

  // Кнопка Code
  code.setParent(this);
  code.setIcon(QIcon(":/resource/pic/edit_code.svg"));
  code.setObjectName("editor_tb_code");

  // Кнопка Clear
  clear.setParent(this);
  clear.setIcon(QIcon(":/resource/pic/edit_clear.svg"));
  clear.setObjectName("editor_tb_clear");

  // Кнопка TextOnly - преобразование выделенного фрагмента в чистый текст
  textOnly.setParent(this);
  textOnly.setIcon(QIcon(":/resource/pic/edit_text_only.svg"));
  textOnly.setObjectName("editor_tb_text_only");

  // Кнопка замены символов мягкого переноса (тег <br/> заменяется на разрыв параграфа </p><p>)
  fixBreakSymbol.setParent(this);
  fixBreakSymbol.setIcon(QIcon(":/resource/pic/edit_fixBreakSymbol.svg"));
  fixBreakSymbol.setObjectName("editor_tb_fix_break_symbol");

  // Кнопка нумерованного списка
  numericList.setParent(this);
  numericList.setIcon(QIcon(":/resource/pic/edit_listnumeric.svg"));
  numericList.setObjectName("editor_tb_numericlist");

  // Кнопка списка с точками
  dotList.setParent(this);
  dotList.setIcon(QIcon(":/resource/pic/edit_listdot.svg"));
  dotList.setObjectName("editor_tb_dotlist");


  // Кнопка увеличения отступа
  indentPlus.setParent(this);
  indentPlus.setIcon(QIcon(":/resource/pic/edit_indentplus.svg"));
  indentPlus.setObjectName("editor_tb_indentplus");

  // Кнопка уменьшения отступа
  indentMinus.setParent(this);
  indentMinus.setIcon(QIcon(":/resource/pic/edit_indentminus.svg"));
  indentMinus.setObjectName("editor_tb_indentminus");


  // Кнопка выравнивания по левому краю
  alignLeft.setParent(this);
  alignLeft.setCheckable(true);
  alignLeft.setIcon(QIcon(":/resource/pic/edit_alignleft.svg"));
  alignLeft.setObjectName("editor_tb_alignleft");

  // Кнопка выравнивания по центру
  alignCenter.setParent(this);
  alignCenter.setCheckable(true);
  alignCenter.setIcon(QIcon(":/resource/pic/edit_aligncenter.svg"));
  alignCenter.setObjectName("editor_tb_aligncenter");

  // Кнопка выравнивания по правому краю
  alignRight.setParent(this);
  alignRight.setCheckable(true);
  alignRight.setIcon(QIcon(":/resource/pic/edit_alignright.svg"));
  alignRight.setObjectName("editor_tb_alignright");

  // Кнопка выравнивания по ширине
  alignWidth.setParent(this);
  alignWidth.setCheckable(true);
  alignWidth.setIcon(QIcon(":/resource/pic/edit_alignwidth.svg"));
  alignWidth.setObjectName("editor_tb_alignwidth");


  // Выбор шрифта
  fontSelect.setParent(this);
  fontSelect.setObjectName("editor_tb_fontselect");


  // Выбор размера шрифта
  fontSize.setParent(this);
  fontSize.setObjectName("editor_tb_fontsize");


  // Кнопка выбора цвета шрифта
  fontColor.setParent(this);
  fontColor.setIcon(QIcon(":/resource/pic/edit_fontcolor.svg"));
  fontColor.setObjectName("editor_tb_fontcolor");


  // Кнопка вызова виджета поиска текста
  findText.setParent(this);
  findText.setIcon(QIcon(":/resource/pic/edit_findtext.svg"));
  findText.setObjectName("editor_tb_findtext");

  // Кнопка вызова виджета конфигурирования редактора
  settings.setParent(this);
  settings.setIcon(QIcon(":/resource/pic/edit_settings.svg"));
  settings.setObjectName("editor_tb_settings");

  // Кнопка редактирования ссылки (URL)
  reference.setParent(this);
  reference.setIcon(QIcon(":/resource/pic/edit_reference.svg"));
  reference.setObjectName("editor_tb_reference");

  // Кнопка просмотра HTML кода
  showHtml.setParent(this);
  showHtml.setIcon(QIcon(":/resource/pic/edit_showhtml.svg"));
  showHtml.setObjectName("editor_tb_showhtml");

  // Кнопка включения отображения символов фарматирования
  showFormatting.setParent(this);
  showFormatting.setCheckable(true);
  showFormatting.setIcon(QIcon(":/resource/pic/edit_showformatting.svg"));
  showFormatting.setObjectName("editor_tb_showformatting");

  // Кнопка добавления новой таблицы
  createTable.setParent(this);
  createTable.setIcon(QIcon(":/resource/pic/edit_createtable.svg"));
  createTable.setObjectName("editor_tb_createtable");

  tableRemoveRow.setParent(this);
  tableRemoveRow.setIcon(QIcon(":/resource/pic/edit_table_remove_row.svg"));
  tableRemoveRow.setObjectName("editor_tb_table_remove_row");

  tableRemoveCol.setParent(this);
  tableRemoveCol.setIcon(QIcon(":/resource/pic/edit_table_remove_col.svg"));
  tableRemoveCol.setObjectName("editor_tb_table_remove_col");

  tableAddRow.setParent(this);
  tableAddRow.setIcon(QIcon(":/resource/pic/edit_table_add_row.svg"));
  tableAddRow.setObjectName("editor_tb_table_add_row");

  tableAddCol.setParent(this);
  tableAddCol.setIcon(QIcon(":/resource/pic/edit_table_add_col.svg"));
  tableAddCol.setObjectName("editor_tb_table_add_col");

  tableMergeCells.setParent(this);
  tableMergeCells.setIcon(QIcon(":/resource/pic/edit_table_merge_cells.svg"));
  tableMergeCells.setObjectName("editor_tb_table_merge_cells");

  tableSplitCell.setParent(this);
  tableSplitCell.setIcon(QIcon(":/resource/pic/edit_table_split_cell.svg"));
  tableSplitCell.setObjectName("editor_tb_table_split_cell");

  tableProperties.setParent(this);
  tableProperties.setIcon(QIcon(":/resource/pic/edit_table_properties.svg"));
  tableProperties.setObjectName("editor_tb_table_properties");

  insertImageFromFile.setParent(this);
  insertImageFromFile.setIcon(QIcon(":/resource/pic/edit_insert_image_from_file.svg"));
  insertImageFromFile.setObjectName("editor_tb_insert_image_from_file");

  mathExpression.setParent(this);
  mathExpression.setIcon(QIcon(":/resource/pic/edit_math_expression.svg"));
  mathExpression.setObjectName("editor_tb_math_expression");

  expandEditArea.setParent(this);
  expandEditArea.setIcon(QIcon(":/resource/pic/edit_expand_text_area.svg"));
  expandEditArea.setObjectName("editor_tb_expand_edit_area");

  expandToolsLines.setParent(this);
  expandToolsLines.setIcon(QIcon(":/resource/pic/edit_expand_tools_lines.svg"));
  expandToolsLines.setObjectName("editor_tb_expand_tools_lines");

  save.setParent(this);
  save.setStatusTip(tr("Save (Ctrl+S)"));
  save.setIcon(QIcon(":/resource/pic/edit_save.svg"));
  save.setObjectName("editor_tb_save");


  // Кнопка редактора "назад", используется в мобильном интерфейсе, для нее не может быть горячей клавиши
  back.setParent(this);
  back.setStatusTip(tr("Back"));
  back.setIcon(QIcon(":/resource/pic/mobile_back.svg"));
  back.setObjectName("editor_tb_back");

  // Кнопка редактора "поиск по базе", используется в мобильном интерфейсе, для нее не может быть горячей клавиши
  findInBase.setParent(this);
  findInBase.setStatusTip(tr("Find in base"));
  findInBase.setIcon(QIcon(":/resource/pic/find_in_base.svg"));
  findInBase.setObjectName("editor_tb_find_in_base");


  // Кнопка "показать текст" для просмотра текста в отдельном окне
  showText.setParent(this);
  showText.setIcon(QIcon(":/resource/pic/edit_show_text.svg"));
  showText.setObjectName("editor_tb_show_text");

  // Кнопка переключения на аттачи
  iconAttachNotExists = QIcon(":/resource/pic/attach.svg");
  iconAttachExists = QIcon(":/resource/pic/attach_exists.svg");
  toAttach.setParent(this);
  toAttach.setIcon(iconAttachNotExists);
  toAttach.setObjectName("editor_tb_attach");

  // Все только что созданные элементы скрываются
  hideAllToolsElements();
}


void EditorToolBar::setupShortcuts(void)
{
    QString actionName, info;
    ShortcutManager::stringRepresentation mode=ShortcutManager::stringRepresentation::brackets;

    actionName="editor-bold";
    info=tr("Bold")+" "+shortcutManager.getKeySequenceHumanReadable(actionName, mode);
    bold.setShortcut(shortcutManager.getKeySequence(actionName));
    bold.setStatusTip(info);
    bold.setToolTip(info);

    actionName="editor-italic";
    info=tr("Italic")+" "+shortcutManager.getKeySequenceHumanReadable(actionName, mode);
    italic.setShortcut(shortcutManager.getKeySequence(actionName));
    italic.setStatusTip(info);
    italic.setToolTip(info);

    actionName="editor-underline";
    info=tr("Underline")+" "+shortcutManager.getKeySequenceHumanReadable(actionName, mode);
    underline.setShortcut(shortcutManager.getKeySequence(actionName));
    underline.setStatusTip(info);
    underline.setToolTip(info);

    actionName="editor-monospace";
    info=tr("Monospace")+" "+shortcutManager.getKeySequenceHumanReadable(actionName, mode);
    monospace.setShortcut(shortcutManager.getKeySequence(actionName));
    monospace.setStatusTip(info);
    monospace.setToolTip(info);

    actionName="editor-code";
    info=tr("Code")+" "+shortcutManager.getKeySequenceHumanReadable(actionName, mode);
    code.setShortcut(shortcutManager.getKeySequence(actionName));
    code.setStatusTip(info+" "+tr("- Select a whole paragraphs to format text as code."));
    code.setToolTip(info);

    actionName="editor-clear";
    info=tr("Clear format")+" "+shortcutManager.getKeySequenceHumanReadable(actionName, mode);
    clear.setShortcut(shortcutManager.getKeySequence(actionName));
    clear.setStatusTip(info+" "+tr("- When selected whole paragraph both text and paragraph format is reset to default or just text format in other case."));
    clear.setToolTip(info);

    actionName="editor-textOnly";
    info=tr("Text only")+" "+shortcutManager.getKeySequenceHumanReadable(actionName, mode);
    textOnly.setShortcut(shortcutManager.getKeySequence(actionName));
    textOnly.setStatusTip(info);
    textOnly.setToolTip(info);

    actionName="editor-fixBreakSymbol";
    info=tr("Return type replace")+" "+shortcutManager.getKeySequenceHumanReadable(actionName, mode);
    fixBreakSymbol.setShortcut(shortcutManager.getKeySequence(actionName));
    fixBreakSymbol.setStatusTip(info+" "+tr("- Replace soft carriage return to standard carriage return"));
    fixBreakSymbol.setToolTip(info);

    actionName="editor-numericList";
    info=tr("Numeric list")+" "+shortcutManager.getKeySequenceHumanReadable(actionName, mode);
    numericList.setShortcut(shortcutManager.getKeySequence(actionName));
    numericList.setStatusTip(info);
    numericList.setToolTip(info);

    actionName="editor-dotList";
    info=tr("Marked list")+" "+shortcutManager.getKeySequenceHumanReadable(actionName, mode);
    dotList.setShortcut(shortcutManager.getKeySequence(actionName));
    dotList.setStatusTip(info);
    dotList.setToolTip(info);

    actionName="editor-indentPlus";
    info=tr("Increase indent")+" "+shortcutManager.getKeySequenceHumanReadable(actionName, mode);
    indentPlus.setShortcut(shortcutManager.getKeySequence(actionName));
    indentPlus.setStatusTip(info);
    indentPlus.setToolTip(info);

    actionName="editor-indentMinus";
    info=tr("Decrease indent")+" "+shortcutManager.getKeySequenceHumanReadable(actionName, mode);
    indentMinus.setShortcut(shortcutManager.getKeySequence(actionName));
    indentMinus.setStatusTip(info);
    indentMinus.setToolTip(info);

    actionName="editor-alignLeft";
    info=tr("Align left")+" "+shortcutManager.getKeySequenceHumanReadable(actionName, mode);
    alignLeft.setShortcut(shortcutManager.getKeySequence(actionName));
    alignLeft.setStatusTip(info);
    alignLeft.setToolTip(info);

    actionName="editor-alignCenter";
    info=tr("Align center")+" "+shortcutManager.getKeySequenceHumanReadable(actionName, mode);
    alignCenter.setShortcut(shortcutManager.getKeySequence(actionName));
    alignCenter.setStatusTip(info);
    alignCenter.setToolTip(info);

    actionName="editor-alignRight";
    info=tr("Align right")+" "+shortcutManager.getKeySequenceHumanReadable(actionName, mode);
    alignRight.setShortcut(shortcutManager.getKeySequence(actionName));
    alignRight.setStatusTip(info);
    alignRight.setToolTip(info);

    actionName="editor-alignWidth";
    info=tr("Align width")+" "+shortcutManager.getKeySequenceHumanReadable(actionName, mode);
    alignWidth.setShortcut(shortcutManager.getKeySequence(actionName));
    alignWidth.setStatusTip(info);
    alignWidth.setToolTip(info);

    actionName="editor-fontColor";
    info=tr("Text color")+" "+shortcutManager.getKeySequenceHumanReadable(actionName, mode);
    fontColor.setShortcut(shortcutManager.getKeySequence(actionName));
    fontColor.setStatusTip(info);
    fontColor.setToolTip(info);

    actionName="editor-findText";
    info=tr("Find text")+" "+shortcutManager.getKeySequenceHumanReadable(actionName, mode);
    findText.setShortcut(shortcutManager.getKeySequence(actionName));
    findText.setStatusTip(info);
    findText.setToolTip(info);

    actionName="editor-settings";
    info=tr("Editor settings")+" "+shortcutManager.getKeySequenceHumanReadable(actionName, mode);
    settings.setShortcut(shortcutManager.getKeySequence(actionName));
    settings.setStatusTip(info);
    settings.setToolTip(info);

    actionName="editor-reference";
    info=tr("Edit reference URL")+" "+shortcutManager.getKeySequenceHumanReadable(actionName, mode);
    reference.setShortcut(shortcutManager.getKeySequence(actionName));
    reference.setStatusTip(info);
    reference.setToolTip(info);

    actionName="editor-showHtml";
    info=tr("Edit HTML code")+" "+shortcutManager.getKeySequenceHumanReadable(actionName, mode);
    showHtml.setShortcut(shortcutManager.getKeySequence(actionName));
    showHtml.setStatusTip(info);
    showHtml.setToolTip(info);

    // Отображение символов форматирования
    actionName="editor-showFormatting";
    info=tr("Show special chars")+" "+shortcutManager.getKeySequenceHumanReadable(actionName, mode);
    showFormatting.setShortcut(shortcutManager.getKeySequence(actionName));
    showFormatting.setStatusTip(info);
    showFormatting.setToolTip(info);

    // Добавление таблицы
    actionName="editor-createTable";
    info=tr("Create a new table")+" "+shortcutManager.getKeySequenceHumanReadable(actionName, mode);
    createTable.setShortcut(shortcutManager.getKeySequence(actionName));
    createTable.setStatusTip(info);
    createTable.setToolTip(info);


    // Действия над таблицей пока не имеют горячих клавиш
    info=tr("Remove row(s)");
    tableRemoveRow.setStatusTip(info);
    tableRemoveRow.setToolTip(info);

    info=tr("Remove column(s)");
    tableRemoveCol.setStatusTip(info);
    tableRemoveCol.setToolTip(info);

    info=tr("Add row(s)");
    tableAddRow.setStatusTip(info);
    tableAddRow.setToolTip(info);

    info=tr("Add column(s)");
    tableAddCol.setStatusTip(info);
    tableAddCol.setToolTip(info);

    info=tr("Merge cells");
    tableMergeCells.setStatusTip(info);
    tableMergeCells.setToolTip(info);

    info=tr("Split cell");
    tableSplitCell.setStatusTip(info);
    tableSplitCell.setToolTip(info);

    info=tr("Table properties");
    tableProperties.setStatusTip(info);
    tableProperties.setToolTip(info);


    actionName="editor-insertImageFromFile";
    info=tr("Insert/edit image")+" "+shortcutManager.getKeySequenceHumanReadable(actionName, mode);
    insertImageFromFile.setShortcut(shortcutManager.getKeySequence(actionName));
    insertImageFromFile.setStatusTip(info+" "+tr("- Insert image from file or edit image properties of selected image"));
    insertImageFromFile.setToolTip(info);

    actionName="editor-mathExpression";
    info=tr("Insert/edit math expression")+" "+shortcutManager.getKeySequenceHumanReadable(actionName, mode);
    mathExpression.setShortcut(shortcutManager.getKeySequence(actionName));
    mathExpression.setStatusTip(info);
    mathExpression.setToolTip(info);

    actionName="editor-expandEditArea";
    info=tr("Expand edit area")+" "+shortcutManager.getKeySequenceHumanReadable(actionName, mode);
    expandEditArea.setShortcut(shortcutManager.getKeySequence(actionName));
    expandEditArea.setStatusTip(info);
    expandEditArea.setToolTip(info);

    actionName="editor-expandToolsLines";
    info=tr("Expand tools")+" "+shortcutManager.getKeySequenceHumanReadable(actionName, mode);
    expandToolsLines.setShortcut(shortcutManager.getKeySequence(actionName));
    expandToolsLines.setStatusTip(info);
    expandToolsLines.setToolTip(info);

    actionName="editor-save";
    info=tr("Forse save note")+" "+shortcutManager.getKeySequenceHumanReadable(actionName, mode);
    save.setShortcut(shortcutManager.getKeySequence(actionName));
    save.setStatusTip(info);
    save.setToolTip(info);

    actionName="editor-showText";
    info=tr("Show detached window")+" "+shortcutManager.getKeySequenceHumanReadable(actionName, mode);
    showText.setShortcut(shortcutManager.getKeySequence(actionName));
    showText.setStatusTip(info);
    showText.setToolTip(info);

    actionName="editor-toAttach";
    info=tr("Attach files")+" "+shortcutManager.getKeySequenceHumanReadable(actionName, mode);
    toAttach.setShortcut(shortcutManager.getKeySequence(actionName));
    toAttach.setStatusTip(info);
    toAttach.setToolTip(info);
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

    if(tool)
    {
      // Если данный инструмент не содержится в списке заблокированных
      if(!disableToolList.contains(toolName))
      {
        tool->setVisible(true);

        line.addWidget(tool); // Инструмент добавляется на панель инструментов

        QToolButton *tb=qobject_cast<QToolButton*>(tool);
        if(tb)
          tb->setAutoRaise(false);
      }
      else
      {
        QToolButton *tb=qobject_cast<QToolButton*>(tool);
        if(tb)
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
