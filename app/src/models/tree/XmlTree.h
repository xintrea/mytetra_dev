#ifndef XMLTREE_H
#define XMLTREE_H

#include <QObject>
#include <QtXml>
#include <QTreeWidgetItem>


class XmlTree : public QObject
{
 Q_OBJECT

 public:
  XmlTree(void);
  ~XmlTree(void);
  bool load(QString file);
  QDomDocument* getDomModel(void);
  
 protected:
  QDomDocument *domModel; // DOM-представление документа
};


#endif // XMLTREE_H
