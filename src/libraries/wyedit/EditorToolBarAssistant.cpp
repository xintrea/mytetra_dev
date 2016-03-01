#include <QColor>
#include <QButtonGroup>

#include "main.h"
#include "EditorToolBarAssistant.h"
#include "Editor.h"
#include "EditorToolBar.h"
#include "EditorTextArea.h"
#include "EditorConfig.h"
#include "EditorIndentSliderAssistant.h"
#include "../TraceLogger.h"


EditorToolBarAssistant::EditorToolBarAssistant(QWidget *parent,
                                               int iViewMode,
                                               EditorTextArea *iTextArea,
                                               QStringList iDisableToolList) : EditorToolBar(parent)
{
  if(parent==NULL)
    criticalError("Call "+QString(__FUNCTION__)+" with NULL of parent.");

  editor=qobject_cast<Editor *>(parent);
  viewMode=iViewMode;
  textArea=iTextArea;

  initDisableToolList(iDisableToolList); // Перед инитом устанавливается список скрываемых инструментов

  // Выясняется перечень кнопок в первой строке на панели инструментов
  QStringList toolsListInLine1=editor->editorConfig->get_tools_line_1().split(",");

  // В мобильном режиме добавляется кнопка back (если ее нет)
  if(viewMode==Editor::WYEDIT_MOBILE_MODE && !toolsListInLine1.contains("back"))
  {
    toolsListInLine1.prepend("separator");
    toolsListInLine1.prepend("back");
  }

  // В мобильном режиме добавляется кнопка find_in_base (если ее нет)
  if(viewMode==Editor::WYEDIT_MOBILE_MODE && !toolsListInLine1.contains("find_in_base"))
  {
    toolsListInLine1.append("separator");
    toolsListInLine1.append("find_in_base");
  }

  // Устанавливается перечень кнопок на панели инструментов
  initToolsLine1(toolsListInLine1); // Первая строка
  initToolsLine2( editor->editorConfig->get_tools_line_2().split(",") ); // Вторая строка

  currentFontFamily="";
  currentFontSize=0;
  currentFontColor="#000000";
  buttonsSelectColor=QColor(125,170,240,150); // 92,134,198
  flagSetFontParametersEnabled=true;

  // Инициализация панели инструментов
  init();

  setupSignals();

  // Устанавливается состояние распахнута или нет панель инструментов
  if(editor->editorConfig->get_expand_tools_lines())
    switchExpandToolsLines(1);
  else
    switchExpandToolsLines(-1);
}


EditorToolBarAssistant::~EditorToolBarAssistant()
{
  // EditorToolBar::~EditorToolBar();
}


void EditorToolBarAssistant::setupSignals()
{
  connect(expandToolsLines, SIGNAL(clicked()), this, SLOT(onExpandToolsLinesClicked()));
}


// Метод только меняет значение, показываемое списком шрифтов
void EditorToolBarAssistant::onChangeFontselectOnDisplay(QString fontName)
{
  // TRACELOG

  if(flagSetFontParametersEnabled==false)
    return;

  flagSetFontParametersEnabled=false;

  if(fontName.size()>0)
    fontSelect->setCurrentIndex(fontSelect->findText(fontName));
  else
    fontSelect->setCurrentIndex(fontSelect->count()-1);

  currentFontFamily=fontName;

  flagSetFontParametersEnabled=true;
}


bool EditorToolBarAssistant::getFlagSetFontParametersEnabled()
{
  return flagSetFontParametersEnabled;
}


// Слот только меняет значение, показываемое списком размеров шрифта
void EditorToolBarAssistant::onChangeFontsizeOnDisplay(int n)
{
  // TRACELOG

  if(flagSetFontParametersEnabled==false)
    return;

  flagSetFontParametersEnabled=false;

  fontSize->setCurrentIndex(fontSize->findData(n));
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


void EditorToolBarAssistant::onChangeFontcolor(QColor color)
{
  // TRACELOG

  // Если цвет правильный
  if(color.isValid())
  {
    fontColor->setPalette(QPalette(color)); // Меняется цвет кнопки, отвечающей за цвет шрифта
    currentFontColor=color.name(); // todo: подумать, а нужна ли эта переменна
  }

}


// Метод для обновления положения кнопок выравнивания текста (зажата/отжата)
void EditorToolBarAssistant::updateAlignButtonSelection()
{
  // TRACELOG

  const int aligment = textArea->alignment();
  foreach(QAbstractButton *button, alignButtons->buttons())
  {
      if (alignButtons->id(button) == aligment)
      {
          button->setChecked(true);
          break;
      }
  }
}


// Обновление подсветки клавиш начертания текста
void EditorToolBarAssistant::onUpdateOutlineButtonHiglight(void)
{
  // TRACELOG

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


void EditorToolBarAssistant::setOutlineButtonHiglight(int button, bool active)
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


void EditorToolBarAssistant::updateToActualFormat(void)
{
  // Список должен показывать текущий шрифт позиции, где находится курсор
  if(currentFontFamily!=textArea->fontFamily())
    onChangeFontselectOnDisplay(textArea->fontFamily());

  // Размер
  if(currentFontSize!=(int)textArea->fontPointSize())
    onChangeFontsizeOnDisplay((int)textArea->fontPointSize());

  // Обновляются кнопки форматирования начертания
  onUpdateOutlineButtonHiglight();

  // Обновляются кнопки выравнивания
  updateAlignButtonSelection();
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
  toolsLine2->setVisible(setFlag);
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
