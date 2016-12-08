#include <stdio.h>

#include <QTextTable>
#include <QtDebug>
#include <QTextDocument>
#include <QTextCursor>
#include <QXmlStreamReader>

#include "EditorAbsTable.h"
#include "EditorAbsTableCell.h"

#include "../../main.h"


// Конструктор пустой таблицы
EditorAbsTable::EditorAbsTable(int x, int y, QObject *parent) : QObject(parent)
{
 columns=x;
 rows=y;
 create_cells_array(columns,rows);
}


// Конструктор таблицы
EditorAbsTable::EditorAbsTable(int x, int y, QString tableText, QObject *parent) : QObject(parent)
{
 columns=x;
 rows=y;
 create_cells_array(x,y);
 convert_table_to_internal(tableText);
}


EditorAbsTable::~EditorAbsTable(void)
{

}


// Заполнение внутреннего представления таблицы из переданной таблицы
void EditorAbsTable::set_table(int x, int y, QString tableText)
{
 create_cells_array(x,y);
 convert_table_to_internal(tableText);
}


// Первичное наполнение двумерного массива таблицы
void EditorAbsTable::create_cells_array(int x, int y)
{
 for(int j=0;j<x;j++)
  {
   QVector <EditorAbsTableCell> cellsLine;
   for(int i=0;i<y;i++)
    {
     EditorAbsTableCell init_cell;
     cellsLine.push_back(init_cell);
    }

   cells.push_back(cellsLine);
  }
}


// Очистка таблицы
void EditorAbsTable::clear_table(void)
{
 for(int i=0;i<columns;i++)
  for(int j=0;j<rows;j++)
   cells[i][j].clear();
}


// Очистка для суперячеек флага, информирующего что был изменен размер
void EditorAbsTable::clear_supercell_size_is_modify(void)
{
 for(int i=0;i<columns;i++)
  for(int j=0;j<rows;j++)
   if(cells[i][j].get_cell_type()==EditorAbsTableCell::IS_SUPER_CELL)
    cells[i][j].set_supercell_size_is_modify(false);
}


