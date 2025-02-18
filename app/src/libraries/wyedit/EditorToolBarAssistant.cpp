#include <QColor>
#include <QMessageBox>
#include <QTextTable>
#include <QTextTableFormat>
#include <QPainter>
#include <QDebug>
#include <QPair>

#include "main.h"
#include "EditorToolBarAssistant.h"
#include "Editor.h"
#include "EditorToolBar.h"
#include "EditorTextArea.h"
#include "EditorConfig.h"
#include "EditorIndentSliderAssistant.h"
#include "../TraceLogger.h"
#include "libraries/helpers/DebugHelper.h"


EditorToolBarAssistant::EditorToolBarAssistant(QWidget *parent,
                                               int iViewMode,
                                               EditorTextArea *iTextArea,
                                               QStringList iDisableToolList) : EditorToolBar(parent)
{
  if(parent==nullptr)
    criticalError("Call "+QString(__FUNCTION__)+" with NULL of parent.");

  editor=qobject_cast<Editor *>(parent);
  viewMode=iViewMode;
  textArea=iTextArea;

  currentFontFamily="";
  currentFontSize=0;
  flagSetFontParametersEnabled=true;

  // Настройка списков кнопок на панелях инструментов редактора
  this->init(iDisableToolList);

  this->setupSignals();

  // Устанавливается состояние распахнута или нет панель инструментов
  if(editor->editorConfig->get_expand_tools_lines())
    switchExpandToolsLines(1);
  else
    switchExpandToolsLines(-1);
}


EditorToolBarAssistant::~EditorToolBarAssistant()
{

}


void EditorToolBarAssistant::init(const QStringList &iDisableToolList)
{
    // Установка списков инструментов, считываемых из конфига
    this->initToolsLists(iDisableToolList);

    // Инициализация панели инструментов, согласно установленным спискам инструментов
    EditorToolBar::init();
}


// Установка списков инструментов, считываемых из конфига
void EditorToolBarAssistant::initToolsLists(const QStringList &iDisableToolList)
{
    // Перед инитом устанавливается список скрываемых инструментов
    EditorToolBar::initDisableToolList(iDisableToolList);


    // Выясняется перечень кнопок в первой строке на панели инструментов
    QStringList toolsList=editor->editorConfig->get_tools_line_1().split(",");

    // В мобильном режиме добавляется кнопка back (если ее нет)
    if(viewMode==Editor::WYEDIT_MOBILE_MODE && !toolsList.contains("back"))
    {
      toolsList.prepend("separator"); // Добавляется в начало панели
      toolsList.prepend("back");
    }

    // В мобильном режиме добавляется кнопка find_in_base (если ее нет)
    if(viewMode==Editor::WYEDIT_MOBILE_MODE && !toolsList.contains("findInBase"))
    {
      toolsList.append("spring"); // Добавляется в конец панели
      toolsList.append("findInBase");
    }


    // Устанавливается перечень кнопок на панели инструментов
    EditorToolBar::initToolsLine1(toolsList); // Первая строка
    EditorToolBar::initToolsLine2( editor->editorConfig->get_tools_line_2().split(",") ); // Вторая строка
}


void EditorToolBarAssistant::reload()
{
    // Очищаются тулбары
    EditorToolBar::clearToolsLines();

    // Из слоя с виджетами линеек панели инструментов убираются все виджеты (тулбары)
    QLayoutItem* item;
    while ( ( item = textformatButtonsLayout.takeAt( 0 ) ) != nullptr )
    {
        textformatButtonsLayout.removeItem(item);
    }

    EditorToolBar::isInit=false;

    // Установка списков инструментов, считанных из конфига
    this->initToolsLists( EditorToolBar::disableToolList );

    // Сборка тулбаров из инструментов
    EditorToolBar::assemblyButtons();

    EditorToolBar::isInit=true;
}


void EditorToolBarAssistant::setupSignals()
{
  connect(expandToolsLines, &QAction::triggered, this, &EditorToolBarAssistant::onExpandToolsLinesClicked);
}


