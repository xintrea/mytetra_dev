#include <QSizePolicy>

#include "InfoFieldEnter.h"
#include "main.h"
#include "models/appConfig/AppConfig.h"
#include "libraries/helpers/DebugHelper.h"

extern AppConfig mytetraConfig;

// Виджет ввода инфополей


InfoFieldEnter::InfoFieldEnter(QWidget *parent) : QWidget(parent)
{
 setup_ui();
 setup_signals();
 assembly();
}

InfoFieldEnter::~InfoFieldEnter()
{

}

void InfoFieldEnter::setup_ui(void)
{
 QPalette bgPalette = this->palette();
 bgPalette.setColor(QPalette::Base, bgPalette.color(QPalette::Background));

 // Элементы для отображения id записи
 recordIdLabel=new QLabel(this);
 recordIdLabel->setText(tr("Id"));
 recordId=new QLineEdit(this);
 recordId->setMinimumWidth(500);
 recordId->setReadOnly(true);
 recordId->setPalette(bgPalette);

 // Элементы для отображения названия каталога
 dirNameLabel=new QLabel(this);
 dirNameLabel->setText(tr("Directory name"));
 dirName=new QLineEdit(this);
 dirName->setMinimumWidth(500);
 dirName->setReadOnly(true);
 dirName->setPalette(bgPalette);

 // Элементы для запроса названия записи
 recordNameLabel=new QLabel(this);
 recordNameLabel->setText(tr("Title"));
 recordName=new QLineEdit(this);
 recordName->setMinimumWidth(500);

 // Элементы для запроса автора (авторов)
 recordAuthorLabel=new QLabel(this);
 recordAuthorLabel->setText(tr("Author(s)"));
 recordAuthor=new QLineEdit(this);

 // Элементы для запроса Url источника
 recordUrlLabel=new QLabel(this);
 recordUrlLabel->setText(tr("Url"));
 recordUrl=new QLineEdit(this);

 // Элементы для запроса текстовых меток
 recordTagsLabel=new QLabel(this);
 recordTagsLabel->setText(tr("Tags"));
 recordTags=new QLineEdit(this);

 // Кнопка раскрытия или закрытия полей author, url, tags...
 // Она в два раза меньше обычного размера
 expandInfo=new QToolButton(this);
 expandInfo->setObjectName("infoFieldEnterExpandInfo");
 expandInfo->setVisible(true);
 int w=expandInfo->geometry().width();
 int h=expandInfo->geometry().height();
 int x=qMin(w,h)/2;
 expandInfo->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed, QSizePolicy::ToolButton));
 expandInfo->setMinimumSize(x,x);
 expandInfo->setMaximumSize(x,x);
 expandInfo->resize(x,x);
 if(mytetraConfig.get_addnewrecord_expand_info()=="0")
  {
   expandInfo->setIcon(QIcon(":/resource/pic/triangl_dn.svg"));
   // expandInfo->setIcon(this->style()->standardIcon(QStyle::SP_ArrowDown));
  }
 else
  {
   expandInfo->setIcon(QIcon(":/resource/pic/triangl_up.svg"));
   // expandInfo->setIcon(this->style()->standardIcon(QStyle::SP_ArrowUp));
  }

  updateReadOnlyFieldsVisibility();
}


void InfoFieldEnter::setup_signals(void)
{
 connect(expandInfo, &QToolButton::pressed, this, &InfoFieldEnter::expandInfoClick);
}


// Элементы собираются в размещалку
void InfoFieldEnter::assembly(void)
{
 // Размещалка элементов
 infoFieldLayout=new QGridLayout(); // Попробовать this
 infoFieldLayout->setMargin(8);
 infoFieldLayout->setSpacing(10);

 int y=-1;

 infoFieldLayout->addWidget(recordNameLabel,++y,0);
 infoFieldLayout->addWidget(recordName,y,1);

 infoFieldLayout->addWidget(expandInfo,y,2);

 infoFieldLayout->addWidget(recordAuthorLabel,++y,0);
 infoFieldLayout->addWidget(recordAuthor,y,1);

 infoFieldLayout->addWidget(recordUrlLabel,++y,0);
 infoFieldLayout->addWidget(recordUrl,y,1);

 infoFieldLayout->addWidget(recordTagsLabel,++y,0);
 infoFieldLayout->addWidget(recordTags,y,1);

 infoFieldLayout->addWidget(recordIdLabel,++y,0);
 infoFieldLayout->addWidget(recordId,y,1);

 infoFieldLayout->addWidget(dirNameLabel,++y,0);
 infoFieldLayout->addWidget(dirName,y,1);

 // Устанавливается видимость или невидимость полей author, url, tags...
 expandInfoOnDisplay( mytetraConfig.get_addnewrecord_expand_info() );

 // Полученый набор элементов устанавливается для текущего виджета
 setLayout(infoFieldLayout);

 // Границы убираются, так как данный объект будет использоваться
 // как виджет
 QLayout *lt;
 lt=layout();
 lt->setContentsMargins(0,0,0,0);

 // setSizePolicy(QSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum));
}


