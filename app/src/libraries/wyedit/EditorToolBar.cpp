#include <QDebug>
#include <QStringList>

#include "main.h"
#include "EditorToolBar.h"
#include "Editor.h"
#include "libraries/ShortcutManager.h"
#include "libraries/helpers/DebugHelper.h"
#include "libraries/helpers/ActionHelper.h"


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
// Ранее в этот метод передавался параметр mode
// Если mode=WYEDIT_DESKTOP_MODE - происходит обычная инициализация
// Если mode=WYEDIT_MOBILE_MODE - при инициализации в первую строку панели инструментов, слева, добавляется кнопка back
// Теперь доп. кнопки добавляются в конструкторе EditorToolBarAssistant
// todo: Нужно проверить, что для доп. кнопок выставляются правильные QAction в mobile-режиме
void EditorToolBar::init()
{
    isInit=true;

    this->setupToolBarTools();
    this->setupShortcuts();
    this->setupSignals();
    this->registryActionsToToolBarWidget();
    this->assemblyButtons();
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

  // Кнопка Undo
  undo=new QAction(this);
  undo->setIcon(QIcon(":/resource/pic/edit_undo.svg"));
  undo->setObjectName("editor_tb_undo");
  undo->setEnabled(false); // undo недоступно, т.к. при создании кнопки еще не было ни одного действия с текстом

  // Кнопка Redo
  redo=new QAction(this);
  redo->setIcon(QIcon(":/resource/pic/edit_redo.svg"));
  redo->setObjectName("editor_tb_redo");
  redo->setEnabled(false); // redo недоступно, т.к. при создании кнопки еще не было ни одного действия с текстом

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

  // Кнопка SuperScript
  superscript=new QAction(this);
  superscript->setCheckable(true);
  superscript->setIcon(QIcon(":/resource/pic/edit_sup.svg"));
  superscript->setObjectName("editor_tb_superscript");

  // Кнопка SubScript
  subscript=new QAction(this);
  subscript->setCheckable(true);
  subscript->setIcon(QIcon(":/resource/pic/edit_sub.svg"));
  subscript->setObjectName("editor_tb_subscript");

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
  textOnly->setObjectName("editor_tb_textOnly");

  // Кнопка замены символов мягкого переноса (тег <br/> заменяется на разрыв параграфа </p><p>)
  fixBreakSymbol=new QAction(this);
  fixBreakSymbol->setIcon(QIcon(":/resource/pic/edit_fixBreakSymbol.svg"));
  fixBreakSymbol->setObjectName("editor_tb_fixBreakSymbol");

  // Кнопка нумерованного списка
  numericList=new QAction(this);
  numericList->setIcon(QIcon(":/resource/pic/edit_listnumeric.svg"));
  numericList->setObjectName("editor_tb_numericList");

  // Кнопка списка с точками
  dotList=new QAction(this);
  dotList->setIcon(QIcon(":/resource/pic/edit_listdot.svg"));
  dotList->setObjectName("editor_tb_dotList");


  // Кнопка увеличения отступа
  indentPlus=new QAction(this);
  indentPlus->setIcon(QIcon(":/resource/pic/edit_indentplus.svg"));
  indentPlus->setObjectName("editor_tb_indentPlus");

  // Кнопка уменьшения отступа
  indentMinus=new QAction(this);
  indentMinus->setIcon(QIcon(":/resource/pic/edit_indentminus.svg"));
  indentMinus->setObjectName("editor_tb_indentMinus");


  // Кнопка выравнивания по левому краю
  alignLeft=new QAction(this);
  alignLeft->setCheckable(true);
  alignLeft->setIcon(QIcon(":/resource/pic/edit_alignleft.svg"));
  alignLeft->setObjectName("editor_tb_alignLeft");

  // Кнопка выравнивания по центру
  alignCenter=new QAction(this);
  alignCenter->setCheckable(true);
  alignCenter->setIcon(QIcon(":/resource/pic/edit_aligncenter.svg"));
  alignCenter->setObjectName("editor_tb_alignCenter");

  // Кнопка выравнивания по правому краю
  alignRight=new QAction(this);
  alignRight->setCheckable(true);
  alignRight->setIcon(QIcon(":/resource/pic/edit_alignright.svg"));
  alignRight->setObjectName("editor_tb_alignRight");

  // Кнопка выравнивания по ширине
  alignWidth=new QAction(this);
  alignWidth->setCheckable(true);
  alignWidth->setIcon(QIcon(":/resource/pic/edit_alignwidth.svg"));
  alignWidth->setObjectName("editor_tb_alignWidth");


  // Выбор шрифта
  fontSelect=new EditorFontFamilyComboBox(this);
  fontSelect->setObjectName("editor_tb_fontSelect");

  // Выбор размера шрифта
  fontSize=new EditorFontSizeComboBox(this);
  fontSize->setObjectName("editor_tb_fontSize");


  // Кнопка выбора цвета шрифта
  fontColor=new QAction(this);
  fontColor->setObjectName("editor_tb_fontColor");


  // Кнопка выбора цвета фона текста
  backgroundColor=new QAction(this);
  backgroundColor->setObjectName("editor_tb_backgroundColor");


  // Кнопка вызова виджета поиска текста
  findText=new QAction(this);
  findText->setIcon(QIcon(":/resource/pic/edit_findtext.svg"));
  findText->setObjectName("editor_tb_findText");

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
  showHtml->setObjectName("editor_tb_showHtml");

  // Кнопка включения отображения символов фарматирования
  showFormatting=new QAction(this);
  showFormatting->setCheckable(true);
  showFormatting->setIcon(QIcon(":/resource/pic/edit_showformatting.svg"));
  showFormatting->setObjectName("editor_tb_showFormatting");

  // Кнопка добавления новой таблицы
  createTable=new QAction(this);
  createTable->setIcon(QIcon(":/resource/pic/edit_createtable.svg"));
  createTable->setObjectName("editor_tb_createTable");

  tableRemoveRow=new QAction(this);
  tableRemoveRow->setIcon(QIcon(":/resource/pic/edit_table_remove_row.svg"));
  tableRemoveRow->setObjectName("editor_tb_tableRemoveRow");

  tableRemoveCol=new QAction(this);
  tableRemoveCol->setIcon(QIcon(":/resource/pic/edit_table_remove_col.svg"));
  tableRemoveCol->setObjectName("editor_tb_tableRemoveCol");

  tableAddRow=new QAction(this);
  tableAddRow->setIcon(QIcon(":/resource/pic/edit_table_add_row.svg"));
  tableAddRow->setObjectName("editor_tb_tableAddRow");

  tableAddCol=new QAction(this);
  tableAddCol->setIcon(QIcon(":/resource/pic/edit_table_add_col.svg"));
  tableAddCol->setObjectName("editor_tb_tableAddCol");

  tableMergeCells=new QAction(this);
  tableMergeCells->setIcon(QIcon(":/resource/pic/edit_table_merge_cells.svg"));
  tableMergeCells->setObjectName("editor_tb_tableMergeCells");

  tableSplitCell=new QAction(this);
  tableSplitCell->setIcon(QIcon(":/resource/pic/edit_table_split_cell.svg"));
  tableSplitCell->setObjectName("editor_tb_tableSplitCell");

  tableProperties=new QAction(this);
  tableProperties->setIcon(QIcon(":/resource/pic/edit_table_properties.svg"));
  tableProperties->setObjectName("editor_tb_tableProperties");

  // Кнопка открытия изображения
  openImage=new QAction(this);
  openImage->setIcon(QIcon(":/resource/pic/open_image.svg"));
  openImage->setObjectName("editor_tb_openImage");

  // Кнопка вставки / редактирования свойств изображения
  insertImageFromFile=new QAction(this);
  insertImageFromFile->setIcon(QIcon(":/resource/pic/edit_insert_image_from_file.svg"));
  insertImageFromFile->setObjectName("editor_tb_insertImageFromFile");

  insertHorizontalLine=new QAction(this);
  insertHorizontalLine->setIcon(QIcon(":/resource/pic/edit_insert_horizontal_line.svg"));
  insertHorizontalLine->setObjectName("editor_tb_insertHorizontalLine");

  mathExpression=new QAction(this);
  mathExpression->setIcon(QIcon(":/resource/pic/edit_math_expression.svg"));
  mathExpression->setObjectName("editor_tb_mathExpression");

  expandEditArea=new QAction(this);
  expandEditArea->setIcon(QIcon(":/resource/pic/edit_expand_text_area.svg"));
  expandEditArea->setObjectName("editor_tb_expandEditArea");

  expandToolsLines=new QAction(this);
  expandToolsLines->setIcon(QIcon(":/resource/pic/edit_expand_tools_lines.svg"));
  expandToolsLines->setObjectName("editor_tb_expandToolsLines");

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
  findInBase->setObjectName("editor_tb_findInBase");


  // Кнопка "показать текст" для просмотра текста в отдельном окне
  showText=new QAction(this);
  showText->setIcon(QIcon(":/resource/pic/edit_show_text.svg"));
  showText->setObjectName("editor_tb_showText");

  // Кнопка переключения на аттачи
  iconAttachNotExists = QIcon(":/resource/pic/attach.svg");
  iconAttachExists = QIcon(":/resource/pic/attach_exists.svg");
  toAttach=new QAction(this);
  toAttach->setIcon(iconAttachNotExists);
  toAttach->setObjectName("editor_tb_toAttach");

  // Все только что созданные элементы скрываются
  this->hideAllToolsElements();
}


