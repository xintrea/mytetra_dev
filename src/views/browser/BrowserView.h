#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <QWidget>
#include <QList>
#include <QUrl>
#include "models/recordTable/Record.h"
#include "libraries/GlobalParameters.h"

class QNetworkReply;
class QSslError;
extern GlobalParameters globalParameters;


class BrowserView : public QWidget {
    Q_OBJECT
public:
    BrowserView(QWidget *parent = 0);
    ~BrowserView();
    QString getField(QString name){return current_record.getField(name);}


public slots:
    void setUrl(const QUrl &url);
    void loadUrl(const QUrl & _url = QUrl());
    void setScrollbars(bool hide);
    void setCache(bool cache, int cacheSize);
    void finished(QNetworkReply *reply);
    void sslErrors(QNetworkReply *reply, const QList<QSslError> &errors);
    QAction * getactionFreeze()
    {
        return actionFreeze;
    }

    void setupActions(void);

private:
    void setupUI(void);
    void setupSignals(void);
    void assembly(void);

    QAction *actionFreeze;
    struct BrowserViewPrivate *d;
    //QUrl current_url;
    Record current_record;
};

#endif // MAINVIEW_H