void InfoFieldEnter::expandInfoOnDisplay(QString expand)
{
 bool i;

 if(expand=="0")
  i=false;
 else
  i=true;

 recordAuthorLabel->setVisible(i);
 recordAuthor->setVisible(i);

 recordUrlLabel->setVisible(i);
 recordUrl->setVisible(i);

 recordTagsLabel->setVisible(i);
 recordTags->setVisible(i);

 bool isDisplayReadOnlyFields = i && !isDisplayOnlyEditableFields;
 recordIdLabel->setVisible(isDisplayReadOnlyFields);
 recordId->setVisible(isDisplayReadOnlyFields);

 dirNameLabel->setVisible(isDisplayReadOnlyFields);
 dirName->setVisible(isDisplayReadOnlyFields);
}


void InfoFieldEnter::expandInfoClick(void)
{
 // Если в данный момент информация "свернута"
 if(mytetraConfig.get_addnewrecord_expand_info()=="0")
 {
  // Надо информацию развернуть
  expandInfoOnDisplay("1");

  mytetraConfig.set_addnewrecord_expand_info("1");

  expandInfo->setIcon(QIcon(":/resource/pic/triangl_up.svg"));
 }
 else
 {
  // Надо информацию свернуть
  expandInfoOnDisplay("0");

  mytetraConfig.set_addnewrecord_expand_info("0");

  expandInfo->setIcon(QIcon(":/resource/pic/triangl_dn.svg"));
 }
}


void InfoFieldEnter::setFocusToStart(void)
{
 recordName->setFocus(Qt::TabFocusReason);
}


bool InfoFieldEnter::checkFieldName(QString name)
{
 if(name=="name" ||
    name=="author" ||
    name=="url" ||
    name=="tags" ||
    name=="id" ||
    name=="dir")
  return true;
 else
  return false;
}


QString InfoFieldEnter::getField(QString name)
{
 if(checkFieldName(name))
  {
   if(name=="name")  return  recordName->text();
   if(name=="author")return  recordAuthor->text();
   if(name=="url")   return  recordUrl->text();
   if(name=="tags")  return  recordTags->text().simplified(); // При возврате значения, для тегов обязательно нужно убирать переносы строк, если они есть
  }
 else
  criticalError("Can not get field "+name+" in InfoFieldEnter method get_field");

 return QString();
}


void InfoFieldEnter::setField(QString name,QString value)
{
 if(checkFieldName(name))
  {
   if(name=="id")  recordId->setText(value);
   if(name=="dir")  dirName->setText(value);
   if(name=="name")  recordName->setText(value);
   if(name=="author")recordAuthor->setText(value);
   if(name=="url")   recordUrl->setText(value);
   if(name=="tags")  recordTags->setText(value.simplified()); // При внешней установке значения, для тегов обязательно нужно убирать переносы строк, если они есть
  }
 else
  criticalError("Can not set field "+name+" in InfoFieldEnter method set_field");
}


void InfoFieldEnter::setReadOnly(bool state)
{
  recordName->setReadOnly(state);
  recordAuthor->setReadOnly(state);
  recordUrl->setReadOnly(state);
  recordTags->setReadOnly(state);
}


bool InfoFieldEnter::isReadOnly()
{
  return recordName->isReadOnly();
}


void InfoFieldEnter::setDisplayOnlyEditableFields(bool value)
{
  isDisplayOnlyEditableFields = value;

  updateReadOnlyFieldsVisibility();
}

void InfoFieldEnter::updateReadOnlyFieldsVisibility()
{
  bool isVisible = !isDisplayOnlyEditableFields;

  recordIdLabel->setVisible(isVisible);
  recordId->setVisible(isVisible);

  dirNameLabel->setVisible(isVisible);
  dirName->setVisible(isVisible);
}
