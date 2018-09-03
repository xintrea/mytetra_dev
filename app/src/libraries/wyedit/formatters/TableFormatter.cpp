#include <QTextTableCell>
#include <QInputDialog>
#include <QMessageBox>
#include <QDebug>
#include <QTextDocumentFragment>

#include "TableFormatter.h"

#include "../Editor.h"
#include "../EditorConfig.h"
#include "../EditorTextArea.h"

#include "../EditorSplitCellForm.h"
#include "../EditorAbsTable.h"
#include "../EditorAddTableForm.h"
#include "../EditorTablePropertiesForm.h"


TableFormatter::TableFormatter()
{

}


// Действия при нажатии кнопки создания новой таблицы
void TableFormatter::onCreatetableClicked(void)
{
  // Создается и запускается диалог создания новой таблицы
  EditorAddTableForm dialog;
  if(dialog.exec()!=QDialog::Accepted) return;

  // Выясняются введенные в диалоге данные
  int tableColumns=dialog.get_columns();
  int tableRows=dialog.get_rows();
  int tableWidth=dialog.get_width();

  // Целочислительный формат ширины таблицы преобразуется в проценты
  QTextLength tableWidthInPercent(QTextLength::PercentageLength, tableWidth);

  // Создается форматирование таблицы
  QTextTableFormat tableFormat;
  tableFormat.setWidth(tableWidthInPercent);
  tableFormat.setAlignment(Qt::AlignHCenter);
  tableFormat.setBorder(1);
  tableFormat.setBorderStyle(QTextFrameFormat::BorderStyle_Solid);
  tableFormat.setPadding(0);
  tableFormat.setCellPadding(0);
  tableFormat.setCellSpacing(-1);

  // Добавляется таблица с нужными размерами и форматированием
  // QTextTable *table=textarea->textCursor().insertTable(table_rows, table_columns, table_format);
  textArea->textCursor().insertTable(tableRows, tableColumns, tableFormat);

  return;
}


// Удаление строк из таблицы
void TableFormatter::onTableRemoveRowClicked(void)
{
  QTextCursor cursor(textArea->textCursor());
  QTextTable *table=cursor.currentTable();
  if(table!=0)
  {
    QTextTableCell cell=table->cellAt(cursor);
    int cellRowCursor=cell.row(); // Текущий номер строки (счет с нуля)
    table->removeRows(cellRowCursor,1); // Удаляются строки таблицы
  }
}


// Удаление столбцов из таблицы
void TableFormatter::onTableRemoveColClicked(void)
{
  QTextCursor cursor(textArea->textCursor());
  QTextTable *table=cursor.currentTable();
  if(table!=0)
  {
    QTextTableCell cell=table->cellAt(cursor);
    int cellColCursor=cell.column(); // Текущий номер столбца (счет с нуля)
    table->removeColumns(cellColCursor,1); // Удаляются столбцы таблицы
  }
}


// Добавление строк в таблицу
void TableFormatter::onTableAddRowClicked(void)
{
  QTextCursor cursor(textArea->textCursor());
  QTextTable *table = cursor.currentTable();
  if(table)
  {
    QTextTableCell cell=table->cellAt(cursor); // Выясняется текущая ячейка
    int cellRowCursor=cell.row(); // Текущий номер строки (счет с нуля)

    bool ok=false;
    int addNum=QInputDialog::getInt(editor, tr("Append rows to table"),tr("Append rows:"), 1, 1, 100, 1, &ok);

    if(ok && addNum > 0)
      table->insertRows(cellRowCursor + 1,addNum);
  }
}


// Добавление столбцов в таблицу
void TableFormatter::onTableAddColClicked(void)
{
  QTextCursor cursor(textArea->textCursor());
  QTextTable *table = cursor.currentTable();
  if(table)
  {
    QTextTableCell cell=table->cellAt(cursor); // Выясняется текущая ячейка
    int cellColCursor=cell.column(); // Текущий номер столбца (счет с нуля)

    bool ok=false;
    int addNum=QInputDialog::getInt(editor, tr("Append columns to table"),tr("Append columns:"), 1, 1, 100, 1, &ok);

    if(ok && addNum > 0)
      table->insertColumns(cellColCursor + 1,addNum);
  }
}


