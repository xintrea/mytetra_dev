#include <QBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QSplitter>
#include <QtDebug>

#include "main.h"
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


extern GlobalParameters globalParameters;
extern AppConfig mytetraConfig;


MetaEditor::MetaEditor(void) : Editor()
{
  Editor::initEnableAssembly(false);
  Editor::initConfigFileName(globalParameters.getWorkDirectory()+"/editorconf.ini");
  Editor::initEnableRandomSeed(false);
  Editor::initDisableToolList( mytetraConfig.getHideEditorTools() );

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
  connect(this, SIGNAL(setFindTextSignal(QString)),
          globalParameters.getFindScreen(), SLOT(setFindText(QString)) );

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
  recordUrl->setOpenExternalLinks(true);
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

 attachTableScreen=new AttachTableScreen(this);
}


void MetaEditor::metaAssembly(void)
{
  // Сборка виджета редактирования текста (основной виджет)
  editorMainScreen=new QWidget(this);
  editorMainLayer=new QGridLayout(editorMainScreen);

  editorMainLayer->addWidget(editorToolBarAssistant,                   0,0, 1,2);
  editorMainLayer->addWidget(indentSliderAssistant->getIndentSlider(), 1,0, 1,2);
  editorMainLayer->addWidget(treePath,                                 2,0, 1,2);
  editorMainLayer->addWidget(recordName,                               3,0, 1,2);
  editorMainLayer->addWidget(recordAuthor,                             4,0, 1,2);
  editorMainLayer->addWidget(textArea,                                 5,0, 1,2);

  editorMainLayer->addWidget(labelUrl,             6,0);
  editorMainLayer->addWidget(recordUrl,            6,1);

  editorMainLayer->addWidget(labelTags,            7,0);
  editorMainLayer->addWidget(recordTagsScrollArea, 7,1); // Было addLayout(recordTagsLayout ...)

  editorMainLayer->setColumnStretch(1,1);

  editorMainLayer->setContentsMargins(0,0,0,0);

  editorMainScreen->setLayout(editorMainLayer);


  // Сборка виджетов в один слой
  metaEditorJoinLayer=new QVBoxLayout(this);
  metaEditorJoinLayer->addWidget(editorMainScreen);
  metaEditorJoinLayer->addWidget(attachTableScreen);

  this->setLayout(metaEditorJoinLayer);


  // Границы убираются, так как данный объект будет использоваться как виджет
  QLayout *lt;
  lt=layout();
  lt->setContentsMargins(0,2,0,0);

  // По-умолчанию отображается слой редатирования
  switchToEditorLayout();
}


void MetaEditor::switchToEditorLayout(void)
{
  attachTableScreen->hide(); // Что бы небыло мерцания, вначале нужно делать сокрытие текущего виджета
  editorMainScreen->show();
}


void MetaEditor::switchToAttachLayout(void)
{
  editorMainScreen->hide();
  attachTableScreen->show();
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
 attachTableScreen->clear();
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
  }
  else
  {
    labelUrl->setVisible(true);
    recordUrl->setVisible(true);

    if(url.size()>64)
      recordUrl->setText("<a href=\""+url+"\">"+url.left(64)+"...</a>");
    else
      recordUrl->setText("<a href=\""+url+"\">"+url+"</a>");
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
   connect(tempLabel, SIGNAL(linkActivated (const QString &)),
           this, SLOT(onClickToTag(const QString &)));

   // Метка запоминается в список меток
   recordTagsLabels << tempLabel;

   // Метка добавляется в область размещения
   recordTagsLayout->addWidget(tempLabel);
  }

 // В конец списка виджетов добавляется распорка
 recordTagsLayout->addStretch();

 // Если нечего выводить на экран
 if(tags.length()==0 || recordTagsTextList.size()==0)
 {
  labelTags->setVisible(false);

  for(int i = 0; i < recordTagsLabels.size(); ++i)
   recordTagsLabels.at(i)->setVisible(false);
 }
 else
 {
  labelTags->setVisible(true);

  for(int i = 0; i < recordTagsLabels.size(); ++i)
   recordTagsLabels.at(i)->setVisible(true);
 }

 recordTagsContainer->adjustSize();
 recordTagsScrollArea->setMaximumHeight(recordTagsContainer->height());
}


// Слот принимает Url метки. Url состоит из порядкового номера метки,
// по нему восстанавливается текст метки
void MetaEditor::onClickToTag(const QString &link_text)
{
 qDebug() << "Click to tag " << link_text;

 // Текст метки
 QString tag=recordTagsTextList.at( link_text.toInt() );
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


// Метод используется, когда запись заблокирована
void MetaEditor::setReadOnly(bool state)
{
  // Слой редактирования
  editorToolBarAssistant->setEnableModifyTextButton( !state );
  indentSliderAssistant->getIndentSlider()->setEnabled( !state );

  textArea->setReadOnly(state); // textArea->setEnabled(false);

  // Слой прикрепленных файлов
  attachTableScreen->setReadOnly(state);
}