// Метод только меняет значение, показываемое списком шрифтов
void EditorToolBarAssistant::onChangeFontselectOnDisplay(QString fontName)
{
    // TRACELOG

    if(flagSetFontParametersEnabled==false)
        return;

    flagSetFontParametersEnabled=false;

    // Шрифт, который будет выставлен в комбобоксе
    QString updateFontFamily;

    fontSelect->setIsProgrammChanged(true);
    if(fontName.size()>0)
    {
        int n=fontSelect->findText(fontName);

        // Если шрифт с заданным названием найден в списке шрифтов (в комбобоксе)
        if(n>=0)
        {
            fontSelect->setCurrentIndex(n);
            updateFontFamily=fontName;
        }
        else
        {
            // Иначе шрифт с указанным названием не найден в списке шрифтов,
            // и надо попробовать выставить комбобоксе стандартный похожий шрифт

            QFontDatabase database; // База всех установленных шрифтов

            // Вывод в консоль полного списка шифтов
            /*
            foreach (const QString &family, database.families()) {
               qDebug() << "Font family:" << family;
               foreach (const QString &style, database.styles(family)) {
                   qDebug() << "     style:" << style;
               }
            }
            */

            QList< QPair< QString, QString > > sameFonts;
            sameFonts << QPair<QString, QString>("Sans Serif",           "MS Sans Serif");
            sameFonts << QPair<QString, QString>("Sans Serif",           "Microsoft Sans Serif");
            sameFonts << QPair<QString, QString>("MS Sans Serif",        "Sans Serif");
            sameFonts << QPair<QString, QString>("Microsoft Sans Serif", "Sans Serif");

            // Перебираются пары похожих шрифтов
            for(auto currentFontPair : sameFonts)
            {
                // Если входящий шрифт имеет похожий шрифт в базе установленных шрифтов
                if(fontName==currentFontPair.first and
                   database.families().contains(currentFontPair.second))
                {
                    updateFontFamily=currentFontPair.second;
                    fontSelect->setCurrentIndex( fontSelect->findText(updateFontFamily) );
                    break;
                }
            }
        }
    }
    else
    {
        fontSelect->setCurrentIndex(0); // Пустой шрифт (теперь не используется, но пока оставлен)
    }
    fontSelect->setIsProgrammChanged(false);

    currentFontFamily=updateFontFamily;

    flagSetFontParametersEnabled=true;
}


bool EditorToolBarAssistant::getFlagSetFontParametersEnabled()
{
    return flagSetFontParametersEnabled;
}


// Слот только устанавливает значение, показываемое списком размеров шрифта
// А размер шрифта текста не меняет
void EditorToolBarAssistant::onChangeFontsizeOnDisplay(int n)
{
    // TRACELOG

    if(flagSetFontParametersEnabled==false)
        return;

    flagSetFontParametersEnabled=false;

    fontSize->setIsProgrammChanged(true); // Устанавливается флаг, что значение меняется программно, а не действиями пользователя
    fontSize->setCurrentIndex(fontSize->findData(n));
    fontSize->setIsProgrammChanged(false); // Снимается флаг, что значение меняется программно
    currentFontSize=n;

    flagSetFontParametersEnabled=true;
}


void EditorToolBarAssistant::onChangeFontFamily(QString fontFamily)
{
    // TRACELOG

    currentFontFamily=fontFamily;
}


void EditorToolBarAssistant::onChangeFontPointSize(int n)
{
    // TRACELOG

    currentFontSize=n;
}


// Изменение цвета иконки выделения цвета шрифта
void EditorToolBarAssistant::onChangeFontcolor(const QColor &color)
{
    // TRACELOG

    // Формат символов под курсором
    QTextCharFormat textAreaFormat = textArea->currentCharFormat();

    // Есть ли ForegroundBrush под курсором
    bool hasForegroundBrush = textAreaFormat.hasProperty(QTextFormat::ForegroundBrush);

    QColor fillColor;

    // Есть ли ForegroundBrush под курсором
    if(hasForegroundBrush && color.isValid())
        fillColor=color;
    else
    {
        // Если нет ForegroundBrush в тексте под курсором, то
        // за цвет кнопки берется цвет foreground редактора textArea (QTextEdit)
        // (это позволяет учитывать также цвет шрифта, заданный в файле stylesheet.css)
        fillColor=textArea->palette().windowText().color();
    }

    fontColor->setIcon( drawIconOverColor(fillColor, QIcon(":/resource/pic/edit_fontcolor.svg")) );
}


// Изменение цвета иконки цвета шрифта, в зависимости от положения курсора
void EditorToolBarAssistant::onChangeIconFontColor(const QTextCharFormat &format)
{
    QColor color = format.foreground().color();
    onChangeFontcolor(color);
}


