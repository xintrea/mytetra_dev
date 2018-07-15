#ifndef __XMLTREE_H__
#define __XMLTREE_H__

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


#endif // __XMLTREE_H__
