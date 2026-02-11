#include <QWidget>
#include <QLabel>
#include <QBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QSplitter>
#include <QtDebug>

#include "MetaEditor.h"

#include "libraries/wyedit/Editor.h"
#include "libraries/wyedit/EditorTextArea.h"
#include "libraries/wyedit/EditorIndentSliderAssistant.h"
#include "libraries/wyedit/EditorToolBarAssistant.h"
#include "libraries/wyedit/indentslider/IndentSlider.h"
#include "libraries/GlobalParameters.h"
#include "views/findInBaseScreen/FindScreen.h"
#include "models/appConfig/AppConfig.h"
#include "views/attachTable/AttachTableScreen.h"
#include "libraries/helpers/ObjectHelper.h"
#include "libraries/helpers/LinkHelper.h"


extern GlobalParameters globalParameters;
extern AppConfig mytetraConfig;


MetaEditor::MetaEditor(QWidget *parent) : Editor(parent)
{
    // Конфигурирование редактора перед инициализацией
    Editor::initEnableAssembly(false); // Стандартная сборка интерфейса редактора отключается
    Editor::initConfigFileName(globalParameters.getWorkDirectory()+"/editorconf.ini");
    Editor::initEnableRandomSeed(false);
    Editor::initDisableToolList( mytetraConfig.getHideEditorTools() );

    // Инициализация редатора
    if(mytetraConfig.getInterfaceMode()=="desktop")
        Editor::init(Editor::WYEDIT_DESKTOP_MODE);
    else if(mytetraConfig.getInterfaceMode()=="mobile")
        Editor::init(Editor::WYEDIT_MOBILE_MODE);
    else
        criticalError("In MetaEditor constructor unknown interface mode: "+mytetraConfig.getInterfaceMode());

    setupLabels();
    setupUI();
    metaAssembly();

    setupSignals();

    // В редакторе устанавливается функция обратного вызова на кнопку Attach
    setAttachCallback( toAttachCallback );

    emit updateIndentSliderGeometry();
}


MetaEditor::~MetaEditor(void)
{

}


void MetaEditor::setupSignals(void)
{
  connect(this,                             &MetaEditor::setFindTextSignal,
          globalParameters.getFindScreen(), &FindScreen::setFindText);

  connect(recordUrl, &QLabel::linkActivated,
          this,      &MetaEditor::onClickToUrl);
}


void MetaEditor::setupLabels(void)
{
  // Путь в дереве до данной записи в виде названий веток (только для мобильного интерфейса)
  treePath=new QLabel(this);
  treePath->setTextInteractionFlags(Qt::TextSelectableByMouse |
                                    Qt::TextSelectableByKeyboard);
  if(mytetraConfig.getInterfaceMode()=="desktop")
    treePath->setVisible(false);
  else
    treePath->setVisible(true);
  treePath->setWordWrap(true);

  // Название записи
  recordName=new QLabel(this);
  recordName->setTextInteractionFlags(Qt::TextSelectableByMouse |
                                      Qt::TextSelectableByKeyboard);
  recordName->setVisible(false);
  recordName->setWordWrap(true);

  // Автор
  recordAuthor=new QLabel(this);
  recordAuthor->setTextInteractionFlags(Qt::TextSelectableByMouse |
                                        Qt::TextSelectableByKeyboard);
  recordAuthor->setVisible(false);
  recordAuthor->setWordWrap(true);

  // Url
  labelUrl=new QLabel(this);
  labelUrl->setText(tr("<B>Url:</B> "));
  labelUrl->setVisible(false);

  recordUrl=new QLabel(this);
  // recordUrl->setOpenExternalLinks(true);
  recordUrl->setTextInteractionFlags(Qt::TextSelectableByMouse |
                                     Qt::TextSelectableByKeyboard |
                                     Qt::LinksAccessibleByMouse |
                                     Qt::LinksAccessibleByKeyboard);
  recordUrl->setVisible(false);
  recordUrl->setWordWrap(true);

  // Текстовые метки
  labelTags=new QLabel(this);
  labelTags->setText(tr("<B>Tags:</B> "));
  labelTags->setVisible(false);
}


