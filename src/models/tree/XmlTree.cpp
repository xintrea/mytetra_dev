#include <QObject>
#include <QMessageBox>
#include <QTextCodec>

#include "XmlTree.h"
#include "main.h"
#include "models/recordTable/RecordTableData.h"

XmlTree::XmlTree(void)
{
  domModel=new QDomDocument();
}

XmlTree::~XmlTree(void)
{
  delete domModel;
}


bool XmlTree::load(QString file)
{
 // Загрузка файла дерева разделов
 QFile xmlFile(file);
 
 // Если файл не может быть открыт
 if(!xmlFile.open(QIODevice::ReadOnly | QIODevice::Text))
 {
  QMessageBox::information(0, tr("Error"),
                              tr("Cannot read file %1:\n%2.")
                              .arg(file)
                              .arg(xmlFile.errorString()));
  return false;
 }

 // Преобразование xml-документа в Dom представление
 QString errorStr;
 int errorLine;
 int errorColumn;
 if (!domModel->setContent(&xmlFile, true, &errorStr, &errorLine,&errorColumn))
  {
   QMessageBox::information(0, tr("Error converting to DOM"),
                               tr("Parse error at line %1, column %2:\n%3")
                               .arg(errorLine)
                               .arg(errorColumn)
                               .arg(errorStr));
   return false;
  }

 return true;
}


QDomDocument* XmlTree::getDomModel(void)
{
  return domModel;
}

