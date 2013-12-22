#include "editor/Editor.h"
#include "MetaEditor.h"
#include "editor/EditorTextArea.h"
#include "main.h"
#include "GlobalParameters.h"
#include "FindScreen.h"

#include <QtDebug>


extern GlobalParameters globalParameters;


MetaEditor::MetaEditor(void) : Editor()
{
 Editor::initEnableAssembly(false);
 Editor::initConfigFileName(globalParameters.getWorkDirectory()+"/editorconf.ini");
 Editor::initEnableRandomSeed(false);
 Editor::init();

 setupLabels();
 metaAssembly();

 setupSignals();

 update_indentline_geometry();
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

 recordTagsLayout=new QHBoxLayout(); // Область текстовых меток
 recordTagsLayout->setAlignment(Qt::AlignLeft);
}


void MetaEditor::metaAssembly(void)
{
 metaEditorAssemblyLayout=new QGridLayout(this);
 metaEditorAssemblyLayout->setObjectName("metaeditor_assembly_layout");

 metaEditorAssemblyLayout->addLayout(textformatButtonsLayout, 0,0, 1,2);
 metaEditorAssemblyLayout->addWidget(recordName,              1,0, 1,2);
 metaEditorAssemblyLayout->addWidget(recordAuthor,            2,0, 1,2);
 metaEditorAssemblyLayout->addWidget(textArea,                3,0, 1,2);

 metaEditorAssemblyLayout->addWidget(labelUrl,                4,0);
 metaEditorAssemblyLayout->addWidget(recordUrl,               4,1);

 metaEditorAssemblyLayout->addWidget(labelTags,               5,0);
 metaEditorAssemblyLayout->addLayout(recordTagsLayout,        5,1);

 metaEditorAssemblyLayout->setColumnStretch(1,1);

 // Полученый виджет устанавливается для текущего окна
 setLayout(metaEditorAssemblyLayout);

 // Границы убираются, так как данный объект будет использоваться
 // как виджет
 QLayout *lt;
 lt=layout();
 lt->setContentsMargins(0,2,0,0);
}


// Слот для установки значений инфополей на экране
void MetaEditor::setField(QString n, QString v)
{
 if     (n=="name")  setName(v);
 else if(n=="author")setAuthor(v);
 else if(n=="url")   setUrl(v);
 else if(n=="tags")  setTags(v);
 else
  critical_error("metaeditor.set_field Undestand field "+n+" with value "+v);
}


void MetaEditor::clearAll(void)
{
 setName("");
 setAuthor("");
 setUrl("");
 setTags("");
 set_textarea("");

 set_work_directory("");
 set_file_name("");
}


void MetaEditor::setName(QString name)
{
 recordName->setVisible(true);
 recordName->setText("<b>"+name+"</b>");
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
  recordAuthor->setText("<i>"+author+"</i>");
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
 FindScreen *findScreen=find_object<FindScreen>("findscreendisp");

 // Если виджет не показан, он выводится на экран
 if(findScreen->isVisible()==false)
  findScreen->widgetShow();

 emit setFindTextSignal(tag);
}