void MetaEditor::setupUI(void)
{
 // Область текстовых меток, которые выглядят на экране как [метка1] [метка2] [метка3] ...
 recordTagsLayout=new QHBoxLayout();
 recordTagsLayout->setAlignment(Qt::AlignLeft);
 recordTagsLayout->setMargin(0);

 // Область текстовых меток QHBoxLayout невозможно добавить в QScrollArea, поэтому оборачивается в виджет
 recordTagsContainer = new QWidget();
 recordTagsContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
 recordTagsContainer->setLayout(recordTagsLayout);

 // Создается QScrollArea и в нее добавляется виджет с QHBoxLayout
 recordTagsScrollArea=new QScrollArea();
 recordTagsScrollArea->setContentsMargins( 0, 0, 0, 0 ); // Убирается отступ от границ содержимого
 recordTagsScrollArea->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
 recordTagsScrollArea->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff ); // Убирается горизонтальная полоса прокрутки
 recordTagsScrollArea->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff ); // Убирается вертикальная полоса прокрутки
 recordTagsScrollArea->setFrameShape(QFrame::NoFrame); // Убирается тонкая линия вокруг QScrollArea
 recordTagsScrollArea->setWidget(recordTagsContainer);

 m_attachTableScreen=new AttachTableScreen(this);
}


// Сборка интерфейса расширенного редактора
void MetaEditor::metaAssembly(void)
{
  // Сборка виджета редактирования текста, который будет вставляться в слой
  m_extendEditorScreen=new QWidget(this);

  // Включается отображение фона чтобы фон закрашивался через CSS
  // По-умолчанию, у классов, унаследованных от QWidget, фон не отображается
  m_extendEditorScreen->setAttribute(Qt::WA_StyledBackground, true);

  m_extendEditorScreen->setObjectName("extendEditorScreen");

  QGridLayout *gridLayout=new QGridLayout();

  gridLayout->addWidget(editorToolBarAssistant,                   0,0, 1,2);
  gridLayout->addWidget(indentSliderAssistant->getIndentSlider(), 1,0, 1,2);
  gridLayout->addWidget(treePath,                                 2,0, 1,2);
  gridLayout->addWidget(recordName,                               3,0, 1,2);
  gridLayout->addWidget(recordAuthor,                             4,0, 1,2);
  gridLayout->addWidget(textArea,                                 5,0, 1,2);

  gridLayout->addWidget(labelUrl,             6,0);
  gridLayout->addWidget(recordUrl,            6,1);
  labelUrl->setProperty("rowNumber", 6); // Запоминается, в какой строке находится Url

  gridLayout->addWidget(labelTags,            7,0);
  gridLayout->addWidget(recordTagsScrollArea, 7,1);
  labelTags->setProperty("rowNumber", 7); // Запоминается, в какой строке находятся текстовые метки

  gridLayout->setColumnStretch(1,1);

  gridLayout->setContentsMargins(0,0,0,0);

  m_extendEditorScreen->setLayout(gridLayout);


  // Сборка виджетов в переключаемые слои
  m_metaEditorJoinLayer=new QVBoxLayout(this);
  m_metaEditorJoinLayer->addWidget(m_extendEditorScreen);
  m_metaEditorJoinLayer->addWidget(m_attachTableScreen);

  this->setLayout(m_metaEditorJoinLayer);


  // Границы убираются, так как данный объект будет использоваться как виджет
  QLayout *lt;
  lt=layout();
  lt->setContentsMargins(0,2,0,0);

  // По-умолчанию отображается слой редактирования
  switchToEditorLayout();
}


void MetaEditor::switchToEditorLayout(void)
{
  m_attachTableScreen->hide(); // Чтобы не было мерцания, вначале нужно делать сокрытие текущего виджета
  m_extendEditorScreen->show();
}


void MetaEditor::switchToAttachLayout(void)
{
  m_extendEditorScreen->hide();
  m_attachTableScreen->show();
}


// Статическая функция, обрабатывает клик в редакторе по кнопке переключения на список прикрепляемых файлов
void MetaEditor::toAttachCallback(void)
{
  // Так как функция статическая, у нее нет this-указателя на объект данного класса, его нужно получить поиском
  MetaEditor *edView=find_object<MetaEditor>("editorScreen");
  edView->switchToAttachLayout();
}


