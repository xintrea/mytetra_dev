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
#include <QColor>

#include "Editor.h"
#include "EditorConfig.h"
#include "EditorConfigDialog.h"
#include "EditorContextMenu.h"
#include "EditorTextArea.h"
#include "indentslider/IndentSlider.h"
#include "EditorIndentSliderAssistant.h"
#include "EditorToolBarAssistant.h"
#include "formatters/Formatter.h"
#include "EditorMultiLineInputDialog.h"
#include "EditorCursorPositionDetector.h"

#include "../../main.h"
#include "../../views/mainWindow/MainWindow.h"
#include "../TraceLogger.h"
#include "libraries/DiskHelper.h"


// Максимально возможная длина выделения текста (в символах) при которой
// происходит проверка, есть ли в выделенном тексте различные шрифты, размеры текста, BUI форматирование
// с соответствующем изменением панели кнопок и другоих элементов форматированиятекста
// Если выделено текста больше, такая проверка не происходит
#define WYEDIT_MAX_SELECTION_SIZE_WHILE_CHECK_TOOLBAR_ITEM 4096


Editor::Editor(QWidget *parent) : QWidget(parent)
{
  isInit=false;

  initDataEnableAssembly=true;
  initDataConfigFileName="";
  initDataEnableRandomSeed=false;
  initDataDisableToolList.clear();

  dirFileEmptyReaction=DIRFILEEMPTY_REACTION_SHOW_ERROR;
}


Editor::~Editor(void)
{
  delete editorConfig;
  // delete editorToolBarAssistant; // Закомментировано, т. к. объект удалится автоматически
  delete buttonsAndEditLayout;
  delete editorContextMenu;
  delete textArea;
  delete cursorPositionDetector;

  delete typefaceFormatter;
  delete placementFormatter;
  delete listFormatter;
  delete tableFormatter;
  delete imageFormatter;
}


const char *Editor::getVersion(void)
{
  static const char *lib_version=WYEDIT_VERSION;
  return(lib_version);
}


// Настроечные методы, вызываемые перед init()

void Editor::initEnableAssembly(bool flag)
{
  if(isInit)
    criticalError("Method "+QString(__FUNCTION__)+" running before init() only.");

  initDataEnableAssembly=flag;
}


void Editor::initConfigFileName(QString name)
{
  if(isInit)
    criticalError("Method "+QString(__FUNCTION__)+" running before init() only.");

  initDataConfigFileName=name;
}


void Editor::initEnableRandomSeed(bool flag)
{
  if(isInit)
    criticalError("Method "+QString(__FUNCTION__)+" running before init() only.");

  initDataEnableRandomSeed=flag;
}


void Editor::initDisableToolList(QStringList toolNames)
{
  if(isInit)
    criticalError("Method "+QString(__FUNCTION__)+" running before init() only.");

  initDataDisableToolList=toolNames;
}


// Инициализация редактора
// Если mode=WYEDIT_DESKTOP_MODE - происходит обычная инициализация
// Если mode=WYEDIT_MOBILE_MODE - при инициализации в первую строку панели инструментов, слева, добавляется кнопка back
void Editor::init(int mode)
{
  // Информационный флаг, что был запущен метод init()
  isInit=true;

  viewMode=mode; // todo: Избавиться от этого свойства?

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

  setupEditorTextArea();
  setupCursorPositionDetector();
  setupIndentSliderAssistant(); // Инициализируется обязательно только после TextArea
  setupEditorToolBarAssistant(mode, textArea, initDataDisableToolList);
  setupFormatters();

  setupSignals();

  if(initDataEnableAssembly)
    assembly();

  // Вначале редактор находится в обычном нераспахнутом состоянии
  expandEditAreaFlag=false;

  emit updateIndentSliderGeometry();

  if(initDataEnableRandomSeed)
  {
    QDateTime datetime=QDateTime::currentDateTime ();
    unsigned int seed=rand()+datetime.toTime_t();
    // qDebug() << "Random generator init " << seed;
    srand(seed);
  }
}