// Преобразование переданной таблицы во внутреннее представление
void EditorAbsTable::convert_table_to_internal(QString tableText)
{
 clear_table();

 qDebug() << "Source table" << tableText;

 QXmlStreamReader xmlReader(tableText);
 xmlReader.readNext(); // Считывается заголовок xml

 int x=-1;
 int y=-1;
 int cellStartPosition=0;
 int cellEndPosition=0;

 int tagTableBalance=0;
 int tagTrBalance=0;
 int tagTdBalance=0;

 // Перебираются теги XML элементов
 while(!xmlReader.atEnd())
  {
   // Если обнаружено начало элемента
   if(xmlReader.tokenType()==QXmlStreamReader::StartElement)
    {
     // Если обнаружено начало таблицы
     if(xmlReader.name().toString()=="table")
      {
       qDebug() << "Find <table> element";

       tagTableBalance++;

       if(tagTableBalance==1)
        {
         // Получаем список атрибутов таблицы
         QXmlStreamAttributes attr=xmlReader.attributes();

         // Список атрибутов таблицы запоминается в виде ассоциативного массива
         for(int i=0; i<attr.size(); ++i)
          {
           QString name=(attr.at(i)).name().toString();
           QString value=(attr.at(i)).value().toString();

           qDebug() << "Table property. Name" << name << "Value" << value;

           htmlProperty[name]=value;
          }

        } // Закончилось условие что это первичная таблица

      } // Закончилась обработка начала таблицы


     // Если обнаружено начало строки таблицы
     if(xmlReader.name().toString()=="tr")
      {
       qDebug() << "Find <tr> element";

       tagTrBalance++;

       // Если это начало первичной строки
       if(tagTrBalance==1)
        {
         x=-1; // Сбрасывается X-координата
         y++;  // Увеличивается Y-координата
        }
      }


     // Если обнаружено начало ячейки таблицы
     if(xmlReader.name().toString()=="td")
      {
       qDebug() << "Find <td> element";

       tagTdBalance++;

       // Если это начало первичной ячейки
       if(tagTdBalance==1)
        {
         x++;

         // Если текущая обрабатываемая ячейка уже занята
         // (Это может произойти при rowspan>1 у каких-то предыдущих ячеек)
         while(cells[x][y].get_cell_type()!=EditorAbsTableCell::IS_NULL_CELL) x++;

         // Получаем список атрибутов ячейки
         QXmlStreamAttributes attr=xmlReader.attributes();

         // Список атрибутов ячейки запоминается
         for(int i=0; i<attr.size(); ++i)
          {
           QString name=(attr.at(i)).name().toString();
           QString value=(attr.at(i)).value().toString();

           qDebug() << "Cell property. Name" << name << "Value" << value;

           cells[x][y].set_html_property(name,value);
          }

         // Получаем начало текста ячейки
         cellStartPosition=xmlReader.characterOffset();

         // Перед сохранением ячейки во внутреннее представление, надо
         // разобраться, растянута ли ячейка на несколько ячеек
         int colSpan=cells[x][y].get_html_property("colspan").toInt();
         int rowSpan=cells[x][y].get_html_property("rowspan").toInt();
         if(colSpan==0)colSpan=1;
         if(rowSpan==0)rowSpan=1;

         // Если ячейка растянута на несколько ячеек по горизонтали или вертикали
         if(colSpan>1 || rowSpan>1)
          {
           qDebug() << "Is super cell, Colspan" << colSpan << "Rowspan" << rowSpan;

           // Ячейка получает признак, что она супер-ячейка
           cells[x][y].set_cell_type(EditorAbsTableCell::IS_SUPER_CELL);

           // Заполняется прямоугольник подчиненных ячеек
           for(int i=0;i<colSpan;i++)
            for(int j=0;j<rowSpan;j++)
             if(!(i==0 && j==0))
              {
               // Ячейка получает признак, что она подчиненная ячейка
               cells[x+i][y+j].set_cell_type(EditorAbsTableCell::IS_SUB_CELL);

               // Эта подчиненная ячейка запоминает координаты своей супер-ячейки
               cells[x+i][y+j].set_ref_super_cell_xy(x,y);
              }
          }
         else
          {
           // Иначе ячейка не растянута ни по вертикали ни по горизонтали
           // И ячейка помечается как обыкновенная
           cells[x][y].set_cell_type(EditorAbsTableCell::IS_NORMAL_CELL);
          }

        } // Закрылось условие что обрабатывается первичная ячейка
      } // Закрылось условие что обнаружено начало ячейки

    } // Закрылось условие что обнаружено начало элемента


   // Если обнаружен конец элемента
   if(xmlReader.tokenType()==QXmlStreamReader::EndElement)
    {
     // Если обнаружен конец таблицы
     if(xmlReader.name().toString()=="table")
      {
       tagTableBalance--;
      }

     // Если обнаружен конец строки
     if(xmlReader.name().toString()=="tr")
      {
       tagTrBalance--;
      }

     // Если обнаружен конец ячейки таблицы
     if(xmlReader.name().toString()=="td")
      {

       tagTdBalance--;

       // Если закрылась первичная ячейка
       if(tagTdBalance==0)
        {
         qDebug() << "Find <td> close element";

         // Получаем позицию конца текста ячейки
         // Нужно учитывать, что текущий тег "</td>" имеет длину 5 символов
         cellEndPosition=xmlReader.characterOffset()-5;

         // Получаем текст ячейки
         QString cell_text=tableText.mid(cellStartPosition, cellEndPosition-cellStartPosition);

         qDebug() << "Cell text" << cell_text;

         // Устанавливаем в ячейке ее текст
         cells[x][y].set_value(cell_text);
        }  // Закрылось условие что закрывается первичная ячейка
      } // Закрылось условие что обнаружено закрытие ячейки

    } // Зкрылось условие что обнаружен конец какого-то элемента


   // Перейти к следующему узлу
   xmlReader.readNext();

  } // Закрылся цикл обхода узлов xml документа

 print_internal_table();

}


