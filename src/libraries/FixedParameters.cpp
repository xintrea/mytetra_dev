#include <QString>
#include <QStringList>
#include <QMutableMapIterator>

#include "FixedParameters.h"


FixedParameters::FixedParameters(QObject *parent) : QObject(parent)
{
  Q_UNUSED(parent);

}


FixedParameters::~FixedParameters()
{

}


// Перечень всех допустимых полей - натуральных и вычислимых
QStringList FixedParameters::recordFieldAvailableList(void) const
{
 // Для скорости задаются напрямую, не вызываются функции формирования из натурального и вычислимого списка полей

 QStringList names;

 names << "id";
 names << "name";
 names << "author";
 names << "url";
 names << "tags";
 names << "ctime";
 names << "dir";
 names << "file";
 names << "crypt";

 names << "hasAttach"; // Вычислимое поле
 names << "attachCount"; // Вычислимое поле

 return names;
}


// Перечень всех натуральных полей - то есть тех, которые напрямую хранятся в XML тегах
QStringList FixedParameters::recordNaturalFieldAvailableList(void) const
{
 QStringList names;

 names << "id";
 names << "name";
 names << "author";
 names << "url";
 names << "tags";
 names << "ctime";
 names << "dir";
 names << "file";
 names << "crypt";

 return names;
}


// Перечень всех вычислимых полей - такие поля нигде не сохраняются
QStringList FixedParameters::recordCalculableFieldAvailableList(void) const
{
 QStringList names;

 names << "hasAttach";
 names << "attachCount";

 return names;
}


bool FixedParameters::isRecordFieldAvailable(QString name) const
{
 if(recordFieldAvailableList().contains(name))
  return true;
 else
  return false;
}


bool FixedParameters::isRecordFieldNatural(QString name) const
{
 if(recordNaturalFieldAvailableList().contains(name))
  return true;
 else
  return false;
}



bool FixedParameters::isRecordFieldCalculable(QString name) const
{
 if(recordCalculableFieldAvailableList().contains(name))
  return true;
 else
  return false;
}


// Получение описаний набора полей
QMap<QString, QString> FixedParameters::recordFieldDescription(QStringList list) const
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


QStringList FixedParameters::recordFieldCryptedList(void) const
{
 QStringList names;

 names << "name";
 names << "author";
 names << "url";
 names << "tags";

 return names;
}
