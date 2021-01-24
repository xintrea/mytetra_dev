#ifndef _EDITORABSTABLECELL_H_
#define _EDITORABSTABLECELL_H_

#include <QObject>
#include <QTextDocumentFragment>
#include <QMap>


// Этот класс нельзя унаследовать от QObject, так как в этом случае
// при вставке ячейки в вектор потребуется конструктор копирования

class EditorAbsTableCell
{

public:
  EditorAbsTableCell();
  EditorAbsTableCell(QString init_value);

  void    set_value(QString init_value);
  QString get_value(void);

  void    set_html_property(QString propName, QString propValue);
  QString get_html_property(QString propName);
  QMap< QString, QString > get_html_property_map(void);

  void clear(void);

  void set_cell_type(int i);
  int  get_cell_type(void);

  void set_ref_super_cell_xy(int x, int y);
  void set_ref_super_cell_x(int x);
  void set_ref_super_cell_y(int y);
  int  get_ref_super_cell_x(void);
  int  get_ref_super_cell_y(void);

  void set_supercell_size_is_modify(bool i);
  bool get_supercell_size_is_modify(void);

  void print_cell(void);

  enum CELL_TYPE
  {
   IS_NULL_CELL,
   IS_NORMAL_CELL,
   IS_SUPER_CELL,
   IS_SUB_CELL
  };

private:

 // Свойства ячейки, заполняются из свойств тега <td>
 QMap< QString, QString > htmlProperty;

 // Содержимое ячейки
 QString value;

 // Тип ячейки
 int cellType;

 // Координаты ведущей ячейки, если ячейка является подчиненной (isSubCell)
 int refSuperCellX;
 int refSuperCellY;

 // Свойство, используемое только в суперячейке
 bool superCellSizeIsModify;
};

#endif // _EDITORABSTABLECELL_H_