// Метод собирает таблицу из внутреннего представления в HTML строку
QString EditorAbsTable::get_table(void)
{
 // Создается элемент table
 QString table="<table ";

 // Задаются свойства элемента table
 QMapIterator<QString, QString> i(htmlProperty);
 while(i.hasNext())
  {
   i.next();
   table=table+i.key()+"=\""+i.value()+"\" ";
  }

 table=table+">";

 for(int y=0;y<rows;y++)
  {
   table=table+"<tr>";

   for(int x=0;x<columns;x++)
    {
     if(cells[x][y].get_cell_type()==EditorAbsTableCell::IS_NORMAL_CELL ||
        cells[x][y].get_cell_type()==EditorAbsTableCell::IS_SUPER_CELL)
      {
       table=table+"<td ";

       QMap< QString, QString > cellProperty;
       cellProperty=cells[x][y].get_html_property_map();

       // Задаются свойства элемента td
       QMapIterator<QString, QString> i(cellProperty);
       while(i.hasNext())
        {
         i.next();
         table=table+i.key()+"=\""+i.value()+"\" ";
        }

       // Конец тега td
       table=table+">";

       // Содержимое ячейки
       table=table+cells[x][y].get_value();

       // Завершение тега td
       table=table+"</td>";
      }
    } // Закрылся цикл перебора ячеек

   table=table+"</tr>";

  } // Закрылся цикл перебора строка

 table=table+"</table>";

 return table;
}


void EditorAbsTable::print_internal_table(void)
{
 for(int y=0;y<rows;y++)
  {
   for(int x=0;x<columns;x++)
    cells[x][y].print_cell();
   printf("\n");
  }
}


int EditorAbsTable::get_cell_colspan(int x, int y)
{
 return get_cell_col_or_row_span(x,y,"colspan");
}


int EditorAbsTable::get_cell_rowspan(int x, int y)
{
 return get_cell_col_or_row_span(x,y,"rowspan");
}


int EditorAbsTable::get_cell_col_or_row_span(int x, int y, QString propName)
{
 if(cells[x][y].get_cell_type()==EditorAbsTableCell::IS_NULL_CELL)
  criticalError("Detect bad table structure");

 if(cells[x][y].get_cell_type()==EditorAbsTableCell::IS_NORMAL_CELL)
  return 1;

 if(cells[x][y].get_cell_type()==EditorAbsTableCell::IS_SUPER_CELL)
  return cells[x][y].get_html_property(propName).toInt();

 if(cells[x][y].get_cell_type()==EditorAbsTableCell::IS_SUB_CELL)
  {
   int x_super=cells[x][y].get_ref_super_cell_x();
   int y_super=cells[x][y].get_ref_super_cell_y();

   return cells[x_super][y_super].get_html_property(propName).toInt();
  }

 return 0;
}