void EditorToolBar::setupShortcuts(void)
{
    qDebug() << "Setup shortcut for" << this->metaObject()->className();

    shortcutManager.initAction("editor-bold", bold );
    shortcutManager.initAction("editor-italic", italic);
    shortcutManager.initAction("editor-underline", underline);
    shortcutManager.initAction("editor-strikeout", strikeout);
    shortcutManager.initAction("editor-superscript", superscript);
    shortcutManager.initAction("editor-subscript", subscript);
    shortcutManager.initAction("editor-monospace", monospace);
    shortcutManager.initAction("editor-code", code);
    shortcutManager.initAction("editor-clear", clear);
    shortcutManager.initAction("editor-textOnly", textOnly);
    shortcutManager.initAction("editor-fixBreakSymbol", fixBreakSymbol);
    shortcutManager.initAction("editor-undo", undo );
    shortcutManager.initAction("editor-redo", redo );
    shortcutManager.initAction("editor-numericList", numericList);
    shortcutManager.initAction("editor-dotList", dotList);
    shortcutManager.initAction("editor-indentPlus", indentPlus);
    shortcutManager.initAction("editor-indentMinus", indentMinus);
    shortcutManager.initAction("editor-alignLeft", alignLeft);
    shortcutManager.initAction("editor-alignCenter", alignCenter);
    shortcutManager.initAction("editor-alignRight", alignRight);
    shortcutManager.initAction("editor-alignWidth", alignWidth);
    shortcutManager.initAction("editor-fontColor", fontColor);
    shortcutManager.initAction("editor-backgroundColor", backgroundColor);

    // Настраиваются скрытые кнопки действия, а надписи настраиваются для самого виджета
    // Скрытые кнопки нужны чтобы работал выбор виджета по горячим кнопкам
    shortcutManager.initAction("editor-fontSelect", fontSelect->toolFocus.getSelectAction() );
    fontSelect->setStatusTip( shortcutManager.getFullDescription("editor-fontSelect") );
    fontSelect->setToolTip( shortcutManager.getDescriptionWithShortcut("editor-fontSelect") );

    shortcutManager.initAction("editor-fontSize", fontSize->toolFocus.getSelectAction() );
    fontSize->setStatusTip( shortcutManager.getFullDescription("editor-fontSize") );
    fontSize->setToolTip( shortcutManager.getDescriptionWithShortcut("editor-fontSize") );

    shortcutManager.initAction("editor-findText", findText);
    shortcutManager.initAction("editor-settings", settings);
    shortcutManager.initAction("editor-reference", reference);
    shortcutManager.initAction("editor-showHtml", showHtml);
    shortcutManager.initAction("editor-showFormatting", showFormatting);
    shortcutManager.initAction("editor-createTable", createTable);
    shortcutManager.initAction("editor-tableRemoveRow", tableRemoveRow);
    shortcutManager.initAction("editor-tableRemoveCol", tableRemoveCol);
    shortcutManager.initAction("editor-tableAddRow", tableAddRow);
    shortcutManager.initAction("editor-tableAddCol", tableAddCol);
    shortcutManager.initAction("editor-tableMergeCells", tableMergeCells);
    shortcutManager.initAction("editor-tableSplitCell", tableSplitCell);
    shortcutManager.initAction("editor-tableProperties", tableProperties);
    shortcutManager.initAction("editor-openImage", openImage);
    shortcutManager.initAction("editor-insertImageFromFile", insertImageFromFile);
    shortcutManager.initAction("editor-insertHorizontalLine", insertHorizontalLine);
    shortcutManager.initAction("editor-mathExpression", mathExpression);
    shortcutManager.initAction("editor-expandEditArea", expandEditArea);
    shortcutManager.initAction("editor-expandToolsLines", expandToolsLines);
    shortcutManager.initAction("editor-save", save);
    shortcutManager.initAction("editor-showText", showText);
    shortcutManager.initAction("editor-toAttach", toAttach);
}


