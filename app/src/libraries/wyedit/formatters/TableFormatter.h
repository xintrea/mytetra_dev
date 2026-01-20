#ifndef TABLEFORMATTER_H
#define TABLEFORMATTER_H

#include "Formatter.h"
#include "qtexttable.h"

// Класс форматирования таблиц в тексте


class TableFormatter : public Formatter
{
  Q_OBJECT

public:
  TableFormatter();

public slots:

  void onCreatetableClicked(void);
  void onTableRemoveRowClicked(void);
  void onTableRemoveColClicked(void);
  void onTableAddRowClicked(void);
  void onTableAddColClicked(void);
  void onTableMergeCellsClicked(void);
  void onTableSplitCellClicked(void);
  void onTablePropertiesClicked(void);

 private:

  QColor getTableBackgroundColor(QTextTable* table);
  QColor getUniformTableCellBackgroundColor(QTextTable* table);
  void removeTableCellBackground(QTextTable* table);

};

#endif // TABLEFORMATTER_H