void EditorAbsTable::split_single_cell_by_horisontal(int x, int y, int div)
{
 // Проверка, можно ли разбивать ячейку
 bool splitEnable=false;
 if(cells[x][y].get_cell_type()==EditorAbsTableCell::IS_NORMAL_CELL)
  splitEnable=true;
 else
  if(cells[x][y].get_html_property("colspan").toInt()==0 ||
     cells[x][y].get_html_property("colspan").toInt()==1)
   splitEnable=true;

 if(splitEnable==false)
  criticalError("Cell ("+QString::number(x)+","+QString::number(y)+") can not split by horisontal to "+QString::number(div)+" parts. Method split_single_cell_by_horisontal().");


 // Перебирается счетчик разбиений
 for(int k=1;k<div;k++)
  {
   clear_supercell_size_is_modify();

   // Вставляется новый столбец справа от столбца с координатой X
   insert_column(x);

   qDebug() << "Internal table after insert column";
   print_internal_table();

   // Столбцы (основной и только что созданный) пробегаются сверху-вниз
   for(int i=0;i<rows;i++)
    {
     // Если обнаружена нулевая ячейка
     if(cells[x][i].get_cell_type()==EditorAbsTableCell::IS_NULL_CELL)
      {
       // Значит что-то не так, нулевых ячеек в этом столбце быть не должно
       criticalError("Detect null cell while split by horisontal cell "+QString::number(x)+" "+QString::number(i));
      }


     // Если обрабатывается ячейка выше или ниже разбиваемой
     if(i!=y)
      {

       switch( cells[x][i].get_cell_type() )
        {
         // Если это обычная ячейка
         case EditorAbsTableCell::IS_NORMAL_CELL:
          {
           // Она становится суперячейкой, объединяющей две ячейки
           cells[x][i].set_cell_type(EditorAbsTableCell::IS_SUPER_CELL);
           cells[x][i].set_html_property("colspan","2");

           // А соседняя справа становится подчиненной ячейкой
           cells[x+1][i].set_cell_type(EditorAbsTableCell::IS_SUB_CELL);
           cells[x+1][i].set_ref_super_cell_xy(x,i);

           break;
          }


         // Если это супер ячейка
         case EditorAbsTableCell::IS_SUPER_CELL:
          {
           // Расширить суперячейку можно только один раз
           if(cells[x][i].get_supercell_size_is_modify()==false)
            {
             // У этой суперячейки увеличивается colspan
             // И отмечается, что размер был изменен
             int colspan=cells[x][i].get_html_property("colspan").toInt();
             if(colspan==0)colspan=1;
             colspan++;
             cells[x][i].set_html_property("colspan",QString::number(colspan));
             cells[x][i].set_supercell_size_is_modify(true);
            }

           // Соседняя справа становится подчиненной ячейкой
           cells[x+1][i].set_cell_type(EditorAbsTableCell::IS_SUB_CELL);
           cells[x+1][i].set_ref_super_cell_xy(x,i);

           break;
          }

         // Если это подчиненная ячейка
         case EditorAbsTableCell::IS_SUB_CELL:
          {
           // Соседняя справа становится подчиненной ячейкой
           cells[x+1][i].set_cell_type(EditorAbsTableCell::IS_SUB_CELL);

           // Выясняются координаты суперячейки для текущей
           int super_x=cells[x][i].get_ref_super_cell_x();
           int super_y=cells[x][i].get_ref_super_cell_y();

           // Если координаты указывают не на разбиваемую ячейку
           if(!(super_x==x && super_y==y))
            {
             // Координаты ссылки на суперячейку (у соседней справа ячейки)
             // ставятся такие же как у текущей
             cells[x+1][i].set_ref_super_cell_xy(super_x,super_y);

             // Расширить суперячейку можно только один раз
             if(cells[super_x][super_y].get_supercell_size_is_modify()==false)
              {
               // В самой суперячейке увеличивается colspan, так как она
               // стала содержать на единицу больше ячеек
               int temp_colspan=cells[super_x][super_y].get_html_property("colspan").toInt();
               cells[super_x][super_y].set_html_property("colspan",QString::number(temp_colspan+1));

               // Отмечается, что размер был изменен
               cells[super_x][super_y].set_supercell_size_is_modify(true);
              }
            }
           else
            {
             // Иначе координаты указывают на разбиваемую ячейку
             // Поэтому координаты ссылки на суперячейку (у соседней с права ячейки)
             // ставятся на суперячейку в только что созданном столбце
             cells[x+1][i].set_ref_super_cell_xy(super_x+1,super_y);
            }

           break;
          }

        } // Закончился выбор (switch) типа ячейки

      }
     else
      {
       // Иначе в данный момент обрабатывается разбиваемая ячейка

       // Эта ячейка обязательно должна быть либо обычной
       // Либо суперячейкой с rowspan>1 и colspan<2
       if(!(cells[x][i].get_cell_type()==EditorAbsTableCell::IS_NORMAL_CELL ||
            cells[x][i].get_cell_type()==EditorAbsTableCell::IS_SUPER_CELL))
        {
         criticalError("Detect splitting target null or sub cell while split by horisontal cell "+QString::number(x)+" "+QString::number(i));
        }

       // Проверяем суперячейку на colspan<2
       if(cells[x][i].get_cell_type()==EditorAbsTableCell::IS_SUPER_CELL)
        {
         int colspan=cells[x][i].get_html_property("colspan").toInt();
         if(colspan>=2)
          criticalError("Detect splitting target super cell with colspan "+QString::number(colspan)+" while split by horisontal cell "+QString::number(x)+" "+QString::number(i));
        }

       // Если ячейка обычная
       if(cells[x][i].get_cell_type()==EditorAbsTableCell::IS_NORMAL_CELL)
        {
         // Соседняя справа тоже становится обычной
         cells[x+1][i].set_cell_type(EditorAbsTableCell::IS_NORMAL_CELL);
        }

       // Если суперячейка
       if(cells[x][i].get_cell_type()==EditorAbsTableCell::IS_SUPER_CELL)
        {
         // Выяснется rowspan (колько ячеек по высоте объединено)
         int rowspan=cells[x][i].get_html_property("rowspan").toInt();

         // Соседняя справа тоже становится суперячейкой
         // и ей устанавливается такой же rowspan
         cells[x+1][i].set_cell_type(EditorAbsTableCell::IS_SUPER_CELL);
         cells[x+1][i].set_html_property("rowspan",QString::number(rowspan));
        }

      } // Закрылось условие обработки ячейки выше или ниже разбиваемой

    } // Закончился цикл раздвигания таблицы сверху вниз

  } // Закончился цикл итераций разбиения ячейки

 qDebug() << "Internal table after slit by horisontal";
 print_internal_table();

}


