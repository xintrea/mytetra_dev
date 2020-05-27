#ifndef EDITORSHOWTEXTDISPATCHER_H
#define EDITORSHOWTEXTDISPATCHER_H

#include <QObject>

#include "EditorShowText.h"

//! \brief
//! Объект для управления открепляемыми окнами
//! Используется для создания окон, обновления содержимого
//! запоминание списка окон и их размещения


class EditorShowTextDispatcher : public QObject
{
    Q_OBJECT
public:

    //! Класс в виде синглтона
    static EditorShowTextDispatcher *instance();

    void createWindow(const QString &noteId);
    void updateWindow(const QString &noteId);
    void updateAllWindow();

    bool isWindowAlreadyCreate( const QString &noteId );

signals:

public slots:

    void onCloseWindow(const QString &noteId);

protected:

    explicit EditorShowTextDispatcher(QObject *parent = nullptr);
    virtual ~EditorShowTextDispatcher();

    QHash<QString, EditorShowText*> mWindowList;

};

#endif // EDITORSHOWTEXTDISPATCHER_H
