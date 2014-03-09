#ifndef __MTSTYLEDITEMDELEGATE_H__
#define __MTSTYLEDITEMDELEGATE_H__

#include <QStyledItemDelegate>

// Этот класс не используется, скорее всего будет удален

class MtStyledItemDelegate : public QStyledItemDelegate
{
 Q_OBJECT
public:
 explicit MtStyledItemDelegate(QObject *parent = 0);

 QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

signals:

public slots:

};

#endif // __MTSTYLEDITEMDELEGATE_H__
