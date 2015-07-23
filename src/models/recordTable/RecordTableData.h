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


// Структура данных одной записи
struct RecordData
{
  QString text;
  QMap<QString, QString> fieldList;
  QMap<QString, QString> attachList;
  QMap<QString, QByteArray> pictureFiles;
  QMap<QString, QByteArray> attachFiles;
};


// Структура данных одной записи без содержимого файлов картинок и приаттаченных файлов
// Нужна для особых случаев для ускорения работы
struct RecordExemplar
{
  QString text;
  QMap<QString, QString> fieldList; // Хранятся пары: Имя поля (атрибута XML) - Значение
  QMap<QString, QString> attachList; // Хранятся пары: id - Имя файла
};


class RecordTableData : public QObject
{
    Q_OBJECT

public:
    RecordTableData(QObject *pobj=0);
    virtual ~RecordTableData();

    // Получение текста указанной записи
    QString getText(int pos);

    // Установка текста и картинок указанной записи
    void setTextAndPictures(int pos,
                            const QString &text,
                            const QMap<QString, QByteArray> &picturesFiles=(QMap<QString, QByteArray>()) );

    bool checkAndFillFileDir(int pos, QString &nameDirFull, QString &nameFileFull);
    
    // Получение значения указанного поля для указанного элемента
    QString getField(QString name, int pos) const;

    // Установка значения указанного поля для указанного элемента
    void setField(QString name, QString value, int pos);
   
    // Получение всех инфополей указанного элемента
    QMap<QString, QString> getFieldList(int pos) const;

    // Получение списка приаттаченных файлов указанного элемента
    QMap<QString, QString> getAttachList(int pos) const;
    
    // Получение образа записи
    RecordExemplar getRecordExemplar(int pos);

    // Первичное заполнение таблицы конечных записей
    void init(TreeItem *item, QDomElement domModel);

    // Удаление всех элементов таблицы конечных записей
    void deleteAllRecords(void);
    
    // Количество записей в таблице данных
    int size(void) const;

    // Функция создания DOM-документа из данных таблицы конечных записей
    QDomDocument exportDataToDom(void);

    // Получение ссылки на объект ветки, которой принадлежит таблица
    TreeItem *getItem(void);

    int insertNewRecord(int mode,
                        int pos,
                        QString text,
                        QMap<QString, QString> fields,
                        QMap<QString, QByteArray> files = (QMap<QString, QByteArray>()) );

    void editRecordFields(int pos,
                          QMap<QString, QString> editFields);

    void deleteRecord(int i);
    void deleteRecordById(QString id);

    bool isRecordExists(QString id);

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

    QMap<QString, QString> getMergeFields(int pos, QMap<QString, QString> fields);

    // Таблица с инфополями записей
    QList< RecordExemplar > tableData;

    // Ссылка на ветку, которой принадлежит данная таблица
    TreeItem *treeItem;

    // Номер записи, с которой работал пользователь
    int workPos;
};


#endif /* __RECORDTABLEDATA_H__ */

