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
#include "libraries/DiskHelper.h"


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
  delete editorConfig;
  delete editorToolBar;
  delete buttonsAndEditLayout;
  delete editorContextMenu;
  delete textArea;
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

  // Создается виджет поиска, обязательно нужно указать parent чтобы
  // могли применяться флаги окна.
  // Виджет будет постоянно включен параллельно с работой редактора.
  // Только будет либо виден, либо невиден.
  findDialog=new EditorFindDialog(this);
  findDialog->hide();

  // Создаётся контекстное меню
  editorContextMenu=new EditorContextMenu(this);

  setupEditorToolBar();
  setupIndentSlider();
  setupEditorArea();

  setupSignals();

  if(initDataEnableAssembly)
    assembly();

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

  updateIndentlineGeometry();

  // Устанавливается состояние распахнуто или нет панель инструментов
  if(editorConfig->get_expand_tools_lines())
    switchExpandToolsLines(1);
  else
    switchExpandToolsLines(-1);

  if(initDataEnableRandomSeed)
  {
    QDateTime datetime=QDateTime::currentDateTime ();
    int seed=datetime.toTime_t();
    // qDebug() << "Random generator init " << seed;
    srand(seed);
    // qDebug() << "Random generator " << rand();
  }
}


void Editor::setupSignals(void)
{
  // Создание сигналов, генерируемых кнопками форматирования текста
  connect(editorToolBar->bold,SIGNAL(clicked()),        this,SLOT(onBoldClicked()));
  connect(editorToolBar->italic,SIGNAL(clicked()),      this,SLOT(onItalicClicked()));
  connect(editorToolBar->underline,SIGNAL(clicked()),   this,SLOT(onUnderlineClicked()));
  connect(editorToolBar->monospace,SIGNAL(clicked()),   this,SLOT(onMonospaceClicked()));
  connect(editorToolBar->code,SIGNAL(clicked()),        this,SLOT(onCodeClicked()));
  connect(editorToolBar->clear,SIGNAL(clicked()),       this,SLOT(onClearClicked()));

  connect(editorToolBar->numericList,SIGNAL(clicked()), this,SLOT(onNumericlistClicked()));
  connect(editorToolBar->dotList,SIGNAL(clicked()),     this,SLOT(onDotlistClicked()));

  connect(editorToolBar->indentPlus,SIGNAL(clicked()),  this,SLOT(onIndentplusClicked()));
  connect(editorToolBar->indentMinus,SIGNAL(clicked()), this,SLOT(onIndentminusClicked()));

  connect(editorToolBar->alignLeft,SIGNAL(clicked()),   this,SLOT(onAlignleftClicked()));
  connect(editorToolBar->alignCenter,SIGNAL(clicked()), this,SLOT(onAligncenterClicked()));
  connect(editorToolBar->alignRight,SIGNAL(clicked()),  this,SLOT(onAlignrightClicked()));
  connect(editorToolBar->alignWidth,SIGNAL(clicked()),  this,SLOT(onAlignwidthClicked()));

  connect(editorToolBar->fontSelect,SIGNAL(currentFontChanged(const QFont &)), this,SLOT(onFontselectChanged(const QFont &)));
  connect(editorToolBar->fontSize,SIGNAL(currentIndexChanged(int)),            this,SLOT(onFontsizeChanged(int)));
  connect(editorToolBar->fontColor,SIGNAL(clicked()),                          this,SLOT(onFontcolorClicked()));

  connect(editorToolBar->showHtml,SIGNAL(clicked()),this,SLOT(onShowhtmlClicked()));
  connect(editorToolBar->findText,SIGNAL(clicked()),this,SLOT(onFindtextClicked()));
  connect(editorToolBar->settings,SIGNAL(clicked()),this,SLOT(onSettingsClicked()));
  connect(editorToolBar->showFormatting,SIGNAL(clicked()),this,SLOT(onShowformattingClicked()));

  // Кнопки работы с таблицами
  connect(editorToolBar->createTable,SIGNAL(clicked()),    this,SLOT(onCreatetableClicked()));
  connect(editorToolBar->tableRemoveRow,SIGNAL(clicked()), this,SLOT(onTableRemoveRowClicked()));
  connect(editorToolBar->tableRemoveCol,SIGNAL(clicked()), this,SLOT(onTableRemoveColClicked()));
  connect(editorToolBar->tableAddRow,SIGNAL(clicked()),    this,SLOT(onTableAddRowClicked()));
  connect(editorToolBar->tableAddCol,SIGNAL(clicked()),    this,SLOT(onTableAddColClicked()));
  connect(editorToolBar->tableMergeCells,SIGNAL(clicked()),this,SLOT(onTableMergeCellsClicked()));
  connect(editorToolBar->tableSplitCell,SIGNAL(clicked()), this,SLOT(onTableSplitCellClicked()));

  // Прочие кнопки
  connect(editorToolBar->insertImageFromFile, SIGNAL(clicked()), this, SLOT(onInsertImageFromFileClicked()));
  connect(editorToolBar->expandEditArea, SIGNAL(clicked()), this, SLOT(onExpandEditAreaClicked()));
  connect(editorToolBar->expandToolsLines, SIGNAL(clicked()), this, SLOT(onExpandToolsLinesClicked()));
  connect(editorToolBar->save, SIGNAL(clicked()), this, SLOT(onSaveClicked()));
  connect(editorToolBar->back, SIGNAL(clicked()), this, SLOT(onBackClicked()));
  connect(editorToolBar->findInBase, SIGNAL(clicked()), this, SLOT(onFindInBaseClicked()));
  connect(editorToolBar->showText, SIGNAL(clicked()), this, SLOT(onShowTextClicked()));
  connect(editorToolBar->toAttach, SIGNAL(clicked()), this, SLOT(onToAttachClicked()));

  // Область редактирования текста
  connect(textArea,SIGNAL(cursorPositionChanged()), this,SLOT(onCursorPositionChanged()));
  connect(textArea,SIGNAL(selectionChanged()),      this,SLOT(onSelectionChanged()));

  // Линейка отступов
  connect(indentSlider,SIGNAL(change_textindent_pos(int)), this,SLOT(onIndentlineChangeTextindentPos(int)));
  connect(indentSlider,SIGNAL(change_leftindent_pos(int)), this,SLOT(onIndentlineChangeLeftindentPos(int)));
  connect(indentSlider,SIGNAL(change_rightindent_pos(int)),this,SLOT(onIndentlineChangeRightindentPos(int)));
  connect(indentSlider,SIGNAL(mouse_release()),            this,SLOT(onIndentlineMouseRelease()));

  connect(this,SIGNAL(send_set_textindent_pos(int)), indentSlider,SLOT(set_textindent_pos(int)));
  connect(this,SIGNAL(send_set_leftindent_pos(int)), indentSlider,SLOT(set_leftindent_pos(int)));
  connect(this,SIGNAL(send_set_rightindent_pos(int)),indentSlider,SLOT(set_rightindent_pos(int)));

  // Соединение сигнал-слот чтобы показать контекстное меню по правому клику в редакторе
  connect(textArea, SIGNAL(customContextMenuRequested(const QPoint &)),
          this, SLOT(onCustomContextMenuRequested(const QPoint &)));

  // Соединение сигнал-слот чтобы показать контекстное меню по долгому нажатию
  connect(textArea, SIGNAL(tapAndHoldGestureFinished(const QPoint &)),
          this, SLOT(onCustomContextMenuRequested(const QPoint &)));

  // connect(textArea->document(), SIGNAL(modificationChanged (bool )),
  //         this, SLOT(onModificationChanged(bool)));


  // Сигналы контекстного меню
  connect(editorContextMenu,SIGNAL(undo()),this,SLOT(onUndo()));
  connect(editorContextMenu,SIGNAL(redo()),this,SLOT(onRedo()));
  connect(editorContextMenu,SIGNAL(cut()),this,SLOT(onCut()));
  connect(editorContextMenu,SIGNAL(copy()),this,SLOT(onCopy()));
  connect(editorContextMenu,SIGNAL(paste()),this,SLOT(onPaste()));
  connect(editorContextMenu,SIGNAL(selectAll()),this,SLOT(onSelectAll()));
  connect(editorContextMenu,SIGNAL(contextMenuEditImageProperties()),this,SLOT(onContextMenuEditImageProperties()));

  // Вызов диалога поиска в тексте
  connect(findDialog, SIGNAL(find_text(const QString &, QTextDocument::FindFlags)),
          this, SLOT(onFindtextSignalDetect(const QString &, QTextDocument::FindFlags)) );

  connect(textArea, SIGNAL(updateIndentlineGeometrySignal()),
          this, SLOT(onUpdateIndentlineGeometrySlot()) );
}