// Объединение ячеек таблицы
void TableFormatter::onTableMergeCellsClicked(void)
{
  QTextCursor cursor(textArea->textCursor());
  QTextTable *table=cursor.currentTable();
  if(table)
    table->mergeCells(cursor);
}


void TableFormatter::onTableSplitCellClicked(void)
{
  QTextCursor cursor(textArea->textCursor());
  QTextTable *table=cursor.currentTable();

  // Если курсор находится где-то внутри таблицы
  if(table)
  {
    // Выясняется, выделены ячейки или нет
    int *currX=new int;
    int *currY=new int;
    int *currWidthX=new int;
    int *currWidthY=new int;
    cursor.selectedTableCells(currX,currY,currWidthX,currWidthY);

    // Если ячейки выделены, и захвачено для выделения более одной ячейки
    if(*currWidthX>1 || *currWidthY>1)
    {
      // Работа возможна только с одной ячейкой
      QMessageBox msgBox;
      msgBox.setWindowTitle(tr("Split cell"));
      msgBox.setText(tr("Select a single cell."));
      msgBox.exec();
      return;
    }

    QTextTableCell cell=table->cellAt(cursor); // Выясняется текущая ячейка
    int cellColCursor=cell.column(); // Текущий номер столбца (счет с нуля)
    int cellRowCursor=cell.row(); // Текущий номер строки (счет с нуля)
    qDebug() << "Current cell is " << cellColCursor << cellRowCursor;

    // Курсором выделяется таблица
    cursor.setPosition(table->firstCursorPosition().position() - 1, QTextCursor::MoveAnchor);
    cursor.setPosition(table->lastCursorPosition().position() + 1, QTextCursor::KeepAnchor);

    // Выделенный фрагмент выделяется в отдельный объект
    QTextDocumentFragment fragment(cursor);

    EditorAbsTable current_table(table->columns(), table->rows(), fragment.toHtml());

    qDebug() << "This cell colspan " << current_table.get_cell_colspan(cellColCursor,cellRowCursor) << " rowspan " << current_table.get_cell_rowspan(cellColCursor,cellRowCursor);


    // Выясняется, на сколько сегментов можно разбивать ячейку
    int hSplitTo;
    int vSplitTo;
    int hCurrMerge=current_table.get_cell_colspan(cellColCursor,cellRowCursor);
    int vCurrMerge=current_table.get_cell_rowspan(cellColCursor,cellRowCursor);

    if(hCurrMerge==0)hCurrMerge=1;
    if(vCurrMerge==0)vCurrMerge=1;

    if(hCurrMerge==1) hSplitTo=0; // Можно разбивать на любое число
    else              hSplitTo=hCurrMerge;

    if(vCurrMerge==1) vSplitTo=0; // Можно разбивать на любое число
    else              vSplitTo=vCurrMerge;


    // Создается диалог запроса на сколько сегментов надо разбивать ячейку
    EditorSplitCellForm dialog(hSplitTo, vSplitTo);
    if(dialog.exec()==QDialog::Accepted)
    {
      int hSplit=dialog.get_num_split_h(); // Эти значения всегда 1 или больше
      int vSplit=dialog.get_num_split_v();

      // Разбивка по горизонтали
      if(hSplit>1)
      {
        if(hCurrMerge==1)
          current_table.split_single_cell_by_horisontal(cellColCursor,cellRowCursor,hSplit);
        else
          current_table.split_merged_cell_by_horisontal(cellColCursor,cellRowCursor);
      }

      // Разбивка по вертикали
      if(vSplit>1)
      {
        if(vCurrMerge==1)
          current_table.split_single_cell_by_vertical(cellColCursor,
                                                      cellColCursor+hSplit-1,
                                                      cellRowCursor,
                                                      vSplit);
        else
          current_table.split_merged_cell_by_vertical(cellColCursor,
                                                      cellColCursor+hSplit-1,
                                                      cellRowCursor);
      }

      qDebug() << "New table HTML text " << current_table.get_table();

      cursor.beginEditBlock();
      cursor.deleteChar();
      cursor.insertHtml(current_table.get_table());
      cursor.endEditBlock();

      return;
    }
    else
    {

    }


  } // Закончилось условие что курсор находится где-то внутри таблицы

}