void EditorAbsTable::split_merged_cell_by_horisontal(int x, int y)
{
 qDebug() << "Split merged cell ("<<x<<","<<y<<") by horisontal";
 print_internal_table();

 // Проверка, можно ли разбивать ячейку
 if(!(cells[x][y].get_cell_type()==EditorAbsTableCell::IS_SUPER_CELL &&
      cells[x][y].get_html_property("colspan").toInt()>=2))
  criticalError("Cell ("+QString::number(x)+","+QString::number(y)+") can not split by horisontal, this is not supercell with colspan>2");

 // На сколько частей по горизонтали можно разбить эту ячейку
 int div=cells[x][y].get_html_property("colspan").toInt();
 qDebug() << "Split cell to"<<div<<"by horisontal";

 // Сколько ячек эта суперячейка объединяет по высоте
 int rowSpan=cells[x][y].get_html_property("rowspan").toInt();
 qDebug() << "This cell merge by vertical"<<rowSpan<<"cells";

 // Если суперячейка объединяла только горизонтальные ячейки
 if(rowSpan<2)
  {
   // Все ячейки превращаются в обычную ячейку
   for(int i=x; i<=(x+div-1); i++)
    {
     cells[i][y].set_cell_type(EditorAbsTableCell::IS_NORMAL_CELL);
     cells[i][y].set_html_property("colspan","0");
     cells[i][y].set_ref_super_cell_xy(0,0);
    }
  }
else
  {
   // Иначе суперячейка объединяла ячейки и по вертикали

   // Все "верхние" ячейки превращаются в суперячейки с нужным rowspan
   for(int i=x; i<=(x+div-1); i++)
    {
     cells[i][y].set_cell_type(EditorAbsTableCell::IS_SUPER_CELL);
     cells[i][y].set_html_property("colspan","0");
     cells[i][y].set_html_property("rowspan",QString::number(rowSpan));
     cells[i][y].set_ref_super_cell_xy(0,0);

     // А все "нижние" уже являются подчиненными ячейками,
     // и им только корректируются ссылки на суперячейку
     for(int j=(y+1); j<=(y+rowSpan-1); j++)
      cells[i][j].set_ref_super_cell_xy(i,y);
    }

  } // Закрылось условие объединяла ли суперячейка только горизонтальные ячейки

 qDebug() << "Internal table after split by horisontal";
 print_internal_table();

}


