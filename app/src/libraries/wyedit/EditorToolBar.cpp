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

  setupToolBarTools();
  setupShortcuts();
  setupSignals();
  assemblyButtons();
}


QAction* EditorToolBar::generateAction(QIcon icon)
{
    actions.resize( actions.size()+1 );

    int currentIndex=actions.size()-1;

    actions[currentIndex]=new QAction();
    actions[currentIndex]->setIcon(icon);

    return actions[currentIndex];
}



void EditorToolBar::setupSignals()
{
    // Обновление горячих клавиш, если они были изменены
    connect(&shortcutManager, &ShortcutManager::updateWidgetShortcut, this, &EditorToolBar::setupShortcuts);
}


// Создание объектов для линейки форматирования текста
void EditorToolBar::setupToolBarTools(void)
{
  // Для того, чтобы WyEdit нормально добавлял кнопки на панель согласно файлу editorconf.ini,
  // имена объектов должны начинаться на "editor_tb"

  // Кнопка Bold
  bold=new QAction(this);
  bold->setCheckable(true);
  bold->setIcon(QIcon(":/resource/pic/edit_bold.svg"));
  bold->setObjectName("editor_tb_bold");

  // Кнопка Italic
  italic=new QAction(this);
  italic->setCheckable(true);
  italic->setIcon(QIcon(":/resource/pic/edit_italic.svg"));
  italic->setObjectName("editor_tb_italic");

  // Кнопка Underline
  underline=new QAction(this);
  underline->setCheckable(true);
  underline->setIcon(QIcon(":/resource/pic/edit_underline.svg"));
  underline->setObjectName("editor_tb_underline");

  // Кнопка StrikeOut
  strikeout=new QAction(this);
  strikeout->setCheckable(true);
  strikeout->setIcon(QIcon(":/resource/pic/edit_strikeout.svg"));
  strikeout->setObjectName("editor_tb_strikeout");

  // Кнопка Monospace
  monospace=new QAction(this);
  monospace->setIcon(QIcon(":/resource/pic/edit_monospace.svg"));
  monospace->setObjectName("editor_tb_monospace");

  // Кнопка Code
  code=new QAction(this);
  code->setIcon(QIcon(":/resource/pic/edit_code.svg"));
  code->setObjectName("editor_tb_code");

  // Кнопка Clear
  clear=new QAction(this);
  clear->setIcon(QIcon(":/resource/pic/edit_clear.svg"));
  clear->setObjectName("editor_tb_clear");

  // Кнопка TextOnly - преобразование выделенного фрагмента в чистый текст
  textOnly=new QAction(this);
  textOnly->setIcon(QIcon(":/resource/pic/edit_text_only.svg"));
  textOnly->setObjectName("editor_tb_text_only");

  // Кнопка замены символов мягкого переноса (тег <br/> заменяется на разрыв параграфа </p><p>)
  fixBreakSymbol=new QAction(this);
  fixBreakSymbol->setIcon(QIcon(":/resource/pic/edit_fixBreakSymbol.svg"));
  fixBreakSymbol->setObjectName("editor_tb_fix_break_symbol");

  // Кнопка нумерованного списка
  numericList=new QAction(this);
  numericList->setIcon(QIcon(":/resource/pic/edit_listnumeric.svg"));
  numericList->setObjectName("editor_tb_numericlist");

  // Кнопка списка с точками
  dotList=new QAction(this);
  dotList->setIcon(QIcon(":/resource/pic/edit_listdot.svg"));
  dotList->setObjectName("editor_tb_dotlist");


  // Кнопка увеличения отступа
  indentPlus=new QAction(this);
  indentPlus->setIcon(QIcon(":/resource/pic/edit_indentplus.svg"));
  indentPlus->setObjectName("editor_tb_indentplus");

  // Кнопка уменьшения отступа
  indentMinus=new QAction(this);
  indentMinus->setIcon(QIcon(":/resource/pic/edit_indentminus.svg"));
  indentMinus->setObjectName("editor_tb_indentminus");


  // Кнопка выравнивания по левому краю
  alignLeft=new QAction(this);
  alignLeft->setCheckable(true);
  alignLeft->setIcon(QIcon(":/resource/pic/edit_alignleft.svg"));
  alignLeft->setObjectName("editor_tb_alignleft");

  // Кнопка выравнивания по центру
  alignCenter=new QAction(this);
  alignCenter->setCheckable(true);
  alignCenter->setIcon(QIcon(":/resource/pic/edit_aligncenter.svg"));
  alignCenter->setObjectName("editor_tb_aligncenter");

  // Кнопка выравнивания по правому краю
  alignRight=new QAction(this);
  alignRight->setCheckable(true);
  alignRight->setIcon(QIcon(":/resource/pic/edit_alignright.svg"));
  alignRight->setObjectName("editor_tb_alignright");

  // Кнопка выравнивания по ширине
  alignWidth=new QAction(this);
  alignWidth->setCheckable(true);
  alignWidth->setIcon(QIcon(":/resource/pic/edit_alignwidth.svg"));
  alignWidth->setObjectName("editor_tb_alignwidth");


  // Выбор шрифта
  fontSelect=new EditorFontFamilyComboBox(this);
  fontSelect->setObjectName("editor_tb_fontselect");


  // Выбор размера шрифта
  fontSize=new EditorFontSizeComboBox(this);
  fontSize->setObjectName("editor_tb_fontsize");


  // Кнопка выбора цвета шрифта
  fontColor=new QAction(this);
  fontColor->setIcon(QIcon(":/resource/pic/edit_fontcolor.svg"));
  fontColor->setObjectName("editor_tb_fontcolor");


  // Кнопка вызова виджета поиска текста
  findText=new QAction(this);
  findText->setIcon(QIcon(":/resource/pic/edit_findtext.svg"));
  findText->setObjectName("editor_tb_findtext");

  // Кнопка вызова виджета конфигурирования редактора
  settings=new QAction(this);
  settings->setIcon(QIcon(":/resource/pic/edit_settings.svg"));
  settings->setObjectName("editor_tb_settings");

  // Кнопка редактирования ссылки (URL)
  reference=new QAction(this);
  reference->setIcon(QIcon(":/resource/pic/edit_reference.svg"));
  reference->setObjectName("editor_tb_reference");

  // Кнопка просмотра HTML кода
  showHtml=new QAction(this);
  showHtml->setIcon(QIcon(":/resource/pic/edit_showhtml.svg"));
  showHtml->setObjectName("editor_tb_showhtml");

  // Кнопка включения отображения символов фарматирования
  showFormatting=new QAction(this);
  showFormatting->setCheckable(true);
  showFormatting->setIcon(QIcon(":/resource/pic/edit_showformatting.svg"));
  showFormatting->setObjectName("editor_tb_showformatting");

  // Кнопка добавления новой таблицы
  createTable=new QAction(this);
  createTable->setIcon(QIcon(":/resource/pic/edit_createtable.svg"));
  createTable->setObjectName("editor_tb_createtable");

  tableRemoveRow=new QAction(this);
  tableRemoveRow->setIcon(QIcon(":/resource/pic/edit_table_remove_row.svg"));
  tableRemoveRow->setObjectName("editor_tb_table_remove_row");

  tableRemoveCol=new QAction(this);
  tableRemoveCol->setIcon(QIcon(":/resource/pic/edit_table_remove_col.svg"));
  tableRemoveCol->setObjectName("editor_tb_table_remove_col");

  tableAddRow=new QAction(this);
  tableAddRow->setIcon(QIcon(":/resource/pic/edit_table_add_row.svg"));
  tableAddRow->setObjectName("editor_tb_table_add_row");

  tableAddCol=new QAction(this);
  tableAddCol->setIcon(QIcon(":/resource/pic/edit_table_add_col.svg"));
  tableAddCol->setObjectName("editor_tb_table_add_col");

  tableMergeCells=new QAction(this);
  tableMergeCells->setIcon(QIcon(":/resource/pic/edit_table_merge_cells.svg"));
  tableMergeCells->setObjectName("editor_tb_table_merge_cells");

  tableSplitCell=new QAction(this);
  tableSplitCell->setIcon(QIcon(":/resource/pic/edit_table_split_cell.svg"));
  tableSplitCell->setObjectName("editor_tb_table_split_cell");

  tableProperties=new QAction(this);
  tableProperties->setIcon(QIcon(":/resource/pic/edit_table_properties.svg"));
  tableProperties->setObjectName("editor_tb_table_properties");

  insertImageFromFile=new QAction(this);
  insertImageFromFile->setIcon(QIcon(":/resource/pic/edit_insert_image_from_file.svg"));
  insertImageFromFile->setObjectName("editor_tb_insert_image_from_file");

  insertHorizontalLine=new QAction(this);
  insertHorizontalLine->setIcon(QIcon(":/resource/pic/edit_insert_horizontal_line.svg"));
  insertHorizontalLine->setObjectName("editor_tb_insert_horizontal_line");

  mathExpression=new QAction(this);
  mathExpression->setIcon(QIcon(":/resource/pic/edit_math_expression.svg"));
  mathExpression->setObjectName("editor_tb_math_expression");

  expandEditArea=new QAction(this);
  expandEditArea->setIcon(QIcon(":/resource/pic/edit_expand_text_area.svg"));
  expandEditArea->setObjectName("editor_tb_expand_edit_area");

  expandToolsLines=new QAction(this);
  expandToolsLines->setIcon(QIcon(":/resource/pic/edit_expand_tools_lines.svg"));
  expandToolsLines->setObjectName("editor_tb_expand_tools_lines");

  save=new QAction(this);
  save->setStatusTip(tr("Save (Ctrl+S)"));
  save->setIcon(QIcon(":/resource/pic/edit_save.svg"));
  save->setObjectName("editor_tb_save");


  // Кнопка редактора "назад", используется в мобильном интерфейсе, для нее не может быть горячей клавиши
  back=new QAction(this);
  back->setStatusTip(tr("Back"));
  back->setIcon(QIcon(":/resource/pic/mobile_back.svg"));
  back->setObjectName("editor_tb_back");

  // Кнопка редактора "поиск по базе", используется в мобильном интерфейсе, для нее не может быть горячей клавиши
  findInBase=new QAction(this);
  findInBase->setStatusTip(tr("Find in base"));
  findInBase->setIcon(QIcon(":/resource/pic/find_in_base.svg"));
  findInBase->setObjectName("editor_tb_find_in_base");


  // Кнопка "показать текст" для просмотра текста в отдельном окне
  showText=new QAction(this);
  showText->setIcon(QIcon(":/resource/pic/edit_show_text.svg"));
  showText->setObjectName("editor_tb_show_text");

  // Кнопка переключения на аттачи
  iconAttachNotExists = QIcon(":/resource/pic/attach.svg");
  iconAttachExists = QIcon(":/resource/pic/attach_exists.svg");
  toAttach=new QAction(this);
  toAttach->setIcon(iconAttachNotExists);
  toAttach->setObjectName("editor_tb_attach");

  // Все только что созданные элементы скрываются
  hideAllToolsElements();
}


