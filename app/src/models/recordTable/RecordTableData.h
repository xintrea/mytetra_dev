#ifndef __RECORDTABLEDATA_H__
#define	__RECORDTABLEDATA_H__

#include <QAbstractListModel>
#include <QModelIndex>
#include <QVariant>
#include <QObject>
#include <QtXml>
#include <QMap>
#include <QString>
#include <QByteArray>

class TreeItem;
class Record;


class RecordTableData
{

public:
    RecordTableData(void);
    virtual ~RecordTableData();

    // Получение текста указанной записи
    QString getText(int pos);

    // Установка текста и картинок указанной записи
    void setTextAndPictures(int pos,
                            const QString &text,
                            const QMap<QString, QByteArray> &picturesFiles=(QMap<QString, QByteArray>()) );
    
    // Получение значения указанного поля для указанного элемента
    QString getField(QString name, int pos) const;

    // Установка значения указанного поля для указанного элемента
    void setField(QString name, QString value, int pos);

    // Получение образа записи
    Record getRecordLite(int pos);
    Record getRecordFat(int pos);
    Record *getRecord(int pos);

    // Первичное заполнение таблицы конечных записей
    void init(TreeItem *item, QDomElement domModel);

    // Удаление всех элементов таблицы конечных записей
    void deleteAllRecords(void);
    
    // Количество записей в таблице данных
    unsigned int size(void) const;

    // Функция создания DOM-документа из данных таблицы конечных записей
    QDomElement exportDataToDom(QDomDocument *doc) const;
    void exportDataToStreamWriter(QXmlStreamWriter *xmlWriter) const;

    // Получение ссылки на объект ветки, которой принадлежит таблица
    TreeItem *getItem(void);

    int insertNewRecord(int mode,
                        int pos,
                        Record record);

    void editRecordFields(int pos,
                          QMap<QString, QString> editFields);

    void deleteRecord(int i);
    void deleteRecordById(QString id);

    bool isRecordExists(QString id);
    bool isBlockRecordExists();

    int getPosById(QString id);

    void empty(void);

    void moveUp(int pos);
    void moveDn(int pos);

    void switchToEncrypt(void);
    void switchToDecrypt(void);

    static void editorLoadCallback(QObject *editor,
                                     QString &loadText);

    static void editorSaveCallback(QObject *editor,
                                     QString saveText);

    int  getWorkPos(void);
    void setWorkPos(int pos);

    void checkAndCreateTextFile(int pos, QString fullFileName);


private:

    // Функция заполнения таблицы из DOM-документа
    void setupDataFromDom(QDomElement *domModel);

    // Таблица записей (в нормальном виде содержит только "легкие" объекты записей)
    QList< Record > tableData;

    // Ссылка на ветку, которой принадлежит данная таблица
    TreeItem *treeItem;

    // Номер записи, с которой работал пользователь
    int workPos;
};


#endif /* __RECORDTABLEDATA_H__ */
