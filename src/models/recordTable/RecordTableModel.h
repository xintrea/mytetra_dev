#ifndef __RECORDTABLEMODEL_H__
#define __RECORDTABLEMODEL_H__

#include <QAbstractListModel>
#include <QModelIndex>
#include <QVariant>
#include <QObject>
#include <QtXml>


class RecordTableData;

class RecordTableModel : public QAbstractListModel
{
    Q_OBJECT

public:
    RecordTableModel(QObject *pobj=0);
    ~RecordTableModel();

    // Интерфейс модели, предоставление данных по указанному индексу
    QVariant data(const QModelIndex &index, int nRole) const;
    
    // Интерфейс модели, сохранение вводимых данных по указанному индексу
    bool setData(const QModelIndex &index, const QVariant &value, int nRole);

    // Интерфейс модели, сколько записей в таблице
    int rowCount(const QModelIndex &parent=QModelIndex()) const;

    // Возвращение указателя на модель конечных записей, т.е. на себя
    QAbstractListModel *getModel(void);

    // Установка указателя на таблицу данных, с которой нужно работать модели
    void setTableData(RecordTableData *rtData);
    
    // Ссылка на данные, с которыми работает модель
    RecordTableData *getTableData(void);
    
    // Обновление данных на экране после их изменения
    void update(void);

    // Удаление записей
    void removeRowsByList(QVector<int> delIdx);
    
private:

    // Указатель на таблицу конечных записей
    RecordTableData *table;

};

#endif // __RECORDTABLEMODEL_H__