// Создание и настройка панели инструментов редактора
void Editor::setupEditorToolBar(void)
{
  // Создается панель с кнопками
  editorToolBar=new EditorToolBar(this);
  editorToolBar->init();
}


// Создание и настройка линейки отступов
void Editor::setupIndentSlider(void)
{
  indentSlider=new IndentSlider(this->width(), 16, this);
  indentSlider->setObjectName("editor_tb_indentslider");
}


// Создание и настройка области редактирования
void Editor::setupEditorArea(void)
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


// Сборка редактора и его активизация
void Editor::assembly(void)
{
  // Создается вертикальная область, которая должна объединить
  // линейку кнопок редактирования и область редактирования текста
  buttonsAndEditLayout=new QVBoxLayout(this);
  buttonsAndEditLayout->setObjectName("buttons_and_edit_layout");

  // Добавляется виджет с кнопками редактора
  buttonsAndEditLayout->addWidget(editorToolBar);

  // Добавляется область редактирования
  buttonsAndEditLayout->addWidget(textArea);

  // Полученый набор элементов устанавливается для текущего окна
  setLayout(buttonsAndEditLayout);

  // Границы убираются, так как данный объект будет использоваться как виджет
  QLayout *lt;
  lt=layout();
  lt->setContentsMargins(0,2,0,0);
}


