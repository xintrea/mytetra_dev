#include <QString>
#include <QStringList>
#include <QMutableMapIterator>

#include "FixedParameters.h"


FixedParameters::FixedParameters(QObject *parent) : QObject(parent)
{
  Q_UNUSED(parent);

  // Для записей
  recordFieldAvailableList=(QStringList() << "id" \
                                          << "name" \
                                          << "author" \
                                          << "url" \
                                          << "tags" \
                                          << "ctime" \
                                          << "dir" \
                                          << "file" \
                                          << "crypt" \
                                          << "block" \
                                                            \
                                          << "hasAttach" \
                                          << "attachCount");

  recordNaturalFieldAvailableList=(QStringList() << "id" \
                                                 << "name" \
                                                 << "author" \
                                                 << "url" \
                                                 << "tags" \
                                                 << "ctime" \
                                                 << "dir" \
                                                 << "file" \
                                                 << "crypt" \
                                                 << "block");

  recordCalculableFieldAvailableList=(QStringList() << "hasAttach" \
                                                    << "attachCount");

  recordFieldCryptedList=(QStringList() << "name" \
                                        << "author" \
                                        << "url" \
                                        << "tags");

  // Для веток
  itemFieldAvailableList=(QStringList() << "id" \
                                        << "name" \
                                        << "ctime" \
                                        << "crypt" \
                                        << "icon");

  itemFieldCryptedList=(QStringList() << "name" \
                                      << "icon");


  // Директория относительно XML-файла, в которой хранятся директории с иконками
  iconsRelatedDirectory="icons";
}


FixedParameters::~FixedParameters()
{

}


bool FixedParameters::isRecordFieldAvailable(QString name) const
{
  if(recordFieldAvailableList.contains(name))
    return true;
  else
    return false;
}


bool FixedParameters::isRecordFieldNatural(QString name) const
{
  if(recordNaturalFieldAvailableList.contains(name))
    return true;
  else
    return false;
}


bool FixedParameters::isRecordFieldCalculable(QString name) const
{
  if(recordCalculableFieldAvailableList.contains(name))
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
