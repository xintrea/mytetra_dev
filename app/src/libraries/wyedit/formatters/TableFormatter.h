#ifndef TABLEFORMATTER_H
#define TABLEFORMATTER_H

#include "Formatter.h"

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

};

#endif // TABLEFORMATTER_H