void Editor::onUpdateIndentlineGeometrySlot()
{
  updateIndentlineGeometry();
}


// Синхронизация размеров линейки отступов относительно области редактирования
void Editor::updateIndentlineGeometry()
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
void Editor::setTextarea(QString text)
{
  textArea->setHtml(text);

  // Очищается URL документа, так как документ создается "из ничего"
  textArea->document()->setMetaInformation( QTextDocument::DocumentUrl, "" );
}


// Установка запрета или разрешения редактирования
void Editor::setTextareaEditable(bool editable)
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
QString Editor::getTextarea(void)
{
  return textArea->document()->toHtml("UTF-8");
}



QTextDocument *Editor::getTextareaDocument(void)
{
  return textArea->document();
}


bool Editor::setWorkDirectory(QString dirName)
{
  QDir directory(dirName);

  if(directory.exists())
  {
    workDirectory=dirName;
    return true;
  }
  else
  {
    criticalError("WyEdit: Can not set work directory to "+dirName+". Directory not exists.");
    return false;
  }
}


QString Editor::getWorkDirectory(void)
{
  return workDirectory;
}


void Editor::setFileName(QString fileName)
{
  workFileName=fileName;
}


QString Editor::getFileName(void)
{
  return workFileName;
}


// Сохранение HTML кода документа в файл
bool Editor::saveTextareaText()
{
  if(workFileName.length()==0)
  {
    criticalError("WyEdit: Save function. Not setted work file name for editor.");
    return false;
  }

  QString fullFileName=workDirectory+"/"+workFileName;

  // Создание нового файла записи
  QFile wfile(fullFileName);

  if (!wfile.open(QIODevice::WriteOnly | QIODevice::Text))
  {
    criticalError("WyEdit: Cant open file "+fullFileName+" for write.");
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
bool Editor::saveTextareaImages(int mode=SAVE_IMAGES_SIMPLE)
{
  qDebug() << "Save images...\n" ;
  qDebug() << "Block count" << textArea->document()->blockCount() << "\n";

  if(workDirectory.length()==0)
  {
    criticalError("WyEdit: Save images function. Not setted work directory.");
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


void Editor::saveTextarea(void)
{
  qDebug() << "Save textarea...";

  // Если запись была открыта на просмотр и изменена
  if(getWorkDirectory().length()!=0 &&
     getFileName().length()!=0 &&
     getTextareaModified()==true)
  {
    // Перенос текущего файла записи в корзину
    qDebug() << "Try remove file " << getFileName() << " from directory " << getWorkDirectory();
    if( QFileInfo( getWorkDirectory()+"/"+getFileName() ).exists() )
    {
      qDebug() << "File exists. Remove it.";
      DiskHelper::removeFileToTrash(getWorkDirectory()+"/"+getFileName());
    }
    else
      qDebug() << "Cant remove file. File not exists.";

    // Если происходит прямая работа с файлом текста
    if(load_callback_func==NULL)
    {
      // Сохранение текста записи в файл
      saveTextareaText();

      // Сохранение картинок, которые есть в записи
      saveTextareaImages(Editor::SAVE_IMAGES_REMOVE_UNUSED);
    }
    else
    {
      // Иначе задана функция обратного вызова для записи текста и картинок

      QString content=textArea->document()->toHtml("UTF-8");
      save_callback_func(qobject_cast<QObject *>(this), content);
    }

    // Так как произошло сохранение,
    // отмечается что новый текст небыл еще изменен
    setTextareaModified(false);
  }
}


bool Editor::loadTextarea()
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
    criticalError("WyEdit: Load function. Not setted work file name for editor.");
    return false;
  }

  if(workDirectory.length()==0)
  {
    criticalError("WyEdit: Not setted work directory for editor.");
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
      criticalError("WyEdit: File "+fileName+" not found");
      return false;
    }

    // Если нужный файл недоступен для чтения
    if(!f.open(QIODevice::ReadOnly))
    {
      criticalError("WyEdit: File "+fileName+" not readable. Check permission.");
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


void Editor::setTextareaModified(bool modify)
{
  qDebug() << "Editor::set_textarea_modified() :" << modify;
  textArea->document()->setModified(modify);
}


bool Editor::getTextareaModified(void)
{
  qDebug() << "Editor::get_textarea_modified() :" << textArea->document()->isModified();
  return textArea->document()->isModified();
}


/////////////////////////////////////////////////
// Форматирование текста
/////////////////////////////////////////////////

// Форматирование Bold
void Editor::onBoldClicked(void)
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

  updateOutlineButtonHiglight();
}

// Форматирование Italic
void Editor::onItalicClicked(void)
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

  updateOutlineButtonHiglight();
}

// Форматирование подчеркивания
void Editor::onUnderlineClicked(void)
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

  updateOutlineButtonHiglight();
}


// Форматирование моноширинным шрифтом
void Editor::onMonospaceClicked(void)
{
  textArea->textCursor().beginEditBlock();

  // Устанавливается шрифт
  QFont font;
  font.fromString(editorConfig->get_monospace_font());
  textArea->setFontFamily(font.family());

  // Новый установленный шрифт показывается в выпадающем списке шрифтов
  setFontselectOnDisplay(font.family());

  // Если в настройках разрешена автоматическая установка нужного размера
  // моноширинного шрифта
  if(editorConfig->get_monospace_font_size_apply()==true)
  {
    // Устанавливается нужный размер
    textArea->setFontPointSize(editorConfig->get_monospace_font_size());

    // В выпадающем списке размеров выставляется установленный размер
    setFontsizeOnDisplay(editorConfig->get_monospace_font_size());
  }

  textArea->textCursor().endEditBlock();
}


// Выбран ли блок текста (т.е. находятся ли начало и конец выделения
// точно на границах блока)
bool Editor::isBlockSelect(void)
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
bool Editor::isCursorOnEmptyLine(void)
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


bool Editor::isCursorOnSpaceLine(void)
{
  // Если есть выделение, функция работать не должна
  if(textArea->textCursor().hasSelection())
    return false;

  // Создаётся дополнительный курсор
  QTextCursor cursor=textArea->textCursor();

  // Выделяется анализируемая строка
  cursor.select(QTextCursor::LineUnderCursor);

  // Текст анализируемой строки
  QString selectedText=cursor.selectedText();

  if(selectedText.simplified().size()>0)
    return false;
  else
    return true;
}


// Форматирование в качестве кода
void Editor::onCodeClicked(void)
{
  // Если выделения нет
  if(!textArea->textCursor().hasSelection()) return;

  bool enableIndent;

  // Проверяется, выбран ли четко блок (блоки) текста
  // Блок - это текст между <p>...</p>
  if(!isBlockSelect())
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
  setFontselectOnDisplay(font.family());

  // Если в настройках разрешена автоматическая установка нужного размера
  if(editorConfig->get_code_font_size_apply()==true)
  {
    // Устанавливается нужный размер
    textArea->setFontPointSize(editorConfig->get_code_font_size());

    // В выпадающем списке размеров выставляется установленный размер
    setFontsizeOnDisplay(editorConfig->get_code_font_size());
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

    updateIndentsliderToActualFormat();
  }

  textArea->textCursor().endEditBlock();
}


// Очистка форматирования, т.е. установка стандартного шрифта,
// размера и убирание утолщения, наклона, подчеркивания
void Editor::onClearClicked(void)
{
  int startCursorPos=textArea->textCursor().position();
  int stopCursorPos=textArea->textCursor().anchor();
  qDebug() << "Cursor start position: " << startCursorPos << "Cursor stop position: " << stopCursorPos;


  bool flag_cursor_on_empty_line=isCursorOnEmptyLine();
  bool flag_cursor_on_space_line=isCursorOnSpaceLine();

  // Очистка возможна только если что-то выделено
  // Или курсор стоит на пустой строке с одним символом перевода строки
  // Или курсор стоит на строке, в которой нет текста
  if(!(textArea->textCursor().hasSelection() ||
       flag_cursor_on_empty_line ||
       flag_cursor_on_space_line))
    return;

  textArea->textCursor().beginEditBlock();

  if(flag_cursor_on_space_line)
    (textArea->textCursor()).select(QTextCursor::LineUnderCursor);


  // Создается стандартный шрифт
  QFont font;
  font.fromString( editorConfig->get_default_font() ); // Стандартное начертание
  font.setPointSize( editorConfig->get_default_font_size() ); // Стандартный размер

  // Применяется стандартный шрифт
  textArea->setCurrentFont(font);

  // Новый установленный шрифт показывается в выпадающем списке шрифтов
  setFontselectOnDisplay(font.family());

  // В выпадающем списке размеров выставляется установленный размер
  setFontsizeOnDisplay(editorConfig->get_default_font_size());


  // Очищается формат символов
  QColor clearColor;
  QBrush clearBrush( clearColor );
  QTextCharFormat clearCharFormat;
  clearCharFormat.setForeground( clearBrush );
  textArea->setCurrentCharFormat( clearCharFormat );


  // Если выделен блок
  // или курсор на пустой линии
  // или курсор на линии на которой нет символов
  if(isBlockSelect() ||
     flag_cursor_on_empty_line ||
     flag_cursor_on_space_line)
  {
    QTextBlockFormat format;

    // Убираются отступы
    format.setLeftMargin(0);   // Убирается левый отступ (который, возможно был установлен слайдером или кнопками изменения отступа)
    format.setRightMargin(0);
    format.setTopMargin(0);    // Убираются межстрочные интервалы, которые самопроизвольно появляются при вставке из других программ
    format.setBottomMargin(0);
    format.setAlignment(Qt::AlignLeft); // Выравнивание по левому краю

    // Применение форматирование
    textArea->textCursor().setBlockFormat(format);
  }


  // Если была работа со строкой, в которой нет символов,
  // курсор переносится на начало строки, чтобы не путать пользователя
  if(flag_cursor_on_space_line)
    textArea->moveCursor(QTextCursor::StartOfLine);


  // Очистка закомментирована, так как она заменена очисткой формата символов setCurrentCharFormat()
  // А так же эта очистка некорректно работает из-за особенностей вставки в Qt (первая строка получает отличный от остальных строк формат).
  // Думать дальше
  /*
 // Удаление какого-либо форматирования стилем
 QString htmlCode=textArea->textCursor().selection().toHtml();
 qDebug() << "Before remove style: " << htmlCode;

 // В регулярных выражениях Qt кванторы по-умолчанию жадные (greedy)
 // Поэтому напрямую регвыру указывается что кванторы должны быть ленивые
 QRegExp replace_expression("style=\".*\"");
 replace_expression.setMinimal(true);

 htmlCode.replace(replace_expression, "");
 qDebug() << "After remove style: " << htmlCode;

 QString currStyleSheet=textArea->document()->defaultStyleSheet();
 textArea->document()->setDefaultStyleSheet(" ");

 textArea->textCursor().removeSelectedText();
 textArea->textCursor().insertHtml(htmlCode);

 textArea->document()->setDefaultStyleSheet(currStyleSheet);

 textArea->textCursor().setPosition(startCursorPos);
 textArea->textCursor().setPosition(stopCursorPos, QTextCursor::KeepAnchor);
 // textArea->setTextCursor( textArea->textCursor() );

 qDebug() << "Cursor position: " << textArea->textCursor().position() << "Cursor anchor: " << textArea->textCursor().anchor();
 */

  textArea->textCursor().endEditBlock();

  // Вызывается метод, как будто переместился курсор с выделением, чтобы
  // обновились состояния подсветок кнопок форматирования
  onSelectionChanged();

  updateIndentsliderToActualFormat();
}


void Editor::formatToList(QTextListFormat::Style setFormat)
{
  // Если выделения нет
  if(!textArea->textCursor().hasSelection()) return;

  // Форматирование в список возможно только если выделен блок
  if(!isBlockSelect()) return;

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
void Editor::onNumericlistClicked(void)
{
  formatToList(QTextListFormat::ListDecimal);
}


// Форматирование в список с точечками
void Editor::onDotlistClicked(void)
{
  formatToList(QTextListFormat::ListDisc);
}


// Добавление отступа
void Editor::onIndentplusClicked(void)
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

  updateIndentsliderToActualFormat();
}


// Убирание отступа
void Editor::onIndentminusClicked(void)
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

  updateIndentsliderToActualFormat();
}