void TableFormatter::onTablePropertiesClicked()
{
  QTextCursor cursor(textArea->textCursor());
  QTextTable *table = cursor.currentTable();

  // Если курсор не внутри таблицы, значит свойства таблицы менять не получится
  if(!table)
  {
    QMessageBox msgBox;
    msgBox.setText(tr("Can't modify table properties. Please set cursor into table."));
    msgBox.exec();

    return;
  }

  EditorTablePropertiesForm form;

  // Ширина таблицы
  qDebug() << "Type of table width lenght: " << table->format().width().type();
  if(table->format().width().type()==QTextLength::PercentageLength) // Если ширина указана в процентах
  {
    int tableWidth=(int) table->format().width().rawValue();
    form.setTableWidth( tableWidth );
  }
  else
  {
    // Иначе ширина фиксированная в пикселях, и нужно перевести пиксели в проценты
    qreal tableWidthPix=(int) table->format().width().rawValue();
    qreal pageWidthPix=1;
    qreal tableWidth=1;

    // Если ширина не была задана вообще
    if(tableWidthPix<1)
      tableWidth=50; // Условно берется 50%
    else
    {
      pageWidthPix=textArea->width();
      tableWidth=(tableWidthPix*100.0)/pageWidthPix;
    }

    form.setTableWidth( (int) tableWidth );
  }

  // Толщина линий
  int borderWidth=(int) table->format().border();
  form.setBorderWidth(borderWidth );

  // Цвет фона таблицы
  QColor tableBackground;
  if(table->format().hasProperty(QTextFormat::BackgroundBrush))
   tableBackground=table->format().background().color(); // Фон таблицы задан явно
  else
    tableBackground.setRgb(255,255,255); // Фон таблицы не задан, используется белый цвет
  form.setBackgroundColor(tableBackground);
  qDebug() << "Table background color is: " << tableBackground.name();

  // Выравнивание таблицы
  EditorTablePropertiesForm::TableAlign align=EditorTablePropertiesForm::Left;
  if(table->format().alignment()==Qt::AlignLeft)
    align=EditorTablePropertiesForm::Left;
  else if((table->format().alignment()==Qt::AlignHCenter) || (table->format().alignment()==Qt::AlignJustify))
    align=EditorTablePropertiesForm::Center;
  else if(table->format().alignment()==Qt::AlignRight)
    align=EditorTablePropertiesForm::Right;
  form.setTableAlign( align);


  // Отрисовывается форма редактирования свойств таблицы
  if(!form.exec())
    return;


  // Создаётся новый формат таблицы
  QTextTableFormat newFormat=table->format();

  // Устанавливается новая ширина таблицы
  QTextLength tableWidthLength(QTextLength::PercentageLength, form.getTableWidth());
  newFormat.setWidth( tableWidthLength );

  // Устанавливается новая толщина линий
  newFormat.setBorder( form.getBorderWidth() );
  newFormat.setBorderStyle(QTextFrameFormat::BorderStyle_Solid);
  newFormat.setPadding(0);
  newFormat.setCellPadding(0);
  newFormat.setCellSpacing(-1);

  // Устанавливается цвет фона
  newFormat.setBackground( form.getBackgroundColor() );

  if(form.getTableAlign()==EditorTablePropertiesForm::Left)
    newFormat.setAlignment( Qt::AlignLeft );
  else if(form.getTableAlign()==EditorTablePropertiesForm::Center)
    newFormat.setAlignment( Qt::AlignHCenter );
  else if(form.getTableAlign()==EditorTablePropertiesForm::Right)
    newFormat.setAlignment( Qt::AlignRight );

  // Новый формат устанавливается текущей таблице
  table->setFormat( newFormat );
}