// Создание и настройка панели инструментов редактора
void Editor::setupEditorToolBarAssistant(int mode, EditorTextArea *textArea, QStringList disableToolList)
{
  // Установлен this в качестве родителя, поэтому вручную удалять объект в деструкторе ненужно
  editorToolBarAssistant=new EditorToolBarAssistant(this, mode, textArea, disableToolList);
  editorToolBarAssistant->setObjectName("toolBarAssistant");
}


// Создание и настройка ассистента линейки отступов
void Editor::setupIndentSliderAssistant(void)
{
  indentSliderAssistant=new EditorIndentSliderAssistant(qobject_cast<QObject *>(this), textArea);
  indentSliderAssistant->setObjectName("indentSliderAssistant");
}


// Создание и настройка области редактирования
void Editor::setupEditorTextArea(void)
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


void Editor::setupCursorPositionDetector(void)
{
  cursorPositionDetector=new EditorCursorPositionDetector();
  cursorPositionDetector->setTextArea(textArea);
}


void Editor::setupFormatters(void)
{
  // Форматирование начертания текста
  typefaceFormatter=new TypefaceFormatter();
  typefaceFormatter->setEditor(this);
  typefaceFormatter->setTextArea(textArea);

  // Форматирование размещения текста
  placementFormatter=new PlacementFormatter();
  placementFormatter->setEditor(this);
  placementFormatter->setTextArea(textArea);

  // Форматирование текста как списка
  listFormatter=new ListFormatter();
  listFormatter->setEditor(this);
  listFormatter->setTextArea(textArea);

  // Форматирование таблиц в тексте
  tableFormatter=new TableFormatter();
  tableFormatter->setEditor(this);
  tableFormatter->setTextArea(textArea);

  // Форматирование картинок в тексте
  imageFormatter=new ImageFormatter();
  imageFormatter->setEditor(this);
  imageFormatter->setTextArea(textArea);

  // Форматирование ссылки
  referenceFormatter=new ReferenceFormatter();
  referenceFormatter->setEditor(this);
  referenceFormatter->setTextArea(textArea);
}