// Форматирование по левому краю
void Editor::onAlignleftClicked(void)
{
  alignText(Qt::AlignLeft);
}


// Форматирование по центру
void Editor::onAligncenterClicked(void)
{
  alignText(Qt::AlignHCenter);
}


// Форматирование по правому краю
void Editor::onAlignrightClicked(void)
{
  alignText(Qt::AlignRight);
}


// Форматирование по ширине
void Editor::onAlignwidthClicked(void)
{
  alignText(Qt::AlignJustify);
}


// Выравнивание текста, вспомогательный метод
void Editor::alignText(Qt::AlignmentFlag mode)
{
  // Создание форматирования
  QTextBlockFormat formatting;
  formatting.setAlignment(mode);

  // Форматирование
  textArea->textCursor().mergeBlockFormat(formatting);

  updateAlignButtonHiglight(true);
}


// Слот, срабатывающий при изменении шрифта в списке шрифтов
void Editor::onFontselectChanged(const QFont &font)
{
  if(flagSetFontParametersEnabled==false) return;

  textArea->setFontFamily(font.family());

  currentFontFamily=font.family();

  // Курсор после выбора возвращается в область редактирования
  textArea->setFocus();
}


// Метод только меняет значение, показываемое списком шрифтов
void Editor::setFontselectOnDisplay(QString fontName)
{
  flagSetFontParametersEnabled=false;

  if(fontName.size()>0)
    editorToolBar->fontSelect->setCurrentIndex(editorToolBar->fontSelect->findText(fontName));
  else
    editorToolBar->fontSelect->setCurrentIndex(editorToolBar->fontSelect->count()-1);

  currentFontFamily=fontName;

  flagSetFontParametersEnabled=true;
}


