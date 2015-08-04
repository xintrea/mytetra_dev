#include <QFile>
#include <QMessageBox>

#include "main.h"

#include "Attach.h"


Attach::Attach(int iType, Record *iParent)
{
  if(iType!=typeFile && iType!=typeLink)
    critical_error("Incorrect attach type in Attach constructor: "+QString::number(iType));

  parent=iParent;
  type=iType;
}


Attach::~Attach()
{

}


int Attach::getType()
{
  return type;
}


void Attach::setId(QString iId)
{
  id=iId;
}


QString Attach::getId()
{
  return id;
}


// Короткое имя файла (т. е. без пути)
void Attach::setFileName(QString iFileName)
{
  if(type!=typeFile)
    critical_error("Can't set file name to non-file attach.");

  fileName=iFileName;
}


QString Attach::getFileName()
{
  if(type!=typeFile)
    critical_error("Can't get file name from non-file attach.");

  return fileName;
}


bool Attach::setLink(QString iLink)
{
  if(type!=typeLink)
    critical_error("Can't set link to non-link attach.");

  QFile tempFile(iLink);

  // Если файла, на который ссылается линк, не существует
  if(!tempFile.exists())
  {
    QMessageBox msgBox;
    msgBox.setWindowTitle(QObject::tr("Warning!"));
    msgBox.setText( QObject::tr("Bad link. File not found.") );
    msgBox.setIcon(QMessageBox::Information);
    msgBox.exec();

    return false;
  }
  else
  {
    link=iLink;
    return true;
  }
}


QString Attach::getLink()
{
  if(type!=typeLink)
    critical_error("Can't get link from non-link attach.");

  return link;
}