void Editor::setupSignals(void)
{
  // Создание сигналов, генерируемых кнопками форматирования текста
  connect( &(editorToolBarAssistant->bold), SIGNAL(clicked()),
           typefaceFormatter,               SLOT  (onBoldClicked()),
           Qt::DirectConnection);
  connect( &(editorToolBarAssistant->italic), SIGNAL(clicked()),
           typefaceFormatter,                 SLOT  (onItalicClicked()),
          Qt::DirectConnection);
  connect( &(editorToolBarAssistant->underline), SIGNAL(clicked()),
           typefaceFormatter,                    SLOT  (onUnderlineClicked()),
          Qt::DirectConnection);
  connect( &(editorToolBarAssistant->monospace), SIGNAL(clicked()),
           typefaceFormatter,                    SLOT  (onMonospaceClicked()),
          Qt::DirectConnection);
  connect( &(editorToolBarAssistant)->code, SIGNAL(clicked()),
           typefaceFormatter,               SLOT  (onCodeClicked()),
          Qt::DirectConnection);
  connect( &(editorToolBarAssistant->clear), SIGNAL(clicked()),
           typefaceFormatter,                SLOT  (onClearClicked()),
          Qt::DirectConnection);
  connect( &(editorToolBarAssistant->textOnly), SIGNAL(clicked()),
           typefaceFormatter,                SLOT  (onTextOnlyClicked()),
          Qt::DirectConnection);
  connect( &(editorToolBarAssistant->fixBreakSymbol), SIGNAL(clicked()),
           typefaceFormatter,                SLOT  (onFixBreakSymbolClicked()),
          Qt::DirectConnection);

  connect( &(editorToolBarAssistant->fontSelect), SIGNAL(currentFontChanged(const QFont &)),
           typefaceFormatter,                     SLOT  (onFontselectChanged(const QFont &)),
          Qt::DirectConnection);
  connect( &(editorToolBarAssistant->fontSize), SIGNAL(currentIndexChanged(int)),
           typefaceFormatter,                   SLOT  (onFontsizeChanged(int)),
          Qt::DirectConnection);
  connect( &(editorToolBarAssistant->fontColor), SIGNAL(clicked()),
           typefaceFormatter,                    SLOT  (onFontcolorClicked()),
          Qt::DirectConnection);


  // Обратка для typefaceFormatter todo: подумать, а надо ли
  connect(typefaceFormatter,      SIGNAL(updateOutlineButtonHiglight()),
          editorToolBarAssistant, SLOT  (onUpdateOutlineButtonHiglight()),
          Qt::DirectConnection);
  connect(typefaceFormatter,      SIGNAL(changeFontselectOnDisplay(QString)),
          editorToolBarAssistant, SLOT  (onChangeFontselectOnDisplay(QString)),
          Qt::DirectConnection);
  connect(typefaceFormatter,      SIGNAL(changeFontsizeOnDisplay(int)),
          editorToolBarAssistant, SLOT  (onChangeFontsizeOnDisplay(int)),
          Qt::DirectConnection);
  connect(typefaceFormatter, SIGNAL(changeFontFamily(QString)),
          textArea,          SLOT  (onChangeFontFamily(QString)),
          Qt::DirectConnection);
  connect(typefaceFormatter,      SIGNAL(changeFontFamily(QString)),
          editorToolBarAssistant, SLOT  (onChangeFontFamily(QString)),
          Qt::DirectConnection);
  connect(typefaceFormatter,      SIGNAL(changeFontPointSize(int)),
          textArea,               SLOT  (onChangeFontPointSize(int)),
          Qt::DirectConnection);
  connect(typefaceFormatter,      SIGNAL(changeFontPointSize(int)),
          editorToolBarAssistant, SLOT  (onChangeFontPointSize(int)),
          Qt::DirectConnection);
  connect(typefaceFormatter, SIGNAL(changeFontcolor(QColor)),
          textArea,          SLOT  (onChangeFontcolor(QColor)),
          Qt::DirectConnection);
  connect(typefaceFormatter,      SIGNAL(changeFontcolor(QColor)),
          editorToolBarAssistant, SLOT  (onChangeFontcolor(QColor)),
          Qt::DirectConnection);


  connect(this,                   SIGNAL(changeFontselectOnDisplay(QString)),
          editorToolBarAssistant, SLOT  (onChangeFontselectOnDisplay(QString)),
          Qt::DirectConnection);
  connect(this,                   SIGNAL(changeFontsizeOnDisplay(int)),
          editorToolBarAssistant, SLOT  (onChangeFontsizeOnDisplay(int)),
          Qt::DirectConnection);


  connect( &(editorToolBarAssistant->indentPlus), SIGNAL(clicked()),
           placementFormatter,                    SLOT  (onIndentplusClicked()),
          Qt::DirectConnection);
  connect( &(editorToolBarAssistant->indentMinus), SIGNAL(clicked()),
           placementFormatter,                     SLOT  (onIndentminusClicked()),
          Qt::DirectConnection);


  connect(placementFormatter,    SIGNAL(updateIndentsliderToActualFormat()),
          indentSliderAssistant, SLOT  (updateToActualFormat()),
          Qt::DirectConnection);
  connect(this,                  SIGNAL(updateIndentsliderToActualFormat()),
          indentSliderAssistant, SLOT  (updateToActualFormat()),
          Qt::DirectConnection);

  connect( &(editorToolBarAssistant->alignLeft), SIGNAL(clicked()),
           placementFormatter,                   SLOT  (onAlignleftClicked()),
          Qt::DirectConnection);
  connect( &(editorToolBarAssistant->alignCenter), SIGNAL(clicked()),
           placementFormatter,                     SLOT  (onAligncenterClicked()),
          Qt::DirectConnection);
  connect( &(editorToolBarAssistant->alignRight), SIGNAL(clicked()),
           placementFormatter,                    SLOT  (onAlignrightClicked()),
          Qt::DirectConnection);
  connect( &(editorToolBarAssistant->alignWidth), SIGNAL(clicked()),
           placementFormatter,                    SLOT  (onAlignwidthClicked()),
          Qt::DirectConnection);


  connect( &(editorToolBarAssistant->numericList), SIGNAL(clicked()),
           listFormatter,                          SLOT  (onNumericlistClicked()),
          Qt::DirectConnection);
  connect( &(editorToolBarAssistant->dotList), SIGNAL(clicked()),
           listFormatter,                      SLOT  (onDotlistClicked()),
          Qt::DirectConnection);


  // Кнопки работы с таблицами
  connect( &(editorToolBarAssistant->createTable), SIGNAL(clicked()),
           tableFormatter,                         SLOT  (onCreatetableClicked()),
          Qt::DirectConnection);
  connect( &(editorToolBarAssistant->tableRemoveRow), SIGNAL(clicked()),
           tableFormatter,                            SLOT  (onTableRemoveRowClicked()),
          Qt::DirectConnection);
  connect( &(editorToolBarAssistant->tableRemoveCol), SIGNAL(clicked()),
           tableFormatter,                            SLOT  (onTableRemoveColClicked()),
          Qt::DirectConnection);
  connect( &(editorToolBarAssistant->tableAddRow), SIGNAL(clicked()),
           tableFormatter,                         SLOT  (onTableAddRowClicked()),
          Qt::DirectConnection);
  connect( &(editorToolBarAssistant->tableAddCol), SIGNAL(clicked()),
           tableFormatter,                         SLOT  (onTableAddColClicked()),
          Qt::DirectConnection);
  connect( &(editorToolBarAssistant->tableMergeCells), SIGNAL(clicked()),
           tableFormatter,                             SLOT  (onTableMergeCellsClicked()),
          Qt::DirectConnection);
  connect( &(editorToolBarAssistant->tableSplitCell), SIGNAL(clicked()),
           tableFormatter,                            SLOT  (onTableSplitCellClicked()),
          Qt::DirectConnection);
  connect( &(editorToolBarAssistant->tableProperties), SIGNAL(clicked()),
           tableFormatter,                             SLOT  (onTablePropertiesClicked()),
          Qt::DirectConnection);

  connect( &(editorToolBarAssistant->reference), SIGNAL(clicked()),
           referenceFormatter,                   SLOT  (onReferenceClicked()),
          Qt::DirectConnection);
  connect( &(editorToolBarAssistant->showHtml), SIGNAL(clicked()),
           this,                                SLOT  (onShowhtmlClicked()),
          Qt::DirectConnection);
  connect( &(editorToolBarAssistant->findText), SIGNAL(clicked()),
           this,                               SLOT  (onFindtextClicked()),
          Qt::DirectConnection);
  connect( &(editorToolBarAssistant->settings), SIGNAL(clicked()),
           this,                                SLOT  (onSettingsClicked()),
          Qt::DirectConnection);
  connect( &(editorToolBarAssistant->showFormatting), SIGNAL(toggled(bool)),
           this,                                      SLOT  (onShowformattingClicked(bool)),
          Qt::DirectConnection);


  connect(this,                   SIGNAL(updateAlignButtonHiglight(bool)),
          editorToolBarAssistant, SLOT  (onUpdateAlignButtonHiglight(bool)),
          Qt::DirectConnection);
  connect(placementFormatter,     SIGNAL(updateAlignButtonHiglight(bool)),
          editorToolBarAssistant, SLOT  (onUpdateAlignButtonHiglight(bool)),
          Qt::DirectConnection);


  // Прочие кнопки
  connect( &(editorToolBarAssistant->insertImageFromFile), SIGNAL(clicked()),
           imageFormatter,                                 SLOT  (onInsertImageFromFileClicked()),
          Qt::DirectConnection);
  connect( &(editorToolBarAssistant->expandEditArea), SIGNAL(clicked()),
           this,                                      SLOT  (onExpandEditAreaClicked()),
          Qt::DirectConnection);
  connect( &(editorToolBarAssistant->save), SIGNAL(clicked()),
           this,                            SLOT  (onSaveClicked()),
          Qt::DirectConnection);
  connect( &(editorToolBarAssistant->back), SIGNAL(clicked()),
           this,                            SLOT  (onBackClicked()),
          Qt::DirectConnection);
  connect( &(editorToolBarAssistant->findInBase), SIGNAL(clicked()),
           this,                                  SLOT  (onFindInBaseClicked()),
          Qt::DirectConnection);
  connect( &(editorToolBarAssistant->showText), SIGNAL(clicked()),
           this,                                SLOT  (onShowTextClicked()),
          Qt::DirectConnection);
  connect( &(editorToolBarAssistant->toAttach), SIGNAL(clicked()),
           this,                                SLOT  (onToAttachClicked()),
          Qt::DirectConnection);

  // Область редактирования текста
  connect(textArea, SIGNAL(cursorPositionChanged()),
          this,     SLOT  (onCursorPositionChanged()),
          Qt::DirectConnection);
  connect(textArea, SIGNAL(selectionChanged()),
          this,     SLOT  (onSelectionChanged()),
          Qt::DirectConnection);

  // Соединение сигнал-слот чтобы показать контекстное меню по правому клику в редакторе
  connect(textArea, SIGNAL(customContextMenuRequested(const QPoint &)),
          this,     SLOT  (onCustomContextMenuRequested(const QPoint &)),
          Qt::DirectConnection);

  // Соединение сигнал-слот чтобы показать контекстное меню по долгому нажатию
  connect(textArea, SIGNAL(tapAndHoldGestureFinished(const QPoint &)),
          this,     SLOT  (onCustomContextMenuRequested(const QPoint &)),
          Qt::DirectConnection);

  connect(textArea,           SIGNAL(textChanged()),
          referenceFormatter, SLOT  (onTextChanged()),
          Qt::DirectConnection);

  // Вызов загрузки картинок
  connect(textArea,       SIGNAL(downloadImages(const QString)),
          imageFormatter, SLOT  (onDownloadImages(const QString)),
          Qt::DirectConnection);

  // Вставка загруженных картинок в текст
  connect(imageFormatter, SIGNAL(downloadImagesSuccessfull(const QString, const QMap<QString, QByteArray>, const QMap<QString, QString>)),
          textArea,       SLOT  (onDownloadImagesSuccessfull(const QString, const QMap<QString, QByteArray>, const QMap<QString, QString>)),
          Qt::DirectConnection);

  // Клик по ссылке при зажатой клавише Ctrl
  connect(textArea,           SIGNAL(clickedOnReference(QString)),
          referenceFormatter, SLOT  (onClickedGotoReference(QString)),
          Qt::DirectConnection);

  // Глобальное нажатие клавиш
  connect(find_object<MainWindow>("mainwindow"), SIGNAL(globalPressKey(int)),
          textArea,                              SLOT  (onGlobalPressKey(int)),
          Qt::DirectConnection);

  // Глобальное отжатие клавиш
  connect(find_object<MainWindow>("mainwindow"), SIGNAL(globalReleaseKey(int)),
          textArea,                              SLOT  (onGlobalReleaseKey(int)),
          Qt::DirectConnection);

  // connect(textArea->document(), SIGNAL(contentsChange(int, int, int)),
  //        referenceFormatter,   SLOT  (onContentsChange(int, int, int)),
  //        Qt::DirectConnection);

  // connect(textArea->document(), SIGNAL(modificationChanged (bool )),
  //         this, SLOT  (onModificationChanged(bool)));


  // Сигналы контекстного меню
  connect(editorContextMenu, SIGNAL(undo()),
          this,              SLOT  (onUndo()),
          Qt::DirectConnection);
  connect(editorContextMenu, SIGNAL(redo()),
          this,              SLOT  (onRedo()),
          Qt::DirectConnection);
  connect(editorContextMenu, SIGNAL(cut()),
          this,              SLOT  (onCut()),
          Qt::DirectConnection);
  connect(editorContextMenu, SIGNAL(copy()),
          this,              SLOT  (onCopy()),
          Qt::DirectConnection);
  connect(editorContextMenu, SIGNAL(paste()),
          this,              SLOT  (onPaste()),
          Qt::DirectConnection);
  connect(editorContextMenu, SIGNAL(selectAll()),
          this,              SLOT  (onSelectAll()),
          Qt::DirectConnection);
  connect(editorContextMenu, SIGNAL(contextMenuEditImageProperties()),
          imageFormatter,    SLOT  (onContextMenuEditImageProperties()),
          Qt::DirectConnection);
  connect(editorContextMenu, SIGNAL(contextMenuGotoReference()),
          referenceFormatter,    SLOT(onContextMenuGotoReference()),
          Qt::DirectConnection);

  // Вызов диалога поиска в тексте
  connect(findDialog, SIGNAL(find_text(const QString &, QTextDocument::FindFlags)),
          this,       SLOT  (onFindtextSignalDetect(const QString &, QTextDocument::FindFlags)),
          Qt::DirectConnection);

  connect(textArea,              SIGNAL(updateIndentlineGeometry()),
          indentSliderAssistant, SLOT  (onUpdateGeometry()),
          Qt::DirectConnection);

  connect(this,                  SIGNAL(updateIndentSliderGeometry()),
          indentSliderAssistant, SLOT  (onUpdateGeometry()),
          Qt::DirectConnection);
}