// Слот, срабатывающий когда изменен размер шрифта через список размеров
void Editor::onFontsizeChanged(int i)
{
  if(flagSetFontParametersEnabled==false) return;

  int n=(editorToolBar->fontSize->itemData(i)).toInt();

  if(n<MINIMUM_ALLOWED_FONT_SIZE) return;
  if(n>MAXIMUM_ALLOWED_FONT_SIZE) return;

  textArea->setFontPointSize(n);

  currentFontSize=n;

  // Курсор после выбора возвращается в область редактирования
  textArea->setFocus();
}


// Метод только меняет значение, показываемое списком размеров шрифта
void Editor::setFontsizeOnDisplay(int n)
{
  flagSetFontParametersEnabled=false;

  editorToolBar->fontSize->setCurrentIndex(editorToolBar->fontSize->findData(n));
  currentFontSize=n;

  flagSetFontParametersEnabled=true;
}


// Слот, срабатыващий при нажатии на кнопку выбора цвета текста
void Editor::onFontcolorClicked()
{
  // Текущий цвет возле курсора
  QColor currentColor=textArea->textColor();

  // Диалог запроса цвета
  QColor selectedColor=QColorDialog::getColor(currentColor, this);

  // Если цвет выбран, и он правильный
  if(selectedColor.isValid())
  {
    // Меняется цвет кнопки
    editorToolBar->fontColor->setPalette(QPalette(selectedColor));

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
void Editor::onSelectionChanged(void)
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
    setFontselectOnDisplay(startFontFamily); // Если всё выделение одним шрифтом
  else
    setFontselectOnDisplay("");

  // Список выбора размера начинает указывать на нужный размер
  if(differentSizeFlag==0)
    setFontsizeOnDisplay((int)startSize); // Если всё отформатировано одним размером
  else
    setFontsizeOnDisplay(0); // В выделении есть разные размеры

  // Кнопка Bold выключается, если есть разное Bold форматирование
  // и включается, если форматирование одинаковое,
  // и выделение начиналось с Bold
  if(differentBoldFlag==1)
    setOutlineButtonHiglight(BT_BOLD,false);
  else
    if(startBold==true)
      setOutlineButtonHiglight(BT_BOLD,true);

  // Кнопка Italic
  if(differentItalicFlag==1)
    setOutlineButtonHiglight(BT_ITALIC,false);
  else
    if(startItalic==true)
      setOutlineButtonHiglight(BT_ITALIC,true);

  // Кнопка Underline
  if(differentUnderlineFlag==1)
    setOutlineButtonHiglight(BT_UNDERLINE,false);
  else
    if(startUnderline==true)
      setOutlineButtonHiglight(BT_UNDERLINE,true);

  // Кнопки выравнивания
  if(differentAlignFlag==1)
    updateAlignButtonHiglight(false);
  else
    updateAlignButtonHiglight(true);
}


void Editor::updateToolLineToActualFormat(void)
{
  // Список должен показывать текущий шрифт позиции, где находится курсор
  if(currentFontFamily!=textArea->fontFamily())
    setFontselectOnDisplay(textArea->fontFamily());

  // Размер
  if(currentFontSize!=(int)textArea->fontPointSize())
    setFontsizeOnDisplay((int)textArea->fontPointSize());

  // Кнопки форматирования начертания
  updateOutlineButtonHiglight();

  // Кнопки выравнивания
  updateAlignButtonHiglight(true);
}


void Editor::updateIndentsliderToActualFormat(void)
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
void Editor::onCursorPositionChanged(void)
{
  // Если одновременно идет режим выделения
  // то обслуживание текущего шрифта и размера идет в on_selection_changed()
  if(textArea->textCursor().hasSelection()) return;

  updateToolLineToActualFormat();

  updateIndentsliderToActualFormat();
}


bool Editor::isKeyForToolLineUpdate(QKeyEvent *event)
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
  if(isKeyForToolLineUpdate(event))
    updateToolLineToActualFormat();
}


