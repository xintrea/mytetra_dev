#ifndef __TREENODE_H__
#define __TREENODE_H__

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QObject>
#include <QtXml>

#include "TreeModel.h"

// KnowTreeModel - класс с удобными методами для управления данными дерева
// Внимание! Данный класс наследуется от класса TreeModel


class ClipboardBranch;

class KnowTreeModel : public TreeModel
{
    Q_OBJECT

public:
    KnowTreeModel(QObject *parent = 0); // KnowTreeModel(const QStringList &headers, QDomDocument domModel, QObject *parent = 0);
    ~KnowTreeModel();

    void initFromXML(QString fileName);
    void reload(void);

    QDomElement exportFullModelDataToDom(TreeItem *root);

    void save(void);

    // Добавление новой подветки к указанной ветке
    void addNewChildBranch(const QModelIndex &index, QString id, QString name);

    // Добавление новой ветки после указанной ветки
    void addNewSiblingBranch(const QModelIndex &index, QString id, QString name);

    // Добавление новой подветки к Item элементу
    void addNewBranch(TreeItem *parent, QString id, QString name);

    // Перемещение ветки вверх и вниз
    QModelIndex moveUpBranch(const QModelIndex &index);
    QModelIndex moveDnBranch(const QModelIndex &index);

    // Получение индекса подчиненного элемента с указанным номером
    QModelIndex indexChildren(const QModelIndex &parent, int n) const;
    
    QModelIndex getIndexByItem(TreeItem *item);

    // Поиск ветки с указанным ID
    TreeItem *getItemById(QString id);

    // Возвращает общее количество записей, хранимых в дереве
    int getAllRecordCount(void);
    
    // Возвращает количество записей в ветке и всех подветках
    int getRecordCountForItem(TreeItem *item);

    QString pasteNewChildBranch(const QModelIndex &index, ClipboardBranch *subbranch);
    QString pasteNewSiblingBranch(const QModelIndex &index, ClipboardBranch *subbranch);

    void reEncrypt(QString previousPassword, QString currentPassword);

    bool isContainsCryptBranches(void);

    QStringList getRecordPath(QString recordId);

    // Проверка наличия идентификатора ветки во всем дереве
    bool isItemIdExists(QString findId);

    // Проверка наличия идентификатора записи во всем дереве
    bool isRecordIdExists(QString findId);

private:

    QString xmlFileName;

    void init(QDomDocument *domModel);

    // Функция заполнения дерева из DOM-документа
    void setupModelData(QDomDocument *dommodel, TreeItem *parent);

    // Преобразование DOM в Item модель. Функция рекурсивного обхода элементов DOM-документа
    void parseNodeElement(QDomElement n, TreeItem *parent);

    // Преобразование Item в DOM модель
    void parseTreeToDom(QDomElement &xmlData, TreeItem *currItem);
    
    // Перемещение ветки вверх или вниз
    QModelIndex moveUpDnBranch(const QModelIndex &index,int direction);

    int getAllRecordCountRecurse(TreeItem *item,int mode);

    QString pasteSubbranchRecurse(TreeItem *item,
                                  QString startBranchId,
                                  ClipboardBranch *subbranch);

    TreeItem *getItemByIdRecurse(TreeItem *item, QString id, int mode);

    bool isContainsCryptBranchesRecurse(TreeItem *item, int mode);

    // Добавление подветки из буфера обмена относительно указанного элемента
    QString pasteSubbranch(TreeItem *item, ClipboardBranch *subbranch);

    QStringList getRecordPathRecurse(TreeItem *item,
                                     QStringList currentPath,
                                     QString recordId,
                                     int mode);

    bool isItemIdExistsRecurse(TreeItem *item, QString findId, int mode);

    bool isRecordIdExistsRecurse(TreeItem *item, QString findId, int mode);

    bool checkFormat(QDomElement elementFormat);

    bool updateSubVersionFrom1To2(void);

    // QModelIndex get_item_index_recurse(QModelIndex currindex, TreeItem *finditem, int mode);
};

#endif // __TREENODE_H__
