#include <QTextTableCell>
#include <QInputDialog>
#include <QMessageBox>
#include <QDebug>
#include <QTextDocumentFragment>
#include <QPointer>

#include "TableFormatter.h"

#include "../Editor.h"
#include "../EditorConfig.h"
#include "../EditorTextArea.h"

#include "../EditorSplitCellForm.h"
#include "../EditorAbsTable.h"
#include "../EditorAddTableForm.h"
#include "../EditorTablePropertiesForm.h"

#include "../../helpers/PaletteHelper.h"


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

  // Создается форма настройки таблицы
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
  QColor tableBackground = getTableBackgroundColor(table);
  if ( !tableBackground.isValid() )
  {
      tableBackground = QColor("transparent");
  }
  form.setBackgroundColor(tableBackground);

  qDebug() << "Table background color is: "
           << PaletteHelper::getRgbaColorText( tableBackground );


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

  // Проверка валидности курсора. Курсор может стать невалидным если пока была открыта
  // форма редактирования свойств таблицы, завершилась синхронизация и был обновлен документ.
  // Из-за невалидности курсора переменная table может начать указывать на уже не
  // существующую область памяти
  QTextCursor newCursor(textArea->textCursor());
  if ( cursor.position() != newCursor.position() ||
       cursor.document() != newCursor.document() )
  {
      // Курсор невалидный, менять цвет таблицы нельзя
      return;
  }

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


  cursor.beginEditBlock(); // Начало редактирования для неделимой отмены

  // Новый формат устанавливается текущей таблице
  table->setFormat( newFormat );

  // Цвет таблицы был применен через формат, и цвет надо удалить
  // для каждой ячейки <td> по отдельности, если он там был задан в виде bgcolor
  removeTableCellBackground(table);

  cursor.endEditBlock(); // Завершены действия по неделимому редактированию
}


// Получение цвета фона таблицы
// Возвращается невалидный QColor(), если фон не установлен, прозрачный или NoBrush
QColor TableFormatter::getTableBackgroundColor(QTextTable* table)
{
    if (!table)
    {
        qDebug() << "Таблица не существует";
        return QColor();
    }


    // 1. Проверка, что у всех ячеек таблиц в <td> задан одинаковый цвет,
    // который визуально перекрывает цвет фона, заданный для всей таблицы
    QColor uniformBackgroundColor = getUniformTableCellBackgroundColor(table);
    if ( uniformBackgroundColor.isValid() )
    {
        return uniformBackgroundColor;
    }


    QTextTableFormat format = table->format();

    // 2. Есть ли вообще свойство BackgroundBrush
    if (!format.hasProperty(QTextFormat::BackgroundBrush))
    {
        qDebug() << "У таблицы нет свойства BackgroundBrush";
        return QColor();
    }

    // 3. Получение кисти
    QBrush backgroundBrush = format.background();

    // 4. Проверка стиля кисти
    if (backgroundBrush.style() == Qt::NoBrush)
    {
        qDebug() << "Стиль кисти: NoBrush (фон не установлен)";
        return QColor();
    }

    // 5. Получение цвета фона таблицы
    QColor color = backgroundBrush.color();

    if (!color.isValid())
    {
        qDebug() << "Цвет кисти невалиден";
        return QColor();
    }

    // 6. Проверка прозрачности
    if (color.alpha() == 0)
    {
        qDebug() << "Цвет полностью прозрачный (alpha = 0)";
        return QColor();
    }

    // 7. Дополнительная проверка для градиентов и текстур
    if (backgroundBrush.style() != Qt::SolidPattern)
    {
        qDebug() << "Фон не является сплошным цветом, а градиент/текстура";
        // Для градиентов можно вернуть усредненный цвет или QColor()
        return QColor();
    }

    // qDebug() << "Найден цвет фона таблицы:" << color.name()
    //          << "alpha:" << color.alpha();

    return color;
}


// Получение комплексного цвета фона таблицы в случае, если для всех
// ячеек <td> установлено значение bgcolor, что визуально перекрывает
// цвет таблицы. Если цвет фона задан не для всех ячеек или цвет
// фона отличатся хоть в одной ячейке, возвращается невалидный пустой цвет
QColor TableFormatter::getUniformTableCellBackgroundColor(QTextTable* table)
{
    if (!table)
    {
        return QColor(); // Таблица не существует
    }

    int rows = table->rows();
    int cols = table->columns();

    if (rows == 0 || cols == 0)
    {
        return QColor(); // Таблица пустая
    }

    QColor firstCellColor;
    bool firstCellHasBackground = false;

    // Проход по всем ячейкам таблицы
    for (int row = 0; row < rows; ++row)
    {
        for (int col = 0; col < cols; ++col)
        {
            QTextTableCell cell = table->cellAt(row, col);
            if (!cell.isValid())
            {
                return QColor(); // Невалидная ячейка
            }

            QTextCharFormat cellFormat = cell.format();

            // Проверка наличия фона в ячейке
            if (cellFormat.hasProperty(QTextFormat::BackgroundBrush))
            {
                QBrush backgroundBrush = cellFormat.background();

                // Проверка, что кисть действительная и не NoBrush
                if (backgroundBrush.style() == Qt::NoBrush)
                {
                    return QColor(); // Фон не установлен
                }

                QColor cellColor = backgroundBrush.color();

                // Проверка валидности цвета и непрозрачности
                if (!cellColor.isValid() || cellColor.alpha() == 0)
                {
                    return QColor(); // Невалидный или прозрачный цвет
                }

                // Для первой ячейки сохраняется цвет
                if (!firstCellHasBackground)
                {
                    firstCellColor = cellColor;
                    firstCellHasBackground = true;
                }
                else // Для последующих - сравнение с первым
                {
                    // Сравнивание RGB компонент
                    if (cellColor != firstCellColor)
                    {
                        return QColor(); // Цвет отличается
                    }
                }
            }
            else
            {
                return QColor(); // Фон не задан, отсутствует свойство BackgroundBrush
            }
        }
    }

    if (firstCellHasBackground)
    {
        return firstCellColor; // Все ячейки имеют одинаковый цвет фона
    }
    else
    {
        return QColor(); // Ни одна ячейка не имеет фона
    }
}


void TableFormatter::removeTableCellBackground(QTextTable* table)
{
    if (!table)
    {
        return; // Таблица не существует
    }

    int rows = table->rows();
    int cols = table->columns();

    if (rows == 0 || cols == 0)
    {
        return; // Таблица пуста
    }


    // Проход по всем ячейкам таблицы
    for (int row = 0; row < rows; ++row)
    {
        for (int col = 0; col < cols; ++col)
        {
            QTextTableCell cell = table->cellAt(row, col);

            if (!cell.isValid())
            {
                continue; // Пропуск невалидной ячейки
            }

            QTextCharFormat cellFormat = cell.format();

            // Если задан фон у ячейки
            if (cellFormat.hasProperty(QTextFormat::BackgroundBrush))
            {
                // Очистка фона
                cellFormat.clearBackground();

                // Применение обновленного формата к ячейке
                cell.setFormat(cellFormat);
            }
        }
    }
}
