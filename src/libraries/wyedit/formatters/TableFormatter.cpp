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

  /*
 // Вставка новой таблицы через генериацию HTML кода
 // Если данные введены нормально
 if(table_vnum > 0 && table_hnum > 0 && table_width > 0)
  {
   QStringList tab;
   tab.clear();

   // QColor table_color;
   // table_color=qApp->palette().color(QPalette::Normal, QPalette::Base);
   // qDebug() << "Table background color " << table_color.name();
   // tab.append(QString("<table border=\"1\" align=\"center\" width=\"%1%\" cellspacing=\"0\" cellpadding=\"0\" bgcolor=\"%2%\">").arg(table_width).arg(table_color.name()));

   tab.append(QString("<table style=\"border-style:solid; border-right:none; border-bottom:none;\" border=\"1\" align=\"center\" width=\"%1%\" cellspacing=\"0\" cellpadding=\"3\">").arg(table_width));

   for (int i=0;i<table_hnum;i++)
    {
     tab.append(QString("<tr>"));
     for (int o=0;o<table_vnum;o++)
      {
       tab.append(QString("<td style=\"border-style:solid; border-right:none; border-bottom:none;\"><p></p></td>"));
      }
     tab.append(QString("</tr>"));
    }

   tab.append(QString("</table>"));

   QString table_text = tab.join("\n");
   QTextDocumentFragment fragment = QTextDocumentFragment::fromHtml(table_text);
   textarea->textCursor().insertFragment(fragment);
  }
 */

  /*
 // Вставка кода таблицы из файла
 QFile file("table.txt");
 if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
  critical_error("Can not open table file");

 QString tablecode;
 while(!file.atEnd())
 {
  QString line=file.readLine();
  tablecode=tablecode+line+"\n";
 }

 file.close();

 QTextDocumentFragment fragment = QTextDocumentFragment::fromHtml(tablecode);
 textarea->textCursor().insertFragment(fragment);

 return;
 */

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
