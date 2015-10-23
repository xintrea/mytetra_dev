#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <QWidget>
#include <QList>
#include <QUrl>

class QNetworkReply;
class QSslError;

class MainView : public QWidget
{
    Q_OBJECT
public:
    MainView(QWidget *parent=0);
    ~MainView();

public slots:
    void setUrl(QUrl url);
    void setScrollbars(bool hide);
    void setCache(bool cache, int cacheSize);
    void finished(QNetworkReply *reply);
    void sslErrors(QNetworkReply *reply, const QList<QSslError> &errors);

private:
    struct MainViewPrivate *d;
};

#endif // MAINVIEW_H