void EditorAbsTable::split_single_cell_by_vertical(int x1, int x2, int y, int div)
{
 qDebug() << "Internal table before split cells ("<<x1<<","<<y<<")-("<<x2<<","<<y<<") to"<<div<<"by vertical";
 print_internal_table();


 // Проверка, можно ли разбивать ячейки
 // Разбиваемые ячейки должны быть неразбиты по вертикали
 // то есть иметь rowspan равный 0 или 1
 for(int x=x1; x<=x2; x++)
  {
   bool splitEnable=false;
   if(cells[x][y].get_cell_type()==EditorAbsTableCell::IS_NORMAL_CELL)
    splitEnable=true;
   else
    if(cells[x][y].get_html_property("rowspan").toInt()==0 ||
       cells[x][y].get_html_property("rowspan").toInt()==1)
     splitEnable=true;

   if(splitEnable==false)
     criticalError("Cell ("+QString::number(x)+","+QString::number(y)+") can not split by vertical to "+QString::number(div)+" parts. Method split_single_cell_by_vertical().");
  }


 // Перебирается счетчик разбиений
 for(int k=1;k<div;k++)
  {
   clear_supercell_size_is_modify();

   // Вставляется новая строка снизу от строки с координатой Y
   insert_row(y);

   qDebug() << "Internal table after insert row";
   print_internal_table();

   // Строки (основная и только что созданная) пробегаются слева-напрво
   for(int i=0;i<columns;i++)
    {
     // Если обнаружена нулевая ячейка
     if(cells[i][y].get_cell_type()==EditorAbsTableCell::IS_NULL_CELL)
      {
       // Значит что-то не так, нулевых ячеек в этой строке быть не должно
       criticalError("Detect null cell while split by vertical cell "+QString::number(i)+" "+QString::number(y));
      }


     // Если обрабатывается ячейка левее или правее разбиваемых ячеек
     if(i<x1 || i>x2)
      {

       switch( cells[i][y].get_cell_type() )
        {
         // Если это обычная ячейка
         case EditorAbsTableCell::IS_NORMAL_CELL:
          {
           // Она становится суперячейкой, объединяющей две ячейки
           cells[i][y].set_cell_type(EditorAbsTableCell::IS_SUPER_CELL);
           cells[i][y].set_html_property("rowspan","2");

           // А соседняя снизу становится подчиненной ячейкой
           cells[i][y+1].set_cell_type(EditorAbsTableCell::IS_SUB_CELL);
           cells[i][y+1].set_ref_super_cell_xy(i,y);

           break;
          }


         // Если это супер ячейка
         case EditorAbsTableCell::IS_SUPER_CELL:
          {
           // Расширить суперячейку можно только один раз
           if(cells[i][y].get_supercell_size_is_modify()==false)
            {
             // У этой суперячейки увеличивается rowspan
             // И отмечается, что размер был изменен
             int rowSpan=cells[i][y].get_html_property("rowspan").toInt();
             if(rowSpan==0)rowSpan=1;
             rowSpan++;
             cells[i][y].set_html_property("rowspan",QString::number(rowSpan));
             cells[i][y].set_supercell_size_is_modify(true);
            }

           // Соседняя снизу становится подчиненной ячейкой
           cells[i][y+1].set_cell_type(EditorAbsTableCell::IS_SUB_CELL);
           cells[i][y+1].set_ref_super_cell_xy(i,y);

           break;
          }

         // Если это подчиненная ячейка
         case EditorAbsTableCell::IS_SUB_CELL:
          {
           // Соседняя снизу становится подчиненной ячейкой
           cells[i][y+1].set_cell_type(EditorAbsTableCell::IS_SUB_CELL);

           // Выясняются координаты суперячейки для текущей
           int superX=cells[i][y].get_ref_super_cell_x();
           int superY=cells[i][y].get_ref_super_cell_y();

           // Если координаты указывают не на разбиваемые ячейки
           if(!(superX>=x1 && superX<=x2 && superY==y))
            {
             // Координаты ссылки на суперячейку (у соседней снизу ячейки)
             // ставятся такие же как у текущей
             cells[i][y+1].set_ref_super_cell_xy(superX,superY);

             // Расширить суперячейку можно только один раз
             if(cells[superX][superY].get_supercell_size_is_modify()==false)
              {
               // В самой суперячейке увеличивается rowspan, так как она
               // стала содержать на единицу больше ячеек
               int tempRowSpan=cells[superX][superY].get_html_property("rowspan").toInt();
               cells[superX][superY].set_html_property("rowspan",QString::number(tempRowSpan+1));

               // Отмечается, что размер был изменен
               cells[superX][superY].set_supercell_size_is_modify(true);
              }
            }
           else
            {
             // Иначе координаты указывают на разбиваемую ячейку
             // Поэтому координаты ссылки на суперячейку (у соседней снизу ячейки)
             // ставятся на суперячейку в только что созданной строке
             cells[i][y+1].set_ref_super_cell_xy(superX,superY+1);
            }

           break;
          }

        } // Закончился выбор (switch) типа ячейки

      }
     else
      {
       // Иначе в данный момент обрабатывается разбиваемая ячейка

       // Эта ячейка обязательно должна быть либо обычной
       // Либо суперячейкой с colspan>1 и rowspan<2
       if(!(cells[i][y].get_cell_type()==EditorAbsTableCell::IS_NORMAL_CELL ||
            cells[i][y].get_cell_type()==EditorAbsTableCell::IS_SUPER_CELL))
        {
         criticalError("Detect splitting target null or sub cell while split by vertical cell "+QString::number(i)+" "+QString::number(y));
        }

       // Проверяем суперячейку на rowspan<2
       if(cells[i][y].get_cell_type()==EditorAbsTableCell::IS_SUPER_CELL)
        {
         int rowSpan=cells[i][y].get_html_property("rowspan").toInt();
         if(rowSpan>=2)
          criticalError("Detect splitting target super cell with rowspan "+QString::number(rowSpan)+" while split by vertical cell "+QString::number(i)+" "+QString::number(y));
        }

       // Если ячейка обычная
       if(cells[i][y].get_cell_type()==EditorAbsTableCell::IS_NORMAL_CELL)
        {
         // Соседняя сснизу тоже становится обычной
         cells[i][y+1].set_cell_type(EditorAbsTableCell::IS_NORMAL_CELL);
        }

       // Если суперячейка
       if(cells[i][y].get_cell_type()==EditorAbsTableCell::IS_SUPER_CELL)
        {
         // Выяснется colspan (колько ячеек по ширине объединено)
         int colSpan=cells[i][y].get_html_property("colspan").toInt();

         // Соседняя снизу тоже становится суперячейкой
         // и ей устанавливается такой же colspan
         cells[i][y+1].set_cell_type(EditorAbsTableCell::IS_SUPER_CELL);
         cells[i][y+1].set_html_property("colspan",QString::number(colSpan));
        }

      } // Закрылось условие обработки ячейки слева или справа разбиваемых

    } // Закончился цикл раздвигания таблицы слева направо

  } // Закончился цикл итераций разбиения ячейки

}


