#ifndef _EDITORABSTABLE_H_
#define _EDITORABSTABLE_H_

#include <QObject>


class QTextTable;
class EditorAbsTableCell;

class EditorAbsTable : public QObject
{
public:
 EditorAbsTable(int x, int y, QObject *parent=0);
 EditorAbsTable(int x, int y, QString tableText, QObject *parent=0);
 ~EditorAbsTable(void);

 void set_table(int x, int y, QString tableText);

 int get_cell_colspan(int x, int y);
 int get_cell_rowspan(int x, int y);

 void split_single_cell_by_horisontal(int x, int y, int div);
 void split_merged_cell_by_horisontal(int x, int y);

 void split_single_cell_by_vertical(int x1, int x2, int y, int div);
 void split_merged_cell_by_vertical(int x1, int x2, int y);

 QString get_table(void);

private:

 void create_cells_array(int x, int y);
 void clear_table(void);
 void clear_supercell_size_is_modify(void);
 void convert_table_to_internal(QString tableText);
 void print_internal_table(void);
 int  get_cell_col_or_row_span(int x, int y, QString propName);

 void insert_column(int insX);
 void insert_row(int insY);

 int columns;
 int rows;

 // Свойства таблицы, заполняются из свойств тега <table>
 QMap< QString, QString > htmlProperty;

 // EditorAbsTableCell **cells;

 QVector < QVector <EditorAbsTableCell> > cells;

};

#endif // _EDITORABSTABLE_H_