// Изменение цвета иконки для фона текста
void EditorToolBarAssistant::onChangeBackgroundColor(const QColor &color)
{
    // TRACELOG

    // Формат символов под курсором
    QTextCharFormat textAreaFormat = textArea->currentCharFormat();

    // Есть ли BackgroundBrush в тексте под курсором
    bool hasTextBackgroundBrush = textAreaFormat.hasProperty(QTextFormat::BackgroundBrush);

    QPixmap pixMap( getIconSize() );
    QColor fillColor;

    // Есть ли BackgroundBrush в тексте под курсором
    if(hasTextBackgroundBrush && color.isValid())
    {
        // Если есть BackgroundBrush под курсором, то
        // кнопку красим в цвет заливки текста
        fillColor=color;
    }
    else
    {
        // Проверка, есть ли таблица под курсором и/или подключены стили из stylesheet.css
        QTextCursor txtCursor = textArea->textCursor();
        QTextTable *textTable = txtCursor.currentTable();
        if(textTable != nullptr)
        {
            // Если курсор находится в таблице
            QTextTableFormat textTableFormat = textTable->format();
            QTextTableCell tableCell = textTable->cellAt(txtCursor);
            QTextCharFormat tableCellFormat = tableCell.format();
            QColor tableColor = textTableFormat.background().color();
            QColor charColor = tableCellFormat.background().color();

            // Есть ли BackgroundBrush в таблице под курсором
            bool hasTableBackgroundBrush = textTableFormat.hasProperty(QTextFormat::BackgroundBrush);

            // Есть ли BackgroundBrush в ячейке под курсором
            bool hasCelBackgroundBrush = tableCellFormat.hasProperty(QTextFormat::BackgroundBrush);

            if(hasTableBackgroundBrush && hasCelBackgroundBrush && charColor.isValid())
            {
                // Если есть BackgroundBrush в таблице под курсором и
                // есть BackgroundBrush в ячейке под курсором, то
                // кнопку красим в цвет заливки ячейки
                fillColor=charColor;
            }
            else if(hasTableBackgroundBrush && !hasCelBackgroundBrush && tableColor.isValid())
            {
                // Если есть BackgroundBrush в таблице под курсором но
                // нет BackgroundBrush в ячейке под курсором, то
                // кнопку красим в цвет заливки таблицы
                fillColor=tableColor;
            }
            else
            {
                // Если нет BackgroundBrush в таблице под курсором и
                // нет BackgroundBrush в ячейке под курсором,
                // то за цвет кнопки берется цвет background редактора textArea (QTextEdit)
                // (это позволяет учитывать также цвет фона, заданный в файле stylesheet.css)
                fillColor=textArea->palette().window().color();
            }
        }
        else
        {
            // Если нет BackgroundBrush в тексте под курсором, то
            // за цвет кнопки берется цвет background редактора textArea (QTextEdit)
            // (это позволяет учитывать также цвет фона, заданный в файле stylesheet.css)
            fillColor=textArea->palette().window().color();
        }
    }

    backgroundColor->setIcon( drawIconOverColor(fillColor, QIcon(":/resource/pic/edit_fontbackgroundcolor.svg")) );
}


// Изменение цвета иконки выделения фона текста, в зависимости от положения курсора
void EditorToolBarAssistant::onChangeIconBackgroundColor(const QTextCharFormat &format)
{
    QColor color = format.background().color();
    onChangeBackgroundColor(color);
}


QPixmap EditorToolBarAssistant::drawIconOverColor(const QColor &fillColor, const QIcon &icon ) const
{
    QPixmap pixMap( getIconSize() );
    pixMap.fill( fillColor );

    QPainter painter;
    painter.begin(&pixMap);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::HighQualityAntialiasing);
    painter.drawPixmap(0, 0, icon.pixmap( pixMap.size() ));
    painter.end();

    return pixMap;
}


// Слот, вызываемый при изменение позиции курсора
void EditorToolBarAssistant::onCursorPositionChanged()
{
    // Изменение цвета иконки выделения фона текста при изменении позиции курсора
    QColor color = textArea->currentCharFormat().background().color();

    // Вызывается слот "Изменение цвета иконки выделения фона текста"
    onChangeBackgroundColor(color);
}


// Слот обновления подсветки кнопок выравнивания текста
// Если параметр activate=false, все кнопки будут выставлены в неактивные
// Если параметр activate=true, будет подсвечена кнопка, соответсвующая текущему форматированию
void EditorToolBarAssistant::onUpdateAlignButtonHiglight(bool activate)
{
    // TRACELOG

    alignLeft->setChecked(false);
    alignCenter->setChecked(false);
    alignRight->setChecked(false);
    alignWidth->setChecked(false);

    if(activate==false)
        return;

    if(textArea->alignment()==Qt::AlignLeft)         alignLeft->setChecked(true);
    else if(textArea->alignment()==Qt::AlignHCenter) alignCenter->setChecked(true);
    else if(textArea->alignment()==Qt::AlignRight)   alignRight->setChecked(true);
    else if(textArea->alignment()==Qt::AlignJustify) alignWidth->setChecked(true);
}