// Метод предполагает, что разбиваются
// либо набор вертикально объединенных суперячеек
// когда разбивка по горизонтали уже была осуществлена (при этом x1<x2)
// либо одна суперячейка с объединением по вертикали
// когда разбивки по горизонтали небыло  (при этом x1=x2)
void EditorAbsTable::split_merged_cell_by_vertical(int x1, int x2, int y)
{
 qDebug() << "Split merged cell ("<<x1<<","<<y<<")-("<<x2<<","<<y<<") by vertical";
 print_internal_table();

 // Проверка, можно ли разбивать ячейки
 for(int x=x1; x<=x2; x++)
  {
   if(!(cells[x][y].get_cell_type()==EditorAbsTableCell::IS_SUPER_CELL &&
        cells[x][y].get_html_property("rowspan").toInt()>=2))
    criticalError("Cell ("+QString::number(x)+","+QString::number(y)+") can not split by vertical, this is not supercell with rowspan>2");
  }

 for(int x=x1; x<=x2; x++)
  if(cells[x][y].get_cell_type()==EditorAbsTableCell::IS_SUPER_CELL)
   {
    // На сколько частей по вертикали можно разбить эту ячейку
    int div=cells[x][y].get_html_property("rowspan").toInt();
    qDebug() << "Split cell to"<<div<<"by vertical";

    // Сколько ячек эта суперячейка объединяет по горизонтали
    int colSpan=cells[x][y].get_html_property("colspan").toInt();
    qDebug() << "This cell merge by horisontal"<<colSpan<<"cells";

    // Если суперячейка объединяла только вертикальные ячейки
    if(colSpan<2)
     {
      // Все ячейки превращаются в обычную ячейку
      for(int i=y; i<=(y+div-1); i++)
       {
        cells[x][i].set_cell_type(EditorAbsTableCell::IS_NORMAL_CELL);
        cells[x][i].set_html_property("colspan","0");
        cells[x][i].set_html_property("rowspan","0");
        cells[x][i].set_ref_super_cell_xy(0,0);
       }
     }
    else
     {
      // Иначе суперячейка объединяла ячейки и по вертикали

      // Все "левые" ячейки превращаются в суперячейки с нужным colspan
      for(int i=y; i<=(y+div-1); i++)
       {
        cells[x][i].set_cell_type(EditorAbsTableCell::IS_SUPER_CELL);
        cells[x][i].set_html_property("rowspan","0");
        cells[x][i].set_html_property("colspan",QString::number(colSpan));
        cells[x][i].set_ref_super_cell_xy(0,0);

        // А все "правые" уже являются подчиненными ячейками,
        // и им только корректируются ссылки на суперячейку
        for(int j=(x+1); j<=(x+colSpan-1); j++)
         cells[j][i].set_ref_super_cell_xy(x,i);
      }

    } // Закрылось условие объединяла ли суперячейка только горизонтальные ячейки

  } // Закрылся цикл перебора суперячеек слева направо

 qDebug() << "Internal table after split by horisontal";
 print_internal_table();

}


