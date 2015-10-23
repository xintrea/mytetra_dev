#include <QWebView> //#include <QtWebKitWidgets/QWebView>
#include <QWebPage>
#include <QWebFrame>
#include <QVBoxLayout>
#include <QNetworkDiskCache>
#include <QDesktopServices>
#include <QNetworkReply>
#include <QSslError>
#include <QSslCertificate>
#include <QSslConfiguration>
#include <QMessageBox>
#include <QMap>
#include <QByteArray>
#include <QDebug>
#include <QFile>
#include "mainview.h"

struct MainViewPrivate
{
    QWebView *view;
    QNetworkAccessManager *nam;
    QMap<QPair<QString,int>, QByteArray> exceptions;
    bool msgShown;
};

MainView::MainView(QWidget *parent)
    : QWidget(parent),
      d(new MainViewPrivate)
{
    d->view = new QWebView(this);
    d->view->page()->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
    d->view->page()->mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);

    d->msgShown = false;

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(d->view);

    d->nam = d->view->page()->networkAccessManager();

    connect(d->nam, SIGNAL(finished(QNetworkReply *)),
            this, SLOT(finished(QNetworkReply *)));

    connect(d->nam, SIGNAL(sslErrors(QNetworkReply *, const QList<QSslError> &)),
            this, SLOT(sslErrors(QNetworkReply *, const QList<QSslError> &)));
}

MainView::~MainView()
{
    delete d;
}

void MainView::setUrl(QUrl url)
{
    d->view->load(url);
}

void MainView::setScrollbars(bool hide)
{
    if (!hide)
    {
        d->view->page()->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAsNeeded);
        d->view->page()->mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAsNeeded);
    }
}

void MainView::setCache(bool cache, int cacheSize)
{
    if (cache)
    {
        QNetworkDiskCache *diskCache = new QNetworkDiskCache(this);
        QString location = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
        diskCache->setCacheDirectory(location);
        diskCache->setMaximumCacheSize(cacheSize*1024*1024); //in MB's
        d->nam->setCache(diskCache);
        qDebug() << QString("Cache location: %1").arg(location);
        qDebug() << QString("Cache maximum size: %1MB").arg(cacheSize);
    }
}

void MainView::finished(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
      qDebug() <<  QString("Network Error: %1").arg(reply->errorString());
    }

    if (reply->attribute(QNetworkRequest::SourceIsFromCacheAttribute).toBool() == true)
    {
      QVariant contentVar = reply->header(QNetworkRequest::ContentTypeHeader);
      qDebug() <<  QString("Cache Used: %1").arg(contentVar.toString());
    }

}

void MainView::sslErrors(QNetworkReply *reply, const QList<QSslError> &errors)
{
    foreach( const QSslError &error, errors ) {
       qDebug() << QString("SSL Error: %1").arg(error.errorString());
    }

    reply->ignoreSslErrors(errors);
}

