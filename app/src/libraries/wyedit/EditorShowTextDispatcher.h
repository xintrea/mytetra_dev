#ifndef EDITORSHOWTEXTDISPATCHER_H
#define EDITORSHOWTEXTDISPATCHER_H

#include <QObject>

class EditorShowTextDispatcher : public QObject
{
    Q_OBJECT
public:
    explicit EditorShowTextDispatcher(QObject *parent = nullptr);

signals:

public slots:
};

#endif // EDITORSHOWTEXTDISPATCHER_H