// Список названий всех контролов (команд) панелей инструментов
QStringList EditorToolBar::getCommandNameList()
{
    QRegularExpression nameMask("editor_tb_.*");
    QList<QWidget *> widgetList = this->findChildren<QWidget *>(nameMask);
    QList<QAction *> actionList = this->findChildren<QAction *>(nameMask);

    QStringList controlsNameList;

    for (int i=0; i!=widgetList.size(); ++i) {
        controlsNameList.append(widgetList.at(i)->objectName().replace("editor_tb_", ""));
    }
    for (int i=0; i!=actionList.size(); ++i) {
        controlsNameList.append(actionList.at(i)->objectName().replace("editor_tb_", ""));
    }
    controlsNameList.sort();

    return controlsNameList;
}


// Выяснение иконки для инструмента панели управления по его имени
QIcon EditorToolBar::getIcon(const QString &name)
{
    QRegularExpression nameMask("editor_tb_"+name);
    QList<QWidget *> widgetList = this->findChildren<QWidget *>(nameMask);
    QList<QAction *> actionList = this->findChildren<QAction *>(nameMask);

    // Если инструмент является виджетом, то у виджета нет иконки
    if (widgetList.size()>0) {
        return QIcon(); // Возвращается пустая иконка
    }

    if (actionList.size()>0) {
        return actionList.at(0)->icon();
    }

    return QIcon();
}


