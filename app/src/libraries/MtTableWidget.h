#ifndef __MTTABLEWIDGET_H__
#define __MTTABLEWIDGET_H__

#include <QWidget>
#include <QTableWidget>
#include <QStyledItemDelegate>

// Этот класс не используется, скорее всего будет удален

// Класс, исправляющий QTableWidget, чтобы правильно применялись QSS-стили

class MtTableWidget : public QTableWidget
{
    Q_OBJECT
public:
    MtTableWidget(QWidget *parent = 0);
    virtual ~MtTableWidget();

private:
    QStyledItemDelegate* itemDelegate;

};

#endif // __MTTABLEWIDGET_H__