void EditorAbsTable::insert_column(int insX)
{
 insX++; // Корректируется координата, куда будет вставлятся столбец

 // Создается пустой столбец
 QVector <EditorAbsTableCell> cellsLine;
 for(int i=0;i<rows;i++)
  {
   EditorAbsTableCell init_cell;
   cellsLine.push_back(init_cell);
  }

 // Вставляется столбец
 cells.insert(insX,1,cellsLine);

 insX--; // Снова корректируется координата

 columns++; // Теперь столбцов стало на единицу больше


 // У всех подчиненных ячеек справа от вставленного столбца
 // надо подправить ссылку на суперячейку,
 // если суперячейка оказалась справа от вставленного слобца
 for(int i=insX+2;i<columns;i++)
  for(int j=0;j<rows;j++)
   if(cells[i][j].get_cell_type()==EditorAbsTableCell::IS_SUB_CELL)
    {
     int superX=cells[i][j].get_ref_super_cell_x();

     // qDebug()<<"For cell ("<<i<<","<<j<<"correct X ref to supercell from"<<super_x;

     if( superX > insX ) cells[i][j].set_ref_super_cell_x(superX+1);
    }

}


void EditorAbsTable::insert_row(int insY)
{
 insY++; // Корректируется координата, куда будет вставлятся строка

 // Перебираются столбцы
 for(int i=0;i<columns;i++)
  {
   // Пустая ячейка
   EditorAbsTableCell init_cell;

   // В каждый столбец по координате ins_y вставляется пустая ячейка
   // чтобы в результате получилсь пустая строка
   cells[i].insert(insY,1,init_cell);
  }

 insY--; // Снова корректируется координата

 rows++; // Теперь строк стало на единицу больше


 // У всех подчиненных ячеек снизу от вставленной строки
 // надо подправить ссылку на суперячейку,
 // если суперячейка оказалась снизу от вставленной строки
 for(int i=0;i<columns;i++)
  for(int j=insY+2;j<rows;j++)
   if(cells[i][j].get_cell_type()==EditorAbsTableCell::IS_SUB_CELL)
    {
     int superY=cells[i][j].get_ref_super_cell_y();
     if( superY > insY ) cells[i][j].set_ref_super_cell_y(superY+1);
    }

}