void EditorToolBar::hideAllToolsElements(void)
{
    QStringList commandNameList=this->getCommandNameList();

    for(auto commandName : commandNameList)
    {
        QWidget *widget=this->findChild<QWidget *>(QString("editor_tb_")+commandName);
        if(widget)
        {
            widget->setVisible(false);
        }

        QAction *action=this->findChild<QAction *>(QString("editor_tb_")+commandName);
        if(action)
        {
            action->setVisible(false);
        }
    }
}


// Установка или снятие доступности кнопок, модифицирующих текст
void EditorToolBar::setEnableModifyTextButton(bool state)
{
    // Перечень суффиксов имен кнопок, которые не влияют на форматирование
    QStringList noModifyButton;
    noModifyButton << "settings" \
                   << "findText" \
                   << "showFormatting" \
                   << "expandEditArea" \
                   << "expandToolsLines" \
                   << "back" \
                   << "findInBase"
                   << "showText" \
                   << "toAttach";

    // Перечень имен всех элементов управления на панели
    QStringList commandNameList=this->getCommandNameList();

    for(auto commandName : commandNameList)
    {
        // Если элемент влияет на форматирование
        if( !noModifyButton.contains( commandName ) )
        {
            QWidget *widget=this->findChild<QWidget *>(QString("editor_tb_")+commandName);
            if(widget)
            {
                widget->setEnabled(state);
            }

            QAction *action=this->findChild<QAction *>(QString("editor_tb_")+commandName);
            if(action)
            {
                action->setEnabled(state);
            }
        }
    }

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

  lt->update();
}


