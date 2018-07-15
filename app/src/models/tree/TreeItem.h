#ifndef TREEITEM_H
#define TREEITEM_H

#include <QList>
#include <QVariant>
#include <QVector>
#include <QStringList>
#include <QDomElement>
#include <QDomDocument>
#include <QIcon>
#include <QXmlStreamWriter>

#include "models/recordTable/RecordTableData.h"

class TreeItem
{
 public:
    TreeItem(const QMap<QString, QString> &data, TreeItem *parent = 0);
    ~TreeItem();

    // Возвращение ссылки на потомка, который хранится в списке childItems
    // под указанным номером
    TreeItem *child(int number);

    // Возвращение количества потомков (т.е. количество записей в списке childItems)
    unsigned int childCount() const;

    // Возвращение количества полей, которые хранит данный элемент.
    int fieldCount() const;

    // Получение значения поля по имени
    QString getField(QString name);

    // Получение всех полей данных
    QMap<QString, QString> getAllFields();

    // Получение всех полей данных напрямую, без преобразований
    QMap<QString, QString> getAllFieldsDirect();

    // Заполнение указанного поля
    void setField(QString name, QString value);

    // Заполнение указанного поля данными напрямую, без преобразований
    void setAllFieldDirect(const QMap<QString, QString> nameAndValue);
    
    // Добавление потомка (потомков) к текущему элементу
    // position - после какой позиции массива childItems вставить
    // count - сколько потомков вставить (обычно 1, но можно и несколько)
    // columns - сколько столбцов должен содержать потомок
    bool insertChildren(int position, int count, int columns);

    // Добавление нового пустого подчиненного элемента в конец списка подчиненных элементов
    bool addChildrenEmpty(void);

    // Добавление уже существующего Item-элемента
    bool addChildrenItem(TreeItem *item);
    
    // Возвращение ссылки на родительский элемент
    TreeItem *parent();

    // Удаление потомков, начиная с позиции position массива childItems
    bool removeChildren(int position, int count);

    // Удаление всех потомков элемента
    void removeAllChildren();

    // Возвращает номер, под которым данный объект хранится
    // в массиве childItems своего родителя
    unsigned int childNumber() const;

    bool moveUp(void);
    bool moveDn(void);
    
    // Возвращает id путь (список идентификаторов от корня до текущего элемента)
    QStringList getPath(void);
    
    // Возвращает путь в виде названий веток дерева
    QStringList getPathAsName(void);

    QString getPathAsNameWithDelimeter(QString delimeter);

    // Возвращает набор значений указанного поля для пути от корня к ветке
    QStringList getPathAsField(QString fieldName);

    // Возвращает массив путей всех подветок, которые содержит ветка
    QList<QStringList> getAllChildrenPath(void);

    // Возвращает набор значений указанного поля для подветок
    QList<QStringList> getAllChildrenPathAsField(QString fieldName);

    // Получение идентификатора элемента
    QString getId();

    // Получение идентификатора родительской ветки
    QString getParentId();

    // Получение иконки ветки
    QIcon getIcon();

    // Шифрация данной ветки и всех подветок
    void switchToEncrypt(void);
    
    // Расшифровка данной ветки и всех подветок
    void switchToDecrypt(void);

    // Первичное заполнение таблицы конечных записей, "промежуточный" метод
    void recordtableInit(QDomElement domModel);

    // Взятие количества записей в таблице конечных записей, "промежуточный" метод
    int recordtableGetRowCount(void);

    // Удаление всех элементов в таблице конечных записей, "промежуточный" метод
    void recordtableDeleteAllRecords(void);
    
    // Преобразование таблицы конечных записей в DOM представление, "промежуточный" метод
    QDomElement recordtableExportDataToDom(QDomDocument *doc);
    void recordtableExportDataToStreamWriter(QXmlStreamWriter *xmlWriter);

    // Взятие ссылки на данные конечных записей
    RecordTableData *recordtableGetTableData(void);

    void setDetached(bool state);
    bool isDetached();
    
private:
    bool removeChildrenLink(int position, int count);

    void empty(void);

    QList<QStringList> getAllChildrenPathAsFieldRecurse(TreeItem *item, QString fieldName, int mode);

    QList<TreeItem*> childItems; // Список ссылок на потомков
    TreeItem *parentItem;      // Ссылка на родителя

    // Таблица инфополей данной ветки
    QMap<QString, QString> fieldsTable;

    // Каждая ветка может содержать таблицу конечных записей
    RecordTableData recordsTable;

    bool detachedState; // Флаг, влияющий на деструктор. Если detachedState=true, подчиненные элементы удаляться не будут

    QIcon icon; // Иконка ветки
};

#endif
