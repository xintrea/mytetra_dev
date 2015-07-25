#ifndef _CLIPBBRANCH_H_
#define _CLIPBBRANCH_H_

#include <QWidget>
#include <QMimeData>
#include <QMap>
#include <QByteArray>
#include <QMultiMap>
#include <QList>

#include "libraries/ClipboardRecords.h"

// Определяется структура данных
// которая будет передаваться через буфер обмена
struct CLIPB_BRANCH_STRUCT
{
 // Список веток
 // Одна строка QList это набор нескольких элементов "ключ => значение"
 // например:
 // "branch_id" => "125"
 // "parent_id" => "100"
 // "name" => "Название ветки" и т.д.
 QList< QMap<QString, QString> > branch;

 // Список записей
 //          +----------------------------- Принадлежность ветке (branch_id)
 //          |        +-------------------- Запись
 //          |        |
 //          V        V
 QMultiMap< QString, Record >record;
};
Q_DECLARE_METATYPE(CLIPB_BRANCH_STRUCT);


struct CLIPB_TREE_ONE_LINE
{
 QString branch_id;
 QStringList subbranches_id;
};


class ClipboardBranch : public QMimeData
{
 Q_OBJECT
 
public:
 ClipboardBranch(void);
 ~ClipboardBranch(void);

 void init(void);

 void addBranch(QString parent_id, QMap<QString, QString> branch_fields);
 void addRecord(QString branch_id, Record record);

 void print(void) const;
 
 QList<CLIPB_TREE_ONE_LINE> getIdTree(void) const;
 void printIdTree(void) const;
 
 // Этот метод QMimeData надо переопределить, так как он виртуальный
 QStringList formats() const;

 // Получение полей для указанной ветки
 QMap<QString, QString> getBranchFieldsById(QString id);

 // Получение списка записей для указанной ветки
 QList<Record> getBranchRecords(QString id);

protected:
 // Этот метод QMimeData надо переопределить, так как он виртуальный
 QVariant retrieveData(const QString &format,QVariant::Type preferredType) const;
 
private:

 CLIPB_BRANCH_STRUCT branchData; // Данные, которые передаются через буфер обмена
 QStringList clipboardBranchFormat; // Строка с идентификатором формата

};

#endif // _CLIPBBRANCH_H_
