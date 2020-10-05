#ifndef EDITORSHOWTEXTDISPATCHER_H
#define EDITORSHOWTEXTDISPATCHER_H

#include <QObject>
#include <QThread>

#include "EditorShowText.h"

//! \brief
//! Объект для управления открепляемыми окнами
//! Используется для создания окон, обновления содержимого,
//! запоминание списка окон и их размещения


class EditorShowTextDispatcher : public QObject
{
    Q_OBJECT
public:

    //! Класс в виде синглтона
    static EditorShowTextDispatcher *instance();

    void createWindow(const QString &noteId, int x=-1, int y=-1, int w=-1, int h=-1);

    void updateWindow(const QString &noteId);
    void updateAllWindows();

    void closeWindow(const QString &noteId);
    void closeAllWindows();
    void closeAllWindowsForExit();

    bool isWindowPresent( const QString &noteId );

    void saveOpenWindows();
    void restoreOpenWindows();

    void switchBehavior(const QString &mode);

signals:

public slots:

    void closeWindowByIdVector(const QVector<QString> &ids);
    void closeWindowByIdList(const QStringList &ids);
    void closeWindowByIdSet(const QSet< QString > &ids);

    void closeWindowForNonExistentRecords();

    void onCloseWindow(const QString &noteId);

protected:

    explicit EditorShowTextDispatcher(QObject *parent = nullptr);
    virtual ~EditorShowTextDispatcher();

    // Код диспетчера должен выполняться в отдельном треде,
    // как минимум методы закрытия окошек по списку
    QThread *mThread;

    // Список открытых окошек, ключ - это ID открытой записи
    QHash<QString, QPointer<EditorShowText> > mWindowsList;


};

#endif // EDITORSHOWTEXTDISPATCHER_H