// Убирание всех инструментов с тулбара (но на очистка списков элементов на тулбарах)
void EditorToolBar::clearToolsLines(void)
{
    // Особенность Qt
    // Владение QWidget-ми, которые вставлены в тулбары через addWidget(),
    // забирается от тулбаров и возвращается текущему объекту
    // Однако, данное действие не нужно делать для QAction,
    // которые вствленны через addAction(), так как для QAction,
    // в момент вставки в тулбар, владение не передается

    // Тулбар 1
    QList<QObject*> objectList=toolsLine1.children();
    for(auto object : objectList)
    {
        QWidget *widget=dynamic_cast<QWidget *>(object);
        if(widget)
        {
            widget->setParent(this);
        }
    }

    // Тулбар 2
    objectList=toolsLine2.children();
    for(auto object : objectList)
    {
        QWidget *widget=dynamic_cast<QWidget *>(object);
        if(widget)
        {
            widget->setParent(this);
        }
    }

    // Очистка тулбаров
    toolsLine1.clear();
    toolsLine2.clear();
}


// Засовывание инструментов на тулбары согласно спискам инструментов на тулбарах
void EditorToolBar::updateToolsLines(void)
{
  this->clearToolsLines();

  for(int i=0;i<toolsListInLine1.size();++i)
  {
    QString b=toolsListInLine1.at(i).trimmed();
    this->insertButtonToToolsLine(b,toolsLine1);
  }

  for(int i=0;i<toolsListInLine2.size();++i)
  {
    QString b=toolsListInLine2.at(i).trimmed();
    this->insertButtonToToolsLine(b,toolsLine2);
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
        }

        // Если данный инструмент не содержится в списке заблокированных
        if(!disableToolList.contains(toolName))
        {
            // Если добавляемый инструмент - это виджет
            if(toolAsWidget)
            {
                toolAsWidget->setVisible(true);
                line.addWidget(toolAsWidget); // Инструмент добавляется на панель инструментов как виджет

                // Для специальных классов добавляются действия активации виджета
                // как невидимые кнопки
                // При динамическом приведении типа, если тип не соответсвует заданному,
                // dynamic_cast вернет NULL для типа указателя коим является toolAsWidget.
                // Т. е. условие сработает только если тип будет успешно приведен к целевому
                if( dynamic_cast<EditorFontFamilyComboBox*>(toolAsWidget) )
                {
                    insertActionAsButton(&line, static_cast<EditorFontFamilyComboBox*>(toolAsWidget)->toolFocus.getSelectAction(), false);
                }
                if( dynamic_cast<EditorFontSizeComboBox*>(toolAsWidget) )
                {
                    insertActionAsButton(&line, static_cast<EditorFontSizeComboBox*>(toolAsWidget)->toolFocus.getSelectAction(), false);
                }
            }

            // Если добавляемый инструмент - это действие
            if(toolAsAction)
            {
                toolAsAction->setVisible(true);
                line.addAction(toolAsAction); // Инструмент добавляется на панель инструментов как кнопка с привязанным действием

                QToolButton* currentButton=qobject_cast<QToolButton*>(line.widgetForAction(toolAsAction));
                currentButton->setAutoRaise(true); // От автоподсвечивания зависит видимость границ кнопок
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


// Действия с шорткатами добавляются к виджету EditorToolBar,
// чтобы шорткаты срабатывали даже если нет кнопок на панелях редактора
void EditorToolBar::registryActionsToToolBarWidget()
{
    // Перебираются все инструменты редактора с шорткатами
    for(auto toolName : shortcutManager.getActionsNameList("editor") )
    {
        // Запрещенные действия добавляться не должны
        if( disableToolList.contains(toolName) )
        {
            continue;
        }

        QAction *toolAsAction=qobject_cast<QAction *>(this->findChild<QObject *>( "editor_tb_"+toolName ));

        // Добавляются только действия, но не виджеты (так как виджеты будут
        // восприняты как дочерние виджеты текущего виджета)
        if(toolAsAction)
        {
            this->addAction( toolAsAction );
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
