#ifndef __MTCOMBOBOX_H__
#define	__MTCOMBOBOX_H__

#include <QObject>
#include <QWidget>
#include <QComboBox>
#include <QStyledItemDelegate>

// Класс, исправляющий QComboBox, чтобы правильно применялись QSS-стили
// Проблема описана здесь: stackoverflow.com/questions/13308341/qcombobox-abstractitemviewitem


class MtComboBox : public QComboBox
{
    Q_OBJECT

public:
    MtComboBox(QWidget *parent=0);
    virtual ~MtComboBox();

private:

    QStyledItemDelegate* itemDelegate;

};

#endif	/* __MTCOMBOBOX_H__ */