// Сборка редактора и его активизация
void Editor::assembly(void)
{
  // Создается вертикальная область, которая должна объединить
  // линейку кнопок редактирования и область редактирования текста
  buttonsAndEditLayout=new QVBoxLayout(this);
  buttonsAndEditLayout->setObjectName("buttons_and_edit_layout");

  // Добавляется виджет с кнопками редактора
  buttonsAndEditLayout->addWidget( editorToolBarAssistant );

  // Добавляется виджет линейки отступов
  if(viewMode==WYEDIT_DESKTOP_MODE) // Виджет линейки отступов виден только в desktop интерфейсе
  {
    indentSliderAssistant->setVisible(true);
    buttonsAndEditLayout->addWidget( indentSliderAssistant->getIndentSlider() );
  }
  else
    indentSliderAssistant->setVisible(false);

  // Добавляется область редактирования
  buttonsAndEditLayout->addWidget(textArea);

  // Полученый набор элементов устанавливается для текущего окна
  setLayout(buttonsAndEditLayout);

  // Границы убираются, так как данный объект будет использоваться как виджет
  QLayout *lt;
  lt=layout();
  lt->setContentsMargins(0,2,0,0);
}


// Установка текста области редактирования
void Editor::setTextarea(QString text)
{
  textArea->setHtml(text);

  // Очищается URL документа, так как документ создается "из ничего"
  textArea->document()->setMetaInformation( QTextDocument::DocumentUrl, "" );
}