// Cлот отлавливает отжатия клавиш
void Editor::keyReleaseEvent(QKeyEvent * event)
{
  if(isKeyForToolLineUpdate(event))
    updateToolLineToActualFormat();
}


void Editor::onUndo(void)
{
  qDebug() << "Undo slot normal running ";
  textArea->undo();
  updateToolLineToActualFormat(); // Обновляется панель с кнопками
}


void Editor::onRedo(void)
{
  qDebug() << "Redo slot normal running ";
  textArea->redo();
  updateToolLineToActualFormat(); // Обновляется панель с кнопками
}


void Editor::onCut(void)
{
  textArea->cut();
  updateToolLineToActualFormat(); // Обновляется панель с кнопками
}


void Editor::onCopy(void)
{
  // Если выбрана только картинка или курсор стоит на позиции картинки
  if(isImageSelect() || isCursorOnImage())
  {
    QTextImageFormat imageFormat;

    if(isImageSelect())
      imageFormat = imageFormatOnSelect();

    if(isCursorOnImage())
      imageFormat = imageFormatOnCursor();

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

  updateToolLineToActualFormat(); // Обновляется панель с кнопками
}


void Editor::onPaste(void)
{
  textArea->paste();
  updateToolLineToActualFormat(); // Обновляется панель с кнопками
}


void Editor::onSelectAll(void)
{
  textArea->selectAll();
  updateToolLineToActualFormat(); // Обновляется панель с кнопками
}


// Обновление подсветки кнопок выравнивания текста
// Если параметр activate=false, все кнопки будут выставлены в неактивные
// Если параметр activate=true, будет подсвечена кнопка, соответсвующая
// текущему форматированию
void Editor::updateAlignButtonHiglight(bool activate)
{
  QPalette palActive, palInactive;
  palActive.setColor(QPalette::Normal, QPalette::Button, buttonsSelectColor);
  palActive.setColor(QPalette::Normal, QPalette::Window, buttonsSelectColor);

  editorToolBar->alignLeft->setPalette(palInactive);
  editorToolBar->alignCenter->setPalette(palInactive);
  editorToolBar->alignRight->setPalette(palInactive);
  editorToolBar->alignWidth->setPalette(palInactive);

  if(activate==false)return;

  if(textArea->alignment()==Qt::AlignLeft)         editorToolBar->alignLeft->setPalette(palActive);
  else if(textArea->alignment()==Qt::AlignHCenter) editorToolBar->alignCenter->setPalette(palActive);
  else if(textArea->alignment()==Qt::AlignRight)   editorToolBar->alignRight->setPalette(palActive);
  else if(textArea->alignment()==Qt::AlignJustify) editorToolBar->alignWidth->setPalette(palActive);
}


// Обновление подсветки клавиш начертания текста
void Editor::updateOutlineButtonHiglight(void)
{
  QPalette palActive, palInactive;
  palActive.setColor(QPalette::Normal, QPalette::Button, buttonsSelectColor);
  palActive.setColor(QPalette::Normal, QPalette::Window, buttonsSelectColor);

  editorToolBar->bold->setPalette(palInactive);
  editorToolBar->italic->setPalette(palInactive);
  editorToolBar->underline->setPalette(palInactive);

  if(textArea->fontWeight()==QFont::Bold) editorToolBar->bold->setPalette(palActive);
  if(textArea->fontItalic()==true)        editorToolBar->italic->setPalette(palActive);
  if(textArea->fontUnderline()==true)     editorToolBar->underline->setPalette(palActive);
}


void Editor::setOutlineButtonHiglight(int button, bool active)
{
  QPalette palActive, palInactive;
  palActive.setColor(QPalette::Normal, QPalette::Button, buttonsSelectColor);
  palActive.setColor(QPalette::Normal, QPalette::Window, buttonsSelectColor);

  if(button==BT_BOLD)
  {
    if(active==false) editorToolBar->bold->setPalette(palInactive);
    else              editorToolBar->bold->setPalette(palActive);
    return;
  }

  if(button==BT_ITALIC)
  {
    if(active==false) editorToolBar->italic->setPalette(palInactive);
    else              editorToolBar->italic->setPalette(palActive);
    return;
  }

  if(button==BT_UNDERLINE)
  {
    if(active==false) editorToolBar->underline->setPalette(palInactive);
    else              editorToolBar->underline->setPalette(palActive);
    return;
  }
}


// Показывание окна с исходным текстом HTML
void Editor::onShowhtmlClicked(void)
{
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


void Editor::onFindtextClicked(void)
{
  findDialog->show();
  findDialog->activateWindow();
}


// Слот, принимающий данные от окна поиска текста
void Editor::onFindtextSignalDetect(const QString &text, QTextDocument::FindFlags flags)
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
void Editor::onCustomContextMenuRequested(const QPoint &pos)
{
  qDebug() << "In Editor on_customContextMenuRequested";

  // Конструирование меню
  // editor_context_menu=textarea->createStandardContextMenu();

  // Включение отображения меню на экране
  // menu.exec(event->globalPos());

  // Если выбрана картинка
  // Или нет выделения, но курсор находится на позиции картинки
  if(isImageSelect() ||
     isCursorOnImage())
    editorContextMenu->set_edit_image_properties( true );
  else
    editorContextMenu->set_edit_image_properties( false );

  // Контекстное меню запускается
  editorContextMenu->exec(textArea->viewport()->mapToGlobal(pos));
}


void Editor::onSettingsClicked(void)
{
  // Создается окно настроек, после выхода из этой функции окно удалится
  EditorConfigDialog dialog;
  dialog.show();
}


// Действия при нажатии кнопки отображения символов редактирования
void Editor::onShowformattingClicked(void)
{
  QPalette palActive, palInactive;
  palActive.setColor(QPalette::Normal, QPalette::Button, buttonsSelectColor);
  palActive.setColor(QPalette::Normal, QPalette::Window, buttonsSelectColor);

  if(textArea->get_showformatting()==false)
  {
    textArea->set_showformatting(true);
    editorToolBar->showFormatting->setPalette(palActive);
  }
  else
  {
    textArea->set_showformatting(false);
    editorToolBar->showFormatting->setPalette(palInactive);
  }
}


// Действия при перемещении движка абзацного отступа
void Editor::onIndentlineChangeTextindentPos(int i)
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
void Editor::onIndentlineChangeLeftindentPos(int i)
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
void Editor::onIndentlineChangeRightindentPos(int i)
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
void Editor::onIndentlineMouseRelease(void)
{
  textArea->show_indetedge(false); // Скрывается вертикальная линия
  textArea->set_indentedge_pos(0); // Координата вертикальной линии обнуляется
}


// Действия при нажатии кнопки создания новой таблицы
void Editor::onCreatetableClicked(void)
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
void Editor::onTableRemoveRowClicked(void)
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
void Editor::onTableRemoveColClicked(void)
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
void Editor::onTableAddRowClicked(void)
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
void Editor::onTableAddColClicked(void)
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
void Editor::onTableMergeCellsClicked(void)
{
  QTextCursor cursor(textArea->textCursor());
  QTextTable *table=cursor.currentTable();
  if(table)
    table->mergeCells(cursor);
}


void Editor::onTableSplitCellClicked(void)
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


void Editor::onContextMenuEditImageProperties()
{
  // Если выделена картинка
  if(isImageSelect() ||
     isCursorOnImage())
  {
    qDebug() << "Image selected";

    editImageProperties();
  }
}


// Метод, определяющий, выбрана ли только одна картинка
bool Editor::isImageSelect(void)
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
QTextImageFormat Editor::imageFormatOnSelect(void)
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
bool Editor::isCursorOnImage(void)
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
QTextImageFormat Editor::imageFormatOnCursor(void)
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
void Editor::onInsertImageFromFileClicked(void)
{

  // Если выделена картинка
  if(isImageSelect() ||
     isCursorOnImage())
  {
    qDebug() << "Image selected";

    editImageProperties();
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
    if(files.size()==0)
      return;

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


void Editor::editImageProperties(void)
{
  // Данные обрабатываемой картинки
  QTextImageFormat imageFormat;

  // Если выбрано изображение
  if(isImageSelect())
    imageFormat=imageFormatOnSelect();

  // Если изображение не выбрано, но курсор находится в позиции изображения
  if(isCursorOnImage())
    imageFormat=imageFormatOnCursor();
  

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
    if(isImageSelect())
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
    if(isCursorOnImage())
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


void Editor::onExpandEditAreaClicked(void)
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


void Editor::onExpandToolsLinesClicked(void)
{
  switchExpandToolsLines();
}


// Метод, переключающий состояние видимости полной панели инструментов
// Если вызывается без параметра (по умолчанию 0), метод сам переключает
// Параметр 1 - включить полную видимость
// Параметр -1 - выключить полную видимость
void Editor::switchExpandToolsLines(int flag)
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
  editorToolBar->toolsLine2->setVisible(setFlag);
  if(viewMode==WYEDIT_DESKTOP_MODE)
    indentSlider->setVisible(setFlag);

  // Запоминается новое состояние
  editorConfig->set_expand_tools_lines(setFlag);

  // На всякий случай обновляется геометрия расположения движков на слайд-панели
  updateIndentlineGeometry();
}


void Editor::onSaveClicked(void)
{
  saveTextarea();
}


void Editor::onBackClicked(void)
{
  // back_callback_func(qobject_cast<QObject *>(this));
  back_callback_func();
}


void Editor::onToAttachClicked(void)
{
  attach_callback_func();
}


void Editor::onFindInBaseClicked(void)
{
  emit wyeditFindInBaseClicked();
}


// Слот нажатия кнопки показа текста в отдельном открепляемом окне
void Editor::onShowTextClicked(void)
{
  if(textArea->toPlainText().length()==0)
  {
    QMessageBox msgBox;
    msgBox.setText("Can't show empty text in detached window");
    msgBox.exec();

    return;
  }

  EditorShowText *showText=new EditorShowText(this);

  // Устанавливается флаг удаления диалога после закрытия его окна
  showText->setAttribute( Qt::WA_DeleteOnClose );

  if(getMiscField("title").length()>0)
    showText->setWindowTitle( getMiscField("title") );

  QTextDocument *cloneDocument=textArea->document()->clone(showText); // Для метода clone указан parent, который уничтожится при закрытии окна, и за собой уничтожит этот временный документ
  showText->setDocument( cloneDocument );

  showText->show();
}


void Editor::setSaveCallback(void (*func)(QObject *editor, QString saveString))
{
  save_callback_func=func;
}


void Editor::setLoadCallback(void (*func)(QObject *editor, QString &String))
{
  load_callback_func=func;
}


void Editor::setBackCallback(void (*func)(void))
{
  back_callback_func=func;
}


void Editor::setAttachCallback(void (*func)(void))
{
  attach_callback_func=func;
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


void Editor::clearAllMiscField(void)
{
  miscFields.clear();
}


void Editor::setDirFileEmptyReaction(int mode)
{
  // Проверяется допустимость переданного значения
  if(mode==DIRFILEEMPTY_REACTION_SHOW_ERROR ||
     mode==DIRFILEEMPTY_REACTION_SUPPRESS_ERROR)
    dirFileEmptyReaction=mode;
  else
    criticalError("Editor::setDirFileEmptyReaction() : Unsupport mode "+QString::number(mode));
}


int Editor::getDirFileEmptyReaction(void)
{
  return dirFileEmptyReaction;
}


// Метод позволяющий управлять доступностью инcтрументов редактирования
void Editor::setDisableToolList(QStringList toolNames)
{
  editorToolBar->setDisableToolList(toolNames);
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


void Editor::switchAttachIconExists(bool isExists)
{
  editorToolBar->switchAttachIconExists(isExists);
}

