#include <QBoxLayout>
#include <QToolButton>
#include <QFontComboBox>
#include <QSpinBox>
#include <QTextEdit>
#include <QMimeData>
#include <QtDebug>

#include "main.h"
#include "ClipboardBranch.h"

ClipboardBranch::ClipboardBranch(void) : QMimeData()
{
 init();
}


ClipboardBranch::~ClipboardBranch(void)
{
 
}


void ClipboardBranch::init(void)
{
 branchData.branch.clear();
 branchData.record.clear();

 clipboardBranchFormat.clear();
 clipboardBranchFormat << "mytetra/branch";
}


void ClipboardBranch::print(void) const
{
 // Перебираются ветки
 typedef QMap<QString, QString> branch_type;
 foreach(branch_type current_branch, branchData.branch)
 {
  // Перебираются и печатаются значения полей текущей ветки
  qDebug() << "Tree item fields:";
  foreach(QString field_name, current_branch.keys())
   if(field_name=="id" || field_name=="name")
    qDebug() << field_name << ":" << current_branch.value(field_name);

  // Запоминается id текущей ветки
  QString branch_id=current_branch.value("id");

  // Находятся все записи, принадлежащие текущей ветке
  foreach(Record current_record, branchData.record.values(branch_id))
  {
   qDebug() << "Record:";

   QMap<QString, QString> current_record_fields=current_record.getNaturalFieldList();
   foreach(QString field_name, current_record_fields.keys ())
    if(field_name=="id" || field_name=="name")
     qDebug() << field_name << ":" << current_record_fields.value(field_name);
  }
 }
}


// Получение идентификторов веток в виде, описывающим дерево
// Формат строк списка:
// ID узла 1, список подчиненных узлов
// ID узла 2, список подчиненных узлов и т.д.
QList<CLIPB_TREE_ONE_LINE> ClipboardBranch::getIdTree(void) const
{
 QList<CLIPB_TREE_ONE_LINE> tree;

 // Перебираются строки с ветками
 typedef QMap<QString, QString> branch_type;
 foreach(branch_type current_branch, branchData.branch)
 {
  QString current_id=current_branch.value("id");

  // Составляется список подветок
  QStringList subbranches;
  foreach(branch_type current_subbranch, branchData.branch)
   if(current_subbranch.value("parent_id")==current_id)
    subbranches << current_subbranch.value("id");

  CLIPB_TREE_ONE_LINE one_line;
  one_line.branch_id=current_id;
  one_line.subbranches_id=subbranches;
  tree << one_line;
 }

 return tree;
}


void ClipboardBranch::printIdTree(void) const
{
 QList<CLIPB_TREE_ONE_LINE> tree=getIdTree();

 foreach(CLIPB_TREE_ONE_LINE one_line, tree)
  qDebug() << one_line.branch_id << one_line.subbranches_id;
}


// Добавление ветки
void ClipboardBranch::addBranch( QString parent_id, QMap<QString, QString> branch_fields)
{
 QMap<QString, QString> line;
 
 line["parent_id"]=parent_id;
 line.unite(branch_fields);

 branchData.branch << line;
}


// Добавление конечной записи
void ClipboardBranch::addRecord(QString branch_id, Record record)
{
 // todo: Сделать проверку, есть ли ветка с указанным id
 branchData.record.insert(branch_id, record);
}


// Получение полей для указанной ветки
QMap<QString, QString> ClipboardBranch::getBranchFieldsById(QString id)
{
 // Перебираются ветки чтобы найти ветку с нужным идентификатором
 typedef QMap<QString, QString> branch_type;
 foreach(branch_type current_branch, branchData.branch)
 {
  if(current_branch.contains("id"))
   if(current_branch["id"]==id)
    return current_branch;
 }

 criticalError("Can not find id "+id+" in clipboard data");
 exit(1);
 return QMap<QString, QString>();
}


// Получение списка записей для указанной ветки
QList< Record > ClipboardBranch::getBranchRecords(QString id)
{
 QList< Record > records;

 // Находятся записи с нужным идентификатором
 // Записи добавляются в records в последовательности задом-наперёд
 // из-за особенностей реализации foreach для QMultiMap
 foreach(Record current_record, branchData.record.values(id))
  records.insert(0, current_record);

 return records;
}


QStringList ClipboardBranch::formats() const
{
 return clipboardBranchFormat;
}


QVariant ClipboardBranch::retrieveData(const QString &format,QVariant::Type preferredType) const
{
 Q_UNUSED(preferredType);

 if(format==clipboardBranchFormat[0])
  {
   QVariant v;
   v.setValue(branchData);
   return v;
  }

 return 0;
} 