// Установка запрета или разрешения редактирования в области редактирования текста
void Editor::setTextareaEditable(bool editable)
{
  textArea->setReadOnly( !editable );

  /*
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
  }
  */
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
      if( fileName.contains(QRegExp("\\.png$")) ) // Обрабатыватся только *.png файлы
        if( !imagesNames.contains(fileName) ) // Только картинки, не встречающиеся в тексте записи
          if( !miscFields["attachFileNameList"].contains(fileName) ) // Только имена файлов, не содержащиеся в прикрепленных файлах
        {
          // Этот файл лишний и он удаляется
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
    if(loadCallbackFunc==NULL)
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
      saveCallbackFunc(qobject_cast<QObject *>(this), content);
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
  if(loadCallbackFunc==NULL)
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
    loadCallbackFunc(qobject_cast<QObject *>(this), resultString);
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


// Слот вызывается при каждом движении курсора в момент выделения текста
void Editor::onSelectionChanged(void)
{
  // TRACELOG

  // Если выделения нет
  if(!textArea->textCursor().hasSelection())
    return;

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

  bool differentFontFlag=false;
  bool differentSizeFlag=false;
  bool differentBoldFlag=false;
  bool differentItalicFlag=false;
  bool differentUnderlineFlag=false;
  bool differentAlignFlag=false;

  // Слишком большие выделения текста нельзя обрабатывать, так как выделение становится слишком медленным
  if(abs(stop-start)>WYEDIT_MAX_SELECTION_SIZE_WHILE_CHECK_TOOLBAR_ITEM)
  {
    differentFontFlag=true;
    differentSizeFlag=true;
    differentBoldFlag=true;
    differentItalicFlag=true;
    differentUnderlineFlag=true;
    differentAlignFlag=true;
  }
  else
  {
    // Программа пробегает по выделенному куску текста дополнительным курсором
    // и выясняет, есть ли разные шрифты, разные размеры,
    // разные начертания символов, разное выравнивание в выделенном тексте
    while(cursor.position()<=stop)
    {
      if(differentFontFlag==false && startFontFamily!=cursor.charFormat().fontFamily())
        differentFontFlag=true;

      if(differentSizeFlag==false && startSize!=cursor.charFormat().fontPointSize())
        differentSizeFlag=true;

      if(differentBoldFlag==false)
      {
        int b=cursor.charFormat().fontWeight();
        if(startBold==false && b==QFont::Bold)
          differentBoldFlag=true;
        else
          if(startBold==true && b==QFont::Normal)
            differentBoldFlag=true;
      }

      if(differentItalicFlag==false && startItalic!=cursor.charFormat().fontItalic())
        differentItalicFlag=true;

      if(differentUnderlineFlag==false && startUnderline!=cursor.charFormat().fontUnderline())
        differentUnderlineFlag=true;

      if(differentAlignFlag==false && startAlign!=cursor.blockFormat().alignment())
        differentAlignFlag=true;

      // Курсор передвигается на одну позицию вперед
      // Если дальше двигаться некуда (конец документа) цикл досрочно завершается
      if(cursor.movePosition(QTextCursor::NextCharacter)==false)
        break;
    }
  }

  // Список выбора шрифта начинает указывать на нужный шрифт
  if(differentFontFlag==false)
    emit changeFontselectOnDisplay(startFontFamily); // Если всё выделение одним шрифтом
  else
    emit changeFontselectOnDisplay("");

  // Список выбора размера начинает указывать на нужный размер
  if(differentSizeFlag==false)
    emit changeFontsizeOnDisplay((int)startSize); // Если всё отформатировано одним размером
  else
    emit changeFontsizeOnDisplay(0); // В выделении есть разные размеры

  // Кнопка Bold выключается, если есть разное Bold форматирование
  // и включается, если форматирование одинаковое,
  // и выделение начиналось с Bold
  if(differentBoldFlag==true)
    editorToolBarAssistant->setOutlineButtonHiglight(EditorToolBarAssistant::BT_BOLD,false);
  else
    if(startBold==true)
      editorToolBarAssistant->setOutlineButtonHiglight(EditorToolBarAssistant::BT_BOLD,true);

  // Кнопка Italic
  if(differentItalicFlag==true)
    editorToolBarAssistant->setOutlineButtonHiglight(EditorToolBarAssistant::BT_ITALIC,false);
  else
    if(startItalic==true)
      editorToolBarAssistant->setOutlineButtonHiglight(EditorToolBarAssistant::BT_ITALIC,true);

  // Кнопка Underline
  if(differentUnderlineFlag==true)
    editorToolBarAssistant->setOutlineButtonHiglight(EditorToolBarAssistant::BT_UNDERLINE,false);
  else
    if(startUnderline==true)
      editorToolBarAssistant->setOutlineButtonHiglight(EditorToolBarAssistant::BT_UNDERLINE,true);

  // Кнопки выравнивания
  if(differentAlignFlag==true)
    emit updateAlignButtonHiglight(false);
  else
    emit updateAlignButtonHiglight(true);
}


// Слот вызывается при каждом перемещении курсора
void Editor::onCursorPositionChanged(void)
{
  // TRACELOG

  // Если одновременно идет режим выделения
  // то обслуживание текущего шрифта и размера идет в on_selection_changed()
  if(textArea->textCursor().hasSelection())
    return;

  editorToolBarAssistant->updateToActualFormat();

  emit updateIndentsliderToActualFormat();
}


// Cлот отлавливает нажатия клавиш
void Editor::keyPressEvent(QKeyEvent *event)
{
  if(editorToolBarAssistant->isKeyForToolLineUpdate(event))
    editorToolBarAssistant->updateToActualFormat();
}


// Cлот отлавливает отжатия клавиш
void Editor::keyReleaseEvent(QKeyEvent *event)
{
  if(editorToolBarAssistant->isKeyForToolLineUpdate(event))
    editorToolBarAssistant->updateToActualFormat();
}


void Editor::onUndo(void)
{
  qDebug() << "Undo slot normal running ";
  textArea->undo();
  editorToolBarAssistant->updateToActualFormat(); // Обновляется панель с кнопками
}


void Editor::onRedo(void)
{
  qDebug() << "Redo slot normal running ";
  textArea->redo();
  editorToolBarAssistant->updateToActualFormat(); // Обновляется панель с кнопками
}


void Editor::onCut(void)
{
  textArea->cut();
  editorToolBarAssistant->updateToActualFormat(); // Обновляется панель с кнопками
}


void Editor::onCopy(void)
{
  // Если выбрана только картинка или курсор стоит на позиции картинки
  if(cursorPositionDetector->isImageSelect() || cursorPositionDetector->isCursorOnImage())
  {
    QTextImageFormat imageFormat;

    if(cursorPositionDetector->isImageSelect())
      imageFormat = imageFormatter->imageFormatOnSelect();

    if(cursorPositionDetector->isCursorOnImage())
      imageFormat = imageFormatter->imageFormatOnCursor();

    // Из формата выясняется имя картинки
    QString imageName=imageFormat.name();

    // Из ресурсов вытягивается картинка
    QVariant imageData=textArea->document()->resource(QTextDocument::ImageResource, QUrl(imageName));
    QImage image=imageData.value<QImage>();

    // Создается ссылка на буфер обмена
    QClipboard *clipboard=QApplication::clipboard();

    // Копирование картинки в буфер обмена
    clipboard->setImage(image);
  }
  else
    textArea->copy(); // Обычное копирование

  editorToolBarAssistant->updateToActualFormat(); // Обновляется панель с кнопками
}


void Editor::onPaste(void)
{
  // В Qt обнаружен баг, заключающийся в том, что при установке ReadOnly на область редактирование,
  // все равно можно сделать вставку текста (и замену выделенного текста на вставляемый) через контекстное меню.
  // Здесь происходит блокировка этого действия
  if(textArea->isReadOnly())
    return;

  textArea->paste();
  editorToolBarAssistant->updateToActualFormat(); // Обновляется панель с кнопками
}


void Editor::onSelectAll(void)
{
  textArea->selectAll();
  editorToolBarAssistant->updateToActualFormat(); // Обновляется панель с кнопками
}


// Показывание окна с исходным текстом HTML
void Editor::onShowhtmlClicked(void)
{
  EditorMultiLineInputDialog dialog(this);

  dialog.setText(textArea->toHtml());
  dialog.setWindowTitle(tr("Edit HTML source"));

  if(dialog.exec()!=QDialog::Accepted)
    return;

  textArea->setHtml(dialog.getText());

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
  if(cursorPositionDetector->isImageSelect() ||
     cursorPositionDetector->isCursorOnImage())
     editorContextMenu->setImageProperties( true );
  else
    editorContextMenu->setImageProperties( false );

  // Если курсор находится на ссылке (URL)
  if(cursorPositionDetector->isCursorOnReference())
    editorContextMenu->setGotoReference( true );
  else
    editorContextMenu->setGotoReference( false );


  // Контекстное меню запускается
  editorContextMenu->exec(textArea->viewport()->mapToGlobal(pos));
}


void Editor::onSettingsClicked(void)
{
  // Создается окно настроек, после выхода из этой функции окно удалится
  EditorConfigDialog dialog;
  dialog.show();
}


void Editor::onShowformattingClicked(bool ok)
{
    textArea->set_showformatting(ok);
}


void Editor::onExpandEditAreaClicked(void)
{
  if(expandEditAreaFlag==false)
  {
    expandEditAreaFlag=true;
    emit send_expand_edit_area(expandEditAreaFlag);
  }
  else
  {
    expandEditAreaFlag=false;
    emit send_expand_edit_area(expandEditAreaFlag);
  }
}


void Editor::onSaveClicked(void)
{
  saveTextarea();
}


void Editor::onBackClicked(void)
{
  // back_callback_func(qobject_cast<QObject *>(this));
  backCallbackFunc();
}


void Editor::onToAttachClicked(void)
{
  attachCallbackFunc();
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

  // Создается открепленное окно
  // Нелья в качестве parent указывать this, так если редактор станет неактивным (например, когда запись не выбрана)
  // то данное окно тоже станет неактивным, и невозможно будет выделить в нем текст
  EditorShowText *showText=new EditorShowText(find_object<MainWindow>("mainwindow"));

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
  saveCallbackFunc=func;
}


void Editor::setLoadCallback(void (*func)(QObject *editor, QString &String))
{
  loadCallbackFunc=func;
}


void Editor::setBackCallback(void (*func)(void))
{
  backCallbackFunc=func;
}


void Editor::setAttachCallback(void (*func)(void))
{
  attachCallbackFunc=func;
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
  editorToolBarAssistant->switchAttachIconExists(isExists);
}