// Обновление подсветки клавиш начертания текста
void EditorToolBarAssistant::onUpdateOutlineButtonHiglight(void)
{
    // TRACELOG

    bold->setChecked(false);
    italic->setChecked(false);
    underline->setChecked(false);
    strikeout->setChecked(false);
    superscript->setChecked(false);
    subscript->setChecked(false);

    if(textArea->fontWeight()==QFont::Bold) bold->setChecked(true);
    if(textArea->fontItalic()==true)        italic->setChecked(true);
    if(textArea->fontUnderline()==true)     underline->setChecked(true);
    if(textArea->textCursor().charFormat().fontStrikeOut()) strikeout->setChecked(true);

    const QTextCharFormat charFormat = textArea->textCursor().charFormat();
    if(charFormat.verticalAlignment() == QTextCharFormat::AlignSuperScript) {
        superscript->setChecked(true);
    } else if(charFormat.verticalAlignment() == QTextCharFormat::AlignSubScript) {
        subscript->setChecked(true);
    }
}


void EditorToolBarAssistant::setOutlineButtonHiglight(int button, bool active)
{
    if(button==BT_BOLD)
    {
        if(active==false) bold->setChecked(false);
        else              bold->setChecked(true);
        return;
    }

    if(button==BT_ITALIC)
    {
        if(active==false) italic->setChecked(false);
        else              italic->setChecked(true);
        return;
    }

    if(button==BT_UNDERLINE)
    {
        if(active==false) underline->setChecked(false);
        else              underline->setChecked(true);
        return;
    }

    if(button==BT_STRIKEOUT)
    {
        if(active==false) strikeout->setChecked(false);
        else              strikeout->setChecked(true);
        return;
    }

    if(button==BT_SUPERSCRIPT)
    {
        if(active==false) superscript->setChecked(false);
        else              superscript->setChecked(true);
        return;
    }

    if(button==BT_SUBSCRIPT)
    {
        if(active==false) subscript->setChecked(false);
        else              subscript->setChecked(true);
        return;
    }
}


void EditorToolBarAssistant::updateToActualFormat(void)
{
    // Текущий шрифт позиции, где находится курсор
    QString actualFontFamily=editor->smartFontFamily( textArea->fontFamily() );
    if(currentFontFamily!=actualFontFamily)
        onChangeFontselectOnDisplay(actualFontFamily);

    // Размер
    int actualFontPointSize=editor->smartFontSize( static_cast<int>(textArea->fontPointSize()) );
    if(currentFontSize!=actualFontPointSize) {
        onChangeFontsizeOnDisplay(actualFontPointSize);
    }


    // Обновляются кнопки форматирования начертания
    onUpdateOutlineButtonHiglight();

    // Обновляются кнопки выравнивания
    onUpdateAlignButtonHiglight(true);
}


void EditorToolBarAssistant::onExpandToolsLinesClicked(void)
{
    // TRACELOG

    switchExpandToolsLines();
}


// Метод, переключающий состояние видимости полной панели инструментов
// Если вызывается без параметра (по умолчанию 0), метод сам переключает
// Параметр 1 - включить полную видимость
// Параметр -1 - выключить полную видимость
void EditorToolBarAssistant::switchExpandToolsLines(int flag)
{
    bool setFlag=true;

    // Если метод был вызван без параметра
    if(flag==0)
    {
        bool is_expand=editor->editorConfig->get_expand_tools_lines();

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
    toolsLine2.setVisible(setFlag);
    if(viewMode==Editor::WYEDIT_DESKTOP_MODE)
        editor->indentSliderAssistant->setVisible(setFlag);

    // Запоминается новое состояние
    editor->editorConfig->set_expand_tools_lines(setFlag);

    // Обновляется геометрия расположения движков на слайд-панели.
    // Это необходимо из-за того, что при появлении/скрытии линейки отступов высота области редактирования меняется,
    // и вертикальная прокрутка при соответствующем размере текста может быть видна или не видна.
    // То есть, возможен вариант, когда вертикальная прокрутка появляется при включении видимости слайд-панели,
    // а ее наличие (ее ширина) влияет на ширину и правый движок слайд-панели
    emit updateIndentSliderGeometry();
}


bool EditorToolBarAssistant::isKeyForToolLineUpdate(QKeyEvent *event)
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


int EditorToolBarAssistant::getFontSizeByNum(int n)
{
    return fontSize->itemData(n).toInt();
}

// Режим представления (мобильный или десктопный)
int EditorToolBarAssistant::getViewMode()
{
    return viewMode;
}
