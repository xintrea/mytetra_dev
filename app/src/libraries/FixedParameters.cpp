#include <QString>
#include <QStringList>
#include <QMutableMapIterator>

#include "FixedParameters.h"


// Поля для записей
const QStringList FixedParameters::recordFieldAvailableList=       {"id", "name", "author", "url", "tags", "ctime", "dir", "file", "crypt", "block", "hasAttach", "attachCount"};
const QStringList FixedParameters::recordNaturalFieldAvailableList={"id", "name", "author", "url", "tags", "ctime", "dir", "file", "crypt", "block"};
const QStringList FixedParameters::recordCalculableFieldAvailableList={"hasAttach", "attachCount"};
const QStringList FixedParameters::recordFieldCryptedList={"name", "author", "url", "tags"};

// Поля для веток
const QStringList FixedParameters::itemFieldAvailableList={"id", "name", "ctime", "crypt", "icon"};
const QStringList FixedParameters::itemFieldCryptedList={"name", "icon"};

// Директория относительно XML-файла, в которой хранятся директории с иконками
const QString FixedParameters::iconsRelatedDirectory="icons";

// Текстовый идентификатор приложения
const QString FixedParameters::appTextId="mytetra";

// Описание заголовка математического выражения в поле Description PNG-файла
const QString FixedParameters::mathExpDescriptionType="mathExpression";
const int FixedParameters::mathExpVersion=1; // Текущая максимальная версия формата хранения формулы
const int FixedParameters::mathExpVersionNumberLen=4; // Сколько символов занимает номер версии (только цифры)
const int FixedParameters::mathExpHeaderLen=29; // Сколько символов занимает весь заголовок (префикс, номер версии, три двоеточия)


FixedParameters::FixedParameters(QObject *parent) : QObject(parent)
{
  Q_UNUSED(parent);

}


FixedParameters::~FixedParameters()
{

}


bool FixedParameters::isRecordFieldAvailable(QString name)
{
  if(recordFieldAvailableList.contains(name))
    return true;
  else
    return false;
}


bool FixedParameters::isRecordFieldNatural(QString name)
{
  if(recordNaturalFieldAvailableList.contains(name))
    return true;
  else
    return false;
}


bool FixedParameters::isRecordFieldCalculable(QString name)
{
  if(recordCalculableFieldAvailableList.contains(name))
    return true;
  else
    return false;
}


// Получение описаний набора полей
QMap<QString, QString> FixedParameters::recordFieldDescription(QStringList list)
{
  QMap<QString, QString> names;

  names["id"]=tr("ID");
  names["name"]=tr("Title");
  names["author"]=tr("Author");
  names["url"]=tr("Url");
  names["tags"]=tr("Tags");
  names["ctime"]=tr("Create time");
  names["dir"]=tr("Directory name");
  names["file"]=tr("File name");
  names["crypt"]=tr("Is crypt");
  names["hasAttach"]=tr("Has attachs");
  names["attachCount"]=tr("Attachs count");
  names["block"]=tr("Block");


  // Удаляются строчки, которых нет в переданном списке
  QMutableMapIterator<QString, QString> iterator(names);
  while (iterator.hasNext())
  {
    iterator.next();

    if( list.contains( iterator.key() )==false )
      iterator.remove();
  }

  return names;
}
