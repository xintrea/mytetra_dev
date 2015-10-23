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
#include <QAction>
#include <QApplication>
#include <QSettings>
#include <QFileInfo>
#include <QDebug>
#include <QDir>
#include <QUrl>

#ifdef Q_WS_QWS
#include <QWSServer>
#endif

#include "BrowserView.h"
#include "browser_config.h"
#include "views/recordTable/RecordTableScreen.h"
#include "controllers/recordTable/RecordTableController.h"


struct BrowserViewPrivate {
    //Q_OBJECT
public:
    QWebView *view;
    QNetworkAccessManager *nam;
    QMap<QPair<QString,int>, QByteArray> exceptions;
    bool msgShown;
    //~BrowserViewPrivate() = default;
};

void BrowserView::loadUrl(const QUrl & _url)
{
    QUrl url;
    // initialize

    QFileInfo settingsFile;

//    QString sb(QApplication::applicationDirPath());
//    sb.append(QDir::separator());
//    sb.append("browser_config.ini");

    QString configFileName = globalParameters.getWorkDirectory() + "/browser_config.ini";
    // check to see if we have a settings file where we started from
    // if not fall back to system hard coded path
    QFileInfo file(configFileName.toLatin1()); //file(sb.toLatin1());
    if (file.exists()) {
        qDebug() << "using local settings file";
        settingsFile.setFile(file.filePath());
    } else {
        qDebug() << "using system defined settings file";
        settingsFile.setFile(SETTINGS_FILE);
    }

    QSettings settings(settingsFile.filePath(),QSettings::NativeFormat);

    settings.beginGroup(SETTINGS_SECTION);
    if( _url.isEmpty()) {
        url = (settings.value("browser_view").toString());  //QUrl url(settings.value("browser_view").toString());
    } else {
        url = _url;
    }

    this->setUrl(url);

    qDebug() << "loading url: " << settings.value("main_view").toString();

    if (settings.value("full_screen",false).toBool()) {
        this->showFullScreen();
    }

    if (settings.value("hide_cursor",false).toBool()) {
        this->setCursor(QCursor(Qt::BlankCursor));
#ifdef Q_WS_QWS
        QWSServer::setCursorVisible(false);
#endif
    }

    this->setScrollbars(settings.value("hide_scrollbars",false).toBool());
    this->setCache(settings.value("enable_cache",false).toBool(), settings.value("cache_size_megabytes",5).toInt());

    settings.endGroup();

    this->show();

    QString title = this->d->view->title();
    if(0 == title.size())title = "default";
    this->current_record.setField("name", title);
    QString author = "default";
    QString tags = "default";

    //QString text = this->d->view->page()->mainFrame()->toHtml();
    //if(0 == text.size())text = "<html>no content</html>";

    this->current_record.setField("author", author);
    this->current_record.setField("tags", tags);
    //this->current_record.setField("text", text);
    this->current_record.setField("url", url.toString());

}

BrowserView::BrowserView(QWidget *parent)
    : QWidget(parent),
      d(new BrowserViewPrivate)
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

    setupActions();

    setupUI();

    setupSignals();
    assembly();

    loadUrl();
}

BrowserView::~BrowserView()
{
    delete d;
}


void BrowserView::setupActions()
{
    actionFreeze=new QAction(tr("Freeze browser view"), this);
    actionFreeze->setStatusTip(tr("Freeze browser view"));
    actionFreeze->setIcon(QIcon(":/resource/pic/freeze_browser_view.png"));
}

void BrowserView::setupUI(void)
{

}

void BrowserView::setupSignals(void)
{
    //connect(this->actionFreeze, SIGNAL(triggered()), globalParameters.getWindowSwitcher(), SLOT(findInBaseClick()));

//    //Connect a signal to a pointer to qobject member function
//    template <typename Func1, typename Func2>
//    static inline QMetaObject::Connection connect(
//        const typename QtPrivate::FunctionPointer<Func1>::Object *sender, Func1 signal,
//        const typename QtPrivate::FunctionPointer<Func2>::Object *receiver, Func2 slot,
//        Qt::ConnectionType type = Qt::AutoConnection);

    QWebView::connect
    <
    void (QWebView::*)(const QUrl&)
    , void (RecordTableController::*)(void)
    >
    (
        this->d->view
        , &QWebView::urlChanged
        , globalParameters.getRecordTableScreen()->getRecordTableController()
        , &RecordTableController::autoAddNewAfterContext
    );
}

void BrowserView::assembly(void)
{

}

void BrowserView::setUrl(const QUrl &_url)
{
    d->view->load(_url);
}

void BrowserView::setScrollbars(bool hide)
{
    if (!hide) {
        d->view->page()->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAsNeeded);
        d->view->page()->mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAsNeeded);
    }
}

void BrowserView::setCache(bool cache, int cacheSize)
{
    if (cache) {
        QNetworkDiskCache *diskCache = new QNetworkDiskCache(this);
        QString location = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
        diskCache->setCacheDirectory(location);
        diskCache->setMaximumCacheSize(cacheSize*1024*1024); //in MB's
        d->nam->setCache(diskCache);
        qDebug() << QString("Cache location: %1").arg(location);
        qDebug() << QString("Cache maximum size: %1MB").arg(cacheSize);
    }
}

void BrowserView::finished(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() <<  QString("Network Error: %1").arg(reply->errorString());
    }

    if (reply->attribute(QNetworkRequest::SourceIsFromCacheAttribute).toBool() == true) {
        QVariant contentVar = reply->header(QNetworkRequest::ContentTypeHeader);
        qDebug() <<  QString("Cache Used: %1").arg(contentVar.toString());
    }

}

void BrowserView::sslErrors(QNetworkReply *reply, const QList<QSslError> &errors)
{
    foreach( const QSslError &error, errors ) {
        qDebug() << QString("SSL Error: %1").arg(error.errorString());
    }

    reply->ignoreSslErrors(errors);
}