// Слот для установки значений инфополей на экране
void MetaEditor::setField(QString n, QString v)
{
  if     (n=="name")  setName(v);
  else if(n=="author")setAuthor(v);
  else if(n=="url")   setUrl(v);
  else if(n=="tags")  setTags(v);
  else
    criticalError("metaeditor.set_field Undestand field "+n+" with value "+v);
}


void MetaEditor::clearAll(void)
{
 qDebug() << "MetaEditor::clearAll()" ;

 // Очистка для слоя редактора
 setName("");
 setAuthor("");
 setUrl("");
 setTags("");

 setTextarea("");
 setWorkDirectory("");
 setFileName("");

 clearAllMiscField();

 // Все элементы управления рекдактором становятся недоступными
 setEnabled(false);

 // Иконка аттачей должна показывать что аттачей нет
 editorToolBarAssistant->switchAttachIconExists(false);

 // Очистка для слоя приаттаченных файлов
 m_attachTableScreen->clear();
}


void MetaEditor::setTreePath(QString path)
{
  treePath->setVisible(true);
  treePath->setText(tr("<b>Path: </b>")+path);
}


void MetaEditor::setName(QString name)
{
  // Наименование записи есть всегда, оно просто отображается
  recordName->setVisible(true);
  recordName->setText("<b>"+name.toHtmlEscaped()+"</b>");
}


void MetaEditor::setAuthor(QString author)
{
  if(author.length()==0)
  {
    recordAuthor->setVisible(false);
    recordAuthor->setText("");
  }
  else
  {
    recordAuthor->setVisible(true);
    recordAuthor->setText("<i>"+author.toHtmlEscaped()+"</i>");
  }
}


void MetaEditor::setUrl(QString url)
{
  if(url.length()==0)
  {
    labelUrl->setVisible(false);
    recordUrl->setVisible(false);

    recordUrl->setText("");

    this->setMininizeGridRow(labelUrl);
  }
  else
  {
    labelUrl->setVisible(true);
    recordUrl->setVisible(true);

    if(url.size()>64)
      recordUrl->setText("<a href=\""+url+"\">"+url.left(64)+"...</a>");
    else
      recordUrl->setText("<a href=\""+url+"\">"+url+"</a>");

    this->setNormalGridRow(labelUrl);
  }
}


void MetaEditor::setTags(QString tags)
{
 // Строка с метками запоминается в явном виде
 recordTagsText=tags;


 // Строка с метками разделяется на отдельные меки
 recordTagsTextList = recordTagsText.split(QRegExp("[,;]+"), QString::SkipEmptyParts);

 // В каждой метке убираются лишние пробелы по краям
 for(int i = 0; i < recordTagsTextList.size(); ++i)
  recordTagsTextList[i]=recordTagsTextList.at(i).trimmed();


 // Очищается слой с метками
 QLayoutItem *childItem;
 while((childItem = recordTagsLayout->takeAt(0)) != 0)
  {
   recordTagsLayout->removeItem(childItem);

   if(childItem->widget()!=NULL && childItem->widget()!=0)
    delete (childItem->widget());

   delete childItem;
  }

 // Очищается список меток
 recordTagsLabels.clear();

 // Создаются метки для экрана и запоминаются в список
 for(int i = 0; i < recordTagsTextList.size(); ++i)
  {
   // Объект метки на экране
   QLabel *tempLabel=new QLabel(this);
   tempLabel->setOpenExternalLinks(false);
   tempLabel->setTextInteractionFlags(Qt::TextSelectableByMouse |
                                      Qt::TextSelectableByKeyboard |
                                      Qt::LinksAccessibleByMouse |
                                      Qt::LinksAccessibleByKeyboard);
   tempLabel->setVisible(true);
   tempLabel->setWordWrap(false);

   // Настройка, чтоб не появлялась некрасивая пунктирная рамка
   // tempLabel->setFrameStyle(QFrame::NoFrame);
   tempLabel->setFocusPolicy(Qt::NoFocus);

   // Метке задается текст
   // В Url указывается порядковый номер метки
   QString labelText="<a href=\""+QString::number(i)+"\">"+recordTagsTextList.at(i)+"</a>";
   tempLabel->setText( labelText );

   // Клик по метке будет вызывать сигнал, слот будет принимать Url метки,
   // то есть в данном случае строку с номером метки
   connect(tempLabel, &QLabel::linkActivated,
           this,      &MetaEditor::onClickToTag);

   // Метка запоминается в список меток
   recordTagsLabels << tempLabel;

   // Метка добавляется в область размещения
   recordTagsLayout->addWidget(tempLabel);
  }

 // В правый конец горизонтального слоя виджетов с метками добавляется распорка
 recordTagsLayout->addStretch();


 // Вычисляется значение логического выражения
 bool visible = !(tags.length()==0 || recordTagsTextList.size()==0);

 // Установка формата отображения строки сетки
 if (visible)
 {
    this->setNormalGridRow(labelTags);
 }
 else
 {
    this->setMininizeGridRow(labelTags);
 }

 // Сокрытие или открытие элементов
 labelTags->setVisible(visible);
 recordTagsScrollArea->setVisible(visible);
 for(int i = 0; i < recordTagsLabels.size(); ++i)
 {
  recordTagsLabels.at(i)->setVisible(visible);
 }

 recordTagsContainer->adjustSize();
 recordTagsScrollArea->setMaximumHeight(recordTagsContainer->height());
}