void EditorToolBar::setupShortcuts(void)
{
    qDebug() << "Setup shortcut for" << this->metaObject()->className();

    shortcutManager.initAction("editor-bold", bold );
    shortcutManager.initAction("editor-italic", italic);
    shortcutManager.initAction("editor-underline", underline);
    shortcutManager.initAction("editor-strikeout", strikeout);
    shortcutManager.initAction("editor-monospace", monospace);
    shortcutManager.initAction("editor-code", code);
    shortcutManager.initAction("editor-clear", clear);
    shortcutManager.initAction("editor-textOnly", textOnly);
    shortcutManager.initAction("editor-fixBreakSymbol", fixBreakSymbol);
    shortcutManager.initAction("editor-numericList", numericList);
    shortcutManager.initAction("editor-dotList", dotList);
    shortcutManager.initAction("editor-indentPlus", indentPlus);
    shortcutManager.initAction("editor-indentMinus", indentMinus);
    shortcutManager.initAction("editor-alignLeft", alignLeft);
    shortcutManager.initAction("editor-alignCenter", alignCenter);
    shortcutManager.initAction("editor-alignRight", alignRight);
    shortcutManager.initAction("editor-alignWidth", alignWidth);
    shortcutManager.initAction("editor-fontColor", fontColor);
    shortcutManager.initAction("editor-findText", findText);
    shortcutManager.initAction("editor-settings", settings);
    shortcutManager.initAction("editor-reference", reference);
    shortcutManager.initAction("editor-showHtml", showHtml);
    shortcutManager.initAction("editor-showFormatting", showFormatting);
    shortcutManager.initAction("editor-createTable", createTable);

    // Действия над таблицей пока не имеют горячих клавиш
    QString info;

    info=tr("Remove row(s)");
    tableRemoveRow->setStatusTip(info);
    tableRemoveRow->setToolTip(info);

    info=tr("Remove column(s)");
    tableRemoveCol->setStatusTip(info);
    tableRemoveCol->setToolTip(info);

    info=tr("Add row(s)");
    tableAddRow->setStatusTip(info);
    tableAddRow->setToolTip(info);

    info=tr("Add column(s)");
    tableAddCol->setStatusTip(info);
    tableAddCol->setToolTip(info);

    info=tr("Merge cells");
    tableMergeCells->setStatusTip(info);
    tableMergeCells->setToolTip(info);

    info=tr("Split cell");
    tableSplitCell->setStatusTip(info);
    tableSplitCell->setToolTip(info);

    info=tr("Table properties");
    tableProperties->setStatusTip(info);
    tableProperties->setToolTip(info);

    shortcutManager.initAction("editor-insertImageFromFile", insertImageFromFile);
    shortcutManager.initAction("editor-insertHorizontalLine", insertHorizontalLine);
    shortcutManager.initAction("editor-mathExpression", mathExpression);
    shortcutManager.initAction("editor-expandEditArea", expandEditArea);
    shortcutManager.initAction("editor-expandToolsLines", expandToolsLines);
    shortcutManager.initAction("editor-save", save);
    shortcutManager.initAction("editor-showText", showText);
    shortcutManager.initAction("editor-toAttach", toAttach);
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

        QWidget *toolAsWidget=qobject_cast<QWidget *>(this->findChild<QObject *>(name));
        QAction *toolAsAction=qobject_cast<QAction *>(this->findChild<QObject *>(name));

        if(!toolAsWidget && !toolAsAction) {
            criticalError("WyEdit: Can not find editor tool with name '"+toolName+"'. Please check editor *.ini file");
            return;
        }

        // Если данный инструмент не содержится в списке заблокированных
        if(!disableToolList.contains(toolName))
        {
            if(toolAsWidget) {
                toolAsWidget->setVisible(true);
                line.addWidget(toolAsWidget); // Инструмент добавляется на панель инструментов как виджет
            }

            if(toolAsAction) {
                toolAsAction->setVisible(true);
                line.addAction(toolAsAction); // Инструмент добавляется на панель инструментов как кнопка с привязанным действием

                QToolButton* currentButton=qobject_cast<QToolButton*>(line.widgetForAction(toolAsAction));
                currentButton->setAutoRaise(false); // Границы кнопки всегда видны
            }
        }
        else
        {
            if(toolAsWidget) {
                toolAsWidget->setEnabled(false);
            }

            if(toolAsAction) {
                toolAsAction->setEnabled(false);
            }
        }
    }
}


void EditorToolBar::switchAttachIconExists(bool isExists)
{
  if(isExists)
    toAttach->setIcon( iconAttachExists );
  else
    toAttach->setIcon( iconAttachNotExists );
}
