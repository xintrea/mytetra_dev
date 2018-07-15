#include <stdio.h>

#include "EditorAbsTableCell.h"

#include "../../main.h"


// Конструктор по умолчанию пустой ячейки
EditorAbsTableCell::EditorAbsTableCell()
{
 clear();
 return;
}


// Конструктор ячейки с указанным содержимым
EditorAbsTableCell::EditorAbsTableCell(QString init_value)
{
 clear();
 value=init_value;
}


// Установка значения содержимого ячейки
void  EditorAbsTableCell::set_value(QString init_value)
{
 value=init_value;
}


QString EditorAbsTableCell::get_value(void)
{
 return value;
}


// Установка HTML-свойства ячейки (свойство в теге td)
void EditorAbsTableCell::set_html_property(QString propName, QString propValue)
{
 htmlProperty[propName]=propValue;
}


// Получение HTML-свойства ячейки по имени
QString EditorAbsTableCell::get_html_property(QString propName)
{
 return htmlProperty[propName];
}


// Получение всех HTML-свойств ячейки
QMap< QString, QString > EditorAbsTableCell::get_html_property_map(void)
{
 return htmlProperty;
}


// Полная очистка ячейки
void EditorAbsTableCell::clear(void)
{
 // Очистка свойств
 htmlProperty.clear();

 // Очистка содержимого
 value=QString();

 // Очистка типа ячейки
 cellType=IS_NULL_CELL;

 // Очистка координат ведущей ячейки
 refSuperCellX=0;
 refSuperCellY=0;

 // Очистка флага модификации суперячейки
 superCellSizeIsModify=false;
}


int EditorAbsTableCell::get_cell_type(void)
{
 return cellType;
}


void EditorAbsTableCell::set_cell_type(int i)
{
 cellType=i;
}


void EditorAbsTableCell::set_ref_super_cell_xy(int x, int y)
{
 refSuperCellX=x;
 refSuperCellY=y;
}


void EditorAbsTableCell::set_ref_super_cell_x(int x)
{
 refSuperCellX=x;
}


void EditorAbsTableCell::set_ref_super_cell_y(int y)
{
 refSuperCellY=y;
}


int EditorAbsTableCell::get_ref_super_cell_x(void)
{
 return refSuperCellX;
}


int EditorAbsTableCell::get_ref_super_cell_y(void)
{
 return refSuperCellY;
}


// Вывод содержимого ячейки для отладки
void EditorAbsTableCell::print_cell(void)
{
 char type;
 if(cellType==IS_NULL_CELL)   type='O';
 if(cellType==IS_NORMAL_CELL) type='N';
 if(cellType==IS_SUPER_CELL)  type='S';
 if(cellType==IS_SUB_CELL)    type='U';

 QString val=value.simplified().trimmed();
 val=val.leftJustified(5,'.',true);

 printf("%c[%d,%d](%d,%d)%s ",type,
                             htmlProperty["colspan"].toInt(),
                             htmlProperty["rowspan"].toInt(),
                             refSuperCellX,
                             refSuperCellY,
                             val.toUtf8().data()); // Ранее было toAscii, но в Qt 5.2 toAscii уже не поддерживается
}


void EditorAbsTableCell::set_supercell_size_is_modify(bool i)
{
 if(cellType!=IS_SUPER_CELL)
  {
   criticalError("Try set flag size_is_modify to not supercell");
   return;
  }
 else
  superCellSizeIsModify=i;
}


bool EditorAbsTableCell::get_supercell_size_is_modify(void)
{
 if(cellType!=IS_SUPER_CELL)
  {
   criticalError("Try get flag size_is_modify from not supercell");
   return false;
  }
 else
  return superCellSizeIsModify;
}

