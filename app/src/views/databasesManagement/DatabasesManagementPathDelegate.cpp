#include <QPainter>

#include "DatabasesManagementPathDelegate.h"


void DatabasesManagementPathDelegate::paint(QPainter *painter,
                                            const QStyleOptionViewItem &option,
                                            const QModelIndex &index) const
{
    /*
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    // Настраиваем текст с усечением в начале
    opt.text = opt.fontMetrics.elidedText(opt.text, Qt::ElideLeft, opt.rect.width());

    // Используем стандартную реализацию отрисовки
    QStyledItemDelegate::paint(painter, opt, index);
    */

    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    // Получаем исходный текст
    QString fullText = opt.text;

    // Рассчитываем ширину текста, который помещается в ячейку
    QFontMetrics fontMetrics(opt.font);
    QString displayedText = fullText;

    if (fontMetrics.horizontalAdvance(displayedText) > opt.rect.width())
    {
        displayedText = QString("…")+displayedText;
    }

    // Циклически удаляется первый (левый) символ до тех пор, пока
    // ширина строки не станет меньше ширины ячейки
    while (fontMetrics.horizontalAdvance(displayedText) > opt.rect.width()) {
        if ( displayedText.startsWith("…") )
        {
            displayedText.remove(1, 1); // Удаление второго символа (после "…")
        }
        else
        {
            displayedText.remove(0, 1); // Удаление первого символа
        }
    }

    // Рисуем обрезанный текст
    painter->save();
    painter->setClipRect(opt.rect); // Ограничиваем область рисования
    painter->drawText(opt.rect, Qt::AlignRight | Qt::AlignVCenter, displayedText);
    painter->restore();
}