// Метод определяет, в какой строке сетки находится переданный виджет
// и настраивается строку сетки так, чтобы она занимала минимальное пространство
void MetaEditor::setMininizeGridRow(QWidget *widget)
{
    int row = this->getGridRowNumber(widget);

    if (row != -1)
    {
        QGridLayout *layout = static_cast<QGridLayout*>( m_extendEditorScreen->layout() );

        // Настройки чтобы вид был минимальным
        layout->setRowMinimumHeight(row, 0);
        layout->setRowStretch(row, 0); // Убираем растяжение
    }
}


// Метод определяет, в какой строке сетки находится переданный виджет
// и настраивается строку сетки так, чтобы она приняла обычное форматирование
void MetaEditor::setNormalGridRow(QWidget *widget)
{
    int row = this->getGridRowNumber(widget);

    if (row != -1)
    {
        QGridLayout *layout = static_cast<QGridLayout*>( m_extendEditorScreen->layout() );

        // Настройки стандарного вида
        layout->setRowMinimumHeight(row, -1); // Авто-высота
        // layout->setRowStretch(row, 1); // Восстанавливается растяжение
    }
}


// Определяется, в какой строке сетки находится виджет,
// согласно выставленному ранее данному виджету свойству
int MetaEditor::getGridRowNumber(QWidget *widget)
{
    int row;
    QVariant rowVariant = widget->property("rowNumber");

    // Если свойство с номером строки есть
    if ( rowVariant.isValid() )
    {
        row = rowVariant.toInt();
    }
    else
    {
        row = -1;
    }

    return row;
}


// Слот принимает Url метки. Url состоит из порядкового номера метки,
// по нему восстанавливается текст метки
void MetaEditor::onClickToTag(const QString &tagNum)
{
 qDebug() << "Click to tag " << tagNum;

 // Текст метки
 QString tag=recordTagsTextList.at( tagNum.toInt() );
 qDebug() << "Tag text " << tag;

 // -----------------------------
 // Запуск поиска по тексту метки
 // -----------------------------

 // Определяется ссылка на виджет поиска
 FindScreen *findScreen=find_object<FindScreen>("findScreenDisp");

 // Если виджет не показан, он выводится на экран
 if(findScreen->isVisible()==false)
  findScreen->widgetShow();

 emit setFindTextSignal(tag);
}


void MetaEditor::onClickToUrl(const QString &link)
{
    LinkHelper::gotoReference( link );
}


// Метод используется, когда запись заблокирована
void MetaEditor::setReadOnly(bool state)
{
  // Слой редактирования
  editorToolBarAssistant->setEnableModifyTextButton( !state );
  indentSliderAssistant->getIndentSlider()->setEnabled( !state );

  textArea->setReadOnly(state); // textArea->setEnabled(false);

  // Слой прикрепленных файлов
  m_attachTableScreen->setReadOnly(state);
}


void MetaEditor::setFocusToBaseWidget()
{
    textArea->setFocus();
}
