#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QObject>
#include <QtXml>


// TreeModel - Это вспомогательный класс! От него наследуется KnowTreeModel

class TreeItem;

class TreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    TreeModel(QObject *parent = 0);
    ~TreeModel(void);

    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;

    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;

    int rowCount(const QModelIndex &itemIndex = QModelIndex()) const;
    int columnCount(const QModelIndex &itemIndex = QModelIndex()) const;


    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole);
    bool setHeaderData(int section, Qt::Orientation orientation,
                       const QVariant &value, int role = Qt::EditRole);

    bool insertRows(int position, int rows,
                    const QModelIndex &parent = QModelIndex());
    bool removeRows(int position, int rows,
                    const QModelIndex &parent = QModelIndex());

    // Возвращение указателя на Item-элемент с указанным index
    // Где index - это индекс объекта в терминах структуры модель-вид
    TreeItem *getItem(const QModelIndex &index) const;

    // Возвращение указателя на Item-элемент с указанным путем
    // в виде последовательности идентификаторов
    TreeItem *getItem(QStringList path) const;

    bool isItemValid(QStringList path) const;

    QModelIndex getIndexByItem(TreeItem *item);

    void emitSignalDataChanged(const QModelIndex &index);

    TreeItem *rootItem; // Ссылка на первый (корневой) item-объект

private:

    QModelIndex getIndexRecurse(QModelIndex index, TreeItem *item, int mode);

    // Элемент, над которым проносят курсор. Используется при Drag And Drop.
    QModelIndex cursorOverIndex;
};

#endif
